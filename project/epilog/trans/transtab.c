/*
 * transtab.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <string.h>
#include <simplelist.h>
#include <comp_exp.h>
#include "transtab.h"

int transtab_compare(void *p1, void *p2);
void transtab_destroy(void *p);

static simplelist_type list;

/* initialise list */
void transtab_init(void)
{
  simplelist_init(&list);
}

/* gives the length of the list */
unsigned int transtab_length(void)
{
  return simplelist_length(list);
}

/* add transformation into table */
int transtab_add(transdef_type trans)
{
  transdef_type *new_trans = NULL;
  
  if (simplelist_find_data(list, (void *) &trans, transtab_compare) == 0)
    return -1;

  if (transdef_copy(&new_trans, trans) != 0)
    return -1;

  return simplelist_add(&list, (void *) new_trans);
}

/* gives a reference to a transdef_type structure */
int transtab_get(char *name, transdef_type **trans)
{
  simplelist_type tmp_list;
  transdef_type tmp_trans;

  if (name == NULL || trans == NULL)
    return -1;

  tmp_trans.name = name;

  if (simplelist_get_data(list, &tmp_list, (void *) &tmp_trans, transtab_compare) != 0)
    return -1;

  /* we know that there are no duplicates in this list so either the resulting
   * list is empty or the transformation we're looking for is the first and
   * only entry. */

  if (simplelist_get_index(tmp_list, 0, (void **) trans) != 0)
    return -1;

  return simplelist_del_index(&tmp_list, 0, NULL);
}

/* returns 0 if the transformation name is in the list */
int transtab_find(char *name)
{
  transdef_type tmp_trans;

  if ((tmp_trans.name = name) == NULL)
    return -1;

  return simplelist_find_data(list,
                              (void *) &tmp_trans,
                              transtab_compare);
}

/* gives the resulting expression after the transformation is performed */
int transtab_transform(gnd_exp_type kb, transref_type tr, gnd_exp_type *res)
{
  transdef_type *tmp_trans;
  gnd_atom_type *tmp_atom;
  gnd_exp_type tmp_precond;
  gnd_exp_type tmp_postcond;
  res_type tmp_res;
  unsigned int i;

  if (res == NULL)
    return -1;

  /* get a reference of the tranformation from the transtab */
  if (transtab_get(tr.name, &tmp_trans) != 0)
    return -1;
  
  /* replace the precondition of the transformation with the list of
   * identifiers provided when the tranformation was referenced */
  if (comp_exp_replace(tmp_trans->precond,
                       &tmp_precond,
                       tmp_trans->varlist,
                       tr.identlist) != 0)
    return -1;

  /* copy kb in res */
  if (gnd_exp_copy(kb, res) != 0)
    return -1;

  /* see if the precondition is true in the kb */
  if (gnd_exp_eval(tmp_precond, kb, &tmp_res) != 0)
    return -1;

  /* before doing anything else, do some cleanup */
  gnd_exp_purge(&tmp_precond);

  /* if the result is false or unknown we just return with a copy
   * of kb in res */
  if (tmp_res == epi_res_false || tmp_res == epi_res_unknown)
    return 0;

  /* at this point we know that the precondition holds in kb so we 
   * add the postcondition in res */
  
  /* replace the postcondition of the transformation with the list of
   * identifiers provided when the tranformation was referenced */
  if (comp_exp_replace(tmp_trans->postcond,
                       &tmp_postcond,
                       tmp_trans->varlist,
                       tr.identlist) != 0)
    return -1;

  /* now we individually add each atom in the postcondition to res */
  for (i = 0; i < gnd_exp_length(tmp_postcond); i++) {
    if (gnd_exp_get(tmp_postcond, i, &tmp_atom) != 0)
      return -1;
    if (gnd_exp_add(res, *tmp_atom) != 0)
      return -1;
  }

  gnd_exp_purge(&tmp_postcond);

  return 0;
}

/* empty the list */
void transtab_purge(void)
{
  unsigned int i;
  
  for (i = 0; i < simplelist_length(list); i++)
    simplelist_del_index(&list, 0, transtab_destroy);
}

/* compare ONLY THE NAME component of p1 and p2 */
int transtab_compare(void *p1, void *p2)
{
  if (p1 == NULL || p2 == NULL)
    return -1;

  return strcmp(((transdef_type *) p1)->name, ((transdef_type *) p2)->name) ? -1 : 0;
}

/* free memory */
void transtab_destroy(void *p)
{
  transdef_destroy((transdef_type *) p);
}
