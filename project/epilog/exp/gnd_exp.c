/* 
 * gnd_exp.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <identlist.h>
#include "gnd_exp.h"

int gnd_exp_cpy(void *p1, void **p2);
int gnd_exp_cmp_subst_group1(void *p1, void *p2);
int gnd_exp_cmp_subst_group2(void *p1, void *p2);
int gnd_exp_cmp_memb_element(void *p1, void *p2);
int gnd_exp_cmp_memb_group(void *p1, void *p2);
int gnd_exp_get_supergroups(ident_type ident,
                            identlist_type *list,
                            gnd_exp_type exp);
int gnd_exp_get_non_supergroups(ident_type ident,
                                identlist_type *list,
                                gnd_exp_type exp);
int gnd_exp_get_subgroups(ident_type ident,
                          identlist_type *list,
                          gnd_exp_type exp);
int gnd_exp_get_non_subgroups(ident_type ident,
                          identlist_type *list,
                          gnd_exp_type exp);
int gnd_exp_get_elements(ident_type ident,
                         identlist_type *list,
                         gnd_exp_type exp);
int gnd_exp_get_non_elements(ident_type ident,
                             identlist_type *list,
                             gnd_exp_type exp);
int gnd_exp_eval_atom(gnd_atom_type atom, gnd_exp_type exp, res_type *res);
int gnd_exp_eval_holds_atom(ident_type sub,
                            ident_type acc,
                            ident_type obj, 
                            gnd_exp_type exp,
                            res_type *res);
int gnd_exp_eval_memb_atom(ident_type element,
                           ident_type group,
                           gnd_exp_type exp,
                           res_type *res);
int gnd_exp_eval_subst_atom(ident_type group1,
                            ident_type group2,
                            gnd_exp_type exp,
                            res_type *res);
int gnd_exp_compare(void *p1, void *p2);
void gnd_exp_destroy(void *p);

/* initialise list */
void gnd_exp_init(gnd_exp_type *exp)
{
  simplelist_init(exp);
}

/* gives the number of atoms in the gnd_exp */
unsigned int gnd_exp_length(gnd_exp_type exp)
{
  return simplelist_length(exp);
}

/* return 0 if the atom is in the gnd_exp */
int gnd_exp_find(gnd_exp_type exp, gnd_atom_type atom)
{
  return simplelist_find_data(exp, (void *) &atom, gnd_exp_compare);
}

/* gives true, false or unknown depending on whether the expression or its
 * negation is in, or is a logical consequence of the other expression. */
int gnd_exp_eval(gnd_exp_type in, gnd_exp_type exp, res_type *res)
{
  unsigned int i;
  gnd_atom_type *tmp_atom;
  res_type tmp_res;

  if (res == NULL)
    return -1;

  *res = epi_res_true;

  for (i = 0; i < simplelist_length(in); i++) {
    if (simplelist_get_index(in, i, (void **) &tmp_atom) != 0)
      return -1;

    /* check if this atom is explicitly in exp */
    if (simplelist_find_data(exp, (void *) tmp_atom, gnd_exp_compare) == 0)
      continue;

    /* now check if the negation of this atom is explicitly in exp */
    EPI_ATOM_NEGATE(*tmp_atom); 
    if (simplelist_find_data(exp, (void *) tmp_atom, gnd_exp_compare) == 0) {
      EPI_ATOM_NEGATE(*tmp_atom); 
      *res = epi_res_false;
      return 0;
    }
    EPI_ATOM_NEGATE(*tmp_atom);
  
    /* now we check to see if the atom is a consequence of the other atoms */
    if (gnd_exp_eval_atom(*tmp_atom, exp, &tmp_res) != 0)
      return -1;

    /* if we see a false, the whole thing becomes false so there is no need
     * to continue further */
    if (tmp_res == epi_res_false) {
      *res = epi_res_false;
      return 0;
    }

    /* if unknown, continue evaluating the other atoms as a false might
     * still arise */
    if (tmp_res == epi_res_unknown)
      *res = epi_res_unknown;
  }

  return 0;
}

/* gives a reference to the index'th atom in the gnd_exp */
int gnd_exp_get(gnd_exp_type exp, unsigned int index, gnd_atom_type **atom)
{
  return simplelist_get_index(exp, index, (void **) atom);
}

