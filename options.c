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
Options *new_options(void)
{
   Options *o;

   if ( NULL == (o = (Options *)malloc(sizeof(Options))) )
   {
      fprintf(stderr, "ERROR: Unable to allocate memory for Options.\n");
      return(NULL);
   }

   /* Set defaults */
   o->bDebug = 0;
   o->bVerbose = 0;
   o->bHelp = 0;
   o->bValidate = 0;
   o->bAbout = 0;
   o->bpffile = NULL;
   o->binfile = NULL;
   o->iPasses = DEFAULT_MAX_PASSES;
   o->bESwap = 0;

   o->cFields = ':';
   o->bShowLabel = 1;
   o->eDumpHex = OUTPUT_DEFAULT;

   /* Send it off */
   return(o);
}

/* ========================================================================= */
Options *ParseOptions(int argc, char *argv[])
{
   Options *o;
   int index;
   int c;

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
   char opt[16];  /* The length here is an arbitrarly long length. */
   char tail[64]; /* Same here. */
   char thisopt;
   int parsed = 0;

   /* These items are checked elsewhere. This is just a final firewall,
      and assert()-like errors would be appropriate for an unset filename
      at this point in the code. */
   assert(NULL != o);
   assert(NULL != o->bpffile);

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
            if ( -1 == (o->bDebug = parse_opt_tail(line)) )
               return(opt_err_msg(thisopt, f->lineno));
            parsed++;
            break;
         case 'c':
            if ( -1 == (o->bValidate = parse_opt_tail(line)) )
               return(opt_err_msg(thisopt, f->lineno));
            parsed++;
            break;
         case 'e':
            if ( -1 == (o->bESwap = parse_opt_tail(line)) )
               return(opt_err_msg(thisopt, f->lineno));
            parsed++;
            break;
         case 'f':
            if ( parse_opt_char(&o->cFields, line) )
               return(opt_err_msg(thisopt, f->lineno));
            parsed++;
            break;
         case 'l':
            if ( -1 == (o->bShowLabel = parse_opt_tail(line)) )
               return(opt_err_msg(thisopt, f->lineno));
            parsed++;
            break;
         case 'p':
            if ( parse_opt_numeric(&o->iPasses, line) )
               return(opt_err_msg(thisopt, f->lineno));
            parsed++;
            break;
         case 'v':
            if ( -1 == (o->bVerbose = parse_opt_tail(line)) )
               return(opt_err_msg(thisopt, f->lineno));
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
