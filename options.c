#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "options.h"
#include "strlib.h"
#include "slfile.h"

/* ========================================================================= */
int set_option(Options *o, int source, int key, int value)
{
   Option *newio;  /* NEW Individual Option */
   Option *thisio;

   thisio = NULL;
   switch ( source )
   {
   case OS_DEFAULT:
      thisio = o->defaults;
      break;
   case OS_CMDLINE:
      thisio = o->cmdline;
      break;
   case OS_CFGFILE:
      thisio = o->cfgfile;
      break;
   default:
      return(1);
   }

   while ( thisio )
   {
      if ( key == thisio->key )
      {
         thisio->value = value;
         return(0);
      }

      thisio = thisio->next;
   }

   /* If we got here, then no match. Create a new object */

   if ( NULL == ( newio = (Option *)malloc(sizeof(Option)) ) )
   {
      /* Obscure messages are ok here. */
      fprintf(stderr, "ERROR: Failed to allocate memory for an option object.\n");
      return(1);
   }

   newio->value = value;
   newio->key = key;
   newio->next = NULL;

   switch ( source )
   {
   case OS_DEFAULT:
      newio->next = o->defaults;
      o->defaults = newio;
      break;
   case OS_CMDLINE:
      newio->next = o->cmdline;
      o->cmdline = newio;
      break;
   case OS_CFGFILE:
      newio->next = o->cfgfile;
      o->cfgfile = newio;
      break;
   }
   
   return(0);
}

/* ========================================================================= */
int get_option(Options *o, int source, int key, int *value)
{
   Option *thisio;

   thisio = NULL;
   switch ( source )
   {
   case OS_DEFAULT:
      thisio = o->defaults;
      break;
   case OS_CMDLINE:
      thisio = o->cmdline;
      break;
   case OS_CFGFILE:
      thisio = o->cfgfile;
      break;
   default:
      return(1);
   }

   while ( thisio )
   {
      if ( key == thisio->key )
      {
         *value = thisio->value;
         return(0);
      }

      thisio = thisio->next;
   }

   /* Not found */
   return(1);
}

/* ========================================================================= */
int count_set_options(Options *o, int source)
{
   Option *thisio;
   int rv = 0;

   thisio = NULL;
   switch ( source )
   {
   case OS_DEFAULT:
      thisio = o->defaults;
      break;
   case OS_CMDLINE:
      thisio = o->cmdline;
      break;
   case OS_CFGFILE:
      thisio = o->cfgfile;
      break;
   default:
      return(0);
   }

   while ( thisio )
   {
      /* Ignore the debug flag - it is undocumented, and compatible with everything */
      if ( thisio->key != OK_DEBUG )
         rv++;

      thisio = thisio->next;
   }

   return(rv);
}

/* ========================================================================= */
int set_default_layer(Options *o)
{
   assert(o != NULL);

   if ( set_option(o, OS_DEFAULT, OK_VERBOSE, 0) )
      return(1);

   if ( set_option(o, OS_DEFAULT, OK_ABOUT, 0) )
      return(1);

   if ( set_option(o, OS_DEFAULT, OK_PASSES, DEFAULT_MAX_PASSES) )
      return(1);

   if ( set_option(o, OS_DEFAULT, OK_ESWAP, 0) )
      return(1);

   if ( set_option(o, OS_DEFAULT, OK_HELP, 0) )
      return(1);

   if ( set_option(o, OS_DEFAULT, OK_DMPHEXUC, 0) )
      return(1);

   if ( set_option(o, OS_DEFAULT, OK_DMPHEXLC, 0) )
      return(1);

   if ( set_option(o, OS_DEFAULT, OK_DEBUG, 0) )
      return(1);

   if ( set_option(o, OS_DEFAULT, OK_VALIDATE, 0) )
      return(1);

   if ( set_option(o, OS_DEFAULT, OK_CFIELD, ':') )
      return(1);

   if ( set_option(o, OS_DEFAULT, OK_SHOWLBL, ':') )
      return(1);

   return(0);
}