/* add an atom into the gnd_exp */
int gnd_exp_add(gnd_exp_type *exp, gnd_atom_type atom)
{
  gnd_atom_type *new_atom = NULL;
  gnd_atom_type false_atom;

  if (exp == NULL)
    return -1;
  
  /* if an atom is already in simply return success */
  if (gnd_exp_find(*exp, atom) == 0)
    return 0;

  /* if the gnd_exp contains a FALSE constant, just return success */
  false_atom.type = EPI_ATOM_CONST;
  false_atom.truth = epi_false;

  if (gnd_exp_find(*exp, false_atom) == 0)
    return 0;

  /* if the negation of an atom is aleady in, or if the atom is a FALSE
   * constant, we replace the the whole gnd_exp with a constant false */
  
  EPI_ATOM_NEGATE(atom);
  if ((EPI_ATOM_IS_CONST(atom) && atom.truth == epi_true) ||
      gnd_exp_find(*exp, atom) == 0) {

    gnd_exp_purge(exp);

    if ((new_atom = EPI_GNDATOM_MALLOC) == NULL)
      return -1;
    if (gnd_atom_create_const(new_atom, epi_false) != 0)
      return -1;
  }
  else {
    if (gnd_atom_copy(&new_atom, atom) != 0)
      return -1;

    EPI_ATOM_NEGATE(*new_atom);
  }

  return simplelist_add(exp, (void *) new_atom);
}

/* delete an atom from the gnd_exp */
int gnd_exp_del(gnd_exp_type *exp, gnd_atom_type atom)
{
  if (exp == NULL)
    return -1;

  return simplelist_del_data(exp,
                             (void *) &atom, 
                             gnd_exp_compare,
                             gnd_exp_destroy);
}

/* copies exp1 to exp2 */
int gnd_exp_copy(gnd_exp_type exp1, gnd_exp_type *exp2)
{
  return simplelist_copy(exp1, exp2, gnd_exp_cpy);
}

/* delete all atoms from this gnd_exp */
void gnd_exp_purge(gnd_exp_type *exp)
{
  simplelist_purge(exp, gnd_exp_destroy);
}

/* makes a duplicate (with malloc) of the atom pointed by p1 to p2 */
int gnd_exp_cpy(void *p1, void **p2)
{
  if (p1 == NULL || p2 == NULL)
    return -1;

  return gnd_atom_copy((gnd_atom_type **) p2, * (gnd_atom_type *) p1);
}

/* returns 0 if the atoms pointed to by p1 and p2 are both subst and that
 * the group1 field matches */
int gnd_exp_cmp_subst_group1(void *p1, void *p2)
{
  gnd_atom_type *tmp_atom1 = NULL;
  gnd_atom_type *tmp_atom2 = NULL;

  if (p1 == NULL || p2 == NULL)
    return -1;

  tmp_atom1 = (gnd_atom_type *) p1;
  tmp_atom2 = (gnd_atom_type *) p2;

  if (tmp_atom1->type != tmp_atom2->type || !EPI_ATOM_IS_SUBST(*tmp_atom1))
    return -1;

  if (tmp_atom1->truth != tmp_atom2->truth)
    return -1;

  if (ident_compare(*(EPI_ATOM_SUBST_GROUP1(*tmp_atom1)),
                    *(EPI_ATOM_SUBST_GROUP1(*tmp_atom2))) != 0)
    return -1;

  return 0;
}
/* returns 0 if the atoms pointed to by p1 and p2 are both subst and that
 * the group2 field matches */
int gnd_exp_cmp_subst_group2(void *p1, void *p2)
{
  gnd_atom_type *tmp_atom1 = NULL;
  gnd_atom_type *tmp_atom2 = NULL;

  if (p1 == NULL || p2 == NULL)
    return -1;

  tmp_atom1 = (gnd_atom_type *) p1;
  tmp_atom2 = (gnd_atom_type *) p2;

  if (tmp_atom1->type != tmp_atom2->type || !EPI_ATOM_IS_SUBST(*tmp_atom1))
    return -1;

  if (tmp_atom1->truth != tmp_atom2->truth)
    return -1;

  if (ident_compare(*(EPI_ATOM_SUBST_GROUP2(*tmp_atom1)),
                    *(EPI_ATOM_SUBST_GROUP2(*tmp_atom2))) != 0)
    return -1;

  return 0;
}


