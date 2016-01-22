#include <stdio.h>

#include "pmath.h"
#include "penum.h"

#include "Version.h"

int main(int argc, char *argv[])
{
   Entity *e;

   printf("pmath test framework version %s starting.\n", VERSION_STRING);
   fflush(stdout);


   if ( NULL == (e = ParseEntity(argv[1])) )
      return(1);
   else
   {
      printf("Dumping entity tree____\n");
      DBG_dump_entity(0, e);
   }

   return(0);
}
