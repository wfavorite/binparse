#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "bpfparse.h"
#include "strlib.h"
#include "penum.h"
#include "pmath.h"
#include "slfile.h"


/* ========================================================================= */
RuleSet *new_ruleset(void)
{
  RuleSet *rs;

  if ( NULL == (rs = malloc(sizeof(RuleSet))) )
  {
    fprintf(stderr, "ERROR: Unable to allocate memory for RuleSet structure.\n");
    return(NULL);
  }

  rs->pplist = NULL;    /* Start with an empty list.                        */
  rs->parserr = 0;      /* No errors at this time.                          */
  rs->pass = 0;         /* No pass has been completed                       */

  rs->elist = NULL;     /* Empty list for the (user-defined) enums          */
  
  if (ApplyBuiltins(rs)) /* This creates all the default builtin enums      */
     return(NULL);

  return(rs);
}

/* ========================================================================= */
int add_pp_to_rs(RuleSet *rs, ParsePoint *pp)
{
  ParsePoint *thispp;

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
    if ( NULL == thispp->next )
    {
      /* Add to the list there */
      thispp->next = pp;
      return(0);
    }

    thispp = thispp->next;
  }

  /* This is just not going to fail */
  return(1);
}

/* ========================================================================= */
ParsePoint *new_parsepoint(int lineno)
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
  /* Boolean flags for status */
  pp->tags_resolved = 0;   /* Pass 2 not complete */
  pp->data_resolved = 0;   /* Pass 3 not complete */
  /* Linked list pointer */
  pp->next = NULL;
  /* Set all the "6th options" to defaults */
  pp->print_result = 1; /* Always print by default */
  pp->use_enum = NULL;  /* No enum by default */
  pp->muste = 0;        /* muste is the flag to read muste_val */
  pp->fail_bail = 0;

  
  return(pp);
}

/* ========================================================================= */
int handle_ppopt(ParsePoint *pp, char *raw_ppopt)
{
   char HIDDEN[] = "hidden";
   char HIDE[] = "hide";

   /* Asserts are more appropriate */
   assert(NULL != pp);
   assert(NULL != raw_ppopt);

   /* Walk off any leading ws - this *should* have no effect */
   eat_ws(&raw_ppopt);

   /* Check for an empty string */
   if ( raw_ppopt[0] == 0 )
      return(1);  /* STUB: Test this case. It *should* be passed over here
                     STUB:    but it should be handled before it gets here.
                     STUB:    So I will error for now, until the calling
                     STUB:    function checks this beforehand. */



   /* Ok... this is not a perfect pattern match, but it works with some
      edge case issues that are likely not an issue.
   */
   if ( raw_ppopt == strstr(raw_ppopt, HIDDEN) )
   {
      pp->print_result = 0;
      return(0);
   }

   if ( raw_ppopt == strstr(raw_ppopt, HIDE) )
   {
      pp->print_result = 0;
      return(0);
   }

   /* STUB: Handle must= */

   /* STUB: Handle enum tag */

   fprintf(stderr, "-------------------------------------------------------------------------------\n");
   fprintf(stderr, "Tag comprehension failure. Directive tag \"%s\" on line %d is not understood.\n", raw_ppopt, pp->lineno);
   fprintf(stderr, "   Insure that this is a supported tag or is proper case.\n");
   pp->fail_bail = 1;
   return(1);
}

/* ========================================================================= */
/* STUB: You gotta pass the lineno! It is used to fill the pp, but also to
   STUB:    indicate where the error was found in the file. */
