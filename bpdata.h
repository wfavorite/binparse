#ifndef BPDATA_H
#define BPDATA_H



/* This is the value used in bpfparse.c for get_parse_point() */
#define MAX_TOKEN_LEN 64


/* STUB: This is not a valid define name. Use what is set elsewhere. */
#define MAX_TAG_LEN 20

/* STUB: This is proposed. Needs to be properly evaluated */
#define MAX_EVP_NAME_LEN 16



/* ========================================================================= */
/* This is the native data type for all internal numerics such as stored
   (generic) values - (Think: offsets, sizes, and must= values.) It is not
   widely used at this time - But will be expanded to support all instances. */
typedef long BPInt;

/* ========================================================================= */
/* An Entity is a parsed item from a parse point line. (It is a numeric-ish
   value. Meaning: it is ultimately translatable to a numeric value (although
   it may be a string or equation.

   The Entity is the "grand container" for all items (of this sort). It can
   be a tag (to be resolved), a value, or an equation (to be solved). If an
   Entity consists of an expression (equation), then that expression is
   made up of two Entitys and an operation.

   The type determines what you can find in the union. It works like this:

   type           Union member      Contains
   ETYPE_NOTYP    None
   ETYPE_VALUE    u.value           Integer
   ETYPE_MEXPR    u.math            Pointer to Expression
   ETYPE_TAGCP    u.tag             Pointer to unresolved string identifier
   ETYPE_TAGRS    u.tag             Pointer to a parse point (the tag)
*/
#define ETYPE_NOTYP 0    /* No type - not set                                */
#define ETYPE_VALUE 1    /* The type is a value                              */
#define ETYPE_MEXPR 2    /* The type is an expression                        */
#define ETYPE_TAGCP 3    /* The type is a tag (char *)                       */
#define ETYPE_TAGRS 4    /* The type is a tag (resolved)                     */

typedef struct entity
{
   char *raw;
   int type;
   union
   {
      BPInt value;
      struct expression *math;
      void *tag;
   } u;
} Entity;

/* ========================================================================= */
/* An expression is a representation of the following:
      ( <LHS> <OPERATOR> <RHS> )
   Such as:
      ( 1 + 1 )
   Or:
      ( 1 * ( mytag * 2 ))

   The LHS and RHS of the expression are Entity(s) and therfore can recurse
   "all the way down".
*/
#define EXPOP_NOP   0     /* No op - initial/error state                     */
#define EXPOP_ADD   1     /* '+'                                             */
#define EXPOP_SUB   2     /* '-'                                             */
#define EXPOP_MUL   3     /* '*'                                             */
#define EXPOP_DIV   4     /* '/'                                             */

typedef struct expression
{
   struct entity *left;
   struct entity *right;
   int operation;
} Expression;
 


















typedef int EVType;

typedef struct envp
{
   EVType value;
   char *name;

   struct envp *next; /* Used to create a linked list... but also to 
                         flag the type of envp this is:
                           (next = NULL) ---> normal
                           (next = this) ---> default value
                      */
} ENVP;


typedef struct enumbase
{
   char *raw;
   char *tag; /* STUB: This does not need a malloc() could be static */
   struct envp *defval;  /* A single default value ENVP. Do NOT follow this
                            as a linked list. The linked list item is
                            self-referential. It is a way of detecting the
                            default value when it gets handled around. */
   struct envp *elist;   /* This is the list of ENVP (enum name value 
                            pairs) that are the non-default values. */
   struct enumbase *next;
} Enum;































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
   /* Field 1 - Offset */
   Entity *Offset;
   /* Field 2 - Size */
   Entity *Size;
   /* Field 3 - Tag */
   char *tag;               /* Shorthand name for this data item             */
   /* Field 4 - Label */
   char *label;             /* Printable name for this data item             */
   /* Field 5 - data type */
   unsigned long dt;        /* How to interpret the data item (define enum)  */
   /* Flags / options (the optional 6th field) */
   int print_result;
   Enum *use_enum;          /* The list of builtin enums                      */
   int muste;
   BPInt muste_val;


   void *data;              /* A pointer to the data item/type               */
   
   int lineno;              /* The line number of the "rule". Used for error
                               generation in the 2nd pass tag parsing.       */
   int fail_bail;           /* This is a valid parse point as returned from
                               get_parse_point() (or perhaps other functions)
                               if this is set, then exit with prejudice.     */
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
   Enum *elist;            /* The list of parsed enums                       */
   Enum *belist;           /* The list of builtin enums                      */
   

   int parserr;
   int pass;               /* The pass that was completed */
} RuleSet;


int CountParsePoints(RuleSet *rs);

#endif
