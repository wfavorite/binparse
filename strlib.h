#ifndef STRLIB_H
#define STRLIB_H

#include <stdint.h>

/* =========================================================================
 * Name: three_dot_trunc
 * Desc: Truncate a string with three dots.
 * Params: char *   String to fill
 *         int      Length (including term char) of fill string
 *         char *   Source string that is likely longer than dest string
 * Returns: Pointer to fill string (so it can be used inline)
 * Side Effects: Modifies fill string
 * Notes: fill_len must be >= 3! It will error silently.
 */
char *three_dot_trunc(char *fill, int fill_len, char *source);


/* =========================================================================
 * Name: mkstring
 * Desc: Create a malloc()'d string given string input
 * Params:
 * Returns:
 * Side Effects:
 * Notes: This will NOT print an error message. It is the rare occasion
 *        where I do not follow the 'error at the point of failure' rule.
 *        This is because it can be used while stdout/stderr are closed
 *        and the program may not have a terminal to print to.
 *
 *        So... CHECK THE RETURN VALUE (even though the possibility of
 *        failure is remote), and PRINT YOUR OWN ERROR MESSAGES (however
 *        your app handles that).
 *
 *        See (no check) nc_mkstring() as an alternative.
 */
char *mkstring(const char *input);

/* =========================================================================
 * Name: nc_mkstring   -----> [no check mkstring]
 * Desc: A mkstring() that you do not need to check return values 
 * Params:
 * Returns: A valid result, or exit()
 * Side Effects: May exit()
 * Notes: This is the same as mkstring(), but it prints to stderr() and
 *        exit()s on error. This is designed for use in simple command line
 *        utilities or daemons *before* they have backgrounded. There is no
 *        need to check the return value, because if it returned, then the
 *        malloc() was successful.
 *
 *        The thinking here is that we will be allocating strings typically
 *        early in a small utilities life. If you cannot malloc() there, (or
 *        *anywhere* for that matter) then you have some larger issues. So
 *        the odds of failure are low, and if you fail here, the most
 *        appropriate thing to do is immediately exit and return memory to
 *        the system.
 */
char *nc_mkstring(const char *input);


/* =========================================================================
 * Name: is_valid_tag_char
 * Desc: Is the char input a valid char in a tag name?
 * Params:
 * Returns: 1 if valid, 0 if not
 * Side Effects: None
 * Notes: This is about establishing an application consistent standard
 *        of how a particular string type should adhere to.
 *
 *        It may be more appropriate to put this in some sort of "language
 *        helper" section of the code.
 */
int is_valid_tag_char(char x);


/* STUB: Where is my header? */
char *mid_trunc(char *str, const char *input, unsigned long len);

/* =========================================================================
 * Name: hash_trunc
 * Desc: Truncate the string after the first (valid) '#' char
 * Params: Input string to be modified
 * Returns: non-zero if input string was modified
 * Side Effects: Will modify the input string
 * Notes: A reasonable attempt is made to insure that the # is not inside
 *        of a quotation. The " is handled fairly well... but the ' case
 *        could be more complex. This function does not handle all edge
 *        cases. Check the integrity of your strings elsewhere.
 */
int hash_trunc(char *istr);

/* =========================================================================
 * Name: leadingwst
 * Desc: Leading White Space Truncate
 * Params: Input string to be "modified"
 * Returns: Location in string of first non-space char
 *          If input NULL, then return NULL.
 *          If input truncated, then beginning of string.
 * Side Effects: Will move your pointer in the string (as return val).
 * Notes: This moves the pointer up to the first non-white space char. It
 *        does not modify the string itself.
 */
char *leadingwst(char *istr);

/* =========================================================================
 * Name: chomp
 * Desc: Chomp() off the EOL (NL or LF)
 * Params: Input string to be modified
 * Returns: Address of input (including NULL input)
 * Side Effects: Will modify the input string
 * Notes: This moves the pointer up to the first non-white space char. It
 *        does not modify the string itself.
 */
char *chomp(char *istr);

/* =========================================================================
 * Name: ishex
 * Desc: True if the input is a hex representation of a number
 * Params: Input string to check
 * Returns: true if string has hex value in it
 * Side Effects: None
 * Notes: Leading and trailing WS is supported.
 */
int ishex(char *istr);

/* =========================================================================
 * Name: ispnumeric
 * Desc: True if the input is positive numeric 
 * Params: Input string to inspect
 * Returns: non-zero if value is positive numeric
 * Side Effects: None
 * Notes: Negative numbers (ie: -7) are false. false == ispnumeric("-7");
 *        Leading and trailing WS is supported.
 */
int ispnumeric(char *istr);

/* =========================================================================
 * Name: nth_token_location
 * Desc: Return a pointer to the nth token in the string
 * Params: Input string to pull from, token you want
 * Returns: Pointer to token location. NULL if no token.
 * Side Effects: None
 * Notes: Input of <= 0 for n will give the address of the input string
 *        
 */
char *nth_token_location(char *istr, int n);

/* =========================================================================
 * Name: copy_out_nth_token
 * Desc: Copy out the nth token
 * Params: 
 * Returns: 
 * Side Effects: 
 * Notes: Quotes will not be included in quoted strings
 *        
 */
int copy_out_nth_token(char *tostr, unsigned long len, char *istr, int n);

/* =========================================================================
 * Name: isanynum
 * Desc: Is the string a numeric (dec or hex)
 * Params: String to be inspected
 * Returns: non-zero if numeric, zero otherwise
 * Side Effects: none
 * Notes: Will ignore leading white space, will terminate on trailing ws.
 *        
 */
int isanynum(char *str);

/* =========================================================================
 * Name: eat_ws
 * Desc: Move the pointer up if it is sitting on white space in a string.
 * Params: Pointer to a pointer to (aka: handle of) a string
 * Returns: void
 * Side Effects: This will move the pointer up to the first non-ws char.
 * Notes: This function was fairly specific to a way of walking a string.
 *        While the pointer is on a space or a tab, this will move the
 *        pointer forward until it hits a non-ws char. 
 *        Call it thusly:
 *
 *        char *mystr;
 *        
 *        mystr = mkstring("  Hell of a world!\n");
 *
 *        [mystr now points to a leading space]
 *
 *        eat_ws(&mystr);
 *
 *        [mystr now starts at "H(ell of a world!\n)"]
 */
void eat_ws(char **shand);

/* =========================================================================
 * Name: str_to_uint32t
 * Desc: Convert whatever is in the string (hex/dec) to unsigned long
 * Params: unsigned long to fill (as reference)
 *         String to be read
 * Returns: 0 on success, non-0 on failure
 * Side Effects: Will modify location of ul on success.
 * Notes: Will ignore leading white space, will terminate on trailing ws.
 *        Will parse dec and hex. 
 *        If the decimal is negative, return non-zero (failure).
 *        If an overflow, return non-zero (failure).
 *        This is a 32bit ul.
 */
int str_to_uint32t(uint32_t *rval, char *str);

#endif
