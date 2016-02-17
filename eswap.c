#include <assert.h>
#include <stdlib.h>

#include "eswap.h"


void eswap_16( void *val )
{
   uint8_t t;
   uint8_t *a;

   assert(NULL != val);

   a = (uint8_t *)val;

   t = a[0];
   a[0] = a[1];
   a[1] = t;
}



void eswap_32( void *val )
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



void eswap_64( void *val )
{
   uint8_t t;
   uint8_t *a;

   assert(NULL != val);

   a = (uint8_t *)val;

   t = a[0];
   a[0] = a[7];
   a[7] = t;

   t = a[1];
   a[1] = a[6];
   a[6] = t;

   t = a[2];
   a[2] = a[5];
   a[5] = t;

   t = a[3];
   a[3] = a[4];
   a[4] = t;
}










#ifdef STUB_REMOVE

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




#endif