/* ========================================================================= */
int validate_layer(Options *o, int source)
{
   int set_opts;
   int bAbout;
   int bHelp;
   int bHEX;
   int bhex;
   int bValidate;

   /* Defaults are not validated */
   if ( source == OS_DEFAULT )
      return(0);

   /* Read in the options - set to 0 if not set (in this layer) */

   set_opts = count_set_options(o, source);

   if ( get_option(o, source, OK_ABOUT, &bAbout) )
      bAbout = 0;

   if ( get_option(o, source, OK_HELP, &bHelp) )
      bHelp = 0;

   if ( get_option(o, source, OK_DMPHEXLC, &bhex) )
      bhex = 0;

   if ( get_option(o, source, OK_DMPHEXUC, &bHEX) )
      bHEX = 0;

   if ( get_option(o, source, OK_VALIDATE, &bValidate) )
      bValidate = 0;


   /*** Some basic stuff ***/
   if (( bAbout ) || ( bHelp ))
   {
      if (( set_opts != 1 ) || ( o->bpffile ) || ( o->binfile ))
      {
         fprintf(stderr, "ERROR: The -a and -h options are mutually exclusive.\n");
         return(1);
      }

      /* Who cares about the rest */
      return(0);
   }
   
   /*** Some issues unique to the command line ***/
   if ( source == OS_CMDLINE )
   {
      if ( NULL == o->bpffile )
      {
         fprintf(stderr, "ERROR: A BPF file was not specified on the command line.\n");
         return(1);
      }
   }

   /*** Items that are general to any source ***/
   if ( ( bHEX ) && ( bhex ) )
   {
      fprintf(stderr, "ERROR: The -X and -x options are mutually exclusive.\n");
      return(1);
   }

   if ( bValidate )
   {
     if ( NULL == o->binfile )
     {
       fprintf(stderr, "ERROR: A target (binary) file was not specified on the command line.\n");
       return(1);
     }
   }


   /* STUB: Resolve this following issue - now that you can */
   /* It is not possible (in the current settings design) to tell if the user
      selected to not show the label, then decided how the label should be printed.
      This is a contradiction of options and the user should be notified, but
      there is no way to tell if the user did this. So (for now) we will just
      ignore these (specific) conflicts. */

   return(0);
}

/* ========================================================================= */
Options *new_options(void)
{
   Options *o;

   fprintf(stderr, "STUB DEBUG: new_options(");

   if ( NULL == (o = (Options *)malloc(sizeof(Options))) )
   {
      fprintf(stderr, "ERROR: Unable to allocate memory for Options.\n");
      return(NULL);
   }

   fprintf(stderr, ".");

   memset(o, 0, sizeof(Options));

   fprintf(stderr, ".");

   if ( set_default_layer(o) )
      return(NULL);

   fprintf(stderr, ")\n");
   /* Send it off */
   return(o);
}

/* ========================================================================= */
/* This is copied from / based on parse_opt_numeric()                        */
int parse_opt_char(char *rv, char *line)
{
   char open_quote = 0;
   char pv; /* Parsed value (not return value) */

   /* Move off leading ws */
   eat_ws(&line);

   if ( *line == 0 )
      return(1);

   if (( *line == '\'' ) || ( *line == '\"' ))
   {
      open_quote = *line;
      line++;
   }

   if ( isprint(*line) )
   {
      pv = *line;
      line++;
   }
   else
      return(1);

   if ( open_quote )
   {
      if ( *line != open_quote )
         return(1);
   }

   /* Good to go */
   *rv = pv;
   return(0);
}

