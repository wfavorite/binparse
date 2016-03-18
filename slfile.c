#include <stdlib.h>

#include "slfile.h"
#include "strlib.h"

/* ========================================================================= */
File *NewFile(char *filename)
{
   File *f;

   if ( NULL == ( f = (File *)malloc(sizeof(File)) ) )
   {
      fprintf(stderr, "ERROR: Unable to allocate memory for File structure.\n");
      return(NULL);
   }
   
   /* Some basic defaults */
   f->fh = NULL;  /* File is closed */
   f->lineno = 0;
   f->eof = 0;
   f->readerr = 0;
   f->line[0] = 0;

   if (NULL == (f->fh = fopen(filename, "r")))
   {
      fprintf(stderr, "ERROR: Unable to open \"%s\".\n", mid_trunc(NULL, filename, 40));
      return(NULL);
   }

   return(f);
}

/* ========================================================================= */
int NextLine(File *f)
{
   char got;
   unsigned long lpos;

   lpos = 0;

   while ( EOF != (got = fgetc(f->fh)) )
   {
      if ( lpos < MAX_LINE_LEN )
      {
         if ( got == '\n' )
         {
            f->line[lpos] = 0;
            f->lineno++;
            return(1);
         }
         else
         {
            f->line[lpos] = got;
         }

         lpos++;
      }
      else
      {
         f->line[lpos - 1] = 0;
         f->readerr = 1;
         return(0);
      }
   }

   f->line[lpos] = 0;
   f->eof = 1;
   return(0);
}

/* ========================================================================= */
int WasAReadError(File *f)
{
   /* Just a gettor */
   return(f->readerr);
}

/* ========================================================================= */
int EndFile(File *f)
{
   if ( f )
   {
      /* Only close if file handle is open */
      if ( f->fh )
         fclose(f->fh);
      
      free(f);
      
      return(0);
   }
   
   return(0);
}