ParsePoint *get_parse_point(File *f)
{
  ParsePoint *pp;
  Entity *e;
  char raw_offset[MAX_TOKEN_LEN];
  char raw_size[MAX_TOKEN_LEN];
  char raw_tag[MAX_TOKEN_LEN];
  char raw_label[MAX_TOKEN_LEN];
  char raw_dt[MAX_TOKEN_LEN];
  char raw_ppopt[MAX_TOKEN_LEN];
  uint32_t nval;
  int i;
  int lineno;
  char *line;

  assert(NULL != f);

  /* Local use convenience */
  lineno = f->lineno;
  line = f->line;

  /* STUB: These return(NULL)s are not very informative! */

  if ( strlen(line) <= 10 )
    return(NULL);
  
  if ( copy_out_nth_token(raw_offset, MAX_TOKEN_LEN, line, 1) )
    return(NULL);


  if ( copy_out_nth_token(raw_size, MAX_TOKEN_LEN, line, 2) )
  {
     /* fprintf(stderr, "STUB DEBUG: copy_out_nth_token(NONE)\n"); */
     return(NULL);
  }

  /*
  fprintf(stderr, "STUB DEBUG: copy_out_nth_token(%s)\n", raw_size);
  */

  if ( copy_out_nth_token(raw_tag, MAX_TOKEN_LEN, line, 3) )
    return(NULL);

  if ( copy_out_nth_token(raw_label, MAX_TOKEN_LEN, line, 4) )
    return(NULL);

  if ( copy_out_nth_token(raw_dt, MAX_TOKEN_LEN, line, 5) )
    return(NULL);
  
  /* If we got here, then we have sufficient data */
  if ( NULL == (pp = new_parsepoint(lineno)) )
  {
    /* We MUST exit on error here. The only means of failure is a bad
       malloc(). We cannot simply return NULL on a bad malloc(). We must
       throw and exception and exit. */
    exit(1);
  }

  /* Parse the options (which are *optional*) after parsing the items
     that are *required*. We do this because they can be more than one,
     and trying to hold them in some sort of temporary list is a PITA.
     So lets parse them right into the parse point struct. For this
     reason, we create the parse point struct before this last set of
     item(s). */
#define STUB_ME_OPTIONS

#ifdef STUB_ME_OPTIONS
  /* Mutually exclusive options - as it stands, you can only use ONE option in
     a parse point! This is because they are ALL mutually exclusive. This may
     not always be the case. */

  /* STUB: Now try 6, 7, 8, until it fails. Take each read
     STUB:   item and try to match it to an expected pattern such
     STUB:   as print, noprint, hex, decimal, etc... */
  i = 6;
  while ( 0 == copy_out_nth_token(raw_ppopt, MAX_TOKEN_LEN, line, i) )
  {
     if( handle_ppopt(pp, raw_ppopt) )
     {
        /* Doh! something happened. Error at point of failure. Bail. */
        /* This should be set elsewhere. I set again to be sure. */
        pp->fail_bail = 1;
        return(pp);
     }

     i++;
  }
#else
  /* I "re-use" i here. Rename it this code path is chosen */
  i = copy_out_nth_token(raw_ppopt, MAX_TOKEN_LEN, line, 6);
#endif

  /*** Copy everything into the ParsePoint ***/
  
  /* The offset */
  if ( NULL != ( e = ParseEntity(f->lineno, raw_offset) ) )
  {
     DBG_dump_entity(0, e);
     pp->Offset = e;
  }
  else
  {
     pp->fail_bail = 1;
     return(pp);
  }

  /* The size */
  if ( NULL != ( e = ParseEntity(f->lineno, raw_size) ) )
  {
     DBG_dump_entity(0, e);
     pp->Size = e;
  }
  else
  {
     pp->fail_bail = 1;
     return(pp);
  }

  /* The two strings (tag and label) */
  pp->tag = nc_mkstring(raw_tag);
  pp->label = nc_mkstring(raw_label);

  /* Parse the data type (from a string to #define/enum */
  if ( 0 == strcmp(raw_dt, "uint8") )
    pp->dt = DT_UINT8;

  if ( 0 == strcmp(raw_dt, "int8") )
    pp->dt = DT_INT8;

  if ( 0 == strcmp(raw_dt, "uint16") )
    pp->dt = DT_UINT16;

  if ( 0 == strcmp(raw_dt, "int16") )
    pp->dt = DT_INT16;

  if ( 0 == strcmp(raw_dt, "uint32") )
    pp->dt = DT_UINT32;

  if ( 0 == strcmp(raw_dt, "int32") )
    pp->dt = DT_INT32;

  if ( 0 == strcmp(raw_dt, "uint64") )
    pp->dt = DT_UINT64;

  if ( 0 == strcmp(raw_dt, "int64") )
    pp->dt = DT_INT64;

  if ( 0 == strcmp(raw_dt, "char") )
    pp->dt = DT_CHAR;

  if ( 0 == strcmp(raw_dt, "ztstr") )
    pp->dt = DT_ZTSTR;

  if ( 0 == strcmp(raw_dt, "flstr") )
    pp->dt = DT_FLSTR;

  if ( DT_NONE == pp->dt )
  {
    fprintf(stderr, "Parse file error: Problems parsing a token.\n  Line: %d\n  Token: %s\n",
	    lineno, raw_dt);
    /* What we can do here...
       1. Press on, and try to parse the rest of the items in the file with
          the risk that this line is NOT a dependent line. Throw away the pp
	       allocation (don't free it - too much trouble not a big deal(?))
       2. Return NULL, toss the pp memory reservation - we will be exiting soon.
       3. Adhere to some command line driven option to determine behaviour of
          the application in regards to #1 or #2.
       4. Do one of the three previous items, but free the memory for the pp.
       5. Return the pp with fail_bail set. This will cause us to error and exit.
    */
    pp->fail_bail = 1; /* Now you did it. We are exiting on option 5. */
    return(pp);
  }

  pp->fail_bail = 0; /* Clear any assumed error. (Currently errors are not
                        assumed - then cleared. It is the opposite. This is
                        just a bit of safety. */
  return(pp);
}

