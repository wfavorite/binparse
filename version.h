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
                     - Where I am: The Entity parsing has been moved into
                       get_parse_point(). Second pass needs to be modified
                       for this new structure / method.
                     - grep $TUB * | wc -l ----> 78
    0.7.0     2/1/16 - Picked an author.
                     - Moved file operations out of bpfparse.c to slfile.h/c.
                       This was because I wanted file parsing in options.c
                       and wanted to use the same "helper" functionality.
                       Find additional commentary in slfile.h.
                     - Started on file-based options parsing.
                     - Wrote a BNF-ish ruleset for the BPF file format.
                     - Started work on cleanup of get_parse_point().
    0.8.0     2/2/16 - Cleanup and strengthening of code used to parse
                       lines out of the BPF file.
                     - Additional documentation of BPF file format.
                     - Minor dead code removal.
                     - $tub killing.
                     - Created bpdata.c to collect stats.
                     - Started initial foray into parsing the "6th item".
                     - Added line numbers to parsing errors.
                     - Added verbose pass result status (items parsed count).
                     - Fixed nth token retrieval.
                     - grep $TUB * | wc -l ----> 78 (from a prev high of 90)
    0.9.0     2/3/16 - Chugging through enum parsing code fixing new syntax
                       requirements and fixing error messages (making them
                       more consistent with app look and feel).
                     - Some dead code persists in pmath.c. This code was
                       written in two stages, then collapsed into one.
                       The initial (naive) approach was to validate, then
                       to parse. This meant two sets of virtually the same
                       code. Validate as you parse. I am keeping this noise
                       a bit longer, but it will have to go. It is already
                       isolated and not used within that file.
                     - grep $TUB * | wc -l ----> 38
    0.10.0    2/4/16 - Cleaned up options parsing.
                     - More cleanup of pp parsing.
                     - grep $TUB * | wc -l ----> 26
                     - Removed dead code in pmath.c
                     - grep $TUB * | wc -l ----> 17
    0.11.0    2/5/16 - Added support for <directive>= parsing.
                     - Added support for 6th token directives.
                     - Tag resolution "works", but does not resolve nested
                       tags such as "( 2 + mytag )".
                     - Started on "Third Pass" code (in binpass.c). Basic
                       expressions can be resolved. (If I dumped most of the
                       cooler capabilites, this project could be done in
                       hours.)
    0.12.0   2/10/16 - OSX port / code warning cleanup. llvm 7.0.2.
                       clang 700.1.81
                     - Fixed (added) recursive resolution of tags in the
		                 second pass.
    0.13.0   2/11/16 - Added -c (check compile) option / capability.
                     - Work on the data retrieval (3rd pass) code.
    0.14.0   2/12/16 - Basic numerical parsing works
                     - Added support for -p(asses) option for 3rd pass code.
                     - Basic support for printing data. Mostly incomplete at
                       this time.
                     - Code cleanup, comments, documentation, etc...
                     - grep $TUB * | wc -l ----> 25
    0.15.0   2/16/16 - AIX port
                     - Work on endianess swapping. Basic testing complete.
    0.16.0   2/17/16 - Re-work of the display (dump) code.
                     - Cleanup / simplification of endian code.
                     - $TUB elimination.
                     - Code cleanup, basic feature work, refactoring.
                     - Added (initial) support of explicit tags.
    0.17.0   2/18/16 - Band-aids on options parsing. It now works in a 
                       generally consistent way, but should be considered
                       for a re-design.
                     - Fixed some bad pointer stuff with string handling in
                       the display code.
    0.18.0   2/18/16 - Pushed to repo to test new markdown documentation.
                     - Code documentation
                     - Label field width now auto-sizes (unbounded).
                     - Solaris port. Clean compile using gcc. Need to move to
                       the Sun compilers yet. Same for AIX.
                     - Moved to native compilers. (AIX and Solaris)
                     - Fixed various compiler warnings on each OS.
                     - grep $TUB * | wc -l ----> 6
    0.19.0   2/19/16 - Applied the mask= value.
                     - Minor fixes
                     - Working with data types
                     - grep $TUB * | wc -l ----> 2
    0.20.0   2/22/16 - Testing of some of the recent features.
    0.21.0   2/23/16 - Started re-write of options parsing.
             2/24/16 - Continued work on options parsing.
    0.21.1    3/1/16 - Minor change to this file.
    0.22.0   3/17/16 - Cleanup, documentation, and refinements.
                     - Validataion of large value parsing.
                     - Documented double dash options in help().
                     - Testing.
    0.23.0   3/18/16 - Working with edge test cases.
                     - Changed line read code to properly detect buffer
                       overruns.
                     - Fixed bug with "setopt t" not working.
    0.24.0   3/23/16 - Review of the "rdata issue". The code seems ok, but
                       is succeptable to some edge cases. A series of notes
                       about a proposal for v2 has been added here and
                       other parts of the code.
                     - Added explicit tag resolution (a hack to make it
                       work in a more limited sense). Todos were added for
                       version 2 to unwind some dependencies in the tag
                       resolution code.

