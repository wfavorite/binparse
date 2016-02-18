#ifndef BINPASS_H
#define BINPASS_H

#include "bpdata.h"
#include "options.h"

/* =========================================================================
 * Name: ResolveData
 * Description: Read through the bin file and find / resolve data
 * Paramaters: RuleSet
 *             Options
 * Returns: 0 on success, non-0 on failure
 * Side Effects: 
 * Notes: Resolve data is called "resolve" because some data can be 
 *        dependent on other data. If a tag is used in an offset or size
 *        field, then the dependent data must be resolved and retrieved
 *        before the data that depends on those tags (to other parse points).
 *        This is done in an iterative, not recursive manner. Discussions on
 *        the algo can be found in the source.
 */
int ResolveData(RuleSet *rs, Options *o);

#endif
