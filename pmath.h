#ifndef PMATH_H
#define PMATH_H

/* STUB: This is not a valid define name. Use what is set elsewhere. */
#define MAX_TAG_LEN 20

/* ========================================================================= */
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
      long value;
      struct expression *math;
      void *tag;
   } u;
} Entity;

/* ========================================================================= */
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
Entity *ParseEntity(char *str);

/* ========================================================================= */
void DBG_dump_entity(int r, Entity *e);

#endif