/* returns 0 if the atoms pointed to by p1 and p2 are both memb and that
 * the element field matches */
int gnd_exp_cmp_memb_element(void *p1, void *p2)
{
  gnd_atom_type *tmp_atom1 = NULL;
  gnd_atom_type *tmp_atom2 = NULL;

  if (p1 == NULL || p2 == NULL)
    return -1;

  tmp_atom1 = (gnd_atom_type *) p1;
  tmp_atom2 = (gnd_atom_type *) p2;

  if (tmp_atom1->type != tmp_atom2->type || !EPI_ATOM_IS_MEMB(*tmp_atom1))
    return -1;

  if (tmp_atom1->truth != tmp_atom2->truth)
    return -1;

  if (ident_compare(*(EPI_ATOM_MEMB_ELEMENT(*tmp_atom1)), 
                    *(EPI_ATOM_MEMB_ELEMENT(*tmp_atom2))) != 0)
    return -1;

  return 0;
}

/* returns 0 if the atoms pointed to by p1 and p2 are both memb and that
 * the group field matches */
int gnd_exp_cmp_memb_group(void *p1, void *p2)
{
  gnd_atom_type *tmp_atom1 = NULL;
  gnd_atom_type *tmp_atom2 = NULL;

  if (p1 == NULL || p2 == NULL)
    return -1;

  tmp_atom1 = (gnd_atom_type *) p1;
  tmp_atom2 = (gnd_atom_type *) p2;

  if (tmp_atom1->type != tmp_atom2->type || !EPI_ATOM_IS_MEMB(*tmp_atom1))
    return -1;

  if (tmp_atom1->truth != tmp_atom2->truth)
    return -1;

  if (ident_compare(*(EPI_ATOM_MEMB_GROUP(*tmp_atom1)), 
                    *(EPI_ATOM_MEMB_GROUP(*tmp_atom2))) != 0)
    return -1;

  return 0;
}

/* assumes list has been initialised. gives a list of all identifiers that
 * contains ident */
int gnd_exp_get_supergroups(ident_type ident,
                            identlist_type *list,
                            gnd_exp_type exp)
{
  unsigned int i;
  gnd_atom_type tmp_atom;
  gnd_exp_type tmp_exp;
  ident_type *tmp_ident = NULL;
  gnd_atom_type *ptr_atom = NULL;

  if (list == NULL)
    return -1;

  tmp_atom.truth = epi_true;

  if (EPI_IDENT_IS_GROUP(ident)) {
    /* if it is a group, we have to look for subset atoms */
    tmp_atom.type = EPI_ATOM_SUBST;
    EPI_ATOM_SUBST_GROUP1(tmp_atom) = &ident;

    /* now search for atoms where in ident is a subset */
    simplelist_get_data(exp, &tmp_exp, (void *) &tmp_atom, gnd_exp_cmp_subst_group1); 
  }
  else {
    /* if it is a non-group, we have to look for memb atoms */
    tmp_atom.type = EPI_ATOM_MEMB;
    EPI_ATOM_MEMB_ELEMENT(tmp_atom) = &ident;

    /* now search for atoms where in ident is a memb */
    simplelist_get_data(exp, &tmp_exp, (void *) &tmp_atom, gnd_exp_cmp_memb_element);
  }

  /* now go through the resulting list and find their supersets */
  for (i = 0; i < simplelist_length(tmp_exp); i++) {
    if (simplelist_get_index(tmp_exp, i, (void **) &ptr_atom) != 0)
      return -1;

    /* get superset */
    if (EPI_IDENT_IS_GROUP(ident))
      tmp_ident = EPI_ATOM_SUBST_GROUP2(*ptr_atom);
    else
      tmp_ident = EPI_ATOM_MEMB_GROUP(*ptr_atom);

    /* see if we already have this identifier */
    if (identlist_find(*list, tmp_ident->name) == 0)
      continue;

    /* no? so we add it to our list */
    if (identlist_add(list, tmp_ident) != 0)
      return -1;

    /* now see if we can find supersets for this identifier */
    if (gnd_exp_get_supergroups(*tmp_ident, list, exp) != 0)
      return -1;
  }

  simplelist_purge(&tmp_exp, NULL);

  return 0;
}

