
#ifndef BINPASS_H
#define BINPASS_H

#include "bpdata.h"
#include "options.h"

int ResolveData(RuleSet *rs, Options *o);




/* STUB: These really belong in the .c file */
#define GEV_RESOLVED   0   /* The entity was resolved           */
#define GEV_DEPENDENT  1   /* Not resolved on dependency issues */
#define GEV_ERROR     -1   /* Not resolved on error             */

#endif






#ifdef STUB_REMOVE
int GetEntityValue(BPInt *out, Entity *ent);
#endif
