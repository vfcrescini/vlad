/*
 * symtab.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_SYMTAB_H
#define __EPI_SYMTAB_H

#include "ident.h"

/* 
 * a structure not unlike identlist. used to hold all the identifiers
 * that will ever be used. reasons for creating this instead of just
 * using indentlist :
 * 
 *  1. identlist may contain more than one instance of the same identifier
 *  2. this will be more convenient to use since the list variable is
 *     declared here.
 *  3. adding to this list requires allocating memory for the string, etc.
 *     in identlist, we can just add by giving a reference to a pre-allocated
 *     memory chunk.
 *  4. when this list is purged, we free everything, including the identifiers.
 */

/* initialise list */
int symtab_init(void);

/* return 0 if name is already in the list */
int symtab_find(char *name);

/* add identifier */
int symtab_add(char *name, unsigned short type);

/* get identifier structure based on name */
int symtab_get(char *name, ident_type **ident);

/* delete identifier entry based on name */
int symtab_del(char *name);

/* delete all entries and free everything */
void symtab_purge(void);

#endif
