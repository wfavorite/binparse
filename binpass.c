#include <stdio.h>
#include <stdlib.h>

#include "binpass.h"

/* ========================================================================= */
int get_entity_value(BPInt *out, Entity *ent);



/* ========================================================================= */
int ResolveData(RuleSet *rs, Options *o)
{
   ParsePoint *thispp;
   BPInt Offset;
   BPInt Size;

   thispp = rs->pplist;
   while(thispp)
   {
      fprintf(stderr, "STUB DEBUG  pp[%s]\n" , thispp->tag);



      if ( GetEntityValue(&Offset, thispp->Offset) )
	return(1);
      
      if ( GetEntityValue(&Size, thispp->Size) )
	return(1);

      fprintf(stderr, "            Offset = %ld\n", Offset);
      fprintf(stderr, "            Size   = %ld\n", Size);

      thispp = thispp->next;
   }


   return(0);
}

/* ========================================================================= */
int get_expression_value(BPInt *out, Expression *expr)
{
   BPInt lhs;
   BPInt rhs;

   /* Get the left value */
   if (get_entity_value(&lhs, expr->left))
     return(1);

   /* Get the right value */
   if (get_entity_value(&rhs, expr->right))
     return(1);

   switch (expr->operation)
   {
   case EXPOP_ADD:
      *out = lhs + rhs;
      break;
   case EXPOP_SUB:
      *out = lhs - rhs;
      break;
   case EXPOP_MUL:
      *out = lhs * rhs;
      break;
   case EXPOP_DIV:
      if ( rhs == 0 )
      {
         fprintf(stderr, "ERROR: Divide by zero error processing expression.\n");
         return(1);
      }
      *out = lhs / rhs;
      break;
   }

   return(0);
}

/* ========================================================================= */
int get_entity_value(BPInt *out, Entity *ent)
{
   switch ( ent->type )
   {
   case ETYPE_VALUE:
      *out = ent->u.value;
      return(0);
      break;

   case ETYPE_MEXPR:
      return(get_expression_value(out, ent->u.math));
      break;

   case ETYPE_TAGCP:
     fprintf(stderr, "ERROR: Encountered unresolved tag \"%s\".\n", (char *)ent->u.tag);
     return(1);





     
   default:
      *out = -7; /* Completely STUBbed */
      return(0);
      break;

   }



   return(1);
}

/* ========================================================================= */
int GetEntityValue(BPInt *out, Entity *ent)
{
   /* This is just a "public" wrapper */
   return(get_entity_value(out, ent));
}

