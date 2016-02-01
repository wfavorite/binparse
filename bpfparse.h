#ifndef BPFPARSE_H
#define BPFPARSE_H

#include "options.h"
#include "bpdata.h"

/* =========================================================================
 * Name: ParseBPFFile
 * Desc: Parse a RuleSet out of a Binary Parse Format file
 * Params:
 * Returns:
 * Side Effects:
 * Notes:
 */
RuleSet *ParseBPFFile(Options *o);

/* =========================================================================
 * Name: ResolveTags
 * Desc: Walk through the ParsePoint list and resolve all tags.
 * Params:
 * Returns: 0 on successful tag resolution
 * Side Effects:
 * Notes: Aka: 2nd pass resolution
 */
int ResolveTags(RuleSet *rs);

/* =========================================================================
 * Name: ParseBPFOptions                                         (cenotaph)
 * Desc: Read option setting lines out of bpf file
 * Params:
 * Returns: 
 * Side Effects:
 * Notes: This is actually in options.h/c
 */
#endif

