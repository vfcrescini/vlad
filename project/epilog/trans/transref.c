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
    return EPI_NULLPTR;

  if ((*trans = EPI_ADT_MALLOC(transref_type)) == NULL)
    return EPI_MALLOCFAILED;

  if (((*trans)->name = EPI_STRING_MALLOC(name)) == NULL) {
    free(*trans);
    return EPI_MALLOCFAILED;
  }
  strcpy((*trans)->name, name);
  (*trans)->identlist = identlist;

  return EPI_OK;
}

/* same as above, but does not allocate new mem for the transref_type struct */
int transref_compose(transref_type *trans,
                     char *name,
                     identlist_type identlist)
{
  if (trans == NULL || name == NULL)
    return EPI_NULLPTR;

  if ((trans->name = EPI_STRING_MALLOC(name)) == NULL) 
    return EPI_MALLOCFAILED;

  strcpy(trans->name, name);
  trans->identlist = identlist;

  return EPI_OK;
}

/* creates a copy of trans1 */
int transref_copy(transref_type **trans2, transref_type trans1)
{
  if (trans2 == NULL)
    return EPI_NULLPTR;

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
