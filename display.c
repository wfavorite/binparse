#include <stdio.h>

#include "display.h"

/* ========================================================================= */
int print_parsepoint(ParsePoint *pp, Options *o)
{
    if ( pp->print_result )
    {
      /* STUB: Stubbed all to hell. */
      printf("%-20s: %ld\n", pp->label, pp->rdata);

    }

    return(0);
}

/* ========================================================================= */
int DumpResults(RuleSet *rs, Options *o)
{
  ParsePoint *thispp;

  thispp = rs->pplist;
  while ( thispp )
  {
    print_parsepoint(thispp, o);
    
    thispp = thispp->next;
  }

  fflush(stdout);
  return(0);
}
