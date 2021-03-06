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

   /* Apply mask= first */
   if ( pp->use_mask )
   {
      /* Masks are assigned as unsigned values */
      switch ( pp->dt )
      {
      case DT_CHAR:
      case DT_INT8:
      case DT_UINT8:
         *(uint8_t *)pp->data = *(uint8_t *)pp->data & (uint8_t)(pp->mask_val & 0xff);
         break;
      case DT_INT16:
      case DT_UINT16:
         *(uint16_t *)pp->data = *(uint16_t *)pp->data & (uint16_t)(pp->mask_val & 0xffff);
         break;
      case DT_INT32:
      case DT_UINT32:
         *(uint32_t *)pp->data = *(uint32_t *)pp->data & (uint32_t)(pp->mask_val & 0xffffffff);
         break;
      case DT_INT64:
      case DT_UINT64:
         *(uint64_t *)pp->data = *(uint64_t *)pp->data & (uint64_t)(pp->mask_val & 0xffffffffffffffff);
         break;
      }
   }

   switch ( pp->dt )
   {
   case DT_INT8:
      pp->rdata = (BPInt)(*((int8_t *)pp->data)) & 0x00000000000000FF;
      break;
   case DT_CHAR:
   case DT_UINT8:
      pp->rdata = (BPInt)(*((uint8_t *)pp->data)) & 0x00000000000000FF;
      break;
   case DT_UINT16:
      pp->rdata = (BPInt)(*((uint16_t *)pp->data)) & 0x000000000000FFFF;
      break;
   case DT_INT16:
      pp->rdata = (BPInt)(*((int16_t *)pp->data)) & 0x000000000000FFFF;
      break;
   case DT_UINT32:
      pp->rdata = (BPInt)(*((uint32_t *)pp->data)) & 0x00000000FFFFFFFF;
      break;
   case DT_INT32:
      pp->rdata = (BPInt)(*((int32_t *)pp->data)) & 0x00000000FFFFFFFF;
      break;
   case DT_UINT64:
      /* This conversion turns a large positive number negative */
      pp->rdata = 0;
      pp->rudata = (BPUInt)(*((uint64_t *)pp->data));
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
      /* This code will fail on large negative values. It does not check the
         range of the input. The ASCII input can overflow the integer.

         The recommended option is to parse as an integer (minus the sign)
         and then store the sign in a separate intger.

         This basically creates a 65 bit integer.

         typedef struct Integer
         {
            uint64_t dfz;       / * Distance From Zero * /
            uint8_t sign;
         } Integer;

         The integer itself can be parsed with strtoull().
         http://pubs.opengroup.org/onlinepubs/9699919799/functions/strtoul.html

         Note that strtoull() will covert both hex and dec input to binary.
      */

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

      /* Unreachable */
   }

   return(1);
}

/* ========================================================================= */
int ParseBPUInt(BPUInt *val, char *str)
{
   BPInt nval;
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

      nval = 0;
      while (( *str >= '0' ) && ( *str <= '9' ))
      {
         nval *= 10;
         nval += ( *str - '0' );
         str++;
      }

      /* Apply the negative sign */
      if ( isneg )
         nval *= -1;

      /* Now convert this to an unsigned value. No special casting, just
         copy the bits over, the meaning of the sign mask will change. */
      memcpy(val, &nval, sizeof(BPUInt));

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
      default:
         return(1);
      }

      /* Unreachable */
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
      rs->maxlabel = strlen(pp->label);
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

         if ( strlen(pp->label) > rs->maxlabel )
            rs->maxlabel = strlen(pp->label);

         thispp->next = pp;
         return(0);
      }

      thispp = thispp->next;
   }

   return(1);
}

/* ========================================================================= */
ParsePoint *NewParsepoint(int lineno)
{
   ParsePoint *pp;

   if (NULL == (pp = (ParsePoint *)malloc(sizeof(ParsePoint))))
   {
      fprintf(stderr, "ERROR: Failed to allocate memory for a parse point.\n");
      /* We exit because the only way to pass back failure is to pass back a
         pp with fail_bail set. When failing on a malloc() here, just exit. */
      exit(1);
   }

   /* Offset value */
   pp->Offset = NULL;
   /* Size value */
   pp->Size = NULL;
   /* The tag */
   pp->tag = NULL;
   /* The label */
   pp->label = NULL;
   /* Data & type */
   pp->data = NULL;
   pp->dt = DT_NULL;
   /* Line number related to this rule */
   /* Set lineno because functions that get passed a fresh ParsePoint
      will rely upon this if they encounter an error. Generally you will be
      dumping data into this struct (in the early setup), but this one piece
      of data is always guarenteed to be good. This is "private". */
   pp->lineno = lineno;
   /* Metadata -- Boolean flags for status */
   pp->fail_bail = 0;
   pp->tags_resolved = 0;   /* Pass 2 not complete */
   pp->data_resolved = 0;   /* Pass 3 not complete */
   pp->rtag_count = 0; /* Stats for how many tags were resolved (in this pp) */
   /* Linked list pointer */
   pp->next = NULL;
   /* Set all the "6th options" to defaults */
   pp->print_result = 1; /* Always print by default */
   pp->use_enum = NULL;
   pp->enum_tag = NULL;
   pp->use_muste = 0;
   pp->muste_val = 0;
   pp->use_mask = 0; /* The thinking is that this is unnessary. That the */
   pp->mask_val = 0; /*   mask_val can default to all-bits-set mask,and  */
                     /*   everything can default to having a mask.       */
   return(pp);
}

/* ========================================================================= */
RuleSet *NewRuleset(Options *o)
{
   RuleSet *rs;

   if ( NULL == (rs = malloc(sizeof(RuleSet))) )
   {
      fprintf(stderr, "ERROR: Unable to allocate memory for RuleSet structure.\n");
      return(NULL);
   }

   rs->pplist = NULL;    /* Start with an empty list.                        */
   rs->parserr = 0;      /* No errors at this time.                          */

   rs->elist = NULL;     /* Empty list for the (user-defined) enums          */
   rs->belist = NULL;    /* Empty list for the (builtin) enums               */
   rs->etlist = NULL;    /* Empty list of explicit tags                      */

   rs->maxlabel = 0;     /* Set to sane default. It will be overwritten.     */

   rs->f = -1;           /* The file has not been opened                     */
   rs->fname = o->binfile; /* The binary file that will be opened later      */

   rs->bESwap = o->bESwap; /* Save off the directive to do byte swapping     */
  
   return(rs);
}
