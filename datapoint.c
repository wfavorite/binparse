#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "datapoint.h"

ParsePoint *NewParsePoint(unsigned long offsetin,
                          unsigned long sizein,
                          char *labelin)
{
   ParsePoint *pp;

   /* STUB: Error here --> printf here */
   if (NULL == (pp = (ParsePoint *)malloc(sizeof(ParsePoint))))
      return(NULL);

   pp->offset = offsetin;
   pp->size = sizein;
   pp->label = labelin;
   if ( NULL == (pp->data = malloc(sizein)) )
      return(NULL);

   return(pp);
}

int CopyInData(ParsePoint *pp, void *dataptr)
{
   memcpy(pp->data, dataptr, pp->size);
   return(0);
}

/*
void *ParsePoint::GetRawPointer(void)
{
   return(data);
}


template<typename T>
T ParsePoint::CopyOutDataType(T *out)
{
   *out = (T)*data;
   return(*out);
};
*/
