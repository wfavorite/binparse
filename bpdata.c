#include <assert.h>
#include <stdlib.h>

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
