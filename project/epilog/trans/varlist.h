/*
 * varlist.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_VARLIST_H
#define __EPI_VARLIST_H

#include <ident.h>
#include <simplelist.h>

typedef struct {
  char *name;
  ident_type *ident;
} var_type;

#define varlist_type simplelist_type

/* initialise list */
int varlist_init(varlist_type *list);

/* add variable entry into list. names are copied, but identifiers are 
 * assumed to have been dynamically allocated */
int varlist_add(varlist_type *list, char *name, ident_type *ident);

/* remove a variable from the list, given the name. (name is freed, identifier
 * is not */
int varlist_del(varlist_type *list, char *name);

/* returns 0 if the variable whose name is "name" is in the list */
int varlist_find(varlist_type list, char *name);

/* gives a reference to the identifer associated with the variable "name" */
int varlist_get(varlist_type list, char *name, ident_type **ident);

#endif
