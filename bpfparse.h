#ifndef BPFPARSE_H
#define BPFPARSE_H

#include "options.h"
#include "bpdata.h"

/* =========================================================================
 * Name: ParseBPFFile
 * Desc: Parse a RuleSet out of a Binary Parse Format file
 * Params: Options struct
 * Returns: Top level RuleSet as parsed from the BPF file
 * Side Effects: Will allocate memory
 * Notes:
 */
RuleSet *ParseBPFFile(Options *o);

/* =========================================================================
 * Name: ResolveTags
 * Desc: Walk through the ParsePoint list and resolve all tags.
 * Params: The RuleSet as returned from ParseBPFFile()
 *         The Options as parsed in options.c/h
 * Returns: 0 on successful tag resolution
 * Side Effects:
 * Notes: Aka: 2nd pass resolution
 */
int ResolveTags(RuleSet *rs, Options *o);

/* =========================================================================
 * Name: ParseBPFOptions                                         (cenotaph)
 * Desc: Read option setting lines out of bpf file
 * Params:
 * Returns: 
 * Side Effects:
 * Notes: This is actually in options.h/c. This comment is here so you
 *        know where to find the code.
 */
#endif

