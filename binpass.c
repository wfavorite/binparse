#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


#include "binpass.h"

  /* How:
       Repeatedly rake the linked list for the data. If it is in order*
       then it should only take a single read of the list/file.**

       [* It can be ordered in the app as it was read, but the user may
       have written a crappy file that was not ordered. The alternative
       would be an ordering of the parse points in the app as a dependency
       tree. This is harder than required, so opting for the more brute
       force method as long as it only takes me a few tries - tops.]

       [** Each walk of the list would be accompanied by a reading of the
       file. We MUST read the file because we need the data to resolve
       other items (such as a size to read). So a walk of this list would
       require reading from the data file.]

       It cannot be assumed that a complete read can be accomplished in
       N walks of the list. The "recursion" could continue with thousands
       of dependencies. The only way to do this is:
        - Put all items in a dependency tree and recursively resolve the
	  tree leaf by leaf.
	- Continually walk the list until it is resolved. Make sure that
	  at least one new item is resolved on each list walk.
	- Continually walk the list until it is resolved. Recurse on every
	  found item. This should only take a single walk. It would also
	  require that a link is kept to the dependent pp during the tag
	  resolution phase. (It would have to be really. This would be the
	  best time to make the linkage, and the linkage is required to
	  resolve both data type (for casting) and the data location.)
  */

/* ========================================================================= */
int get_entity_value(RuleSet *rs, BPInt *out, Entity *ent);

/* ========================================================================= */
int bin_read_pp(RuleSet *rs, ParsePoint *pp)
{
  BPInt got;
  
  /* Open the file if it is not already open */
  if ( -1 == rs->f )
  {
    if ( -1 == (rs->f = open(rs->fname, O_RDONLY)) )
    {
      fprintf(stderr, "ERROR: Unable to open binfile \"%s\".\n", rs->fname);
      return(1);
    }
  }

  if ( pp->rOffset < 0 )
  { 
    fprintf(stderr, "-------------------------------------------------------------------------------\n");
    fprintf(stderr, "Data lookup failure. Offset for \"%s\" on line %d\n", pp->tag, pp->lineno);
    fprintf(stderr, "   is a negative value.\n");
    return(1);
  }

  /* STUB: Validate that the datatype and size match. */

  /* STUB: Validate that the size is positive */

  /* STUB: Validate that the size + offset does not extend beyond the end of the file */

  if ( NULL == (pp->data = malloc(pp->rSize)) )
  {
    fprintf(stderr, "ERROR: Failed to allocate memory for a data item.\n");
    return(1);
  }

  if ( pp->rSize != pread(rs->f, pp->data, pp->rSize, pp->rOffset) )
  {
    fprintf(stderr, "-------------------------------------------------------------------------------\n");
    fprintf(stderr, "Data retrieval failure. Parse point \"%s\" on line %d\n", pp->tag, pp->lineno);
    fprintf(stderr, "   cannot be read.\n");
    return(1);
  }

  /* STUB: Validate that the data type can be converted to a BPInt */
  pp->rdata = (BPInt)pp->data;
  
  return(0);
}





