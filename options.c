#include <stdlib.h>
#include <stdio.h>

#include "options.h"
#include "strlib.h"

Options *new_options(void)
{
  Options *o;

  if ( NULL == (o = (Options *)malloc(sizeof(Options))) )
  {
    fprintf(stderr, "ERROR: Unable to allocate memory for Options.\n");
    return(NULL);
  }

  /* Set defaults */
  o->bDebug = 0;
  o->bVerbose = 0;
  o->bHelp = 0;
  o->bAbout = 0;
  o->bpffile = NULL;

  /* Send it off */
  return(o);
}




Options *ParseOptions(int argc, char *argv[])
{
  Options *o;

  if ( NULL == ( o = new_options() ) )
    return(NULL);
  
  /* STUB: Parse the options */
  
  /* STUB: This is where I tweak options (no CL parsing code at this time) */
  o->bpffile = nc_mkstring("sample.bpf");
  o->bVerbose = 1;
  //o->bHelp = 1;



  /* STUB: Validate the options */
  



  return(o);
}
