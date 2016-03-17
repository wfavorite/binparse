#ifndef ESWAP_H
#define ESWAP_H

#include <stdint.h>

/* ========================================================================= */
/* These jump, use pointers, and are really designed for this app.

   Ideally, one should look for a better source to steal from if they are
   looking here.

   These are called (potentially) for each parse point, and are not used
   in anything resembling tight performant code.
*/
void eswap_16( void *val );
void eswap_32( void *val );
void eswap_64( void *val );

/* ========================================================================= */
/* This is just too self-explanatory */
#define GE_BIG_ENDIAN    0
#define GE_LITTLE_ENDIAN 1
int GetEndian(void);

#endif
