#ifndef STRLIB_H
#define STRLIB_H

#include <stdint.h>

char *mkstring(const char *input);
char *nc_mkstring(const char *input);

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