/* ========================================================================= */
#define MAX_PASSES 1
int ResolveData(RuleSet *rs, Options *o)
{
   ParsePoint *thispp;
   BPInt Offset;
   BPInt Size;
   int not_done;
   int passes;
   int got_off;
   int got_siz;

   passes = 0;
   not_done = 1;
   while ( not_done )
   {
     /* Start with the assumption that all is resolved */
     not_done = 0;

     /* Walk the list */
     thispp = rs->pplist;
     while(thispp)
     {
       got_off = 0;
       got_siz = 0;

       if ( 0 == IsPPDataResolved(thispp, DR_DATA) )
       {
	 fprintf(stderr, "STUB DEBUG  pp[%s]\n" , thispp->tag);

	 /* Check Offset before trying again */
	 if ( 0 == IsPPDataResolved(thispp, DR_OFFSET) )
	 {
	   /* Not set, so try */
	   if ( GEV_ERROR == ( got_off = get_entity_value(rs, &Offset, thispp->Offset) ) )
	     return(1);

	   if ( GEV_RESOLVED == got_off )
	   {
	     thispp->rOffset = Offset;
	     SetPPDataResolved(thispp, DR_OFFSET);
	   }
	 }

	 /* Check Size before trying again */
	 if ( 0 == IsPPDataResolved(thispp, DR_SIZE) )
	 {
	   /* Not set, so try */
	   if ( GEV_ERROR == ( got_off = get_entity_value(rs, &Size, thispp->Size) ) )
	     return(1);

	   if ( GEV_RESOLVED == got_siz )
	   {
	     thispp->rSize = Size;
	     SetPPDataResolved(thispp, DR_SIZE);
	   }
	 }

	 /* We *know* data is not resolved - just check the Off and Size */
	 if ( IsPPDataResolved(thispp, DR_SIZE | DR_OFFSET ) )
	 {
	   /* Read in the actual data. This should not fail, as dependencies have
	      been resolved. */
	   if (bin_read_pp(rs, thispp))
	     return(1);

	   if ( o->bVerbose )
	   {
	     fprintf(stderr, "  Resolved data for parse point \"%s\".\n", thispp->tag);
	   }
	 }
	 else
	   not_done = 1; /* Data not resolved, we are not done. */
	 
       } /* if ( data not resolved ) */

       /* STUB: Debuggery */
       fprintf(stderr, "            Offset = %ld\n", Offset);
       fprintf(stderr, "            Size   = %ld\n", Size);

       
       thispp = thispp->next;
     } /* while ( thispp ) <----- Walking through the list */

     passes++;

     /* STUB: passes should be compared against a "tunable" that 
	STUB:   defaults to MAX_PASSES. Options are passed to this
	STUB:   function, so we have that to act on. */
     if ( passes >= MAX_PASSES )
     {
       fprintf(stderr, "-------------------------------------------------------------------------------\n");
       fprintf(stderr, "Unable to resolve data after %d %s. This BPF file does not appear to\n  be resolvable.\n",
	       passes,
	       passes == 1 ? "pass" : "passes");
       return(1);
     }
   }


   return(0);
}

/* ========================================================================= */
int get_expression_value(RuleSet *rs, BPInt *out, Expression *expr)
{
   BPInt lhs;
   BPInt rhs;

   /* Get the left value */
   if (get_entity_value(rs, &lhs, expr->left))
     return(1);

   /* Get the right value */
   if (get_entity_value(rs, &rhs, expr->right))
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
/* Get a PP data value - if resolved */
int get_pp_value(RuleSet *rs, BPInt *out, ParsePoint *pp)
{
  if ( IsPPDataResolved(pp, DR_DATA ) )
  {
    *out = pp->rdata;
    return(0);
  }

  return(1);
}



/* ========================================================================= */
int get_entity_value(RuleSet *rs, BPInt *out, Entity *ent)
{
   switch ( ent->type )
   {
   case ETYPE_VALUE:
      *out = ent->u.value;
      return(GEV_RESOLVED);
      break;

   case ETYPE_MEXPR:
     return(get_expression_value(rs, out, ent->u.math));
      break;

   case ETYPE_TAGCP:
     fprintf(stderr, "ERROR: Encountered unresolved tag \"%s\".\n", (char *)ent->u.tag);
     return(GEV_ERROR);

   case ETYPE_TAGPP:
     return(get_pp_value(rs, out, ent->u.tag));

   case ETYPE_TAGET:
     /* STUB: Temp fail state. This will ultimately be resolvable. */
     fprintf(stderr, "ERROR: Encountered unresolved tag type while processing an entity.\n");
     return(GEV_ERROR);





     
   default:
      *out = -7; /* Completely STUBbed */
      return(0);
      break;

   }



   return(1);
}




















/* ========================================================================= */
/* STUB: Ignored for now! */
int GetEntityValue(BPInt *out, Entity *ent)
{
   /* This is just a "public" wrapper */
  /* STUB: The first argument CANNOT be NULL!!!!! */
  return(get_entity_value(NULL, out, ent));
}

