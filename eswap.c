#include <assert.h>
#include <stdlib.h>

#include "eswap.h"

void eswap_uint16( uint16_t *val )
{
   uint8_t t;
   uint8_t *a;

   assert(NULL != val);

   a = (uint8_t *)val;

   t = a[0];
   a[0] = a[1];
   a[1] = t;
}

void eswap_int16( int16_t *val )
{
   uint8_t t;
   uint8_t *a;

   assert(NULL != val);

   a = (uint8_t *)val;

   t = a[0];
   a[0] = a[1];
   a[1] = t;
}


void eswap_uint32( uint32_t *val )
{
   uint8_t t;
   uint8_t *a;

   assert(NULL != val);

   a = (uint8_t *)val;

   t = a[0];
   a[0] = a[3];
   a[3] = t;

   t = a[1];
   a[1] = a[2];
   a[2] = t;
}






void eswap_int32( int32_t *val )
{
   uint8_t t;
   uint8_t *a;

   assert(NULL != val);

   a = (uint8_t *)val;

   t = a[0];
   a[0] = a[3];
   a[3] = t;

   t = a[1];
   a[1] = a[2];
   a[2] = t;
}



/* STUBbed for 64 bit */

void eswap_uint64( uint64_t *val )
{
   uint8_t t;
   uint8_t *a;

   assert(NULL != val);

   a = (uint8_t *)val;

   t = a[0];
   a[0] = a[3];
   a[3] = t;

   t = a[1];
   a[1] = a[2];
   a[2] = t;
}


void eswap_int64( int64_t *val )
{
   uint8_t t;
   uint8_t *a;

   assert(NULL != val);

   a = (uint8_t *)val;

   t = a[0];
   a[0] = a[3];
   a[3] = t;

   t = a[1];
   a[1] = a[2];
   a[2] = t;
}



