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
int transtab_init(void)
{
  return simplelist_init(&list);
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
  int tmp_res;
  
  if (simplelist_find_data(list, (void *) &trans, transtab_compare) == EPI_OK)
    return EPI_DUPLICATE;

  if ((tmp_res = transdef_copy(&new_trans, trans)) != EPI_OK)
    return tmp_res;

  return simplelist_add(&list, (void *) new_trans);
}

/* gives a reference to a transdef_type structure */
int transtab_get(char *name, transdef_type **trans)
{
  simplelist_type tmp_list;
  transdef_type tmp_trans;
  int tmp_res;

  if (name == NULL || trans == NULL)
    return EPI_NULLPTR;

  tmp_trans.name = name;

  if ((tmp_res = simplelist_get_data(list, &tmp_list, (void *) &tmp_trans, transtab_compare)) != EPI_OK)
    return tmp_res;

  /* we know that there are no duplicates in this list so either the resulting
   * list is empty or the transformation we're looking for is the first and
   * only entry. */

  if ((tmp_res = simplelist_get_index(tmp_list, 0, (void **) trans)) != EPI_OK)
    return tmp_res;

  return simplelist_del_index(&tmp_list, 0, NULL);
}

/* returns 0 if the transformation name is in the list */
int transtab_find(char *name)
{
  transdef_type tmp_trans;

  if ((tmp_trans.name = name) == NULL)
    return EPI_NULLPTR;

  return simplelist_find_data(list,
                              (void *) &tmp_trans,
                              transtab_compare);
}

/* gives the resulting expression after the transformation is performed */
int transtab_transform(gnd_exp_type kb, transref_type tr, gnd_exp_type *ans)
{
  transdef_type *tmp_trans;
  gnd_atom_type *tmp_atom;
  gnd_exp_type tmp_precond;
  gnd_exp_type tmp_postcond;
  unsigned short int tmp_ans;
  unsigned int i;
  int tmp_res;

  if (ans == NULL)
    return EPI_NULLPTR;

  /* get a reference of the tranformation from the transtab */
  if ((tmp_res = transtab_get(tr.name, &tmp_trans)) != EPI_OK)
    return tmp_res;
  
  /* replace the precondition of the transformation with the list of
   * identifiers provided when the tranformation was referenced */
  if ((tmp_res = comp_exp_replace(tmp_trans->precond,
                                  &tmp_precond,
                                  tmp_trans->varlist,
                                  tr.identlist)) != EPI_OK)
    return tmp_res;

  /* copy kb in ans */
  if ((tmp_res = gnd_exp_copy(kb, ans)) != EPI_OK)
    return tmp_res;

  /* see if the precondition is true in the kb */
  if ((tmp_res = gnd_exp_eval(tmp_precond, kb, &tmp_ans)) != EPI_OK)
    return tmp_res;

  /* before doing anything else, do some cleanup */
  gnd_exp_purge(&tmp_precond);

  /* if the result is false or unknown we just return with a copy
   * of kb in ans */
  if (tmp_ans == EPI_RESULT_FALSE || tmp_ans == EPI_RESULT_UNKNOWN)
    return EPI_OK;

  /* at this point we know that the precondition holds in kb so we 
   * add the postcondition in ans */
  
  /* replace the postcondition of the transformation with the list of
   * identifiers provided when the tranformation was referenced */
  if ((tmp_res = comp_exp_replace(tmp_trans->postcond,
                                  &tmp_postcond,
                                  tmp_trans->varlist,
                                  tr.identlist)) != EPI_OK)
    return tmp_res;

  /* now we individually add each atom in the postcondition to ans */
  for (i = 0; i < gnd_exp_length(tmp_postcond); i++) {
    if ((tmp_res = gnd_exp_get(tmp_postcond, i, &tmp_atom)) != EPI_OK)
      return tmp_res;
    if ((tmp_res = gnd_exp_add(ans, *tmp_atom)) != EPI_OK)
      return tmp_res;
  }

  gnd_exp_purge(&tmp_postcond);

  return EPI_OK;
}

/* empty the list */
void transtab_purge(void)
{
  simplelist_purge(&list, transtab_destroy);
}

/* compare ONLY THE NAME component of p1 and p2 */
int transtab_compare(void *p1, void *p2)
{
  if (p1 == NULL || p2 == NULL)
    return EPI_NULLPTR;

  return strcmp(((transdef_type *) p1)->name, ((transdef_type *) p2)->name) ? EPI_FAILURE : EPI_OK;
}

/* free memory */
void transtab_destroy(void *p)
{
  transdef_destroy((transdef_type *) p);
}
