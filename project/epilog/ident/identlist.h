/*
 * identlist.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_IDENTLIST_H
#define __EPI_IDENTLIST_H

#include <simplelist.h>
#include "ident.h"

/* 
 * structure to hold identifiers. allows identifiers to be added more than 
 * once. added identifiers are assumed to have been dynamically allocated.
 */

#define identlist_type simplelist_type

/* initialise list */
void identlist_init(identlist_type *list);

/* gives the number of identifiers in the list */
unsigned int identlist_length(identlist_type list);

/* return 0 if name is already used */
int identlist_find(identlist_type list, char *name);

/* add identifier */
int identlist_add(identlist_type *list, ident_type *ident);

/* get identifier structure based on index */
int identlist_get(identlist_type list, unsigned int index, ident_type **ident);

/* delete identifier entry based on index but not free the indentifier */
int identlist_del(identlist_type *list, unsigned int index);

/* delete all entries but not free the identifiers */
void identlist_purge(identlist_type *list);

#endif
