#include <stdio.h>

#include "options.h"
#include "bpfparse.h"
#include "version.h"

/* Prototypes (for functions in this file) */
int show_about(void);
int show_help(void);

/* ========================================================================= */
int main ( int argc, char *argv[] )
{
  Options *o;
  RuleSet *r;

  /* Read in command line options */
  if ( NULL == (o = ParseOptions(argc, argv)) )
    return(1);

  /* Verbose let the user know we started */
  if ( o->bVerbose )
    fprintf(stderr, "bp version %s started.\n", VERSION_STRING);
  
  /* Handle some debuggery */
  if ( o->bDebug )
  {
    fprintf(stderr, "Options parsed:\n");
    fprintf(stderr, " bDebug = %d\n", o->bDebug);
    fprintf(stderr, " bAbout = %d\n", o->bAbout);
    fprintf(stderr, " bHelp = %d\n", o->bHelp);

    if ( o->bpffile )
      fprintf(stderr, "  bpffile = \"%s\"\n", o->bpffile);
    else
      fprintf(stderr, "  bpffile = NULL\n");

    if ( o->binfile )
      fprintf(stderr, "  binfile = \"%s\"\n", o->binfile);
    else
      fprintf(stderr, "  binfile = NULL\n");
  }

  /*** Handle all simple options ***/
  if ( o->bAbout )
    return(show_about());
  
  if ( o->bHelp )
    return(show_help());

  /*** Zeroth pass: Read options from the bpf file ***/
  if ( o->bVerbose )
  {
     /* Odds are, the verbose flag will not be set when starting. */
     fprintf(stderr, "Pre-compile pass starting.\n");
  }
  
  if ( ParseBPFOptions(o) )
     return(1);

  /*** First pass: Read in parsed items, without tag resolution ***/
  if ( o->bVerbose )
  {
    fprintf(stderr, "Pre-compile pass complete.\n");
    fprintf(stderr, "First pass compile starting.\n");
  }

  /* Read in the BPF file */
  if (NULL == (r = ParseBPFFile(o)))
    return(1);
  
  /*** Second pass: Resolve all tags ***/
  if ( o->bVerbose )
  {
    fprintf(stderr, "First pass compile complete.\n");
    fprintf(stderr, "Second pass compile starting.\n");
  }

  if ( ResolveTags(r) )
  {
    if ( o->bVerbose )
      fprintf(stderr, "Second pass compile failed.\n");

    return(1);
  }

  /*** Third pass: Read in data ***/
  /*     aka: Resolve the data. */
  if ( o->bVerbose )
  {
    fprintf(stderr, "Second pass compile complete.\n");
    fprintf(stderr, "Third pass compile starting.\n");
  }

  
  /* How:
       Repeatedly rake the linked list for the data. If it is in order*
       then it should only take a single read of the list/file.**

       [* It can be ordered in the app as it was read, but the user may
       have written a crappy file that was not ordered. The alternative
       would be an ordering of the parse points in the app as a dependency
       tree. This is harder than required, so opting for the more brute
       force method as long as it only takes me a few tries - tops.]

       [** Each walk of the list would be accompanied by a reading of the
       file. We MUST read the file because we need the data to resolve
       other items (such as a size to read). So a walk of this list would
       require reading from the data file.]

       It cannot be assumed that a complete read can be accomplished in
       N walks of the list. The "recursion" could continue with thousands
       of dependencies. The only way to do this is:
        - Put all items in a dependency tree and recursively resolve the
	  tree leaf by leaf.
	- Continually walk the list until it is resolved. Make sure that
	  at least one new item is resolved on each list walk.
	- Continually walk the list until it is resolved. Recurse on every
	  found item. This should only take a single walk. It would also
	  require that a link is kept to the dependent pp during the tag
	  resolution phase. (It would have to be really. This would be the
	  best time to make the linkage, and the linkage is required to
	  resolve both data type (for casting) and the data location.)
  */

  /*** Finally ***/
  /* Render the data as per each listed type requires.
     
     This will require that the list be walked in order as it was given
     by the source file. The assumption must be that the user placed
     items in the file as expected to be in the output. */
  
   return(0);
}


/* ========================================================================= */
int show_about(void)
{
  printf("bp - A binary parse tool\n");
  printf("   Version: %s\n", VERSION_STRING);
  printf("   Vera Brittain <vbrittain@vad.gov.uk>\n");
  printf("   William Favorite <wfavorite@tablespace.net>\n");

  fflush(stdout);

  return(0);
}


/* ========================================================================= */
int show_help(void)
{
  printf("bp - A binary parse tool\n");
  printf("   Version: %s\n", VERSION_STRING);

  printf("   Options:\n");
  printf("     -a         Show \"about\" information (and exit).\n");
  printf("     -c         <label>:<value>\n");
  printf("     -e         <label>=<value>\n");
  printf("     -h         Show \"help\" information (and exit).\n");
  printf("     -s         <value>\n");
  printf("     -t         <tag>:<value>\n");
  printf("     -p <file>  Specify the parse file. (Required)\n");
  printf("                [The bpf file is case sensitive]\n");
  printf("     -v         Be verbose\n");
  fflush(stdout);
  return(0);
}
