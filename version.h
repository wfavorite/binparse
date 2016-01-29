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
    0.5.0    1/28/16 - Making the bp part of the Makefile 'dominate' after
                       the merge. Merging some of the make build dependencies
                       and formatting.
                     - Added in the options.code (still somewhat minimalistic).
                     - Pulled the support.h/c from the code.
                     - Options parsing now supports the basic switch input
                       and setting the config file executable / magic option.
                     - Integrated (minimally) the enum parsing code.
                     - grep $TUB * | wc -l ----> 80
    0.6.0    1/29/16 - Enum parsing integration is largely complete.
                     - Moved in the Entity parsing. Integration requires a
                       bit of refactoring of code. It is cleaner, but my
                       scalpel looks more like a hatchet.
*/
#define VERSION_STRING "0.6.0"
/*
  Notes:
    - Just wanted to capture this somewhere. I thought it up and think I should
      use it from now on out. It is:
      When completing a simple STUB comment, the STUB text should be removed
      and the comment should remain as a clear denotation of where we are
      doing. When completing a design choice with a block of commentary, then
      the STUB strings should be converted to RESOLVED, CHOSEN & REJECTED tags.

  ToDo:                                                                      !
   [ ] Offset can be 0, but size cannot. This should be validated during one
       of the passes. Probably during the final pass.
   [ ] When ParsePoints are added, the tag identifiers must be compared to
       insure that they are unique. Tag name collisions are not allowed.
   [ ] Will you support "defined"/set values. For example:
       settag hdroffset 16
       This would create a tag called hdroffset that is a hard-set value of
       16 (decimal).
   [ ] Write the must= clause support.
   [Q] pmath "5 6" passes, it should not. Perhaps it should. It sees 5<space>
       and considers 5 a valid number. The 6 would be parsed as a different
       entity / token on the line and fail there. I will keep this here
       until that is tested. But parsing $(pmath "5 6") into 5 is appropriate. 
   [ ] Eliminate redundant error messages. Make parsing errors consistent
       across all fail points.
   [ ] Write parser for (in-bpf-file) command line options. Such as:
       setopt c         <---- Same as -c
       setopt x myarg   <---- Same as -x myarg
   [ ] Create more sample.bpf files - perhaps they should be part of the test
       suite.
   [ ] Setup 3-dot truncation for strings used in error messages. DO NOT
       OVERFLOW THE LINE!
   [ ] No documentation of struct members or ParseOptions() in options.h.
   [ ] Some functions in strlib.h are not documented.
   [ ] The datapoint.* code probably needs to go. It was part of the original
       C++ design.
   [ ] Start working the $stubs throught the code. Currently at 80 across the
       entire project. Clear this todo when it is <= 40.
   [ ] Enum parsing in bpfparse.c::ParseBPFFile() needs to handle the $tubs
       dealing with return values from ParseEnum() as well as changes to 
       ParseEnum() itself in terms of input.
   [ ] Technically.... You should be able to call the executable bpf file with
       command line options thusly: ./mybpf -c mybin
       This means that the args would be:
        argv[1] = "./mybpf"
        argv[2] = "-c"
        argv[3] = "mybin"
       From that, we see the need to parse command line options "out of
       order" so to speak. This means that they are incompatible with the
       getopt() API (as intended - possibly we could advance the pointer
       when we hit a word/non-dash argument).
   [_] Pull out all references to support.c. This was always a temp file that
       would be covered by the proper functions in strlib.h/c.
   [ ] options.c::ParseOptions() should validate the options before returning.
   [ ] The help output has a few different output methods. These are not
       covered in the options struct (with appropriate flags), nor is the
       help output clear on what the options mean. (Specifically: The output
       needs to relate that this option modifies the output format.)
   [ ] Consider removing all the validate_* code in pmath.c. This should all
       be doable in a single pass. Drop the naieve approach and move on. It is
       incorrect to have parsing rules in two different places.
   [ ] Remove the remaining support_c/h.txt files from the project.
   [ ] Remove all unnessary (redundant) error message handling.
   [ ] Hide all the debug messages behind pre-processor directives.
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
   [X] Decide if enum tags can have collisions (with builtins). Decide if
       builtin enums will be supported.
   [X] Put this string into the test framework.
   [W] Rename the binary directory to "bp". WITHDRAWN: The repo is "binparse"
       so, there you go.
   [X] Doh! You never wrote the code to parse an "enum list". That has the
       following syntax (for a 1-byte boolean):
	    { 0 = "no"; default = "yes" }
       This is the point of the EnumList and EnumPair structs at the top of
	    bpfparse.h.
   [X] The lineno is not set.
   [X] The ParsePoint list needs to have a 2nd-pass "compile".
   [X] Establish primary data structures.
   [X] Perhaps it should be bp (bin parse) rather than bd (bin dump).
   [X] Start a strlib (String Library).
   [X] Setup options.h definitions (what we will support).
*/
#endif
