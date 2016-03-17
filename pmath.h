#ifndef PMATH_H
#define PMATH_H

#include "bpdata.h"
 
/* =========================================================================
 * Name: ParseEntity
 * Description: Convert the string containing an entity to a Entity structure
 * Paramaters: The string containing the entity (representation)
 *             The line it is on (for meaningful error messages)
 * Returns: Pointer to Entity on success, NULL on failure
 * Side Effects: Will allocate memory
 * Notes: 
 */
Entity *ParseEntity(char *str, int lineno);

/* =========================================================================
 * Name: DBG_dump_entity
 * Description: Used to dump the Entity structure for debug purposes
 * Paramaters: 
 * Returns: 
 * Side Effects: 
 * Notes: 
 */
void DBG_dump_entity(int r, Entity *e);

/* =========================================================================
 * Name: IsETagLine
 * Description: "Sniff" the line to see if this is a "settag" line
 * Paramaters: The line to sniff
 * Returns: non-0 if settag line; 0 if not.
 * Side Effects: None
 * Notes: This is a loose pattern match through the line for the settag 
 *        operator.
 */
int IsETagLine(char *estr);

/* =========================================================================
 * Name: ParseETag
 * Description: Parse an explicit tag (including the entity) from a string
 * Paramaters: String from which to parse the tag
 *             The line number it was on (for error message purposes)
 * Returns: Pointer to an ExplicitTag on success, NULL on failure
 * Side Effects: Will allocate memory
 * Notes: 
 */
ExplicitTag *ParseETag(char *estr, int lineno);


#endif


