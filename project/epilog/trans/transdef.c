/*
 * transdef.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <string.h>
#include "transdef.h"

/* creates a newly-allocated trans; copies and allocates name but only the
 * reference for varlist, precond and postcond */
int transdef_create(transdef_type **trans,
                    char *name,
                    stringlist_type varlist,
                    comp_exp_type precond,
                    comp_exp_type postcond)
{
  if (trans == NULL || name == NULL)
    return -1;

  if ((*trans = (transdef_type *) malloc(sizeof(transdef_type))) == NULL)
    return -1;

  if (((*trans)->name = (char *) malloc(sizeof(char) * (strlen(name) + 1))) == NULL) {
    free(*trans);
    return -1;
  }
  strcpy((*trans)->name, name);
  (*trans)->varlist = varlist;
  (*trans)->precond = precond;
  (*trans)->postcond = postcond;

  return 0;
}

/* same as above, but does not allocate new mem for the transdef_type struct */
int transdef_compose(transdef_type *trans,
                     char *name,
                     stringlist_type varlist,
                     comp_exp_type precond,
                     comp_exp_type postcond)
{
  if (trans == NULL || name == NULL)
    return -1;

  if ((trans->name = (char *) malloc(sizeof(char) * (strlen(name) + 1))) == NULL) 
    return -1;

  strcpy(trans->name, name);
  trans->varlist = varlist;
  trans->precond = precond;
  trans->postcond = postcond;

  return 0;
}

/* creates a copy of trans1 */
int transdef_copy(transdef_type **trans2, transdef_type trans1)
{
  if (trans2 == NULL)
    return -1;

  return transdef_create(trans2,
                      trans1.name,
                      trans1.varlist,
                      trans1.precond,
                      trans1.postcond);
}

/* frees this trans and all its members */
void transdef_destroy(transdef_type *trans)
{
  if (trans != NULL) {
    if (trans->name != NULL)
     free(trans->name);

    stringlist_purge(&(trans->varlist));
    comp_exp_purge(&(trans->precond));
    comp_exp_purge(&(trans->postcond));

    free(trans);
  }
}
