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
  rs->belist = NULL;    /* Empty list for the (builtin) enums               */
  
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
   /* Metadata -- Boolean flags for status */
   pp->fail_bail = 0;
   pp->tags_resolved = 0;   /* Pass 2 not complete */
   pp->data_resolved = 0;   /* Pass 3 not complete */
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
/* This is effectively an inline function just for handle_ppopt(). It really
   should not be used for anything else. It was designed with that STRICTLY
   in mind. Definetly do not error here. */
int copyout_equal(char *fill, char *eqsrc)
{
   int c = 0;

   assert( NULL != fill );
   
   if ( NULL == eqsrc )
      return(1);

   /* We know that the input is "<pattern>=". So we can safely walk through
      the equal sign. */
   while ( *eqsrc != '=' )
      eqsrc++;

   eqsrc++; /* Move off the '=' symbol. */

   /* Step through all the possible chars - filling the target location */
   while ( ( *eqsrc != ' ' ) && ( *eqsrc != '\t' ) && ( *eqsrc != 0 ) )
   {
      *fill = *eqsrc;
      fill++;
      eqsrc++;
      c++;

      if ( c == MAX_TOKEN_LEN )
         return(1);
   }

   *fill = 0; /* Terminate */
   
   /* No data */
   if ( c == 0 )
      return(1);


   return(0);
}

/* ========================================================================= */
int handle_ppopt(ParsePoint *pp, char *raw_ppopt)
{
   int gotit;
   BPInt rhsn; /* Right Hand Side Numeric */

   /* Asserts are more appropriate */
   assert(NULL != pp);
   assert(NULL != raw_ppopt);

   char eqstr[MAX_TOKEN_LEN];

   /* Walk off any leading ws - this *should* have no effect */
   eat_ws(&raw_ppopt);

   /* Check for an empty string */
   if ( raw_ppopt[0] == 0 )
      return(0); /* Exit without an error. It has been shown that this
                    is not a possibility (this bug no longer needs to be
                    caught). So just go back as though nothing happend. */

   /* Ok... this is not a perfect pattern match, but it works with some
      edge case issues that are likely not an issue.

      Also note that I am equating hidden= and hide=, but using hidden= in
      the error message. This effectively means that hidden= is the
      desired syntax, but hide= is supported.
   */
   if (( raw_ppopt == strstr(raw_ppopt, "hidden=") ) || ( raw_ppopt == strstr(raw_ppopt, "hide=") ))
   {
      if(copyout_equal(eqstr, raw_ppopt))
      {
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Optional token comprehension failure. Unable to locate right hand side of\n");
         fprintf(stderr, "\"hidden=\" operator on line %d.\n", pp->lineno);
         pp->fail_bail = 1;
         return(1);
      }

      conv_to_uc(eqstr); /* Make it upper case */

      gotit = 0;

      if ( eqstr == strstr(eqstr, "TRUE") )
      {
         gotit++;
         pp->print_result = 0;
      }

      if ( eqstr == strstr(eqstr, "FALSE") )
      {
         gotit++;
         pp->print_result = 1;
      }

      if ( 0 == gotit )
      {
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Optional token comprehension failure. Unable to parse right hand side of\n");
         fprintf(stderr, "\"hidden=\" operator on line %d.\n", pp->lineno);
         pp->fail_bail = 1;
         return(1);
      }

      return(0);
   }

   if ( raw_ppopt == strstr(raw_ppopt, "must=") )
   {
      if(copyout_equal(eqstr, raw_ppopt))
      {
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Optional token comprehension failure. Unable to locate right hand side of\n");
         fprintf(stderr, "\"must=\" operator on line %d.\n", pp->lineno);
         pp->fail_bail = 1;
         return(1);
      }

      if ( ParseBPInt(&rhsn, eqstr) )
      {
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Optional token comprehension failure. Unable to parse right hand side of\n");
         fprintf(stderr, "\"must=\" operator on line %d.\n", pp->lineno);
         pp->fail_bail = 1;
         return(1);
      }

      pp->use_muste = 1;    /* This means the next value is "set".       */
      pp->muste_val = rhsn; /* Assign (set) a value                      */

      return(0);
   }

   if ( raw_ppopt == strstr(raw_ppopt, "mask=") )
   {
      if(copyout_equal(eqstr, raw_ppopt))
      {
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Optional token comprehension failure. Unable to locate right hand side of\n");
         fprintf(stderr, "\"mask=\" operator on line %d.\n", pp->lineno);
         pp->fail_bail = 1;
         return(1);
      }

      if ( ParseBPInt(&rhsn, eqstr) )
      {
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Optional token comprehension failure. Unable to parse right hand side of\n");
         fprintf(stderr, "\"mask=\" operator on line %d.\n", pp->lineno);
         pp->fail_bail = 1;
         return(1);
      }

      pp->use_mask = 1;    /* This means the next value is "set".       */
      pp->mask_val = rhsn; /* Assign (set) a value                      */

      return(0);
   }


   if ( raw_ppopt == strstr(raw_ppopt, "enum=") )
   {
      if(copyout_equal(eqstr, raw_ppopt))
      {
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Optional token comprehension failure. Unable to locate right hand side of\n");
         fprintf(stderr, "\"enum=\" operator on line %d.\n", pp->lineno);
         pp->fail_bail = 1;
         return(1);
      }

      pp->enum_tag = nc_mkstring(eqstr);

      return(0);
   }










   fprintf(stderr, "-------------------------------------------------------------------------------\n");
   fprintf(stderr, "Tag comprehension failure. Directive tag \"%s\" on line %d is not understood.\n", raw_ppopt, pp->lineno);
   fprintf(stderr, "   Insure that this is a supported tag or is proper case.\n");
   pp->fail_bail = 1;
   return(1);
}

