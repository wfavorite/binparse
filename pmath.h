#ifndef PMATH_H
#define PMATH_H

#include "bpdata.h"
 
/* =========================================================================
 * Name: ParseEntity
 * Description: 
 * Paramaters: 
 * Returns: 
 * Side Effects: 
 * Notes: 
 */
Entity *ParseEntity(char *str, int lineno);

/* =========================================================================
 * Name: DBG_dump_entity
 * Description: 
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
 * Name: 
 * Description: 
 * Paramaters: 
 * Returns: 
 * Side Effects: 
 * Notes: 
 */
ExplicitTag *ParseETag(char *estr, int lineno);


#endif


