/*
 * transdeflist.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_TRANSDEFLIST_H
#define __EPI_TRANSDEFLIST_H

#include <simplelist.h>
#include "transdef.h"

#define transdeflist_type simplelist_type

/* initialise list */
int transdeflist_init(transdeflist_type *list);

/* add into list given the name, varlist, precondition and postcondition */
int transdeflist_add_name(transdeflist_type *list,
                          char *name,
                          stringlist_type varlist,
                          expression_type precond,
                          expression_type postcond);

/* same as above, but with a transdef_type structure */
int transdeflist_add_trans(transdeflist_type *list, transdef_type trans);

/* gives the length of the list */
int transdeflist_length(transdeflist_type list, unsigned int *len);

/* gives a reference to a transdef_type structure */
int transdeflist_get_name(transdeflist_type list,
                          char *name,
                          transdef_type **trans);

/* same as above, but with index */
int transdeflist_get_index(transdeflist_type list,
                           unsigned int index,
                           transdef_type **trans);

/* returns 0 if the transformation name is in the list */
int transdeflist_find(transdeflist_type list, char *name);

/* empty the list */
int transdeflist_purge(transdeflist_type *list);

/* USE WITH CAUTION. as above, also purges varlist, precond and postcond */
int transdeflist_purge_all(transdeflist_type *list);

#endif