/* ========================================================================= */
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
  //int lineno;
  char *line;

  assert(NULL != f);

  /* Local use convenience */
  //lineno = f->lineno;
  line = f->line;

  /* If we got here, then we have sufficient data */
  if ( NULL == (pp = new_parsepoint(f->lineno)) )
  {
    /* We MUST exit on error here. The only means of failure is a bad
       malloc(). We cannot simply return NULL on a bad malloc(). We must
       throw and exception and exit. */
    exit(1);
  }

  /* Check first to see if this is an empty line. That is a no-error return */
  eat_ws(&line);
  if ( *line == 0 )
     return(NULL);

  if ( strlen(line) <= 10 )
  {
     fprintf(stderr, "-------------------------------------------------------------------------------\n");
     fprintf(stderr, "Problems parsing line %d due to length issues.\n", pp->lineno);
     pp->fail_bail = 1;
     return(pp);
  }
  
  if ( copy_out_nth_token(raw_offset, MAX_TOKEN_LEN, line, 1) )
  {
     fprintf(stderr, "-------------------------------------------------------------------------------\n");
     fprintf(stderr, "Problems parsing the first element in line %d.\n", pp->lineno);
     pp->fail_bail = 1;
     return(pp);
  }

  if ( copy_out_nth_token(raw_size, MAX_TOKEN_LEN, line, 2) )
  {
     fprintf(stderr, "-------------------------------------------------------------------------------\n");
     fprintf(stderr, "Problems parsing the second element in line %d.\n", pp->lineno);
     pp->fail_bail = 1;
     return(pp);
  }

  if ( copy_out_nth_token(raw_tag, MAX_TOKEN_LEN, line, 3) )
  {
     fprintf(stderr, "-------------------------------------------------------------------------------\n");
     fprintf(stderr, "Problems parsing the third element in line %d.\n", pp->lineno);
     pp->fail_bail = 1;
     return(pp);
  }

  if ( copy_out_nth_token(raw_label, MAX_TOKEN_LEN, line, 4) )
  {
     fprintf(stderr, "-------------------------------------------------------------------------------\n");
     fprintf(stderr, "Problems parsing the fourth element in line %d.\n", pp->lineno);
     pp->fail_bail = 1;
     return(pp);
  }

  if ( copy_out_nth_token(raw_dt, MAX_TOKEN_LEN, line, 5) )
  {
     fprintf(stderr, "-------------------------------------------------------------------------------\n");
     fprintf(stderr, "Problems parsing the fifth element in line %d.\n", pp->lineno);
     pp->fail_bail = 1;
     return(pp);
  }

  /* Parse the options (which are *optional*) after parsing the items
     that are *required*. We do this because they can be more than one,
     and trying to hold them in some sort of temporary list is a PITA.
     So lets parse them right into the parse point struct. For this
     reason, we create the parse point struct before this last set of
     item(s). */
