#include <stdio.h>

#include "options.h"
#include "bpfparse.h"
#include "version.h"
#include "binpass.h"
#include "display.h"
#include "eswap.h"

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
   {
      fprintf(stderr, "bp version %s started.\n", VERSION_STRING);
      fprintf(stderr, "Running on %s endian architecture.\n",
              GetEndian() == GE_BIG_ENDIAN ? "big" : "little");
   }

   /* Collapse the first two layers so -a and -h can be processed */
   if ( CollapseOptionLayers(o) )
      return(-1);

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

   /* The last layer was succesfully read. Now collapse the layers. */
   if ( CollapseOptionLayers(o) )
      return(-1);

   /*** First pass: Read in parsed items, without tag resolution ***/
   if ( o->bVerbose )
   {
      fprintf(stderr, "Pre-compile pass complete.\n");
      fprintf(stderr, "  File-set options parsed : %d\n", bpfopt);

      /* Dump the options if in debug mode */
      DbgDumpOptions(o);

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
   printf("   Endianness: %s\n",
              GetEndian() == GE_BIG_ENDIAN ? "Big" : "Little");
   printf("   Credits:\n");
   printf("     Vera Brittain <vbrittain@vad.gov.uk>\n");
   printf("     William Favorite <wfavorite@tablespace.net>\n");

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
   printf("     -e         Swap endianness of the data\n");
   printf("     -f X       Output separator ---> <label>X<value>\n");
   printf("     -h         Show \"help\" information (and exit).\n");
   printf("     -l         Simplified output ---> <value>\n");
   printf("     -t         Use tag instead of label in output ---> <tag>:<value>\n");
   printf("     -v         Be verbose\n");
   printf("     -x         Dump output in Hex (0xdeadbeef)\n");
   printf("     -X         Dump output in Hex (0XDEADBEEF)\n");
   printf("\n");
   printf("    Double dash options:\n");
   printf("      --help/-h           --about/-a          --validate/-c\n");
   printf("      --verbose/-v        --endian_swap/-e    --field_separator/-f\n");
   printf("      --passes/-p         --no_labels/-l      --show_tag/-t\n");
   printf("      --dump_hex/-x       --dump_HEX/-X\n");
   fflush(stdout);
   return(0);
}