/* ========================================================================= */
Options *ParseOptions(int argc, char *argv[])
{
   Options *o;
   int si;      /* String index */
   int ci;      /* Char index */
   int is_option_arg;
   /* int tmpint; STUB: for now */
   char tmpchar;

   fprintf(stderr, "STUB DEBUG: ParseOptions(%d, ...)\n", argc);

   /* Allocate and initialize a new options struct */
   if ( NULL == ( o = new_options() ) )
      return(NULL);
  
   is_option_arg = 0;
   si = 1;
   while ( si < argc )
   {
      fprintf(stderr, "STUB DEBUG: Working on item [%s]\n", argv[si]);

      /* Is this parsing an option to an argument? */
      if ( is_option_arg )
      {
         /* This is the remnant of a previous arg */
         switch ( is_option_arg )
         {
         case OK_CFIELD:
            if ( parse_opt_char(&tmpchar, argv[si]) )
            {
               /* STUB: Is an error message appropriate? */
               return(NULL);
            }
            
            if ( set_option(o, OS_CMDLINE, OK_CFIELD, tmpchar) )
               return(NULL);
            break;
         default:
            /* Obscure error message for internal problem. */
            fprintf(stderr, "ERROR: Unexpected option handling order.\n");
            return(NULL);
         }

         si++;
         continue;
      }

      if ( argv[si][0] == '-' )
      {
         if ( argv[si][1] == '-' )
         {

            if ( 0 == strcmp(argv[si], "--help") )
            {
               if ( set_option(o, OS_CMDLINE, OK_HELP, 1) )
                  return(NULL);
               
               si++;
               continue;
            }

            if ( 0 == strcmp(argv[si], "--about") )
            {
               if ( set_option(o, OS_CMDLINE, OK_ABOUT, 1) )
                  return(NULL);
              
               si++;
               continue;
            }

            if ( 0 == strcmp(argv[si], "--validate") )
            {
               if ( set_option(o, OS_CMDLINE, OK_VALIDATE, 1) )
                  return(NULL);
              
               si++;
               continue;
            }

            if ( 0 == strcmp(argv[si], "--endian_swap") )
            {
               if ( set_option(o, OS_CMDLINE, OK_ESWAP, 1) )
                  return(NULL);
              
               si++;
               continue;
            }


            if ( 0 == strcmp(argv[si], "--field_separator") )
            {
               is_option_arg = OK_CFIELD;

               si++;
               continue;
            }

         } /* Done handling the double dashes */

         /* Gotta be a single-dash */
         ci = 1;

         while ( argv[si][ci] != 0 )
         {
            switch ( argv[si][ci] )
            {
            case '+':
               if ( set_option(o, OS_CMDLINE, OK_DEBUG, 1) )
                  return(NULL);
               break;
            case 'a':
               if ( set_option(o, OS_CMDLINE, OK_ABOUT, 1) )
                  return(NULL);
               break;
            case 'c':
               if ( set_option(o, OS_CMDLINE, OK_VALIDATE, 1) )
                  return(NULL);
               break;
            case 'e':
               if ( set_option(o, OS_CMDLINE, OK_ESWAP, 1) )
                  return(NULL);
               break;
            case 'f':
               if ( 0 == argv[si][ci + 1] )
               {
                  is_option_arg = OK_CFIELD;
               }
               else
               {
                  if ( parse_opt_char(&tmpchar, &argv[si][ci + 1]) )
                  {
                     /* STUB: Is an error message appropriate? */
                     return(NULL);
                  }
                  
                  if ( set_option(o, OS_CMDLINE, OK_CFIELD, tmpchar) )
                     return(NULL);
               }
               break;
            case 'h':
               if ( set_option(o, OS_CMDLINE, OK_HELP, 1) )
                  return(NULL);
               break;
            case 'l':
               if ( set_option(o, OS_CMDLINE, OK_SHOWLBL, 0) )
                  return(NULL);
               break;
            case 'p':
               if ( 0 == argv[si][ci + 1] )
               {
                  is_option_arg = OK_PASSES;
               }
               else
               {
                  /* STUB: This is the wrong function */
                  if ( parse_opt_char(&tmpchar, &argv[si][ci + 1]) )
                  {
                     /* STUB: Is an error message appropriate? */
                     return(NULL);
                  }
                  
                  if ( set_option(o, OS_CMDLINE, OK_PASSES, tmpchar) ) /* STUB: NOT a char! */
                     return(NULL);
               }
               break;
            case 'v':
               if ( set_option(o, OS_CMDLINE, OK_VERBOSE, 1) )
                  return(NULL);
               break;
            case 'x':
               if ( set_option(o, OS_CMDLINE, OK_DMPHEXLC, 1) )
                  return(NULL);
               break;
            case 'X':
               if ( set_option(o, OS_CMDLINE, OK_DMPHEXUC, 1) )
                  return(NULL);
               break;
            }

            ci++;
         }

         si++;
         continue;
      } /* Done handling the single dash options */


      /* Has to be a single string - a file name */
      if ( NULL == o->bpffile )
      {
         if (NULL == (o->bpffile = nc_mkstring(argv[si])))
            return(NULL);
      }
      else
      {
         if ( NULL == o->binfile )
         {
            if (NULL == (o->binfile = nc_mkstring(argv[si])))
               return(NULL);
         }
         else
         {
            fprintf(stderr, "ERROR: Extra arguments \"%s\" not understood.\n", argv[si]);
            return(NULL);
         }
      }

      si++;

   }

   /* Validate what was just set */
   if ( validate_layer(o, OS_CMDLINE) )
      return(NULL);

   /* Now add the config file options */
   if ( ParseBPFOptions(o) )
      return(NULL);

   /* Validate what was just set */
   if ( validate_layer(o, OS_CFGFILE) )
      return(NULL);

  return(o);
}




























































