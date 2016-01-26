/*
  This is a version file for PART of the code. It is designed to be folded
  into a regular version.h file once the two code bases are merged. I will
  keep a modified version string (that will not be used in the VERSION_STRING
  macro in a production release - only in the test framework tools.

  Version History:

  PREMERGE.1  1/22/16 - First versioned release
  PREMERGE.2  1/26/16 - Cloned from github.com


*/
#define VERSION_STRING "PREMERGE.2"
/*
  ToDos:
  [ ] Consider removing all the validate_* code in pmath.c. This should all
      be doable in a single pass. Drop the naieve approach and move on. It is
      incorrect to have parsing rules in two different places.
  [ ] Remove all unnessary (redundant) error message handling.
  [ ] Hide all the debug messages behind pre-processor directives.
  [ ] Put this string into the test framework.

  Done:

*/