*/
#define VERSION_STRING "0.24.0"
/*
  Notes:

  ToDo:                                                                      !
   [ ] Many of the "pending done" ToDos really only need testing. Test them.
       I have marked them with "T" for test.
   [T] Test support of settag operator. Specifically tag resolution and tag
       collisions with other types (pp, enum, et). Does the PP insert code
       properly check this list?
   [ ] FreeBSD port.
   [ ] There is a $TU8 in options.c. It is not clear if this is a problem or
       not.
   [ ] Insure that must= is not used with non-numeric data.
   [ ] Write man pages for bp(1) and bpf(5).
   [D] Need to properly differentiate between ' and " in the strlib.
   [D] Test strlib.c::mid_trunc(). It looks like a weak implementation.
   [D] How do you handle exceptions in strlib.c::mid_trunc()?

  ToDo (Version 2):
   [ ] Resolving explicit tags in ResolveTags() uses the
       resolve_entity_tag() API. The problem is that this is written for
       ParsePoint resolution. The error messages use this, as well as some
       statistics keeping. This needs to be somehow changed so that the
       lineno and the (explicit) tag name gets passed.
   [ ] Write check to insure that if a mask= is used, that the data type
       is compatible.
   [ ] The types BPInt and BPUint are disasters. This should be converted
       into a "hybrid" data type that uses uint64 for the distance from
       0, and another int to hold the sign. This removes some of the ugly
       compares and conversions that make this program fragile at the 
       edges.
   [ ] Every option that is settable in the config file, should have an
       inverse value on the command line so that it can be overridden. For
       example: Config file has verbose set, but the command line only has
       the option to set verbose again.
   [ ] Document the --options in options.c::ParseOptions().
   [ ] An option should exist to print in hex (specify hex) in the parse
       point. The suggested syntax is: base=HEX, base=hex, base=dec. This
       would override the global command line or file setting.

  Done:
   [X] rdata should not be used when an unsigned long. (This code works,
       but has some considerable exposure in edge cases - specifically when
       entering large decimal numbers via input in things like must= (as
       parsed by ParseBPUInt()). I have given a pass on this code for now,
       but this data type issue needs to be resolved in v2.0, and I have
       put that fix as a todo in the 2.0 list.)
   [X] The MAX_TAG_LEN define in bpdata.h is used only in penum.c. It needs
       to be utilized in parsing a ParsePoint.
   [X] TagVal (-t) defaults to 1. This should be 0. 
   [X] Make the location of the function description comment consistent.
       Consider creating function comment blocks for *all* functions.
   [X] Test flstr parsing and printing.
   [X] Fill out all the empty function paramater comment blocks.
   [X] The double dash options in options.c is not in the help() output.
   [X] Check where the mask= value is set. (The number is converted from a
       signed value to a unsigned. This should likely always be unsigned.
       or converted to an unsigned without dropping the sign bit.
   [X] The discussion in options.h does not appear to match the code.
       Investigate this and resolve.
   [X] The must= parsing does not parse leading F in uint64. (Not a bug. There
       was a problem in the BPF file. Some cleanup was done to the code as
       part of this effort.)
   [X] Update the markdown doc with more detailed info on the project.
   [X] ./bp kdkdkd segfaults after failing to open the file.
   [X] Zeroth (pre-compile) pass should print options set when in verbose mode.
   [X] Completely re-design the options parsing to have *layers* of options
       (defaults, file-set, and CL-set) that can be read out of order, but
       applied in order. These should be read (out of order), then "flattened"
       where some validation checks can be done. There is another todo in 
       this area that deals with option parsing and using file magic. This
       should be integrated into this design.
   [X] Technically.... You should be able to call the executable bpf file with
       command line options thusly: ./mybpf -c mybin
       This means that the args would be:
        argv[1] = "./mybpf"
        argv[2] = "-c"
        argv[3] = "mybin"
       From that, we see the need to parse command line options "out of
       order" so to speak. This means that they are incompatible with the
       getopt() API (as intended - possibly we could advance the pointer
       when we hit a word/non-dash argument).
   [X] The options parsed in the BPF file override those set on the command
       line. This is reverse of what it *should* be. (Note: At the time of
       this writing, command-line options are not allowed when setting your
       BPF file executable and using file magic to get your bp interperter.)
   [X] "setopt X true" does not work. (Fixed. Not tested.)
   [X] The -e option is not documented in the -h output.
   [X] Write endian_is() function. Put results in verbose output.
   [X] Apply the mask= in bpdata.c. (Search for mask=, there is a $TUB.)
   [X] Write an enum testcase.
   [X] Enums are not printed.
   [X] For now... Find the longest label and use this as the length of the
       default output for all labels. Such as printf("%-Xs : ...) where X
       is the length of the longest label.
   [X] The Sun Studio port errors on compile. Fix this.
   [X] Solaris port.
   [X] Read length of label to determine label length allotment (%<something>s)
   [X] binpass.h does not have a function header definition.
   [X] display.h does not have a function header definition.
   [X] Write a test case that has two different types of strings in it
       (similar to simple.h).
   [X] There is a hole in ParseOptions that lets a config escape without
       paramater checking. Fix this.
   [X] Inserting parse points does not *completely* look for name collisions.
   [X] RuleSet->pass does not appear to have been used anywhere. This was
       designed to denote that the *entire* pass was completed. I am not sure
       that this is required any more.
   [X] Consider removing all the validate_* code in pmath.c. This should all
       be doable in a single pass. Drop the naieve approach and move on. It is
       incorrect to have parsing rules in two different places.
   [X] When printing the error messages in bpfparse.c::resolve_tag(), the
       void pointer in the union is used. How do you know this is good (that
       it points to a string and not another struct)? It may be appropriate
       to write a gettor function, check this before writing, or have a
       solid look at the code and comment the shit out of it.
   [X] Output should be offered in -x (hex).
   [X] Re-indent all source files (I don't have my .emacs file on this
       computer and the indentation is all messed up.)
   [X] Some of the print statements (such as bVerbose) are stdin AND stderr.
   [X] Move InsertEnum() and InsertETag() to bpdata.h/c
   [X] Close the binary file on end of read.
   [X] There is still no support for the "settag" operator.
   [X] Finish the 64bit endian code.
   [X] The help output has a few different output methods. These are not
       covered in the options struct (with appropriate flags), nor is the
       help output clear on what the options mean. (Specifically: The output
       needs to relate that this option modifies the output format.)
   [X] A swap endian feature needs to be added to support different endian
       files.
   [X] Verbose mode should show the reads. It is too easy to mess up the BPF
       file format. Some help would make this more useful for the user.
   [X] You should move app-specific string checking and manipulation to
       an app specific source file. (This is done mostly in bpdata.c.)
   [X] Will you support "defined"/set values. For example:
       settag hdroffset 16
       This would create a tag called hdroffset that is a hard-set value of
       16 (decimal).
       Notes on this issue:
       1. The "settag" operator is inconsistent with how enums are set.
       2. A line beginning with a tagXXX (the enumXXX = method) will be
          indiscernable from parse point lines that can begin with that
          same tag (in the offset field).
       3. "settag" and "setenum" operators seem to make the most sense, and
          are consistent (with each other).
       (This has been "standardized". setenum now works this way. I am removing
       this item because behaviour has been defined and this (todo) is now
       redundant.)
   [X] You should establish size and allocate memory for the data items.
       This should probably be done during the third pass (DT_ZTSTR will
       not be known until it is read).
   [X] Write actual options parsing code (instead of stubing defaults).
   [X] Cleanup comments and structure in bpdata.h.
   [X] Need to check that stated data types and size operators match.
   [X] Setup a test binary file.
   [X] Offset can be 0, but size cannot. This should be validated during one
       of the passes. Probably during the final pass. The offset, nor the
       offset + size cannot extend beyond the end of the file.
   [X] Data types need to be 'compatibility checked' before converting them
       to BPInt data types.
   [X] Data should be 'cached' in the PP so multiple lookups will be faster.
       This means that once the data is properly resolved, it is copied into
       a BPInt for future use.
   [X] RuleSet needs a file descriptor and related 'flags'.
   [X] The function to count explicit tags should be written. The parsing
       support is not complete, but the data structures have been defined.
   [X] Resolve the "assert() $TUB" in bpfparse.c::ResolveTags().
   [X] The tag member of the union in Entity can be a string and a pointer
       to a parse point. What if it points to an explicit tag? The resolution
       will be different - meaning you need a new type to cover this type of
       tag to be resolved (and how to resolve it).
   [X] Tag resolution is not recursive. If a tag is buried in a mathematical
       statement, then the tag resolving algo will miss it. (For right now,
       no test cases have these "nested" tags.)
   [X] When ParsePoints are added, the tag identifiers must be compared to
       insure that they are unique. Tag name collisions are not allowed.
   [X] Time to focus on resolving tags (2nd pass operations).
   [X] No documentation of struct members or ParseOptions() in options.h.
   [X] Write the must= clause support. This is in bpfparse.c::handle_ppopt().
   [X] Re-write the enum definition to have an operator word (setenum).
   [X] Create more sample.bpf files - perhaps they should be part of the test
       suite.
   [X] Eliminate redundant error messages. Make parsing errors consistent
       across all fail points.
   [W] Create a separate todo list for strlib. This will be portable to
       other projects - an "inline object".
   [X] Hide all the debug messages behind pre-processor directives.
   [X] options.c::ParseOptions() should validate the options before returning.
   [W] pmath "5 6" passes, it should not. Perhaps it should. It sees 5<space>
       and considers 5 a valid number. The 6 would be parsed as a different
       entity / token on the line and fail there. I will keep this here
       until that is tested. But parsing $(pmath "5 6") into 5 is appropriate. 
   [X] Verbose mode should show stats at the end of the first (and other)
       pass(es).
   [X] handle_ppopt() does not properly get the line number. Nor does it deal
       with empty strings.
   [X] Some functions in strlib.h are not documented.
   [X] The datapoint.* code probably needs to go. It was part of the original
       C++ design.
   [X] Enum parsing in bpfparse.c::ParseBPFFile() needs to handle the $tubs
       dealing with return values from ParseEnum() as well as changes to 
       ParseEnum() itself in terms of input.
   [X] penum.c::parse_enum_pair() does not have lineno.
   [X] All error messages in penum.c are inconsistent with general app look
       and feel.
   [X] Write parser for (in-bpf-file) command line options. Such as:
       setopt c         <---- Same as -c
       setopt x myarg   <---- Same as -x myarg
   [X] Write the struct mini-comment blocks.
   [X] Finish ParseOptions().
   [X] The calls to ParseEntity() inside get_parse_point() do not have the
       line numbers passed, nor do they have proper exception handling.
   [X] Start working the $stubs throught the code. Currently at 80 across the
       entire project. Clear this todo when it is <= 40.
   [X] Create file parser for .bpf (bin parse format) files.
   [X] Pull out all references to support.c. This was always a temp file that
       would be covered by the proper functions in strlib.h/c.
   [X] You shoulda wrote a isvalidtagchar() function to test each character
       in a tag string. This means that the application rule set can be
       established in a single place. Ideally it would be inline.
   [X] Insert function toumbstones in slfile.h.
   [X] Setup 3-dot truncation for strings used in error messages. DO NOT
       OVERFLOW THE LINE!
   [W] Remove version string from help().
   [X] Move strlib code headers to .h file (not in .c file!).
   [X] The current RuleSet return by ParseBPFFile() is empty. The PPs are
       not inserted into the list. (Way old... long since resolved.)
   [X] Remove all unnessary (redundant) error message handling.
   [X] Work on consistent error messaging. (This is knowingly not a
       definitive todo, but the start of unifying all sources in this
       context. (now that all data structures have merged.))
   [X] Remove the remaining support_c/h.txt files from the project.
   [X] The copy_out_nth_token() code (most likely) does not properly parse
       (mete out) the tokens. Add the proposed algo to fix this.
   [X] Put names in about().
   [X] Document the BPF file format somewhere - at least in text. Perhaps
       write a mBNF ruleset for the file.
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
