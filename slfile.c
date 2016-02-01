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
  if ( fgets(f->line, MAX_LINE_LEN, f->fh) )
  {
    f->lineno++;
    /* Fall through to success */
  }
  else
  {
    f->line[0] = 0;
    f->eof = 1;
    return(0);
  }

  /* Non-zero means keep on trucking... */
  return(1);
}

/* ========================================================================= */
int EndFile(File *f)
{
  if ( f )
  {
    if ( f->fh )
      fclose(f->fh);

    free(f);

    return(0);
  }

  return(0);
}
