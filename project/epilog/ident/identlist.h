/*
 * identlist.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_IDENTLIST_H
#define __EPI_IDENTLIST_H

#include "ident.h"

/* initialise list */
int identlist_init(void);

/* return 0 if name is already used */
int identlist_find(char *name);

/* add identifier */
int identlist_add(char *name, unsigned short type);

/* get identifier structure based on name */
int identlist_get(char *name, ident_type **ident);

/* delete identifier entry based on name */
int identlist_del(char *name);

/* delete all entries */
int identlist_purge(void);
	       
#endif
