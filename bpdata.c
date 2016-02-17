#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "bpdata.h"
#include "strlib.h"

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

/* ========================================================================= */
int CountExplicitTags(RuleSet *rs)
{
   ExplicitTag *thiset;
   int i;

   i = 0;
   thiset = rs->etlist;
   while ( thiset )
   {
      i++;
      thiset = thiset->next;
   }

   return(i);
}

/* ========================================================================= */
int SetPPDataResolved(ParsePoint *pp, int flag)
{
   pp->data_resolved |= flag;

   return(pp->data_resolved);
}

/* ========================================================================= */
int IsPPDataResolved(ParsePoint *pp, int flag)
{
   if ( flag == ( pp->data_resolved & flag ) )
      return(1);

   return(0);
}

/* ========================================================================= */
int SetBPIntFromVoid(ParsePoint *pp)
{
   switch ( pp->dt )
   {
   case DT_INT8:
      pp->rdata = (BPInt)(*((int8_t *)pp->data));
      break;
   case DT_CHAR:
   case DT_UINT8:
      pp->rdata = (BPInt)(*((int8_t *)pp->data));
      break;
   case DT_UINT16:
      pp->rdata = (BPInt)(*((uint16_t *)pp->data));
      break;
   case DT_INT16:
      pp->rdata = (BPInt)(*((int16_t *)pp->data));
      break;
   case DT_UINT32:
      pp->rdata = (BPInt)(*((uint32_t *)pp->data));
      break;
   case DT_INT32:
      pp->rdata = (BPInt)(*((int32_t *)pp->data));
      break;
   case DT_UINT64:
      pp->rdata = (BPInt)(*((uint64_t *)pp->data));
      break;
   case DT_INT64:
      pp->rdata = (BPInt)(*((int64_t *)pp->data));
      break;
   case DT_ZTSTR:
   case DT_FLSTR:
      break;
   case DT_NULL:
   default:
      /* Likely impossible to reach. (This is really only called
         in one place, and this is checked beforehand.) */
      return(1);
   }

   /* STUB: Apply the mask= here */
  
   return(0);
}

/* ========================================================================= */
int ParseBPInt(BPInt *val, char *str)
{
   int isneg = 0;

   /* Walk off leading white space */
   eat_ws(&str);

   if ( *str == '-' )
   {
      isneg = 1;
      str++; /* Move off the '-' char */

      /* Walk off leading white space. Note: This could be an if(), but
         my fear is the extreme edge case of two spaces from the neg to the
         actual number. Technically any amount of white space should be fair
         but I am being somewhat limiting here. */
      eat_ws(&str);

      *val = 0;
      while (( *str >= '0' ) && ( *str <= '9' ))
      {
         *val *= 10;
         *val += ( *str - '0' );
         str++;
      }

      /* Apply the negative sign */
      if ( isneg )
         *val *= -1;

      eat_ws(&str);

      switch ( *str )
      {
         /* This is end of string */
      case 0:
         /* Other terminating conditions (various maths) */
      case ')':
      case '+':
      case '-':
      case '*':
      case '/':
         /* Syntax */
      case ';':
      case ':':
         /* Space terminated */
      case ' ':
      case '\t':
         return(0);
         break;
      default:
         return(1);
         break;
      }
   }

   if ( *str == '0' )
   {
      *val = 0;

      /* Leading zero of a numeric or hex notation */
      str++;
      if (( *str == 'x' ) || ( *str == 'X' ))
      {
         str++;

         while (( *str != ' ' ) && ( *str != '\t' ) && ( *str != 0 ) && ( *str != ')' ))
         {
            if (( *str >= '0' ) && ( *str <= '9' ))
            {
               *val *= 16;
               *val += (*str - '0');
               str++;
            }
            else if (( *str >= 'a' ) && ( *str <= 'f' ))
            {
               *val *= 16;
               *val += ((*str - 'a') + 10);
               str++;
            }
            else if (( *str >= 'A' ) && ( *str <= 'F' ))
            {
               *val *= 16;
               *val += ((*str - 'A') + 10);
               str++;
            }
            else
            {
               *val = 0;
               return(1);
            }
         }
         
         return(0);
      }

      /* See if the leading zero was the *only* numerid (therefore zero) */
      if (( *str == ' ' ) || ( *str == '\t' ) || ( *str == 0 ) || ( *str == ')' ))
      {
         /* This is a leading zero - that IS zero */
         *val = 0;
         return(0);
      }

      /* If it is a numeric with a leading zero, then fall through to next block */
   }

   if (( *str >= '0' ) && ( *str <= '9' ))
   {
      *val = 0;
      while (( *str >= '0' ) && ( *str <= '9' ))
      {
         *val *= 10;
         *val += ( *str - '0' );
         str++;
      }

      /* TCHNICALLY... if we got this far, we are ok. The numeric is complete
         once we hit white space. The one error condition is if the number is
         followed (immediately - without a space) by any invalid char. For example:
         ...3h      <----- Invalid 3h is not a number, nor a tag, etc...
         ...3+      <----- Could be a mathematical expression
         ...3 4     <----- Is invalid (because 4 has no context), but would NOT
         be flagged as wrong here.
      */

      switch ( *str )
      {
         /* This is end of string */
      case 0:
         /* Other terminating conditions (various maths) */
      case ')':
      case '+':
      case '-':
      case '*':
      case '/':
         /* Syntax */
      case ';':
      case ':':
         /* Space terminated */
      case ' ':
      case '\t':
         return(0);
         break;
      default:
         return(1);
         break;
      }

      return(1);
   }

   return(1);
}