#define ALLOW_MULTIPLE_OPTIONS

#ifdef ALLOW_MULTIPLE_OPTIONS
  /* This code path allows multiple options. The options are not
     mutually exclusive, so (in some cases), more than one might
     exist together. */
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
  /* Mutually exclusive options - as it stands, you can only use ONE option in
     a parse point! This is because they are ALL mutually exclusive. This may
     not always be the case. */

  /* I "re-use" i here. Rename it this code path is chosen */
  i = copy_out_nth_token(raw_ppopt, MAX_TOKEN_LEN, line, 6);
#endif

  /*** Copy everything into the ParsePoint ***/
  
  /* The offset */
  if ( NULL != ( e = ParseEntity(raw_offset, f->lineno) ) )
  {
     /* STUB: What to do here? 
        DBG_dump_entity(0, e); */
     pp->Offset = e;
  }
  else
  {
     pp->fail_bail = 1;
     return(pp);
  }

  /* The size */
  if ( NULL != ( e = ParseEntity(raw_size, f->lineno) ) )
  {
     /* STUB: Make this an option of debug?
        DBG_dump_entity(0, e); */
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
	    pp->lineno, raw_dt);
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

   /* Here we go.... The plan:
      1. Grab a line ----> NextLine()
      2. Strip off comments, leading, & trailing ws
      3. Sniff it for a line type
      4. Try to parse it as that type
   */

   /* Now start pulling lines */
   while(NextLine(f))
   {
      /* ...for each line... */
      line = f->line;

      chomp(line);              /* Kill EOL chars */
      line = leadingwst(line);  /* Kill leading WS */
      hash_trunc(line);         /* Truncate Hash based comments */
      /* STUB: Why not clear off trailing ws? */

      /* Eliminate empty lines (early) */
      if ( *line == 0 )
         continue;

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

      if ( IsSetOpt(line) )
      {
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
   }
  
   /* Close the file */
   EndFile(f);

   /* Check for empty parse point list */
   if ( NULL == rs->pplist )
   {
      fprintf(stderr, "-------------------------------------------------------------------------------\n");
      fprintf(stderr, "Unable to parse any parse-point rules from BPF file.\n");
      return(NULL);
   }

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
               fprintf(stderr, "Tag resolution failure. Offset tag \"%s\" on line %d\n", (char *)pp->Offset->u.tag, pp->lineno);
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
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Tag resolution failure. Unable to resolve the offset tag \"%s\"\n", (char *)pp->Offset->u.tag);
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
               fprintf(stderr, "Tag resolution failure. Size tag \"%s\" on line %d\n", (char *)pp->Size->u.tag, pp->lineno);
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
         fprintf(stderr, "-------------------------------------------------------------------------------\n");
         fprintf(stderr, "Tag resolution failure. Unable to resolve the size tag \"%s\"\n", (char *)pp->Size->u.tag);
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
   /* RESOLVED: This second assert should not be:
      RESOLVED:  1. required. It should be checked elsewhere. <-------- It is
      RESOLVED:  2. an assert(). It should be handled as a message to
      RESOLVED:     the user that the list is empty. <--------- It is, elsewhere
   */
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
