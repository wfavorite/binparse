#include <stdio.h>

#include "display.h"

/* ========================================================================= */
int print_parsepoint(ParsePoint *pp, Options *o)
{
   char *lhs;

   if ( pp->print_result )
   {
      if ( o->bTagVal )
         lhs = pp->tag;
      else
         lhs = pp->label;

      /* STUB: Handle all these types - properly! */
      switch ( pp->dt )
      {
      case DT_UINT8:
      case DT_INT8:
      case DT_CHAR:
      case DT_UINT16:
      case DT_INT16:
      case DT_UINT32:
      case DT_INT32:
      case DT_UINT64:
      case DT_INT64:
         printf("%-20s%c %ld\n", lhs, o->cFields, pp->rdata);
         break;
      case DT_ZTSTR:
      case DT_FLSTR:
         printf("%-20s%c STUB\n", lhs, o->cFields);
         break;
      case DT_NULL:
      default:
         fprintf(stderr, "ERROR: Data type for \"%s\" was not properly set.\n", pp->tag);
         return(1);
      }

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