/* ========================================================================= */
RuleSet *ParseBPFFile(Options *o)
{
   RuleSet *rs;
   ParsePoint *pp;
   Enum *e;
   File *f;
   char *line;
   char *filename;

   /* Pull the filename to a more convenient pointer */
   filename = o->bpffile;
  
   if ( NULL == filename )
   {
      fprintf(stderr, "ERROR: Borked file name. How did we get this far without a file name?!\n");
      return(NULL);
   }

   if ( filename[0] == 0 )
   {
      fprintf(stderr, "ERROR: Empty filename cannot be located or parsed.\n       Looks like an internal error.\n");
      return(NULL);
   }

   /* Create a new base ruleset */
   if ( NULL == ( rs = new_ruleset() ) )
      return(NULL);

   /* Open the file */
   if ( NULL == (f = NewFile(filename)) )
   {
      /* Message at point of error. */
      return(NULL);
   }

   /* Now start pulling lines */
   while(NextLine(f))
   {
      /* ...for each line... */
      line = f->line;

      chomp(line);              /* Kill EOL chars */
      line = leadingwst(line);  /* Kill leading WS */
      hash_trunc(line);         /* Truncate Hash based comments */

      /* Line sniff for enum */
      if ( IsEnumLine(line) )
      {
         if ( NULL != ( e = ParseEnum(line, f->lineno) ) )
         {
            /* This needs to be 'uniquely sorted' into the list (to avoid
               collisions). See commentary on the InsertEnum() declaration.  */
            if ( InsertEnum(rs, e) )
            {
               /* Error message at point of failure */
               return(NULL);
            }
         }
         
         /* Line was enum. Don't try to parse as something else.
            Instead, go get another line. */
         continue;
      }

      /* STUB: Line sniff for htagXXXX */

      /* STUB: Line sniff for option */



      if ( NULL != (pp = get_parse_point(f)) )
      {
         if ( pp->fail_bail )
         {
            /* Error message at point of failure */
            return(NULL);
         }

         add_pp_to_rs(rs, pp); /* This is just not going to fail */

         if ( o->bVerbose )
         {
            printf("  Added parse point rule named \"%s\".\n", pp->tag);
         }
      
         if ( o->bDebug )
         {
            /* Print verbose debuggery */
            printf("%03d:%s\n", f->lineno, line);
            printf(" pp->Offset->type = %d\n", pp->Offset->type);
            /* STUB: data type? */
            printf(" pp->tag    = %s\n", pp->tag);
            printf(" pp->label  = %s\n", pp->label);
         }
      }
      /* EREIAM - Do we want to exit here!?  STUB */
   }
  
   /* Close the file */
   EndFile(f);

   /* STUB: This would be the location to test for an empty pplist item
      STUB:   in the RuleList struct. There is reference to this as an
      STUB:   assert() in the ResolveTags() function. Either check there,
      STUB:   check here, or write and call a specific check API. Hint:
      STUB:   you should check here. */
   return(rs);
}

