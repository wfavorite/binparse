#ifndef OPTIONS_H
#define OPTIONS_H

typedef struct Options
{
  int bAbout;
  int bHelp;
  int bDebug;
  int bVerbose;

  char *bpffile;
  char *binfile;
} Options;


/* =========================================================================
 * Name: 
 * Description: 
 * Paramaters: 
 * Returns: 
 * Side Effects: 
 * Notes: 
 */
Options *ParseOptions(int argc, char *argv[]);

/* =========================================================================
 * Name: 
 * Description: 
 * Paramaters: 
 * Returns: 
 * Side Effects: 
 * Notes: This may have been included in bpfparse.h/c. But since it is just
 *        a matter of simple string pattern matching, this does not really
 *        have much to do with parsing that file (in full). This might have
 *        even just been called from ParseOptions() above. This does draw
 *        some light on it, and it has a cenotaph in bpfparse.h.
 *
 *        This means that 
 */
int ParseBPFOptions(Options *o);

#endif
