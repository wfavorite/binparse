#ifndef PENUM_H
#define PENUM_H

#include "bpdata.h"
#include "slfile.h"

/* =========================================================================
 * Name: ParseEnum
 * Description: Convert the syntax of an enum string to a data structure
 * Paramaters: The string containing the enum
 *             The line it can be found on (for error messages)
 * Returns: Pointer to an Enum (and its sub-structures)
 * Side Effects: Will allocate memory.
 * Notes: 
 */
Enum *ParseEnum(char *estr, int lineno);


/* =========================================================================
 * Name: IsEnumLine 
 * Description: Used to sniff a line to see if it is (likely) an enum
 * Paramaters: A pointer to the line to be checked
 * Returns: 0 if not an enum line, non-0 if (possibly) an enum line
 * Side Effects: None
 * Notes: The paramater here is a pointer to the line. The line will not
 *        be modified. This funciton only looks for an expected pattern
 *        of an enum entry. The pattern match is very simple, it is not
 *        a complete parsing (checking) of the line. It just "sniffs" the
 *        front of the line to see the required *start* of an enum line.
 *        ParseEnum() will do a *full* parsing/validation.
 */
int IsEnumLine(char *estr);

/* =========================================================================
 * Name: InsertEnum
 * Description: 
 * Paramaters: 
 * Returns: 
 * Side Effects: 
 * Notes: For an enum to be inserted, we could always insert at the top of
 *        the list, but then when lookups are done, we can stop on *first*
 *        match. This would mean that collisions are ok, and that the second
 *        definition wins.
 * 
 *        The behaviour I want is for user-defined enums should NOT allow for
 *        collisions, while users can override builtin enums. So... The user-
 *        defined list will be searched on insert, but not the builtin list.
 *
 *        The algo for looking up enums is: Search the user, then builtin
 *        list until a match is found. Walking off the end of the builtin
 *        list means that a pattern match was not found.
 */
int InsertEnum(RuleSet *rs, Enum *e);

/* =========================================================================
 * Name: ApplyBuiltins
 * Description: Add in a number of builtin enums into the RuleSet
 * Paramaters: The existing RuleSet
 * Returns: 0 on success, non-0 on failure
 * Side Effects: Will allocate memory for new structures and pin them to
 *         the RuleSet.
 * Notes: 
 */
int ApplyBuiltins(RuleSet *rs);

#endif

