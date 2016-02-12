#include <stdio.h>

#include "options.h"
#include "bpfparse.h"
#include "version.h"
#include "binpass.h"
#include "display.h"

/* Prototypes (for functions in this file) */
int show_about(void);
int show_help(void);

/* ========================================================================= */
int main ( int argc, char *argv[] )
{
  Options *o;
  RuleSet *r;
  int bpfopt;

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
  
  if ( -1 == ( bpfopt = ParseBPFOptions(o) ) )
     return(1);

  /*** First pass: Read in parsed items, without tag resolution ***/
  if ( o->bVerbose )
  {
    fprintf(stderr, "Pre-compile pass complete.\n");
    fprintf(stderr, "  File-set options parsed : %d\n", bpfopt);
    fprintf(stderr, "First pass compile starting.\n");
  }

  /* Read in the BPF file */
  if (NULL == (r = ParseBPFFile(o)))
    return(1);
  
  /*** Second pass: Resolve all tags ***/
  if ( o->bVerbose )
  {
    fprintf(stderr, "First pass compile complete.\n");
    fprintf(stderr, "  Decoded parse points    : %d\n", CountParsePoints(r));
    fprintf(stderr, "  Decoded enums           : %d\n", CountParsedEnums(r));
    fprintf(stderr, "  Builtin enums           : %d\n", CountBuiltinEnums(r));
    fprintf(stderr, "  Explicit tags           : %d\n", CountExplicitTags(r));
    fprintf(stderr, "Second pass compile starting.\n");
  }

  if ( ResolveTags(r, o) )
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
  }

  if ( o->bValidate )
  {
    if ( o->bVerbose )
      fprintf(stderr, "BPF file validation complete.\n");

    printf("BPF file passed validation (parsing and tag resolution).\n");
    fflush(stdout);

    return(0);
  }

  if ( o->bVerbose )
  {
    fprintf(stderr, "Third pass compile starting.\n");
  }


  /* This is the point were data is retrieved / resolved */
  if ( ResolveData(r, o) )
    return(1);
  
  if ( o->bVerbose )
  {
    fprintf(stderr, "Third pass compile complete.\n");
  }

  /*** Finally ***/
  /* Render the data as per each listed type requires. */
  DumpResults(r, o);
  
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
  printf("   Usage: bp -a | -h | <options> <bpf_file> [bin_file]\n");
  printf("   Options:\n");
  printf("     -a         Show \"about\" information (and exit).\n");
  printf("     -c         Validate the BPF file (stop after 2nd stage compile).\n");
  printf("*    -C         <label>:<value>\n");
  printf("*    -e         <label>=<value>\n");
  printf("     -h         Show \"help\" information (and exit).\n");
  printf("*    -s         <value>\n");
  printf("*    -t         <tag>:<value>\n");
  printf("     -v         Be verbose\n");
  fflush(stdout);
  return(0);
}