/* ========================================================================= */
/* This is a rare case of not messaging at the point of error.               */
int parse_opt_tail(char *line)
{
   char LINE[16];
   int i;

   /* Move off leading ws */
   eat_ws(&line);

   i = 0;
   /* Coulda shoulda done isalpha() on this */
   while ( (( line[i] >= 'a' ) && ( line[i] <= 'z' )) ||
           (( line[i] >= 'A' ) && ( line[i] <= 'Z' )) ||
           (( line[i] >= '0' ) && ( line[i] <= '9' )) )
   {
      if ( ( line[i] >= 'a' ) && ( line[i] <= 'z' ) )
         LINE[i] = line[i] - 32;
      else
         LINE[i] = line[i];

      i++;

      if ( i >= 15 ) /* Input is too long */
         return(-1);
   }

   LINE[i] = 0; /* Terminate */

   while (( line[i] == ' ' ) || ( line[i] == '\t' ))
      i++;
   
   /* The next char should be termination or the # starting a comment. And
      the comment was filtered in the calling function. */
   if (( line[i] != '#' ) && ( line[i] != 0 ))
   {
      return(-1);
   }

   if ( 0 == strcmp(LINE, "TRUE") )
      return(1);

   if ( 0 == strcmp(LINE, "ON") )
      return(1);

   if ( 0 == strcmp(LINE, "FALSE") )
      return(0);

   if ( 0 == strcmp(LINE, "OFF") )
      return(0);

   /* Fall through to error */
   return(-1);
}

/* ========================================================================= */
/* This is a rare case of not messaging at the point of error.               */
int parse_opt_numeric(int *rv, char *line)
{
   int i;   /* Index for string */

   /* Move off leading ws */
   eat_ws(&line);

   *rv = 0;
   i = 0;
   while (( line[i] >= '0' ) && ( line[i] <= '9' ))
   {
     *rv *= 10;
     *rv += (line[i] - '0');
     i++;
   }

   /* If nothing was parsed */
   if ( i == 0 )
     return(1);

   while (( line[i] == ' ' ) || ( line[i] == '\t' ))
     i++;
   
   /* The next char should be termination or the # starting a comment. And
      the comment was filtered in the calling function. */
   if (( line[i] != '#' ) && ( line[i] != 0 ))
      return(1);
   
   /* Fall through to success */
   return(0);
}

/* ========================================================================= */
/* This is an error function specific to ParseBPFOptions()                   */
int opt_err_msg(char thisopt, int lineno)
{
   fprintf(stderr, "-------------------------------------------------------------------------------\n");
   fprintf(stderr, "BPF file setopt failure. Problems parsing the argument to \"%c\" on line %d.\n", thisopt, lineno);
   return(-1);
}

