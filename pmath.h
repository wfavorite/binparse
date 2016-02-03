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

#endif


