#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

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
   o->bAbout = 0;
   o->bpffile = NULL;
   o->binfile = NULL;

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
   while ( -1 != ( c = getopt(argc, argv, "+ahv" ) ) )
   {
      switch(c)
      {
      case '+':
         o->bDebug = 1;
         break;
      case 'a':
         o->bAbout = 1;
         break;
      case 'h':
         o->bHelp = 1;
         break;
      case 'v':
         o->bVerbose = 1;
         break;  
      case ':':
         fprintf (stderr, "ERROR: Missing the argument to the \"-%c\" option.\n", optopt);
         return(NULL); /* STUB: Just bail */
         break;

      case '?': /* User entered some unknown/unsupported argument */
         if (isprint (optopt))
            fprintf (stderr, "ERROR: Unknown option \"-%c\".\n", optopt);
         else
            fprintf (stderr,
                     "ERROR: Unknown option character `\\x%x'.\n",
                     optopt);
         break;
      default: /* Really an unreachable place */
         return(o);
      }
   }

   /* Read the non-flag options (the bpf and binary files) */
   index = optind;
   while ( index < argc )
   {
      if ( NULL == o->bpffile )
      {
         if (NULL == (o->bpffile = mkstring(argv[index])))
            return(NULL);
      }
      else
      {
         if ( NULL == o->binfile )
         {
            if (NULL == (o->binfile = mkstring(argv[index])))
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

   /* No additional processing required if any of these are set */
   if (( o->bAbout ) || ( o->bHelp ))
      return(o);





















  
   /* STUB: Used to hard-code debug. 
  o->bpffile = nc_mkstring("sample.bpf");
   */



  /* STUB: Validate the options */
  



  return(o);
}





/* ========================================================================= */
int ParseBPFOptions(Options *o)
{
   File *f;
   char *line;

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
      if (( line[0] == 's' ) &&
          ( line[1] == 'e' ) &&
          ( line[2] == 't' ) &&
          ( line[3] == 'o' ) &&
          ( line[4] == 'p' ) &&
          ( line[5] == 't' ))
      {

         /* STUB: No options actually parsed at this time */
         fprintf(stderr, "DEBUG: %s\n", line);
      }
   }
  
   /* Close the file */
   EndFile(f);

   return(0);
}
