#ifdef PORT_Linux
#define _XOPEN_SOURCE 500
#endif

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "binpass.h"
#include "eswap.h"

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

   The option chose here was to repeatedly walk the list, and make the
   number of passes a tunable (-p).
*/

/* ========================================================================= */
#define GEV_RESOLVED   0   /* The entity was resolved           */
#define GEV_DEPENDENT  1   /* Not resolved on dependency issues */
#define GEV_ERROR     -1   /* Not resolved on error             */
int get_entity_value(RuleSet *rs, BPInt *out, Entity *ent);

/* ========================================================================= */
int bin_read_pp(RuleSet *rs, ParsePoint *pp)
{
   BPInt msize;
   struct stat s;
   BPInt expected_size = 0;
   char *casthelp;          /* Used (only) to avoid an ugly cast */
  
   /* Open the file if it is not already open */
   if ( -1 == rs->f )
   {
      if ( -1 == (rs->f = open(rs->fname, O_RDONLY)) )
      {
         fprintf(stderr, "ERROR: Unable to open binfile \"%s\".\n", rs->fname);
         return(1);
      }

      if(fstat(rs->f, &s))
      {
         fprintf(stderr, "ERROR: Unable to stat binfile \"%s\".\n", rs->fname);
         return(1);
      }

      /* Save off the size. We will use this for safety checks later. */
      rs->fsize = s.st_size; 
   }

   /* Offset must be positive */
   if ( pp->rOffset < 0 )
   { 
      fprintf(stderr, "-------------------------------------------------------------------------------\n");
      fprintf(stderr, "Data lookup failure. Offset for \"%s\" on line %d\n", pp->tag, pp->lineno);
      fprintf(stderr, "   is a negative value.\n");
      return(1);
   }

   /* Size must be > 0 */
   if ( pp->rSize <= 0 )
   { 
      fprintf(stderr, "-------------------------------------------------------------------------------\n");
      fprintf(stderr, "Data lookup failure. Size for \"%s\" on line %d\n", pp->tag, pp->lineno);
      fprintf(stderr, "   is a negative or zero value.\n");
      return(1);
   }

   /* Offset + Size must be less than the file size */
   if ( pp->rSize + pp->rOffset > rs->fsize )
   { 
      fprintf(stderr, "-------------------------------------------------------------------------------\n");
      fprintf(stderr, "Data lookup failure. Data read for \"%s\" on line %d\n", pp->tag, pp->lineno);
      fprintf(stderr, "   exceeds the length of the binary file.\n");
      return(1);
   }

   /* Find the expected size of the read */
   switch ( pp->dt )
   {
   case DT_UINT8:
   case DT_INT8:
   case DT_CHAR:
      expected_size = 1;
      break;
   case DT_UINT16:
   case DT_INT16:
      expected_size = 2;
      break;
   case DT_UINT32:
   case DT_INT32:
      expected_size = 4;
      break;
   case DT_UINT64:
   case DT_INT64:
      expected_size = 8;
      break;
   case DT_ZTSTR:
   case DT_FLSTR:
      expected_size = -1;
      break;
   case DT_NULL:
   default:
      fprintf(stderr, "ERROR: Data type for \"%s\" was not properly set.\n", pp->tag);
      return(1);
   }

   /* Compare the expected size against the read size */
   if ( expected_size > 0 )
   {
      if ( expected_size != pp->rSize )
      { 
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Data size mismatch. Data type (%ld bytes) does not match the read size (%ld", expected_size, pp->rSize);
         fprintf(stderr, "   bytes) for \"%s\" on line %d.\n", pp->tag, pp->lineno);
         return(1);
      }
   }

   /* Allocate the memory for the read-into target */
   msize = pp->rSize;     /* Start with the requested size */
   if ( pp->dt == DT_FLSTR ) /* Add a byte if it is a fixed length string */
      msize++;            /* WHY: Because I *could* treat this as a normal
                                  NULL terminated string. Here, the thinking
                                  is that I want that safety, and I want
                                  aligned data. */

   /* Now insure that the malloc is alligned (It would be padded by allocator
      anyways. This just rounds out the malloc()). */
   while( msize % 4 != 0 )
      msize++;

   if ( NULL == (pp->data = malloc(msize)) )
   {
      fprintf(stderr, "ERROR: Failed to allocate memory for a data item.\n");
      return(1);
   }

   /* Actually do the read */
   if ( pp->rSize != pread(rs->f, pp->data, pp->rSize, pp->rOffset) )
   {
      fprintf(stderr, "-------------------------------------------------------------------------------\n");
      fprintf(stderr, "Data retrieval failure. Parse point \"%s\" on line %d.\n", pp->tag, pp->lineno);
      fprintf(stderr, "   cannot be read.\n");
      return(1);
   }

   if ( pp->dt == DT_FLSTR )
   {
      /* Throw in some termination for fixed length strings. These should be
         printed differently (not simply treated as a null terminated string),
         but we add a term character here to insure that it is not mishandled.

         This is "safe" programming... "safe" in that it opens me up to potential
         exposure later. This data type should *always* be treated differently.
         This "let's be safe" operation justifies treating it the same (as a
         NULL terminated string).
      */

      /* Compiler warned on void casting. I could work through a solid
         casting effort, or just assign it to a specific pointer type. */
      casthelp = (char *)pp->data;
      casthelp[pp->rSize] = 0;
   }

   if ( rs->bESwap )
   {
      switch ( pp->dt )
      {
      case DT_UINT16:
      case DT_INT16:
         eswap_16(pp->data);
         break;
      case DT_UINT32:
      case DT_INT32:
         eswap_32(pp->data);
         break;
      case DT_UINT64:
      case DT_INT64:
         eswap_64(pp->data);
         break;
      }
   }

   /* Validate that the data type can be converted to a BPInt (before trying) */
   /* Note: The mask (mask=) is applied in the following function. */
   if(SetBPIntFromVoid(pp))
      return(1);

   /* Check the muste (must=) value */
   if ( pp->use_muste )
   {
      BPUInt *muste_compare;

      if ( pp->dt == DT_UINT64 )
         muste_compare = &pp->rudata;
      else
         muste_compare = (BPUInt *)&pp->rdata; /* rdata is (a pointer to) a BPInt */

      /* NOTE: Locking this out as it is debug related. I could keep it
               and write it conditionally, but I do not know what value
               that might bring to the user.
      fprintf(stderr, "rdata         [%lX]\n", pp->rdata);
      fprintf(stderr, "rudata        [%lX]\n", pp->rudata);
      fprintf(stderr, "muste_compare [%lX]\n", *muste_compare);
      fprintf(stderr, "muste_val     [%lX]\n", pp->muste_val);
      */

      if ( pp->muste_val != *muste_compare )
      {
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Data test failure. The \"must=\" rule failed for the parse point named\n");
         fprintf(stderr, "   \"%s\" on line %d. Expected: %ld; Actual: %ld.\n", pp->tag, pp->lineno, pp->muste_val, pp->rdata);

         return(1);
      }
   }
  
   SetPPDataResolved(pp, DR_DATA);
   return(0);
}

