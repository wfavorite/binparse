#include <assert.h>
#include <stdlib.h>

#include "bpdata.h"

/* ========================================================================= */
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

/* ========================================================================= */
int CountParsedEnums(RuleSet *rs)
{
   Enum *e;
   int ctr;

   assert(NULL != rs);

   ctr = 0;
   e = rs->elist;
   while(e)
   {
      ctr++;
      e = e->next;
   }

   return(ctr);
}

/* ========================================================================= */
int CountBuiltinEnums(RuleSet *rs)
{
   Enum *e;
   int ctr;

   assert(NULL != rs);

   ctr = 0;
   e = rs->belist;
   while(e)
   {
      ctr++;
      e = e->next;
   }

   return(ctr);
}
