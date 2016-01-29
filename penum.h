#ifndef PDNUM_H
#define PENUM_H

#include "bpdata.h"

/* =========================================================================
 * Name: ParseEnum
 * Description: 
 * Paramaters: 
 * Returns: 
 * Side Effects: 
 * Notes: 
 */
Enum *ParseEnum(char *estr);


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
 * Name: 
 * Description: 
 * Paramaters: 
 * Returns: 
 * Side Effects: 
 * Notes: 
 */
int ApplyBuiltins(RuleSet *rs);

#endif






/*                               STUB REMOVE                                 */
/* VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV */


#ifdef STUB_REMOVE
/* STUB: This is not a valid define name. Use what is set elsewhere. */
#define MAX_TAG_LEN 20

/* STUB: This is proposed. Needs to be properly evaluated */
#define MAX_EVP_NAME_LEN 16


/* STUB: See formatting used in pmath.h. Utilize that here. */

typedef int EVType;

typedef struct envp
{
   EVType value;
   char *name;

   struct envp *next; /* Used to create a linked list... but also to 
                         flag the type of envp this is:
                           (next = NULL) ---> normal
                           (next = this) ---> default value
                      */
} ENVP;


typedef struct enumbase
{
   char *raw;
   char *tag; /* STUB: This does not need a malloc() could be static */
   struct envp *defval;

   struct envp *elist;
} Enum;
#endif
