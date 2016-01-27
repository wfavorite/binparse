#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "bpfparse.h"
#include "strlib.h"

#define MAX_LINE_LEN 1024

typedef struct File
{
  FILE *fh;      /* If not NULL, then it is open */
  int lineno;
  int readerr;
  int eof;       /* Set when eof is reached      */
  char line[MAX_LINE_LEN + 4]; /* 4 keeps me aligned */
} File;
  

/* ========================================================================= */
RuleSet *new_ruleset(void)
{
  RuleSet *rs;

  if ( NULL == (rs = malloc(sizeof(RuleSet))) )
  {
    fprintf(stderr, "ERROR: Unable to allocate memory for RuleSet structure.\n");
    return(NULL);
  }

  rs->pplist = NULL;    /* Start with an empty list.          */
  rs->parserr = 0;      /* No errors at this time.            */
  rs->pass = 0;         /* No pass has been completed         */

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
File *new_file(char *filename)
{
  File *f;

  if ( NULL == ( f = (File *)malloc(sizeof(File)) ) )
  {
    fprintf(stderr, "ERROR: Unable to allocate memory for File structure.\n");
    return(NULL);
  }

  /* Some basic defaults */
  f->fh = NULL;  /* File is closed */
  f->lineno = 0;
  f->eof = 0;
  f->readerr = 0;
  f->line[0] = 0;

  if (NULL == (f->fh = fopen(filename, "r")))
  {
    fprintf(stderr, "ERROR: Unable to open \"%s\".\n", mid_trunc(NULL, filename, 40));
    return(NULL);
  }

  return(f);
}

/* ========================================================================= */
int next_line(File *f)
{
  if ( fgets(f->line, MAX_LINE_LEN, f->fh) )
  {
    f->lineno++;
    /* Fall through to success */
  }
  else
  {
    f->line[0] = 0;
    f->eof = 1;
    return(0);
  }

  /* Non-zero means keep on trucking... */
  return(1);
}

/* ========================================================================= */
int end_file(File *f)
{
  if ( f )
  {
    if ( f->fh )
      fclose(f->fh);

    free(f);

    return(0);
  }

  return(0);
}

/* ========================================================================= */
ParsePoint *new_parsepoint(void)
{
  ParsePoint *pp;

  if (NULL == (pp = (ParsePoint *)malloc(sizeof(ParsePoint))))
    return(NULL);

  /* Offset related values */
  pp->offset = 0;
  pp->otag = NULL;
  pp->opp = NULL;
  /* Size related values */
  pp->size = 0;
  pp->stag = NULL;
  pp->spp = NULL;
  /* The tag */
  pp->tag = NULL;
  /* The label */
  pp->label = NULL;
  /* Data & type */
  pp->data = NULL;
  pp->dt = DT_NULL;
  /* Line number related to this rule */
  pp->lineno = 0;
  /* Boolean flags for status */
  pp->tags_resolved = 0;
  pp->data_resolved = 0;
  /* Linked list pointer */
  pp->next = NULL;
  
  return(pp);
}

/* ========================================================================= */
#define MAX_TOKEN_LEN 64
/* STUB: You gotta pass the lineno! It is used to fill the pp, but also to
   STUB:    indicate where the error was found in the file. */
ParsePoint *get_parse_point(int lineno, char *line)
{
  ParsePoint *pp;
  char raw_offset[MAX_TOKEN_LEN];
  char raw_size[MAX_TOKEN_LEN];
  char raw_tag[MAX_TOKEN_LEN];
  char raw_label[MAX_TOKEN_LEN];
  char raw_dt[MAX_TOKEN_LEN];
  uint32_t nval;
  
  if ( strlen(line) <= 10 )
    return(NULL);
  

  if ( copy_out_nth_token(raw_offset, MAX_TOKEN_LEN, line, 1) )
    return(NULL);

  if ( copy_out_nth_token(raw_size, MAX_TOKEN_LEN, line, 2) )
    return(NULL);

  if ( copy_out_nth_token(raw_tag, MAX_TOKEN_LEN, line, 3) )
    return(NULL);

  if ( copy_out_nth_token(raw_label, MAX_TOKEN_LEN, line, 4) )
    return(NULL);

  if ( copy_out_nth_token(raw_dt, MAX_TOKEN_LEN, line, 5) )
    return(NULL);

  /* STUB: Now try 6, 7, 8, until it fails. Take each read
     STUB:   item and try to match it to an expected pattern such
     STUB:   as print, noprint, hex, decimal, etc... */
  
  /* If we got here, then we have sufficient data */
  if ( NULL == (pp = new_parsepoint()) )
  {
    /* We MUST exit on error here. The only means of failure is a bad
       malloc(). We cannot simply return NULL on a bad malloc(). We must
       throw and exception and exit. */
    exit(1);
  }

  /* Assume that the tags are resolved (because you may not even have tags.
     Set the tags_resolved flag to not-resolved (0) when a tag is inserted. */
  pp->tags_resolved = 1;
  
  /*** Copy everything into the ParsePoint ***/

  /* The lineno */
  pp->lineno = lineno;
  
  if ( isanynum(raw_offset) )
  {
    if ( str_to_uint32t(&nval, raw_offset) )
    {
      /* STUB: Some sort of error - was a num, but unparsable */
      fprintf(stderr, "Parse file error: Problems parsing a token.\n  Line: %d\n  Token: %s\n",
	      lineno, "Offset (1)");
      
      return(NULL);
    }
    
    pp->offset = nval;
  }
  else /* offset must be a tag */
  {
    if(NULL == (pp->otag = mkstring(raw_offset)))
    {
      fprintf(stderr, "ERROR: Failed to allocate memory for a string.");
      return(NULL);
    }
    /* We have an unresolved tag. Mark our tags as unresolved */
    pp->tags_resolved = 0;
  }

  if ( isanynum(raw_size) )
  {
    if ( str_to_uint32t(&nval, raw_size) )
    {
      /* STUB: Some sort of error - was a num, but unparsable */
      fprintf(stderr, "Parse file error: Problems parsing a token.\n  Line: %d\n  Token: %s\n",
	      lineno, "Size (1)");
      
      return(NULL);
    }
    
    pp->size = nval;
  }
  else /* size must be a tag */
  {
    if(NULL == (pp->stag = mkstring(raw_size)))
    {
      fprintf(stderr, "ERROR: Failed to allocate memory for a string.");
      return(NULL);
    }
    /* We have an unresolved tag. Mark our tags as unresolved */
    pp->tags_resolved = 0;
  }

  pp->tag = mkstring(raw_tag);
  pp->label = mkstring(raw_label);

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
    */
    return(NULL);
  }

  return(pp);
}

