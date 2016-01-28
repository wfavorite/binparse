#ifndef PDNUM_H
#define PENUM_H

/* STUB: This is not a valid define name. Use what is set elsewhere. */
#define MAX_TAG_LEN 20

/* STUB: This is proposed. Needs to be properly evaluated */
#define MAX_EVP_NAME_LEN 16


/* STUB: See formatting used in pmath.h. Utilize that here. */

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
   struct envp *defval;

   struct envp *elist;
} Enum;


/* =========================================================================
 * Name: 
 * Description: 
 * Paramaters: 
 * Returns: 
 * Side Effects: 
 * Notes: 
 */
/* STUB: The line number needs to be passed as an argument */
Enum *ParseEnum(char *estr);


/* =========================================================================
 * Name: 
 * Description: 
 * Paramaters: 
 * Returns: 0 if not an enum line, non-0 if (possibly) an enum line
 * Side Effects: None
 * Notes: 
 */
int IsEnumLine(char *estr);

#endif