/* assumes list has been initialised. gives a list of all group identifiers 
 * that DOES NOT contain ident */
int gnd_exp_get_non_supergroups(ident_type ident,
                                identlist_type *list,
                                gnd_exp_type exp)
{
  unsigned int i;
  gnd_atom_type tmp_atom;
  gnd_exp_type tmp_exp;
  ident_type *tmp_ident = NULL;
  gnd_atom_type *ptr_atom = NULL;

  if (list == NULL)
    return -1;

  /* phase 1: 
   * get all groups that does not contain ident and all groups inside them */

  tmp_atom.truth = epi_false;

  if (EPI_IDENT_IS_GROUP(ident)) {
    /* if it is a group, we have to look for subset atoms */
    tmp_atom.type = EPI_ATOM_SUBST;
    EPI_ATOM_SUBST_GROUP1(tmp_atom) = &ident;

    /* now search for atoms where in ident is a subset */
    simplelist_get_data(exp, &tmp_exp, (void *) &tmp_atom, gnd_exp_cmp_subst_group1); 
  }
  else {
    /* if it is a non-group, we have to look for memb atoms */
    tmp_atom.type = EPI_ATOM_MEMB;
    EPI_ATOM_MEMB_ELEMENT(tmp_atom) = &ident;

    /* now search for atoms where in ident is a memb */
    simplelist_get_data(exp, &tmp_exp, (void *) &tmp_atom, gnd_exp_cmp_memb_element);
  }

  /* now go through the resulting list and find their non-supersets */
  for (i = 0; i < simplelist_length(tmp_exp); i++) {
    if (simplelist_get_index(tmp_exp, i, (void **) &ptr_atom) != 0)
      return -1;

    /* get non-superset */
    if (EPI_IDENT_IS_GROUP(ident))
      tmp_ident = EPI_ATOM_SUBST_GROUP2(*ptr_atom);
    else
      tmp_ident = EPI_ATOM_MEMB_GROUP(*ptr_atom);

    /* see if we already have this identifier */
    if (identlist_find(*list, tmp_ident->name) == 0)
      continue;

    /* no? so we add it to our list */
    if (identlist_add(list, tmp_ident) != 0)
      return -1;

    /* now see if we can find groups inside this non-supergroup */
    if (gnd_exp_get_subgroups(*tmp_ident, list, exp) != 0)
      return -1;
  }

  simplelist_purge(&tmp_exp, NULL);

  /* phase 2: 
   * get all groups (group 1) that contain ident then find all groups (group 2)
   * that do not contain them (group 1), plus all the groups (group 3) inside 
   * them (group 2) */

  tmp_atom.truth = epi_true;
  tmp_atom.type = EPI_ATOM_SUBST;
  EPI_ATOM_SUBST_GROUP1(tmp_atom) = &ident;

  /* now search for atoms where in ident is a superset */
  simplelist_get_data(exp, &tmp_exp, (void *) &tmp_atom, gnd_exp_cmp_subst_group1);

  /* now go through the resulting list and find groups that do not contain them */
  for (i = 0; i < simplelist_length(tmp_exp); i++) {
    if (simplelist_get_index(tmp_exp, i, (void **) &ptr_atom) != 0)
      return -1;

    if (gnd_exp_get_non_supergroups(*(EPI_ATOM_SUBST_GROUP2(*ptr_atom)), list, exp) != 0)
     return -1;
  }

  simplelist_purge(&tmp_exp, NULL);

  return 0;
}

/* assumes list has been initialised. gives a list of all group identifiers 
 * that is contained in ident */