/* ========================================================================= */
RuleSet *ParseBPFFile(Options *o)
{
  RuleSet *rs;
  ParsePoint *pp;
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

  if ( NULL == ( rs = new_ruleset() ) )
    return(NULL);

  f = new_file(filename);

  /* Now start pulling lines */
  while(next_line(f))
  {
    /* ...for each line... */
    line = f->line;

    chomp(line);              /* Kill EOL chars */
    line = leadingwst(line);  /* Kill leading WS */
    hash_trunc(line);         /* Truncate Hash based comments */

    if ( NULL != (pp = get_parse_point(f->lineno, line)) )
    {
      add_pp_to_rs(rs, pp); /* This is just not going to fail */
      if ( o->bVerbose )
      {
	printf("  Added parse point rule named \"%s\".\n", pp->tag);
      }
      
      if ( o->bDebug )
      {
	/* Print verbose debuggery */
	printf("%03d:%s\n", f->lineno, line);
	printf(" pp->offset = %lu\n", pp->offset);
	printf(" pp->otag   = %s\n", pp->otag);
	printf(" pp->size   = %lu\n", pp->size);
	printf(" pp->stag   = %s\n", pp->stag);
	/* STUB: data type? */
	printf(" pp->tag    = %s\n", pp->tag);
	printf(" pp->label  = %s\n", pp->label);
      }
    }
  }
  
  /* Close the file */
  end_file(f);

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
  if ( pp->otag )
  {
    /* Offset is a tag (that needs to be resolved) */
    thispp = rs->pplist;
    while ( thispp )
    {
      if ( 0 == strcmp(pp->otag, thispp->tag) )
      {
	if ( pp == thispp )
	{
	  fprintf(stderr, "-------------------------------------------------------------------------------\n");
	  fprintf(stderr, "Tag resolution failure. Offset tag \"%s\" on line %d\n", pp->otag, pp->lineno);
	  fprintf(stderr, "   is self referential. A tagged offset can not reference the same line.\n");
	  return(1);
	}
	
	/* Fall through to success */
	pp->opp = thispp;
      }
      thispp = thispp->next;
    }

    if ( NULL == pp->opp )
    {
      fprintf(stderr, "STUB: This line is used to determine the default width of the terminal screen..\n");
      fprintf(stderr, "Tag resolution failure. Unable to resolve the offset tag \"%s\"\n", pp->otag);
      fprintf(stderr, "   for item \"%s\" on line %d.\n", pp->tag, pp->lineno);
      return(1);
    }
  }

  /*** Resolve the size tag next ***/
  if ( pp->stag )
  {
    /* Size is a tag (that needs to be resolved) */
    thispp = rs->pplist;
    while ( thispp )
    {
      if ( 0 == strcmp(pp->stag, thispp->tag) )
      {
	if ( pp == thispp )
	{
	  fprintf(stderr, "-------------------------------------------------------------------------------\n");
	  fprintf(stderr, "Tag resolution failure. Size tag \"%s\" on line %d\n", pp->stag, pp->lineno);
	  fprintf(stderr, "   is self referential. A tagged size can not reference the same line.\n");
	  return(1);
	}
	
	/* Fall through to success */
	pp->spp = thispp;
      }
      thispp = thispp->next;
    }

    if ( NULL == pp->spp )
    {
      fprintf(stderr, "STUB: This line is used to determine the default width of the terminal screen..\n");
      fprintf(stderr, "Tag resolution failure. Unable to resolve the size tag \"%s\"\n", pp->otag);
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