/* ========================================================================= */
int ParseBPFOptions(Options *o)
{
   File *f;
   char *line;
   char thisopt;
   int parsed = 0;

   int temptail;
   char tempchar;
   int tempnum;

   /* With the new modified code, o should never be null, but bpffile
      might be. So one is an assert, the other is a situation where 
      we cannot read the file - for an edge case where the file is not
      needed. (-a | -h) */
   assert(NULL != o);

   if ( NULL == o->bpffile )
      return(0);

   f = NewFile(o->bpffile);

   /* Now start pulling lines */
   while(NextLine(f))
   {
      /* ...for each line... */
      line = f->line;

      chomp(line);              /* Kill EOL chars */
      line = leadingwst(line);  /* Kill leading WS */
      hash_trunc(line);         /* Truncate Hash based comments */

      /* Line sniff for setting options */
      if ( line == strstr(line, "setopt"))
      {
         line += 6;

         eat_ws(&line);

         /* Clear off the '-' character if the user set it */
         if ( *line == '-' )
            line++;

         switch( *line )
         {
         case '+':
         case 'v':
         case 'c':
         case 'e':
         case 'f':
         case 'l':
         case 'p':
         case 'x':
         case 'X':
            thisopt = *line;
            break;
         case 'a':
         case 'h':
            fprintf(stderr, "-------------------------------------------------------------------------------\n");
            fprintf(stderr, "BPF file setopt failure. Option \"%c\" on line %d is not supported as a \n", *line, f->lineno);
            fprintf(stderr, "   file-based option. (It can only be set from command line.)\n");
            /* Let's exit. This could be ignored, but best to error because results will not match intent. */
            return(-1);
         default:
            fprintf(stderr, "-------------------------------------------------------------------------------\n");
            fprintf(stderr, "The BPF file setopt option on line %d is not understood. \n", f->lineno);
            /* Exit - don't ignore. */
            return(-1);
            break; /* A compiler thing */
         }
         
         line++; /* Move off the option character. */
         
         if ( ( *line != ' ' ) && ( *line != '\t' ) )
         {
            fprintf(stderr, "-------------------------------------------------------------------------------\n");
            fprintf(stderr, "BPF file setopt failure. Problems parsing the argument on line %d. Only one\n", f->lineno);
            fprintf(stderr, "   option is allowed per \"setopt\" directive.\n");
            /* Only allow a single option per line */
            return(-1);
         }

         switch( thisopt )
         {
         case '+':
            if ( -1 == (temptail = parse_opt_tail(line)) )
               return(opt_err_msg(thisopt, f->lineno));
            
            if ( set_option(o, OS_CFGFILE, OK_DEBUG, temptail) )
               return(1);

            parsed++;
            break;
         case 'c':
            if ( -1 == (temptail = parse_opt_tail(line)) )
               return(opt_err_msg(thisopt, f->lineno));


            if ( set_option(o, OS_CFGFILE, OK_VALIDATE, temptail) )
               return(1);

            parsed++;
            break;
         case 'e':
            if ( -1 == (temptail = parse_opt_tail(line)) )
               return(opt_err_msg(thisopt, f->lineno));

            if ( set_option(o, OS_CFGFILE, OK_ESWAP, temptail) )
               return(1);

            parsed++;
            break;
         case 'f':
            if ( parse_opt_char(&tempchar, line) )
               return(opt_err_msg(thisopt, f->lineno));

            if ( set_option(o, OS_CFGFILE, OK_CFIELD, tempchar) )
               return(1);

            parsed++;
            break;
         case 'l':
            if ( -1 == (temptail = parse_opt_tail(line)) )
               return(opt_err_msg(thisopt, f->lineno));

            /* STUB: Should this be inverted? */
            if ( set_option(o, OS_CFGFILE, OK_SHOWLBL, temptail) )
               return(1);

            parsed++;
            break;
         case 'p':
            if ( parse_opt_numeric(&tempnum, line) )
               return(opt_err_msg(thisopt, f->lineno));

            if ( set_option(o, OS_CFGFILE, OK_PASSES, tempnum) )
               return(1);

            parsed++;
            break;
         case 'v':
            if ( -1 == (temptail = parse_opt_tail(line)) )
               return(opt_err_msg(thisopt, f->lineno));

            if ( set_option(o, OS_CFGFILE, OK_VERBOSE, temptail) )
               return(1);

            parsed++;
            break;
         case 'x':
            if ( -1 == (temptail = parse_opt_tail(line)) )
               return(opt_err_msg(thisopt, f->lineno));

            if ( set_option(o, OS_CFGFILE, OK_DMPHEXLC, temptail) )
               return(1);

            parsed++;
            break;
         case 'X':
            if ( -1 == (temptail = parse_opt_tail(line)) )
               return(opt_err_msg(thisopt, f->lineno));

            if ( set_option(o, OS_CFGFILE, OK_DMPHEXUC, temptail) )
               return(1);

            parsed++;
            break;
            /* You can't have a "default" option here. Other paths have been eliminated. */
         }
      }
   }
  
   /* Close the file */
   EndFile(f);

   return(parsed);
}

/* ========================================================================= */
int IsSetOpt(char *line)
{
   eat_ws(&line);

   if ( line == strstr(line, "setopt") )
      line += 6;
   else
      return(0);

   if (( *line == ' ' ) || ( *line == '\t' ))
      return(1);

   return(0);
}























#ifdef STUB_REMOVE_ME

/* ========================================================================= */
/* This is copied from / based on parse_opt_numeric()                        */
int parse_opt_char(char *rv, char *line)
{
   char open_quote = 0;
   char pv; /* Parsed value (not return value) */

   /* Move off leading ws */
   eat_ws(&line);

   if ( *line == 0 )
      return(1);

   if (( *line == '\'' ) || ( *line == '\"' ))
   {
      open_quote = *line;
      line++;
   }

   if ( isprint(*line) )
   {
      pv = *line;
      line++;
   }
   else
      return(1);

   if ( open_quote )
   {
      if ( *line != open_quote )
         return(1);
   }

   /* Good to go */
   *rv = pv;
   return(0);
}



