#ifndef ESWAP_H
#define ESWAP_H

#include <stdint.h>

/* These jump, use pointers, and are really designed for this app.

   Ideally, one should look for a better source to steal from if they are
   looking here.

   These are called (potentially) for each parse point, and are not used
   in anything resembling tight performant code.
*/

void eswap_uint16( uint16_t *val );
void eswap_int16( int16_t *val );

void eswap_uint32( uint32_t *val );
void eswap_int32( int32_t *val );

void eswap_uint64( uint64_t *val );
void eswap_int64( int64_t *val );

#endif