int gnd_exp_get_subgroups(ident_type ident,
                          identlist_type *list,
                          gnd_exp_type exp)
{
  unsigned int i;
  gnd_atom_type tmp_atom;
  gnd_exp_type tmp_exp;
  ident_type *tmp_ident = NULL;
  gnd_atom_type *ptr_atom = NULL;

  if (list == NULL)
    return -1;

  if (!EPI_IDENT_IS_GROUP(ident)) 
    return-1;

  tmp_atom.truth = epi_true;
  tmp_atom.type = EPI_ATOM_SUBST;
  EPI_ATOM_SUBST_GROUP2(tmp_atom) = &ident;

  /* now search for atoms where in ident is a superset */
  simplelist_get_data(exp, &tmp_exp, (void *) &tmp_atom, gnd_exp_cmp_subst_group2); 

  /* now go through the resulting list and find their supersets */
  for (i = 0; i < simplelist_length(tmp_exp); i++) {
    if (simplelist_get_index(tmp_exp, i, (void **) &ptr_atom) != 0)
      return -1;

    tmp_ident = EPI_ATOM_SUBST_GROUP1(*ptr_atom);

    /* see if we already have this identifier */
    if (identlist_find(*list, tmp_ident->name) == 0)
      continue;

    /* no? so we add it to our list */
    if (identlist_add(list, tmp_ident) != 0)
      return -1;

    /* now see if we can find subsets for this identifier */
    if (gnd_exp_get_subgroups(*tmp_ident, list, exp) != 0)
      return -1;
  }

  simplelist_purge(&tmp_exp, NULL);

  return 0;
}

/* assumes list has been initialised. gives a list of all group identifiers 
 * that IS NOT in ident */
int gnd_exp_get_non_subgroups(ident_type ident,
                              identlist_type *list,
                              gnd_exp_type exp)
{
  unsigned int i;
  gnd_atom_type tmp_atom;
  gnd_exp_type tmp_exp;
  ident_type *tmp_ident = NULL;
  gnd_atom_type *ptr_atom = NULL;

  if (list == NULL)
    return -1;

  if (!EPI_IDENT_IS_GROUP(ident))
    return -1;

  /* phase 1: 
   * get all groups that does not contain ident and all groups inside them */

  tmp_atom.truth = epi_false;
  tmp_atom.type = EPI_ATOM_SUBST;
  EPI_ATOM_SUBST_GROUP2(tmp_atom) = &ident;

  /* now search for atoms where in ident is a subset */
  simplelist_get_data(exp, &tmp_exp, (void *) &tmp_atom, gnd_exp_cmp_subst_group2); 

  /* now go through the resulting list and find their non-subsets */
  for (i = 0; i < simplelist_length(tmp_exp); i++) {
    if (simplelist_get_index(tmp_exp, i, (void **) &ptr_atom) != 0)
      return -1;

    /* get non-subset */
    tmp_ident = EPI_ATOM_SUBST_GROUP1(*ptr_atom);

    /* see if we already have this identifier */
    if (identlist_find(*list, tmp_ident->name) == 0)
      continue;

    /* no? so we add it to our list */
    if (identlist_add(list, tmp_ident) != 0)
      return -1;

    /* now see if we can find groups inside this non-subgroup */
    if (gnd_exp_get_subgroups(*tmp_ident, list, exp) != 0)
      return -1;
  }

  simplelist_purge(&tmp_exp, NULL);

  /* phase 2: 
   * get all groups (group 1) that are in ident then find all groups (group 2)
   * that are not in them (group 1), plus all the groups (group 3) inside 
   * them (group 2) */

  tmp_atom.truth = epi_true;
  tmp_atom.type = EPI_ATOM_SUBST;
  EPI_ATOM_SUBST_GROUP2(tmp_atom) = &ident;

  /* now search for atoms where in ident is a superset */
  simplelist_get_data(exp, &tmp_exp, (void *) &tmp_atom, gnd_exp_cmp_subst_group2);

  /* now go through the resulting list and find groups that do not contain them */
  for (i = 0; i < simplelist_length(tmp_exp); i++) {
    if (simplelist_get_index(tmp_exp, i, (void **) &ptr_atom) != 0)
      return -1;

    if (gnd_exp_get_non_subgroups(*(EPI_ATOM_SUBST_GROUP1(*ptr_atom)), list, exp) != 0)
     return -1;
  }

  simplelist_purge(&tmp_exp, NULL);

  return 0;
}

