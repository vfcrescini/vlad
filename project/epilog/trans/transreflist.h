/*
 * transreflist.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_TRANSREFLIST_H
#define __EPI_TRANSREFLIST_H

#include <simplelist.h>
#include "transref.h"

#define transreflist_type simplelist_type

/* initialise list */
int transreflist_init(transreflist_type *list);

/* add into list given the transref_type structure */
int transreflist_add(transreflist_type *list, transref_type trans);

/* gives the length of the list */
int transreflist_length(transreflist_type list, unsigned int *len);

/* gives ar reference to the index'th transref */
int transreflist_get_index(transreflist_type list,
                        unsigned int index,
                        transref_type **trans);

/* empty the list along with its members */
int transreflist_purge(transreflist_type *list);

#endif
