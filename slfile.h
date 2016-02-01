#ifndef SLFILE_H
#define SLFILE_H

#include <stdio.h>

/* What the heck is slfile, and why does it have its own source file?

   - The struct File and related functions are really a means to simplify
     opening and pulling lines from files. (Not the it is really all that
     difficult. I just wanted to move some of the file noise out of the
     more technical source. This code is by no means some kind of
     revolutionary uber-class of file oprations. I just wanted to make
     things more readable.
   - This was in in bpfparse.h/c, but I moved it out because of two reasons:
     1. bpfparse.c was large.
     2. I wanted to use this for parsing options (and I had put that
        functionality in options.h/c).
   - "slfile" stands for single line file (meaning: you are pulling by
     individual single lines). This is really because I did not want a
     file named File.c/h and defines that matched. It seemed too likely
     to collide somewhere. The Silly Little pre-pendage fixes that.
*/


#define MAX_LINE_LEN 1024

typedef struct File
{
  FILE *fh;      /* If not NULL, then it is open */
  int lineno;
  int readerr;
  int eof;       /* Set when eof is reached      */
  char line[MAX_LINE_LEN + 4]; /* 4 keeps me aligned */
} File;



File *NewFile(char *filename);

int NextLine(File *f);

int EndFile(File *f);




#endif
