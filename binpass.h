
#ifndef BINPASS_H
#define BINPASS_H

#include "bpdata.h"
#include "options.h"

int ResolveData(RuleSet *rs, Options *o);





#define GEV_RESOLVED   0   /* The entity was resolved           */
#define GEV_DEPENDENT  1   /* Not resolved on dependency issues */
#define GEV_ERROR     -1   /* Not resolved on error             */
int GetEntityValue(BPInt *out, Entity *ent);


#endif
