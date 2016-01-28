#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "support.h"

/* ========================================================================= */
void eat_ws(char **shand)
{
   /* Without the local copy, the compiler complained
      that the value was never used. It was pass by
      reference, and the C compiler did not figure that
      out. */
   char *str;

   str = *shand;

   /* Walk off leading white space */
   while ((*str == ' ')||(*str == '\t'))
      str++;

   *shand = str;
}


/* ========================================================================= */
/* STUB: This is a temp function that will be provided elsewhere.
   !!!!NOTE!!!!
   You do not check the return values on mkstring later in this code, and the
   more library-ish version will NOT print error messages!
*/
char *mkstring(char *cpin)
{
   char *cpout;

   if (NULL == cpin)
      return(NULL);

   if ( 0 == cpin[0] )
      return(NULL);

   if ( NULL == ( cpout = (char *)malloc(strlen(cpin) + 1) ) )
   {
      fprintf(stderr, "ERROR: Failed to allocate memory for a simple string.");
      return(NULL);
   }

   strcpy(cpout, cpin);

   return(cpout);
}
/* STUB: Yank to here */