/* ========================================================================= */
/* STUB: Deprecated - remove this */
Options *ParseOpts(int argc, char *argv[])
{
   Options *o;
   int index;
   int c;
   extern char *optarg;
   extern int optind, optopt;


   /* Allocate and initialize a new options struct */
   if ( NULL == ( o = new_options() ) )
      return(NULL);
  
   /* Parse the options */
   while ( -1 != ( c = getopt(argc, argv, "+acef:hlp:vxX" ) ) )
   {
      switch(c)
      {
      case '+':
         o->bDebug = 1;
         break;
      case 'a':
         o->bAbout = 1;
         break;
      case 'c':
         o->bValidate = 1;
         break;
      case 'e':
         o->bESwap = 1;
         break;
      case 'f':
         o->cFields = optarg[0];
         break;
      case 'h':
         o->bHelp = 1;
         break;
      case 'l':
         o->bShowLabel = 0;
         break;
      case 'p':
         o->iPasses = atoi(optarg);
         break;
      case 'v':
         o->bVerbose = 1;
         break;  
      case 'x':
         o->eDumpHex |= OUTPUT_HEX_LC;
         break;  
      case 'X':
         o->eDumpHex |= OUTPUT_HEX_UC;
         break;  
      case ':':
         fprintf (stderr, "ERROR: Missing the argument to the \"-%c\" option.\n", optopt);
         return(NULL); /* Just bail */
      case '?': /* User entered some unknown/unsupported argument */
         if (isprint (optopt))
            fprintf (stderr, "ERROR: Unknown option \"-%c\".\n", optopt);
         else
            fprintf (stderr,
                     "ERROR: Unknown option character `\\x%x'.\n",
                     optopt);
         return(NULL); /* Just bail */
      default: /* Really an unreachable place */
         fprintf(stderr, "ERROR: Encountered problems parsing the command line options.\n");
         return(NULL);
      }
   }

   /* Read the non-flag options (the bpf and binary files) */
   index = optind;
   while ( index < argc )
   {
      if ( NULL == o->bpffile )
      {
         if (NULL == (o->bpffile = nc_mkstring(argv[index])))
            return(NULL);
      }
      else
      {
         if ( NULL == o->binfile )
         {
            if (NULL == (o->binfile = nc_mkstring(argv[index])))
               return(NULL);
         }
         else
         {
            fprintf(stderr, "ERROR: Extra arguments \"%s\" not understood.\n", argv[index]);
            return(NULL);
         }
      }

      index++;
   }

   /*** Validate the options ***/

   /* No additional processing required if any of these are set */
   if (( o->bAbout ) || ( o->bHelp ))
   {
      if (( o->bAbout ) && ( o->bHelp ))
      {
         /* Just quietly give them help */
         o->bAbout = 0;
      }

      /* The next two if statements have exactly the same results. I just thought
         that two if statements (one for filenames, one for options) was a bit more
         readable. */
      if ((o->bpffile) || (o->binfile))
      {
         fprintf(stderr, "ERROR: The -a and -h options are mutually exclusive.\n");
         return(NULL);
      }

      if ((o->bVerbose) || (o->bDebug) || ( o->eDumpHex & OUTPUT_HEX_UC ) || ( o->eDumpHex & OUTPUT_HEX_LC ))
      {
         fprintf(stderr, "ERROR: The -a and -h options are mutually exclusive.\n");
         return(NULL);
      }

      return(o);
   }

   if ( NULL == o->bpffile )
   {
     fprintf(stderr, "ERROR: A BPF file was not specified on the command line.\n");
     return(NULL);
   }

   /* It is not possible (in the current settings design) to tell if the user
      selected to not show the label, then decided how the label should be printed.
      This is a contradiction of options and the user should be notified, but
      there is no way to tell if the user did this. So (for now) we will just
      ignore these (specific) conflicts. */

   if ( ( o->eDumpHex & OUTPUT_HEX_UC ) && ( o->eDumpHex & OUTPUT_HEX_LC ) )
   {
      fprintf(stderr, "ERROR: The -X and -x options are mutually exclusive.\n");
      return(NULL);
   }

   if ( 0 == o->bValidate )
   {
     if ( NULL == o->binfile )
     {
       fprintf(stderr, "ERROR: A target (binary) file was not specified on the command line.\n");
       return(NULL);
     }
   }
       
  return(o);
}

#endif
