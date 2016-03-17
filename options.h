#ifndef OPTIONS_H
#define OPTIONS_H

/* ========================================================================= */
/* Options parsing - how it works

   I have turned the options into *individual* options items. Some may be set
   and others may be skipped. These live in layers that are then "collapsed".

   For example:

   1. Set default options (all of them)
   2. Read file options (most will not be set)
   3. Read command line options (most will not be set)

   The trick is reading these out of order, and then collapsing them in such
   a way to not cause incompatibiliy and cause unnessary alarm.

   This is done by putting options in linked lists of "generic" options.
   When every level has been parsed, they are all collapsed to one set of
   options (in struct Options).
*/



/* ========================================================================= */
/* The DEFAULT_MAX_PASSES value determines the number of "rakes" we make
   as we rake/pass/parse the data. In short, it is a maximum measure of
   the number of iterations through the data trying to resolve all the
   data points. */
#define DEFAULT_MAX_PASSES 5

/* ========================================================================= */
/* The Options struct.
   This holds all command-line and file-supplied options for the app. This
   gets passed to any function that acts differently according to user
   dictates.
*/

/* Defines for the eDumpHex value. This single var covers two switches
   so the code for this is somewhat different. */
#define OUTPUT_DEC     0
#define OUTPUT_HEX_UC  1
#define OUTPUT_HEX_LC  2
#define OUTPUT_DEFAULT OUTPUT_DEC

/* ========================================================================= */
/* Option sources */
#define OS_DEFAULT 0
#define OS_CMDLINE 1
#define OS_CFGFILE 2

/* Option keys (potential options) 
   These should be *sequential* with no gaps. They are processed in order
   in apply_opt_layer(). */
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
#define OK_DMPHEXLC 12
#define OK_ERROR    13
#define OK_STARTKEY OK_VERBOSE
#define OK_LASTKEY  OK_ERROR

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


int CollapseOptionLayers(Options *o);


/* STUB: I reserve the right to rename and mvoe this funciton later */
void DbgDumpOptions(Options *o);

#endif





