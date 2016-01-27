#ifndef VERSION_H
#define VERSION_H
/*
  Version History:
    0.1.0   12/21/15 - Conversion from the c++ version. First versioned copy
                       anywhere.
                     - Primarily about laying out the data structures and the
                       very top of top-down design at this point.
                     - Compiles, runs.
    0.2.0   12/31/15 - Continued work.
                     - Added supporting files/code.
    0.3.0     1/1/16 - First pass parsing (mostly) complete.
                     - Second pass parsing (partially) complete.
                     - Command line options roughed out (but not implemented).
 --------------------- Items merged in from parallel effort
 PREMERGE.1  1/22/16 - First versioned release
 PREMERGE.2  1/26/16 - Cloned from github.com
 --------------------- END
    0.4.0    1/26/16 - The merge, updates to this file
*/
#define VERSION_STRING "0.4.0"
/*
  Notes:
    - Just wanted to capture this somewhere. I thought it up and think I should
      use it from now on out. It is:
      When completing a simple STUB comment, the STUB text should be removed
      and the comment should remain as a clear denotation of where we are
      doing. When completing a design choice with a block of commentary, then
      the STUB strings should be converted to RESOLVED, CHOSEN & REJECTED tags.

  ToDo:                                                                      !
   [ ] Consider removing all the validate_* code in pmath.c. This should all
       be doable in a single pass. Drop the naieve approach and move on. It is
       incorrect to have parsing rules in two different places.
   [ ] Remove all unnessary (redundant) error message handling.
   [ ] Hide all the debug messages behind pre-processor directives.
   [ ] Put this string into the test framework.
   [ ] Doh! You never wrote the code to parse an "enum list". That has the
       following syntax (for a 1-byte boolean):
	    { 0 = "no"; default = "yes" }
       This is the point of the EnumList and EnumPair structs at the top of
	    bpfparse.h.
   [ ] Rename the binary directory to "bp".
   [ ] Resolve the "assert() $TUB" in bpfparse.c::ResolveTags().
   [ ] Document the BPF file format somewhere - at least in text. Perhaps
       write a mBNF ruleset for the file.
   [ ] Make the location of the function description comment consistent.
       Consider creating function comment blocks for *all* functions.
   [ ] You should establish size and allocate memory for the data items.
       This should probably be done during the third pass (DT_ZTSTR will
       not be known until it is read).
   [ ] The current RuleSet return by ParseBPFFile() is empty. The PPs are
       not inserted into the list.
   [ ] Create a separate todo list for strlib. This will be portable to
       other projects - an "inline object".
   [ ] Fill out all the empty function paramater comment blocks.
   [ ] Write the struct mini-comment blocks.
   [_] Create file parser for .bpf (bin parse format) files.
   [ ] Put names in about().
   [ ] Remove version string from help().
   [ ] Write man pages for bp(1) and bpf(5).
   [ ] Need to properly differentiate between ' and " in the strlib.
   [ ] Test strlib.c::mid_trunc(). It looks like a weak implementation.
   [ ] Move strlib code headers to .h file (not in .c file!).
   [ ] Write actual options parsing code (instead of stubing defaults).
   [ ] How do you handle exceptions in strlib.c::mid_trunc()?
  Done:
   [X] The lineno is not set.
   [X] The ParsePoint list needs to have a 2nd-pass "compile".
   [X] Establish primary data structures.
   [X] Perhaps it should be bp (bin parse) rather than bd (bin dump).
   [X] Start a strlib (String Library).
   [X] Setup options.h definitions (what we will support).
*/
#endif
