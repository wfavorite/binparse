#ifndef OPTIONS_H
#define OPTIONS_H

/* I am making a design decision here.
   It is primarily about effeciency in the development effort, and how much
   time should be spent chasing more expensive and esoteric features. The
   general rule I have is - If it takes longer to explain than code, then
   just code it. Here I choose to explain.

   The problem is two-fold:

   1. Options parsed in the file *override* those on the command line. This
      is generally inconsistent with how this nomrally works.

   2. You cannot pass command line options when using interpreter magic
      because the <unistd> getopt() will not allow parsing flags after
      strings. (The magic approach leaves the args as follows: arg0 is the
      binary (bp), arg1 is the executable text file with the magic in the
      header, argN are the options you pass after the invocation of the
      magic'd file.)

   The fix is brutal and efficient:

   When you are using a magic file, pass options by the file. When you are
   using the command line, use command line options, and be aware of any 
   setopt options in the file.

   Why?:

   Because it is much easier to code. What would have been required to solve
   these problems:

   1. Parse all command line options, save the values to a staging area.
      Parse all file options (using the file supplied on the command line).
      Apply file options. Apply command line options.

   2. Ditch getopt() (Somthing that I have traditionally been in favor of.),
      and write your own parsing code that does not care about order (except
      for the ordering of the filename strings).

   If I see some acceptance of the tool, perhaps I will invest in this area.
   Until then, I would rather invest the coding time in areas will more
   impact on the usability of the product.
*/

#define DEFAULT_MAX_PASSES 5

/* ========================================================================= */
/* The Options struct.
   This holds all command-line and file-supplied options for the app. This
   gets passed to any function that acts differently according to user
   dictates.
*/
#define OUTPUT_DEC     0
#define OUTPUT_HEX_UC  1
#define OUTPUT_HEX_LC  2
#define OUTPUT_DEFAULT OUTPUT_DEC
#ifdef STUB_REMOVE
typedef struct Options
{
   /* Immediate exit options */
   int bAbout;           /* Show about. Exit                                 */
   int bHelp;            /* Show help. Exit.                                 */

   /* General behaviour modification options */
   int bDebug;           /* Turn on diag messages                            */
   int bVerbose;         /* Show some extra verbosity while working.         */
   int bValidate;        /* Validate (stop at third stage of compile)        */
   int iPasses;          /* The number of passes to attempt on data retrieval*/
   int bESwap;           /* To endian swap or not                            */

   /* Output-specific options */
   int bTagVal;          /* Use the tag name, not the label for output       */
   char cFields;
   int bShowLabel;
   int eDumpHex;         /* Uses OUTPUT_* macros                             */

   /* The files */
   char *bpffile;
   char *binfile;

} Options;
#endif







/* ========================================================================= */
/* Option sources */
#define OS_DEFAULT 0
#define OS_CMDLINE 1
#define OS_CFGFILE 2

/* Option keys (potential options) */
#define OK_VERBOSE  1
#define OK_ABOUT    2
#define OK_HELP     3
#define OK_DEBUG    4
#define OK_VALIDATE 5
#define OK_PASSES   6
#define OK_ESWAP    7
#define OK_TAGVAL   8
#define OK_CFIELD   9
#define OK_SHOWLBL  10
#define OK_DMPHEXUC 11
#define OK_DMPHEXLC 11
#define OK_ERROR    12

/* ========================================================================= */
typedef struct Option
{
   int key;
   int value;

   struct Option *next;
} Option;

/* ========================================================================= */
typedef struct Options
{
   int collapsed;        /* Values have been collapsed into the base struct  */
   /* Immediate exit options */
   int bAbout;           /* Show about. Exit                                 */
   int bHelp;            /* Show help. Exit.                                 */

   /* General behaviour modification options */
   int bDebug;           /* Turn on diag messages                            */
   int bVerbose;         /* Show some extra verbosity while working.         */
   int bValidate;        /* Validate (stop at third stage of compile)        */
   int iPasses;          /* The number of passes to attempt on data retrieval*/
   int bESwap;           /* To endian swap or not                            */

   /* Output-specific options */
   int bTagVal;          /* Use the tag name, not the label for output       */
   char cFields;
   int bShowLabel;
   int eDumpHex;         /* Uses OUTPUT_* macros                             */

   struct Option *defaults;
   struct Option *cmdline;
   struct Option *cfgfile;

   /* The files */
   char *bpffile;
   char *binfile;

} Options;










/* =========================================================================
 * Name: ParseOptions
 * Description: Read command line options into an Options struct
 * Paramaters: Everything from main()
 * Returns: Options struct on success, NULL on failure
 * Side Effects: Allocates memory
 * Notes: This is the first thing called in main().
 */
Options *ParseOptions(int argc, char *argv[]);

/* =========================================================================
 * Name: ParseBPFOptions
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
 *        This means that options are parsed in "reverse" order of user
 *        expectations. (See the discussion at the top of this file.)
 */
int ParseBPFOptions(Options *o);

/* Basic sniffing for setopt line */
int IsSetOpt(char *line);

#endif