/* assumes list has been initialised. gives a list of all element identifiers 
 * that is contained in ident */
int gnd_exp_get_elements(ident_type ident,
                         identlist_type *list,
                         gnd_exp_type exp)
{
  unsigned int i;
  gnd_atom_type tmp_atom;
  gnd_exp_type tmp_exp;
  gnd_atom_type *ptr_atom = NULL;

  if (list == NULL)
    return -1;

  if (!EPI_IDENT_IS_GROUP(ident))
    return -1;

  tmp_atom.truth = epi_true;
  tmp_atom.type = EPI_ATOM_MEMB;
  EPI_ATOM_MEMB_GROUP(tmp_atom) = &ident;

  /* now search for atoms where in ident is a group */
  simplelist_get_data(exp, &tmp_exp, (void *) &tmp_atom, gnd_exp_cmp_memb_group);

  /* now go through the resulting list and find the elements */
  for (i = 0; i < simplelist_length(tmp_exp); i++) {
    if (simplelist_get_index(tmp_exp, i, (void **) &ptr_atom) != 0)
      return -1;

    /* see if we already have this identifier */
    if (identlist_find(*list, EPI_ATOM_MEMB_ELEMENT(*ptr_atom)->name) == 0)
      continue;

    /* no? so we add it to our list */
    if (identlist_add(list, EPI_ATOM_MEMB_ELEMENT(*ptr_atom)) != 0)
      return -1;
  }

  simplelist_purge(&tmp_exp, NULL);

  /* now see if we can find subsets for this identifier */
  tmp_atom.truth = epi_true;
  tmp_atom.type = EPI_ATOM_SUBST;
  EPI_ATOM_SUBST_GROUP2(tmp_atom) = &ident;

  /* search for atoms where in this ident is a sub group */
  simplelist_get_data(exp, &tmp_exp, (void *) &tmp_atom, gnd_exp_cmp_subst_group2);

  /* now go through the resulting list and find the elements */
  for (i = 0; i < simplelist_length(tmp_exp); i++) {
    if (simplelist_get_index(tmp_exp, i, (void **) &ptr_atom) != 0)
      return -1;

    /* finally, get the elements of this group */
    if (gnd_exp_get_elements(*EPI_ATOM_SUBST_GROUP1(*ptr_atom), list, exp) != 0)
      return -1;
  }

  simplelist_purge(&tmp_exp, NULL);

  return 0;
}

/* assumes list has been initialised. gives a list of all element identifiers 
 * that is NOT contained in ident */
int gnd_exp_get_non_elements(ident_type ident,
                             identlist_type *list,
                             gnd_exp_type exp)
{
  unsigned int i;
  gnd_atom_type tmp_atom;
  gnd_exp_type tmp_exp;
  gnd_atom_type *ptr_atom = NULL;
  identlist_type tmp_list;
  ident_type *ptr_ident = NULL;

  if (list == NULL)
    return -1;

  if (!EPI_IDENT_IS_GROUP(ident))
    return -1;

  tmp_atom.truth = epi_false;
  tmp_atom.type = EPI_ATOM_MEMB;
  EPI_ATOM_MEMB_GROUP(tmp_atom) = &ident;

  /* now search for atoms where in ident is a group */
  simplelist_get_data(exp, &tmp_exp, (void *) &tmp_atom, gnd_exp_cmp_memb_group);

  /* now go through the resulting list and find the elements */
  for (i = 0; i < simplelist_length(tmp_exp); i++) {
    if (simplelist_get_index(tmp_exp, i, (void **) &ptr_atom) != 0)
      return -1;

    /* see if we already have this identifier */
    if (identlist_find(*list, EPI_ATOM_MEMB_ELEMENT(*ptr_atom)->name) == 0)
      continue;

    /* no? so we add it to our list */
    if (identlist_add(list, EPI_ATOM_MEMB_ELEMENT(*ptr_atom)) != 0)
      return -1;
  }

  simplelist_purge(&tmp_exp, NULL);

  /* now we need to find the groups that do not contain this set (all
   * elements of those groups will be non-elements of this group) */

  identlist_init(&tmp_list);

  if (gnd_exp_get_non_supergroups(ident, &tmp_list, exp) != 0)
    return -1;

  /* then we go through the list of non-groups to get their respective
   * elements. result of which are non-elements of this group */

  for (i = 0; i < identlist_length(tmp_list); i++) {
    if (identlist_get(tmp_list, i, &ptr_ident) != 0)
      return -1;

    if (gnd_exp_get_elements(*ptr_ident, list, exp) != 0)
      return -1;
  }

  identlist_purge(&tmp_list);

  /* finally we need to find the groups that are not in this set (all
   * elements of those groups will be non-elements of this group) */

  identlist_init(&tmp_list);

  if (gnd_exp_get_non_subgroups(ident, &tmp_list, exp) != 0)
    return -1;

  /* then we go through the list of non-subgroups to get their respective
   * elements. result of which are non-elements of this group */

  for (i = 0; i < identlist_length(tmp_list); i++) {
    if (identlist_get(tmp_list, i, &ptr_ident) != 0)
      return -1;

    if (gnd_exp_get_elements(*ptr_ident, list, exp) != 0)
      return -1;
  }

  identlist_purge(&tmp_list);

  return 0;
}

