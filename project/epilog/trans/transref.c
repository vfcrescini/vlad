/*
 * transref.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <string.h>
#include "transref.h"

/* creates a newly-allocated trans; copies and allocates name but only the
 * reference for varlist, precond and postcond */
int transref_create(transref_type **trans,
                    char *name,
                    identlist_type identlist)
{
  if (trans == NULL || name == NULL)
    return -1;

  if ((*trans = (transref_type *) malloc(sizeof(transref_type))) == NULL)
    return -1;

  if (((*trans)->name = (char *) malloc(sizeof(char) * (strlen(name) + 1))) == NULL) {
    free(*trans);
    return -1;
  }
  strcpy((*trans)->name, name);
  (*trans)->identlist = identlist;

  return 0;
}

/* same as above, but does not allocate new mem for the transref_type struct */
int transref_compose(transref_type *trans,
                     char *name,
                     identlist_type identlist)
{
  if (trans == NULL || name == NULL)
    return -1;

  if ((trans->name = (char *) malloc(sizeof(char) * (strlen(name) + 1))) == NULL) 
    return -1;

  strcpy(trans->name, name);
  trans->identlist = identlist;

  return 0;
}

/* creates a copy of trans1 */
int transref_copy(transref_type **trans2, transref_type trans1)
{
  if (trans2 == NULL)
    return -1;

  return transref_create(trans2,
                      trans1.name,
                      trans1.identlist);
}

/* frees this trans and all its members */
void transref_destroy(transref_type *trans)
{
  if (trans != NULL) {
    if (trans->name != NULL)
     free(trans->name);

    identlist_purge(&(trans->identlist));

    free(trans);
  }
}
