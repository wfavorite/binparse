#include <assert.h>
#include <stdlib.h>

#include "bpdata.h"

int CountParsePoints(RuleSet *rs)
{
   ParsePoint *pp;
   int ctr;

   assert(NULL != rs);

   ctr = 0;
   pp = rs->pplist;
   while(pp)
   {
      ctr++;
      pp = pp->next;
   }

   return(ctr);
}