/* ========================================================================= */
int InsertEnum(RuleSet *rs, Enum *e)
{
   Enum *thise;
   ExplicitTag *thiset;
   ParsePoint *thispp;

   assert(NULL != rs);
   assert(NULL != e);

   /* Walk the explicit tag list looking for a pattern match */
   thiset = rs->etlist;
   while(thiset)
   {
      if ( 0 == strcmp(e->tag, thiset->tag) )
      {
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Enum tag naming collision. Two tags are called \"%s\".\n", e->tag);
         return(1);
      }

      thiset = thiset->next;
   }
   
   /* Walk the pp list looking for a pattern match */
   thispp = rs->pplist;
   while(thispp)
   {
      if ( 0 == strcmp(e->tag, thispp->tag) )
      {
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Enum tag naming collision. Two tags are called \"%s\".\n", e->tag);
         return(1);
      }

      thispp = thispp->next;
   }

   /* Walk the enum list looking for a pattern match */
   thise = rs->elist;
   while(thise)
   {
      if ( 0 == strcmp(e->tag, thise->tag) )
      {
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Enum naming collision. Two enums are called \"%s\".\n", e->tag);
         return(1);
      }

      thise = thise->next;
   }

   /* If we made it here, then we are unique (on the user-defined list) */
   e->next = rs->elist;
   rs->elist = e;

   return(0);
}

/* ========================================================================= */
int InsertETag(RuleSet *rs, ExplicitTag *e)
{
   Enum *thise;
   ExplicitTag *thiset;
   ParsePoint *thispp;

   assert(NULL != rs);
   assert(NULL != e);

   /* Walk the explicit tag list looking for a pattern match */
   thiset = rs->etlist;
   while(thiset)
   {
      if ( 0 == strcmp(e->tag, thiset->tag) )
      {
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Explicit tag naming collision. Two tags are called \"%s\".\n", e->tag);
         return(1);
      }

      thiset = thiset->next;
   }
   
   /* Walk the pp list looking for a pattern match */
   thispp = rs->pplist;
   while(thispp)
   {
      if ( 0 == strcmp(e->tag, thispp->tag) )
      {
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Explicit tag naming collision. Two tags are called \"%s\".\n", e->tag);
         return(1);
      }

      thispp = thispp->next;
   }

   /* Walk the enum list looking for a pattern match */
   thise = rs->elist;
   while(thise)
   {
      if ( 0 == strcmp(e->tag, thise->tag) )
      {
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Enum naming collision. Two enums are called \"%s\".\n", e->tag);
         return(1);
      }

      thise = thise->next;
   }

   /* Just insert - order does not matter */
   e->next = rs->etlist;
   rs->etlist = e;

   return(0);
}

/* ========================================================================= */
int InsertPP(RuleSet *rs, ParsePoint *pp)
{
   Enum *thise;
   ExplicitTag *thiset;
   ParsePoint *thispp;

   assert(NULL != rs);
   assert(NULL != pp);

   /* Walk the explicit tag list looking for a pattern match */
   thiset = rs->etlist;
   while(thiset)
   {
      if ( 0 == strcmp(pp->tag, thiset->tag) )
      {
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Explicit tag naming collision. Two tags are called \"%s\".\n", pp->tag);
         return(1);
      }

      thiset = thiset->next;
   }

   /* Walk the enum list looking for a pattern match */
   thise = rs->elist;
   while(thise)
   {
      if ( 0 == strcmp(pp->tag, thise->tag) )
      {
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Enum naming collision. Two enums are called \"%s\".\n", pp->tag);
         return(1);
      }

      thise = thise->next;
   }

   /* Is it the first item in the list? */
   if ( NULL == rs->pplist )
   {
      rs->pplist = pp;
      return(0);
   }

   /* Walk the list in an *ordered* manner so we can preserve the order as 
      specified by the user in the bpf file. */
   thispp = rs->pplist;
   while(thispp)
   {
      if ( 0 == strcmp(thispp->tag, pp->tag) )
      {
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Unable to (re)use tag named \"%s\". Check tags on lines %d and %d.\n", pp->tag, thispp->lineno, pp->lineno);
         return(1);
      }

      if ( NULL == thispp->next )
      {
         /* Add to the list there */
         thispp->next = pp;
         return(0);
      }

      thispp = thispp->next;
   }

   return(1);
}
