/*
 * varlist.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_VARLIST_H
#define __EPI_VARLIST_H

#include <ident.h>
#include <simplelist.h>

/* a list structure to hold variables. holds the variable name plus
 * an identifier placeholder (to be allocated externally) that can be
 * referenced by non-ground exresssions. */

typedef struct {
  char *name;
  ident_type *ident;
} var_type;

#define varlist_type simplelist_type

/* initialise list */
int varlist_init(varlist_type *list);

/* gives the length of the list */
int varlist_length(varlist_type list, unsigned int *len);

/* add variable entry into list. names are copied, but identifiers are 
 * assumed to have been dynamically allocated */
int varlist_add(varlist_type *list, char *name, ident_type *ident);

/* remove a variable from the list, given the name. (name is freed, identifier
 * is not */
int varlist_del(varlist_type *list, char *name);

/* returns 0 if the variable whose name is "name" is in the list */
int varlist_find(varlist_type list, char *name);

/* gives a reference to the identifer associated with the variable "name" */
int varlist_get_data(varlist_type list, char *name, ident_type **ident);

/* same as above, but with an index, also gives a reference to the entire
 * var_type structure */ 
int varlist_get_index(varlist_type list, unsigned int index, var_type **var);

/* destroy this varlist */
int varlist_purge(varlist_type *list);

#endif
