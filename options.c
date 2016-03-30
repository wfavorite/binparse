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

   if ( set_option(o, OS_DEFAULT, OK_TAGVAL, 0) )
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

   if ( set_option(o, OS_DEFAULT, OK_SHOWLBL, 1) )
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
   int bShowLabel;
   int bTagVal;

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

   if ( get_option(o, source, OK_SHOWLBL, &bShowLabel) )
      bShowLabel = 1; /* -l sets it to 0, not 1 */

   if ( get_option(o, source, OK_TAGVAL, &bTagVal) )
      bTagVal = 0;


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

   if (( bTagVal ) && (0 == bShowLabel))
   {
      fprintf(stderr, "ERROR: The -l option is not compatible with the -t option.\n");
      return(1);
   }

   return(0);
}

/* ========================================================================= */
Options *new_options(void)
{
   Options *o;

   if ( NULL == (o = (Options *)malloc(sizeof(Options))) )
   {
      fprintf(stderr, "ERROR: Unable to allocate memory for Options.\n");
      return(NULL);
   }

   memset(o, 0, sizeof(Options));

   if ( set_default_layer(o) )
      return(NULL);

   /* Send it off */
   return(o);
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
int apply_opt_layer(Options *o, int layer)
{
   int key;
   int value;

   key = OK_STARTKEY;
   while ( key <= OK_LASTKEY )
   {
      if ( 0 == get_option(o, layer, key, &value) )
      {
         switch ( key )
         {
         case OK_VERBOSE:
            o->bVerbose = value;
            break;
         case OK_ABOUT:
            o->bAbout = value;
            break;
         case OK_HELP:
            o->bHelp = value;
            break;
         case OK_DEBUG:
            o->bDebug = value;
            break;
         case OK_VALIDATE:
            o->bValidate = value;
            break;
         case OK_PASSES:
            o->iPasses = value;
            break;
         case OK_ESWAP:
            o->bESwap = value;
            break;
         case OK_TAGVAL:
            o->bTagVal = value;
            break;
         case OK_CFIELD:
            o->cFields = value;
            break;
         case OK_SHOWLBL:
            o->bShowLabel = value;
            break;
         case OK_DMPHEXUC: /* These cannot both be set (we check for that) */
            if ( value )
               o->eDumpHex = OUTPUT_HEX_UC;
            break;
         case OK_DMPHEXLC:
            if ( value )
               o->eDumpHex = OUTPUT_HEX_LC;
            break;
            /* OK_ERROR is not a flag to be set / applied in a layer */
         }
      }

      key++;
   }

   return(0);
}

/* ========================================================================= */
int CollapseOptionLayers(Options *o)
{
   if ( apply_opt_layer(o, OS_DEFAULT) )
      return(1);

   if ( apply_opt_layer(o, OS_CMDLINE) )
      return(1);

   if ( apply_opt_layer(o, OS_CFGFILE) )
      return(1);

   return(0);
}

/* ========================================================================= */
Options *ParseOptions(int argc, char *argv[])
{
   Options *o;
   int si;      /* String index */
   int ci;      /* Char index */
   int is_option_arg;
   int tmpint;
   char tmpchar;

   /* Allocate and initialize a new options struct */
   if ( NULL == ( o = new_options() ) )
      return(NULL);
  
   is_option_arg = 0;
   si = 1;
   while ( si < argc )
   {
      /* Is this parsing an option to an argument? */
      if ( is_option_arg )
      {
         /* This is the remnant of a previous arg */
         switch ( is_option_arg )
         {
         case OK_CFIELD:
            if ( parse_opt_char(&tmpchar, argv[si]) )
            {
               fprintf(stderr, "ERROR: Unable to parse C in the \"-f C\" option argument.\n");
               return(NULL);
            }
            
            if ( set_option(o, OS_CMDLINE, OK_CFIELD, tmpchar) )
               return(NULL);
            break;
         case OK_PASSES:
            if ( parse_opt_numeric(&tmpint, argv[si]) )
            {
               fprintf(stderr, "ERROR: Unable to parse N in the \"-p N\" option argument.\n");
               return(NULL);
            }
                  
            if ( set_option(o, OS_CMDLINE, OK_PASSES, tmpint) )
               return(NULL);
            break;
         default:
            /* Obscure error message for internal problem. */
            fprintf(stderr, "ERROR: Unexpected option handling order.\n");
            return(NULL);
         }

         is_option_arg = 0; /* Clear this so we don't hit it on the next arg */
         si++;
         continue;
      }

      if ( argv[si][0] == '-' )
      {
         if ( argv[si][1] == '-' )
         {

            if ( 0 == strcmp(argv[si], "--help") ) /* -h */
            {
               if ( set_option(o, OS_CMDLINE, OK_HELP, 1) )
                  return(NULL);
               
               si++;
               continue;
            }

            if ( 0 == strcmp(argv[si], "--about") ) /* -a */
            {
               if ( set_option(o, OS_CMDLINE, OK_ABOUT, 1) )
                  return(NULL);
              
               si++;
               continue;
            }

            if ( 0 == strcmp(argv[si], "--validate") ) /* -c */
            {
               if ( set_option(o, OS_CMDLINE, OK_VALIDATE, 1) )
                  return(NULL);
              
               si++;
               continue;
            }

            if ( 0 == strcmp(argv[si], "--endian_swap") ) /* -e */
            {
               if ( set_option(o, OS_CMDLINE, OK_ESWAP, 1) )
                  return(NULL);
              
               si++;
               continue;
            }

            if ( 0 == strcmp(argv[si], "--verbose") ) /* -v */
            {
               if ( set_option(o, OS_CMDLINE, OK_VERBOSE, 1) )
                  return(NULL);
              
               si++;
               continue;
            }

            if ( 0 == strcmp(argv[si], "--field_separator") ) /* -f */
            {
               is_option_arg = OK_CFIELD;

               si++;
               continue;
            }

            if ( 0 == strcmp(argv[si], "--passes") ) /* -p */
            {
               is_option_arg = OK_PASSES;

               si++;
               continue;
            }

            if ( 0 == strcmp(argv[si], "--no_labels") ) /* -l */
            {
               if ( set_option(o, OS_CMDLINE, OK_SHOWLBL, 0) )
                  return(NULL);
              
               si++;
               continue;
            }

            if ( 0 == strcmp(argv[si], "--show_tag") ) /* -t */
            {
               if ( set_option(o, OS_CMDLINE, OK_TAGVAL, 1) )
                  return(NULL);
              
               si++;
               continue;
            }

            if ( 0 == strcmp(argv[si], "--dump_hex") ) /* -x */
            {
               if ( set_option(o, OS_CMDLINE, OK_DMPHEXLC, 1) )
                  return(NULL);
              
               si++;
               continue;
            }

            if ( 0 == strcmp(argv[si], "--dump_HEX") ) /* -X */
            {
               if ( set_option(o, OS_CMDLINE, OK_DMPHEXUC, 1) )
                  return(NULL);
              
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
                     fprintf(stderr, "ERROR: Unable to parse C in the \"-fC\" option argument.\n");
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
                  if ( parse_opt_numeric(&tmpint, &argv[si][ci + 1]) )
                  {
                     fprintf(stderr, "ERROR: Unable to parse N in the \"-pN\" option argument.\n");
                     return(NULL);
                  }
                  
                  if ( set_option(o, OS_CMDLINE, OK_PASSES, tmpint) )
                     return(NULL);
               }
               break;
            case 't':
               if ( set_option(o, OS_CMDLINE, OK_TAGVAL, 1) )
                  return(NULL);
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

  return(o);
}

/* ========================================================================= */
void dbg_dump_key_int(Options *o, int key)
{
   int dval, cval, fval;

   if ( 0 == get_option(o, OS_DEFAULT, key, &dval) )
      printf(" %7d", dval);
   else
      printf("       -");

   if ( 0 == get_option(o, OS_CMDLINE, key, &cval) )
      printf(" %7d", cval);
   else
      printf("       -");

   if ( 0 == get_option(o, OS_CFGFILE, key, &fval) )
      printf(" %7d", fval);
   else
      printf("       -");

   printf("\n");
}

/* ========================================================================= */
void dbg_dump_key_char(Options *o, int key)
{
   int dval, cval, fval;

   if ( 0 == get_option(o, OS_DEFAULT, key, &dval) )
      printf("       %c", dval);
   else
      printf("       -");

   if ( 0 == get_option(o, OS_CMDLINE, key, &cval) )
      printf("       %c", cval);
   else
      printf("       -");

   if ( 0 == get_option(o, OS_CFGFILE, key, &fval) )
      printf("       %c", fval);
   else
      printf("       -");

   printf("\n");
}

/* ========================================================================= */
void DbgDumpOptions(Options *o)
{
   if (( 0 == o->bDebug ) && ( 0 == o->bVerbose ))
      return;

   printf("  User-configurable options dump\n");
   printf("   %-16s %4s %7s %7s %7s %7s\n", "Option", "Flag", "SetVal", "Default", "CmdLine", "BPFile");
   
   /* Verbose */
   printf("   %-16s    %c %7d", "Verbosity", 'v', o->bVerbose);
   dbg_dump_key_int(o, OK_VERBOSE);

   /* About */
   printf("   %-16s    %c %7d", "About", 'a', o->bAbout);
   dbg_dump_key_int(o, OK_ABOUT);

   /* About */
   printf("   %-16s    %c %7d", "Help", 'h', o->bHelp);
   dbg_dump_key_int(o, OK_HELP);

   /* Validate */
   printf("   %-16s    %c %7d", "Validate", 'c', o->bValidate);
   dbg_dump_key_int(o, OK_VALIDATE);

   /* Passes */
   printf("   %-16s    %c %7d", "Passes", 'p', o->iPasses);
   dbg_dump_key_int(o, OK_PASSES);

   /* ESwap */
   printf("   %-16s    %c %7d", "ByteSwap", 'e', o->bESwap);
   dbg_dump_key_int(o, OK_ESWAP);

   /* TagVal */
   printf("   %-16s    %c %7d", "TagVal", 't', o->bTagVal);
   dbg_dump_key_int(o, OK_TAGVAL);

   /* cFields */
   printf("   %-16s    %c       %c", "FieldSep", 'f', o->cFields);
   dbg_dump_key_char(o, OK_CFIELD);

   /* bShowLabel */
   printf("   %-16s    %c %7d", "ShowLabel", 'l', o->bShowLabel);
   dbg_dump_key_int(o, OK_SHOWLBL);

   /* eDumpHex */
   printf("   %-16s      %7d\n", "DumpHex", o->eDumpHex);
   printf("   %-16s    %c        ", "DumpHex(LC)", 'x');
   dbg_dump_key_int(o, OK_DMPHEXLC);
   printf("   %-16s    %c        ", "DumpHex(UC)", 'X');
   dbg_dump_key_int(o, OK_DMPHEXUC);

   if ( o->bpffile )
      printf("   bpffile = \"%s\"\n", o->bpffile);
   else
      printf("   bpffile = NULL\n");
   
   if ( o->binfile )
      printf("   binfile = \"%s\"\n", o->binfile);
   else
      printf("   binfile = NULL\n");

   fflush(stdout);
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

   if ( NULL == (f = NewFile(o->bpffile)) )
      return(-1);

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
         case 't':
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

            /* This is inverted. The bShowLabel value is backwards from everything
               else. A bad choice of variable name mucks everything up. */
            if ( set_option(o, OS_CFGFILE, OK_SHOWLBL, ! temptail) )
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
         case 't':
            if ( -1 == (temptail = parse_opt_tail(line)) )
               return(opt_err_msg(thisopt, f->lineno));

            if ( set_option(o, OS_CFGFILE, OK_TAGVAL, temptail) )
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

   /* Validate the layer we just parsed */
   if ( validate_layer(o, OS_CFGFILE) )
      return(-1);

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