/* =========================================================================
 * Name: ResolveTags
 * Desc: Walk through the ParsePoint list and resolve all tags.
 * Params:
 * Returns: 0 on successful tag resolution
 * Side Effects:
 * Notes: Aka: 2nd pass resolution
 */
int resolve_tag(RuleSet *rs, ParsePoint *pp)
{
   ParsePoint *thispp;

   /*** Resolve the offset tag first ***/
   if ( pp->Offset->type == ETYPE_TAGCP )
   {
      /* Offset is a tag (that needs to be resolved) */
      thispp = rs->pplist;
      while ( thispp )
      {
         if ( 0 == strcmp(pp->Offset->u.tag, thispp->tag) )
         {
            if ( pp == thispp )
            {
               fprintf(stderr, "-------------------------------------------------------------------------------\n");
               fprintf(stderr, "Tag resolution failure. Offset tag \"%s\" on line %d\n", pp->Offset->u.tag, pp->lineno);
               fprintf(stderr, "   is self referential. A tagged offset can not reference the same line.\n");
               return(1);
            }
	
            /* Fall through to success */
            pp->Offset->u.tag = thispp;
            pp->Offset->type = ETYPE_TAGRS;    /* Mark tag resolved */
         }
         thispp = thispp->next;
      }

      
      if ( pp->Offset->type == ETYPE_TAGCP )
      {
         fprintf(stderr, "STUB: This line is used to determine the default width of the terminal screen..\n");
         fprintf(stderr, "Tag resolution failure. Unable to resolve the offset tag \"%s\"\n", pp->Offset->u.tag);
         fprintf(stderr, "   for item \"%s\" on line %d.\n", pp->tag, pp->lineno);
         return(1);
      }
   }

   /*** Resolve the size tag next ***/
   if ( pp->Size->type == ETYPE_TAGCP )
   {
      /* Size is a tag (that needs to be resolved) */
      thispp = rs->pplist;
      while ( thispp )
      {
         if ( 0 == strcmp(pp->Size->u.tag, thispp->tag) )
         {
            if ( pp == thispp )
            {
               fprintf(stderr, "-------------------------------------------------------------------------------\n");
               fprintf(stderr, "Tag resolution failure. Size tag \"%s\" on line %d\n", pp->Size->u.tag, pp->lineno);
               fprintf(stderr, "   is self referential. A tagged offset can not reference the same line.\n");
               return(1);
            }
	
            /* Fall through to success */
            pp->Size->u.tag = thispp;
            pp->Size->type = ETYPE_TAGRS;    /* Mark tag resolved */
         }
         thispp = thispp->next;
      }

      
      if ( pp->Size->type == ETYPE_TAGCP )
      {
         fprintf(stderr, "STUB: This line is used to determine the default width of the terminal screen..\n");
         fprintf(stderr, "Tag resolution failure. Unable to resolve the size tag \"%s\"\n", pp->Size->u.tag);
         fprintf(stderr, "   for item \"%s\" on line %d.\n", pp->tag, pp->lineno);
         return(1);
      }
   }

  /* If we got here, then all tags for this item are resolved. */
  pp->tags_resolved = 1;
  return(0);
} 

/* =========================================================================
 * Name: ResolveTags
 * Desc: Walk through the ParsePoint list and resolve all tags.
 * Params:
 * Returns: 0 on successful tag resolution
 * Side Effects:
 * Notes: Aka: 2nd pass resolution
 */
int ResolveTags(RuleSet *rs)
{
   ParsePoint *thispp;
  
   assert( NULL != rs );
   /* STUB: This second assert should not be:
      STUB:  1. required. It should be checked elsewhere.
      STUB:  2. an assert(). It should be handled as a message to
      STUB:     the user that the list is empty. */
   assert( NULL != rs->pplist );
   
   thispp = rs->pplist;
   while ( thispp )
   {
      if ( 0 == thispp->tags_resolved ) 
      {
         if ( resolve_tag(rs, thispp) )
         {
            /* Print the error at point of failure. Standard stuff... */
            return(1);
         }
      }
      
      thispp = thispp->next;
   }
   
   return(0);
}
