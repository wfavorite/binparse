#include <stdio.h>

#include "pmath.h"
#include "penum.h"

#include "version.h"

int main(int argc, char *argv[])
{
   Enum *en;
   ENVP *nvp;

   printf("penum test framework version %s starting.\n", VERSION_STRING);
   fflush(stdout);

   if ( NULL == (en = ParseEnum(argv[1], 1)) )
      return(1);
   else
   {
      printf("ENUM:\n");
      printf("  tag = \"%s\"\n", en->tag);
      printf("  default = \"%s\"\n", en->defval->name);

      nvp = en->elist;
      while ( nvp )
      {
         printf("  NVPair: %ld ---> \"%s\"\n", nvp->value, nvp->name);
         nvp = nvp->next;
      }

      fflush(stdout);
   }

   return(0);
}