/* ========================================================================= */
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

         thispp = thispp->next;
      } /* while ( thispp ) <----- Walking through the list */

      passes++;

      /* Look at the number of pass attempts */
      if ( passes >= o->iPasses )
      {
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Unable to resolve data after %d %s. This BPF file does not appear to be\n",
                 passes,
                 passes == 1 ? "pass" : "passes");
         fprintf(stderr, "   resolvable in this number of data sweeps. Consider increasing pass\n");
         fprintf(stderr, "   attempts with -p on the command line or \"setopt p\" in the BPF file.\n");
         return(1);
      }
   }

   if ( rs->f >= 0 )
   {
      /* Error conditions let the file implicitly close on exit. Here we
         explicitly close it. */
      close(rs->f);
      rs->f = -1;
   }

   return(0);
}

/* ========================================================================= */
int get_expression_value(RuleSet *rs, BPInt *out, Expression *expr)
{
   BPInt lhs;
   BPInt rhs;
   int rv;

   /* Get the left value */
   switch(rv = get_entity_value(rs, &lhs, expr->left))
   {
   case GEV_DEPENDENT:
   case GEV_ERROR:
      return(rv);
   }

   /* Get the right value */
   switch(rv = get_entity_value(rs, &rhs, expr->right))
   {
   case GEV_DEPENDENT:
   case GEV_ERROR:
      return(rv);
   }

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

   return(GEV_RESOLVED);
}

/* ========================================================================= */
/* Get a PP data value - if resolved */
int get_pp_value(RuleSet *rs, BPInt *out, ParsePoint *pp)
{
   /* STUB: This returns a non-get_entity_value() number! */
   if ( IsPPDataResolved(pp, DR_DATA ) )
   {
      *out = pp->rdata;
      return(0);
   }

   return(1);
}

/* ========================================================================= */
/* Get a Explicit Tag data value - if resolved */
int get_et_value(RuleSet *rs, BPInt *out, ExplicitTag *et)
{
   return( get_entity_value(rs, out, et->e) );
}

/* ========================================================================= */
int get_entity_value(RuleSet *rs, BPInt *out, Entity *ent)
{
   switch ( ent->type )
   {
   case ETYPE_VALUE:
      *out = ent->u.value;
      return(GEV_RESOLVED);

   case ETYPE_MEXPR:
      return(get_expression_value(rs, out, ent->u.math));

   case ETYPE_TAGCP:
      /* This is an assert()-like error, hence the format difference. */
      fprintf(stderr, "ERROR: Encountered unresolved tag \"%s\".\n", (char *)ent->u.tag);
      return(GEV_ERROR);

   case ETYPE_TAGPP:
      return(get_pp_value(rs, out, ent->u.tag));

   case ETYPE_TAGET:
      return(get_et_value(rs, out, ent->u.tag));
   }

   return(1);
}

