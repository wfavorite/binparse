#ifndef BPDATA_H
#define BPDATA_H

/* This is the value used in bpfparse.c for get_parse_point() */
#define MAX_TOKEN_LEN 64


/* This is the length of a tag. It is used explicity in Enum parsing.
   It is NOT used in ParsePoint parsing. ParsePoint tags default to
   MAX_TOKEN_LEN size. It is used in enum= parsing. */
#define MAX_TAG_LEN 20

/* ========================================================================= */
/* This is the maximum length of the string in a enum *name*-value pair.     */
/* Used in penum.c::parse_envp_name()                                        */
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
#define ETYPE_TAGPP 4    /* The type is a tag (resolved to a parse point)    */
#define ETYPE_TAGET 5    /* The type is a tag (resolved to an explicit tag)  */

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

/* ========================================================================= */
/* ENVP - Enum Name Value Pair

   This is a struct that ties a value to the string to represent that value
   in output. (It is a single item in an enum.

   This can also represent the default value if next points to the struct
   that next is in. (If it is self referential.) This does not qualify me
   as a genius... it leans more towards making me look like the fool that
   I am. This is a circular linked list of one. So it is easy to test for
   but it is also easy to walk a linked list that never ends. Be careful.  */
typedef struct envp
{
   BPInt value;       /* The value to match to activate the enum. Note
                         that the value is ignored when it is the
                         "default" value. */
   char *name;        /* This is the string to represent this specific
                         value when printing. */
   struct envp *next; /* Used to create a linked list... but also to 
                         flag the type of envp this is:
                           (next = NULL) ---> normal
                           (next = this) ---> default value
                      */
} ENVP;

/* ========================================================================= */
/* Enum - A complete enum structure

   The ENVP only has a name and value pair. An enum has (potentially) MANY
   name value pairs. This struct holds them.... By holding a list of ENVPs.

   The defval holds a special ENVP that has no value (beacuse the value
   is any value not explicitly mentioned in the enum. The next pointer in
   defval ENVP points to itself.

   A default value is required.

   next is used to link all the enums together. There are two lists of them.
   One is the builtin list. It consists of enums that are included in the
   build. The other list is those parsed from the BPF file.                  */
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

/* ========================================================================= */
/* This is a STUBbed version of an explicit tag */
/* Explicit tags allow you to define a value in the BPF file and then use
   it referentially throught the file.
   
   The rules for this have not been defined at this time, it is expected 
   that it will work like any other entity.                                  */
typedef struct etag
{
  char *tag;
  Entity *e;
  struct etag *next;
  
} ExplicitTag;

/* ========================================================================= */
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
   /** Field 1 - Offset */
   Entity *Offset;
   BPInt rOffset;   /* Resolved (cached) Offset */
   /** Field 2 - Size */
   Entity *Size;
   BPInt rSize;   /* Resolved (cached) Size */
   /** Field 3 - Tag */
   char *tag;               /* Shorthand name for this data item             */
   /** Field 4 - Label */
   char *label;             /* Printable name for this data item             */
   /** Field 5 - data type */
   unsigned long dt;        /* How to interpret the data item (define enum)  */
   /** Flags / options (the optional 6th field) */
   int print_result;        /* To print results or not. 1=print, 0=not.      */
   Enum *use_enum;          /* Resolved enum to use to decode                */
   char *enum_tag;          /* Unresolved enum tag to use to decode          */
   int   use_muste;         /* Boolean --> 1 use must=, 0 do not use must=   */
   BPInt muste_val;         /* The value to use for must= directives         */
   int   use_mask;          /* Boolean --> 1 use mask=, 0 do not use mask=   */
   BPInt mask_val;          /* The value to use for mask= directives         */

   /** Data and metadata */
   void *data;              /* A pointer to the data item/type               */
   BPInt rdata;             /* Cached, standardized, copy of the int data    */ 
  
   int lineno;              /* The line number of the "rule". Used for error
                               generation in the 2nd pass tag parsing.       */
   int fail_bail;           /* This is a valid parse point as returned from
                               get_parse_point() (or perhaps other functions)
                               if this is set, then exit with prejudice.     */
   int tags_resolved;       /* Flag to determine if the tags are resolved    */
                            /* This is a required factor to completing the
       	                       second pass "compile" stage of the process.   */
   int rtag_count;          /* The number of resolved tags (for stats)       */
   int data_resolved;       /* Flag to determine if the data has resolved    */
                            /* This is a required factor in the completion
                               of stage three (third pass compilation) where
                               the data dependencies are resolved.           */
  
   struct ParsePoint *next; /* Used to build the RuleSet->rulelist of pps    */
} ParsePoint;

/* ========================================================================= */
/* This is the data structure that ties it all together.                     */
typedef struct RuleSet
{
   ParsePoint *pplist;     /* The list of parsed rule data points            */
   Enum *elist;            /* The list of parsed enums                       */
   Enum *belist;           /* The list of builtin enums                      */
   ExplicitTag *etlist;    /* The list of explicit tags                      */

   int parserr;
   int pass;               /* The pass that was completed */

   /* (Bin) File related data */
   int f;                  /* File descriptor                                */
   char *fname;            /* File name (used for open())                    */
   BPInt fsize;            /* Size of the file                               */
   int bESwap;             /* Swap endian on supported types                 */
} RuleSet;

/* =========================================================================
 * Name: Count*
 * Desc: Walk the list of * and count the number
 * Params: RuleSet
 * Returns: The count of ParsePoints in the RuleSet
 * Side Effects: None
 * Notes: 
 */
int CountParsePoints(RuleSet *rs);
int CountParsedEnums(RuleSet *rs);
int CountBuiltinEnums(RuleSet *rs);
int CountExplicitTags(RuleSet *rs);

/* =========================================================================
 * Name: ParseBPInt
 * Desc: Read a numeric in a string (dec or hex) and put it in a BPInt
 * Params: BPInt to fill
 *         char * to read from
 * Returns: 0 on success, non-0 on failure
 * Side Effects: 
 * Notes: 
 */
int ParseBPInt(BPInt *val, char *str);

/* =========================================================================
 * Name: SetBPIntFromVoid
 * Desc: "Copy" the data value to rdata (a standardized form) & apply mask
 * Params: The ParsePoint *THAT HAS THE data FIELD FILLED*
 * Returns: 0 on success, non-0 on failure
 * Side Effects: Modifies data items in the ParsePoint
 * Notes: 
 */
int SetBPIntFromVoid(ParsePoint *pp);

/* =========================================================================
 * Name: *PPDataResolved
 * Desc: Settor and tester of flags for data resolved in Parse Point
 * Params: ParsePoint - The pp to set or check
 * Returns: Current value on set, boolean on test
 * Side Effects: Settor will modify the data resolved value of the pp
 * Notes: These are simple settor and gettor functions for the data
 *        resolved flags. This is used in the third pass (reading data from
 *        the file). It keeps track of what is done, and what is yet to be
 *        done.
 */
#define DR_OFFSET 1   /* Offset is known (resolved)   */
#define DR_SIZE   2   /* Size is known (resolved)     */
#define DR_DATA   4   /* Data is read (retrieved)     */

int SetPPDataResolved(ParsePoint *pp, int flag);
int IsPPDataResolved(ParsePoint *pp, int flag);

#endif
