/*
 * translist.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_TRANSLIST_H
#define __EPI_TRANSLIST_H

#include <simplelist.h>
#include "trans.h"

#define translist_type simplelist_type

/* initialise list */
int translist_init(translist_type *list);

/* add into list given the name, varlist, precondition and postcondition */
int translist_add_name(translist_type *list,
                       char *name,
                       stringlist_type varlist,
                       expression_type precond,
                       expression_type postcond);

/* same as above, but with a trans_type structure */
int translist_add_trans(translist_type *list, trans_type trans);

/* gives the length of the list */
int translist_length(translist_type list, unsigned int *len);

/* gives a reference to a trans_type structure */
int translist_get_name(translist_type list,
                       char *name,
                       trans_type **trans);

/* same as above, but with index */
int translist_get_index(translist_type list,
                        unsigned int index,
                        trans_type **trans);

/* returns 0 if the transformation name is in the list */
int translist_find(translist_type list, char *name);

/* empty the list */
int translist_purge(translist_type *list);

/* USE WITH CAUTION. as above, also purges varlist, precond and postcond */
int translist_purge_all(translist_type *list);

#endif
