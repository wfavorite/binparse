#ifndef OPTIONS_H
#define OPTIONS_H

typedef struct Options
{
  int bAbout;
  int bHelp;
  int bDebug;
  int bVerbose;

  char *bpffile;
} Options;

Options *ParseOptions(int argc, char *argv[]);


#endif
