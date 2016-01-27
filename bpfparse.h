#ifndef BPFPARSE_H
#define BPFPARSE_H

#include "options.h"

/* STUB: This was from an earlier design. I have no clue what it would
   STUB:   be used for here. */
typedef struct EnumPair
{
   void *value;
   char *label;
} EnumPair;

typedef struct EnumList
{
   EnumPair *plist;
   char *deflabel;
} EnumList;










/*** Defines for the ParsePoint->dt value ***/
/* First used in get_parse_point() */
#define DT_NULL     0
#define DT_NONE     DT_NULL
#define DT_UINT8    1
#define DT_INT8     2
#define DT_UINT16   3
#define DT_INT16    4
#define DT_UINT32   5
#define DT_INT32    6
#define DT_UINT64   7
#define DT_INT64    8

#define DT_CHAR     20  /* Printable character (8 bits)    */
#define DT_ZTSTR    21  /* Zero terminated string          */
#define DT_FLSTR    22  /* Fixed length string             */

typedef struct ParsePoint
{
  unsigned long offset;    /* Valid if otag is NULL                         */
  char *otag;              /* Use offset if this is NULL                    */
  struct ParsePoint *opp;  /* Set this (2nd pass) if otag is not NULL       */
  unsigned long size;      /* This is really too big, but it is consistent  */
  char *stag;              /* Use size if this is NULL                      */
  struct ParsePoint *spp;  /* Set this (2nd pass) if stag is not NULL       */
  char *tag;               /* Shorthand name for this data item             */
  char *label;             /* Printable name for this data item             */
  void *data;              /* A pointer to the data item/type               */
  unsigned long dt;        /* How to interpret the data item (define enum)  */
  int lineno;              /* The line number of the "rule". Used for error
			      generation in the 2nd pass tag parsing.       */
  int tags_resolved;       /* Flag to determine if the tags are resolved    */
                           /* This is a required factor to completing the
			      second pass "compile" stage of the process.   */
  int data_resolved;       /* Flag to determine if the data has resolved    */
                           /* This is a required factor in the completion
			      of stage three (third pass compilation) where
			      the data dependencies are resolved.           */
  
  struct ParsePoint *next; /* Used to build the RuleSet->rulelist of pps    */
} ParsePoint;

typedef struct RuleSet
{
  ParsePoint *pplist;     /* The list of parsed rule data points            */
  int parserr;
  int pass;               /* The pass that was completed */

} RuleSet;

/* =========================================================================
 * Name: ParseBPFFile
 * Desc: Parse a RuleSet out of a Binary Parse Format file
 * Params:
 * Returns:
 * Side Effects:
 * Notes:
 */
RuleSet *ParseBPFFile(Options *o);

/* =========================================================================
 * Name: ResolveTags
 * Desc: Walk through the ParsePoint list and resolve all tags.
 * Params:
 * Returns: 0 on successful tag resolution
 * Side Effects:
 * Notes: Aka: 2nd pass resolution
 */
int ResolveTags(RuleSet *rs);

#endif