/* gives true, false or unknown depending on whether holds(sub, acc, obj)
 * can be derived from exp or not. */
int gnd_exp_eval_holds_atom(ident_type sub,
                            ident_type acc,
                            ident_type obj, 
                            gnd_exp_type exp,
                            res_type *res)
{
  unsigned int i;
  unsigned int j;
  unsigned int k;
  gnd_atom_type tmp_atom;
  identlist_type tmp_sub;
  identlist_type tmp_acc;
  identlist_type tmp_obj;

  if (res == NULL)
    return -1;

  *res = epi_res_unknown;
  tmp_atom.truth = epi_true;
  tmp_atom.type = EPI_ATOM_HOLDS;

  identlist_init(&tmp_sub);
  identlist_init(&tmp_acc);
  identlist_init(&tmp_obj);

  /* get supergroups of all three identifiers */
  if (gnd_exp_get_supergroups(sub, &tmp_sub, exp) != 0)
    return -1;
  if (gnd_exp_get_supergroups(acc, &tmp_acc, exp) != 0)
    return -1;
  if (gnd_exp_get_supergroups(obj, &tmp_obj, exp) != 0)
    return -1;

  /* now go through all the possible combinations of all three lists to
   * see if we find a match */
  for (i = 0; i < identlist_length(tmp_sub); i++) {
    if (identlist_get(tmp_sub, i, &(EPI_ATOM_HOLDS_SUBJECT(tmp_atom))) != 0)
      return -1;
    for (j = 0; j < identlist_length(tmp_acc); j++) {
      if (identlist_get(tmp_acc, j, &(EPI_ATOM_HOLDS_ACCESS(tmp_atom))) != 0)
        return -1;
      for (k = 0; k < identlist_length(tmp_obj); k++) {
        if (identlist_get(tmp_obj, k, &(EPI_ATOM_HOLDS_OBJECT(tmp_atom))) != 0)
          return -1;
        if (simplelist_find_data(exp, (void *) &tmp_atom, gnd_exp_compare) == 0) { 
          *res = epi_res_true;
          identlist_purge(&tmp_sub);
          identlist_purge(&tmp_acc);
          identlist_purge(&tmp_obj);
          return 0;
        }
        /* also try the negation while we're here */
        EPI_ATOM_NEGATE(tmp_atom);
        if (simplelist_find_data(exp, (void *) &tmp_atom, gnd_exp_compare) == 0) { 
          *res = epi_res_false;
          identlist_purge(&tmp_sub);
          identlist_purge(&tmp_acc);
          identlist_purge(&tmp_obj);
          return 0;
        }
        EPI_ATOM_NEGATE(tmp_atom);
      }
    }
  }
  identlist_purge(&tmp_sub);
  identlist_purge(&tmp_acc);
  identlist_purge(&tmp_obj);

  return 0;
}

