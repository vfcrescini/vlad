/*
 * identlist.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_IDENTLIST_H
#define __EPI_IDENTLIST_H

#include <simplelist.h>
#include "ident.h"

#define identlist_type simplelist_type

/* initialise list */
int identlist_init(identlist_type *list);

/* return 0 if name is already used */
int identlist_find(identlist_type list, char *name);

/* add identifier */
int identlist_add(identlist_type *list, char *name, unsigned short type);

/* get identifier structure based on name */
int identlist_get(identlist_type list, char *name, ident_type **ident);

/* delete identifier entry based on name */
int identlist_del(identlist_type *list, char *name);

/* delete all entries */
int identlist_purge(identlist_type *list);
	       
#endif
