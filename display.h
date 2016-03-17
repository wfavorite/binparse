#ifndef DISPLAY_H
#define DISPLAY_H

#include "bpdata.h"
#include "options.h"

/* =========================================================================
 * Name: DumpResults
 * Description: Print results of the data parsing to stdout
 * Paramaters:  RuleSet - The data
 *              Options - How to print the data
 * Returns: 0 on success, 1 on failure
 * Side Effects: Writes to stdout
 * Notes: Fairly straightforward. The devil is in the details (see comments
 *        within display.c for elaboration).
 */
int DumpResults(RuleSet *rs, Options *o);

#endif