/* check to see if we can derive the truth value of memb. */
int gnd_exp_eval_memb_atom(ident_type element,
                           ident_type group,
                           gnd_exp_type exp,
                           res_type *res)
{
  identlist_type tmp_grp;

  if (res == NULL)
    return -1;

  *res = epi_res_unknown;
  identlist_init(&tmp_grp); 

  /* get all the elements of the group */
  if (gnd_exp_get_elements(group, &tmp_grp, exp) != 0)
    return -1;

  /* check for truthfulness */
  if (identlist_find(tmp_grp, element.name) == 0) {
    *res = epi_res_true;
    identlist_purge(&tmp_grp);
    return 0;
  }

  identlist_purge(&tmp_grp);

  /* get all the non-elements of the group */
  if (gnd_exp_get_non_elements(group, &tmp_grp, exp) != 0)
    return -1;

  /* check for falseness */
  if (identlist_find(tmp_grp, element.name) == 0) {
    *res = epi_res_false;
    identlist_purge(&tmp_grp);
    return 0;
  }

  identlist_purge(&tmp_grp);

  return 0;
}

int gnd_exp_eval_subst_atom(ident_type group1,
                            ident_type group2,
                            gnd_exp_type exp,
			    res_type *res)
{
  identlist_type tmp_grp;

  if (res == NULL)
    return -1;

  *res = epi_res_unknown;
  identlist_init(&tmp_grp);

  /* check for truthfulness */
  if (gnd_exp_get_supergroups(group1, &tmp_grp, exp) != 0)
    return -1;

  if (identlist_find(tmp_grp, group2.name) == 0) {
    *res = epi_res_true;
    identlist_purge(&tmp_grp);
    return 0;
  }
  
  identlist_purge(&tmp_grp);

  /* check for falseness */
  if (gnd_exp_get_non_supergroups(group1, &tmp_grp, exp) != 0)
      return -1;

  if (identlist_find(tmp_grp, group2.name) == 0) {
    *res = epi_res_false;
    identlist_purge(&tmp_grp);
    return 0;
  }

  identlist_purge(&tmp_grp);

  return 0;
}

/* gives true, false or unknown depending on whether atom or its negation 
 * can be derived from expression, or not. */
int gnd_exp_eval_atom(gnd_atom_type atom, gnd_exp_type exp, res_type *res)
{
  if (res == NULL)
    return -1;

  *res = epi_res_unknown;

  if (EPI_ATOM_IS_HOLDS(atom)) {
    if (gnd_exp_eval_holds_atom(*(EPI_ATOM_HOLDS_SUBJECT(atom)),
                                *(EPI_ATOM_HOLDS_ACCESS(atom)),
                                *(EPI_ATOM_HOLDS_OBJECT(atom)),
                                exp,
                                res) != 0)
      return -1;

    if (*res != epi_res_unknown)
      *res = (*res == atom.truth) ? epi_res_true : epi_res_false;
  }
  else if (EPI_ATOM_IS_MEMB(atom)) {
    if (gnd_exp_eval_memb_atom(*(EPI_ATOM_MEMB_ELEMENT(atom)),
                               *(EPI_ATOM_MEMB_GROUP(atom)),
                               exp,
                               res) != 0)
      return -1;

    if (*res != epi_res_unknown)
      *res = (*res == atom.truth) ? epi_res_true : epi_res_false;
  }
  else if (EPI_ATOM_IS_SUBST(atom)) {
    if (gnd_exp_eval_subst_atom(*(EPI_ATOM_SUBST_GROUP1(atom)),
                               *(EPI_ATOM_SUBST_GROUP2(atom)),
                               exp,
                               res) != 0)
      return -1;

    if (*res != epi_res_unknown)
      *res = (*res == atom.truth) ? epi_res_true : epi_res_false;
  }
  else if (EPI_ATOM_IS_CONST(atom)) {
    /* true is implied on any expression that is not false */
    *res = (atom.truth == epi_true) ? epi_res_true : epi_res_false;
  }

  return 0;
}

/* returns 0 if the ATOMS pointed to by p1 and p2 are equivalent */
int gnd_exp_compare(void *p1, void *p2)
{
  if (p1 == NULL || p2 == NULL)
    return -1;

  return gnd_atom_compare(* (gnd_atom_type *) p1, * (gnd_atom_type *) p2);
}

/* free memory */
void gnd_exp_destroy(void *p)
{
  gnd_atom_destroy((gnd_atom_type *) p);
}
