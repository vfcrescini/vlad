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
gnd_atom_type *gnd_exp_get_ref(gnd_atom_type atom, gnd_exp_type exp);
int gnd_exp_get_supergroups(ident_type ident,
                            identlist_type *list,
                            gnd_exp_type exp,
                            int immediate);
int gnd_exp_get_non_supergroups(ident_type ident,
                                identlist_type *list,
                                gnd_exp_type exp,
                                int immediate);
int gnd_exp_get_subgroups(ident_type ident,
                          identlist_type *list,
                          gnd_exp_type exp,
                          int immediate);
int gnd_exp_get_non_subgroups(ident_type ident,
                          identlist_type *list,
                          gnd_exp_type exp,
                          int immediate);
int gnd_exp_get_elements(ident_type ident,
                         identlist_type *list,
                         gnd_exp_type exp,
                         int immediate);
int gnd_exp_get_non_elements(ident_type ident,
                             identlist_type *list,
                             gnd_exp_type exp,
                             int immediate);
int gnd_exp_eval_atom(gnd_atom_type atom,
                      gnd_exp_type exp,
                      unsigned short int *ans);
int gnd_exp_eval_holds_atom(ident_type sub,
                            ident_type acc,
                            ident_type obj, 
                            gnd_exp_type exp,
                            unsigned short int *ans);
int gnd_exp_eval_memb_atom(ident_type element,
                           ident_type group,
                           gnd_exp_type exp,
                           unsigned short int *ans);
int gnd_exp_eval_subst_atom(ident_type group1,
                            ident_type group2,
                            gnd_exp_type exp,
                            unsigned short int *ans);
int gnd_exp_compare(void *p1, void *p2);
void gnd_exp_destroy(void *p);

/* initialise list */
int gnd_exp_init(gnd_exp_type *exp)
{
  return simplelist_init(exp);
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
int gnd_exp_eval(gnd_exp_type in, gnd_exp_type exp, unsigned short int *ans)
{
  unsigned int i;
  gnd_atom_type *tmp_atom;
  unsigned short int tmp_ans;
  int tmp_res;

  if (ans == NULL)
    return EPI_NULLPTR;

  *ans = EPI_RESULT_TRUE;

  for (i = 0; i < simplelist_length(in); i++) {
    if ((tmp_res = simplelist_get_index(in, i, (void **) &tmp_atom)) != EPI_OK)
      return tmp_res;

    /* check if this atom is explicitly in exp */
    if (simplelist_find_data(exp, (void *) tmp_atom, gnd_exp_compare) == EPI_OK)
      continue; 

    /* now check if the negation of this atom is explicitly in exp */
    EPI_ATOM_NEGATE(*tmp_atom); 
    if (simplelist_find_data(exp, (void *) tmp_atom, gnd_exp_compare) == EPI_OK) {
      EPI_ATOM_NEGATE(*tmp_atom); 
      *ans = EPI_RESULT_FALSE;
      return EPI_OK;
    }
    EPI_ATOM_NEGATE(*tmp_atom);
  
    /* now we check to see if the atom is a consequence of the other atoms */
    if ((tmp_res = gnd_exp_eval_atom(*tmp_atom, exp, &tmp_ans)) != EPI_OK)
      return tmp_res;

    /* if we see a false, the whole thing becomes false so there is no need
     * to continue further */
    if (tmp_ans == EPI_RESULT_FALSE) {
      *ans = EPI_RESULT_FALSE;
      return EPI_OK;
    }

    /* if unknown, continue evaluating the other atoms as a false might
     * still arise */
    if (tmp_ans == EPI_RESULT_UNKNOWN)
      *ans = EPI_RESULT_UNKNOWN;
  }

  return EPI_OK;
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
  gnd_atom_type negated_atom;
  unsigned short int tmp_ans;
  int tmp_res;

  if (exp == NULL)
    return EPI_NULLPTR;

  gnd_atom_create_const(&false_atom, EPI_FALSE);
  gnd_atom_copy(&negated_atom, atom);
  EPI_ATOM_NEGATE(negated_atom);
  
  /* if an atom is already in simply return success */
  if (gnd_exp_find(*exp, atom) == EPI_OK)
    return EPI_OK;

  /* if the gnd_exp contains a FALSE constant, just return success */
  if (gnd_exp_find(*exp, false_atom) == EPI_OK)
    return EPI_OK;

  /* now we check if the negation of the atom is a logical consequence 
   * of the other atoms in the expression */
  if ((tmp_res = gnd_exp_eval_atom(negated_atom, *exp, &tmp_ans)) != EPI_OK)
    return tmp_res;

  /* the negation of the atom is implied by the other atoms in the
   * expression. this is a conflict */
  if (tmp_ans == EPI_RESULT_TRUE)
    return EPI_NEGIMPLIED;

  /* if the negation of the atom is already in, but is not implied by
   * any of the other atoms, we replace it. */
  if ((new_atom = gnd_exp_get_ref(negated_atom, *exp)) != NULL) {
    EPI_ATOM_NEGATE(*new_atom); 
    return EPI_OK;
  }

  if ((EPI_ATOM_IS_CONST(atom) && atom.truth == EPI_FALSE)) {
    /* if the atom is a FALSE constant, we replace the whole expression
     * with a constant false */
    gnd_exp_purge(exp);

    if ((new_atom = EPI_GNDATOM_MALLOC) == NULL)
      return EPI_MALLOCFAILED;
    if ((tmp_res = gnd_atom_create_const(new_atom, EPI_FALSE)) != EPI_OK)
      return tmp_res;
  }
  else {
    /* all clear. just add this new atom in */
    if ((tmp_res = gnd_atom_dup(&new_atom, atom)) != 0)
      return tmp_res;
  }

  return simplelist_add(exp, (void *) new_atom);
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
    return EPI_NULLPTR;

  return gnd_atom_dup((gnd_atom_type **) p2, * (gnd_atom_type *) p1);
}

/* returns 0 if the atoms pointed to by p1 and p2 are both subst and that
 * the group1 field matches */
int gnd_exp_cmp_subst_group1(void *p1, void *p2)
{
  gnd_atom_type *tmp_atom1 = NULL;
  gnd_atom_type *tmp_atom2 = NULL;

  if (p1 == NULL || p2 == NULL)
    return EPI_NULLPTR;

  tmp_atom1 = (gnd_atom_type *) p1;
  tmp_atom2 = (gnd_atom_type *) p2;

  if (tmp_atom1->type != tmp_atom2->type || !EPI_ATOM_IS_SUBST(*tmp_atom1))
    return EPI_FAILURE;

  if (tmp_atom1->truth != tmp_atom2->truth)
    return EPI_FAILURE;

  if (ident_compare(*(EPI_ATOM_SUBST_GROUP1(*tmp_atom1)),
                    *(EPI_ATOM_SUBST_GROUP1(*tmp_atom2))) != EPI_OK)
    return EPI_FAILURE;

  return EPI_OK;
}
/* returns 0 if the atoms pointed to by p1 and p2 are both subst and that
 * the group2 field matches */
int gnd_exp_cmp_subst_group2(void *p1, void *p2)
{
  gnd_atom_type *tmp_atom1 = NULL;
  gnd_atom_type *tmp_atom2 = NULL;

  if (p1 == NULL || p2 == NULL)
    return EPI_NULLPTR;

  tmp_atom1 = (gnd_atom_type *) p1;
  tmp_atom2 = (gnd_atom_type *) p2;

  if (tmp_atom1->type != tmp_atom2->type || !EPI_ATOM_IS_SUBST(*tmp_atom1))
    return EPI_FAILURE;

  if (tmp_atom1->truth != tmp_atom2->truth)
    return EPI_FAILURE;

  if (ident_compare(*(EPI_ATOM_SUBST_GROUP2(*tmp_atom1)),
                    *(EPI_ATOM_SUBST_GROUP2(*tmp_atom2))) != EPI_OK)
    return EPI_FAILURE;

  return EPI_OK;
}

/* returns 0 if the atoms pointed to by p1 and p2 are both memb and that
 * the element field matches */
int gnd_exp_cmp_memb_element(void *p1, void *p2)
{
  gnd_atom_type *tmp_atom1 = NULL;
  gnd_atom_type *tmp_atom2 = NULL;

  if (p1 == NULL || p2 == NULL)
    return EPI_NULLPTR;

  tmp_atom1 = (gnd_atom_type *) p1;
  tmp_atom2 = (gnd_atom_type *) p2;

  if (tmp_atom1->type != tmp_atom2->type || !EPI_ATOM_IS_MEMB(*tmp_atom1))
    return EPI_FAILURE;

  if (tmp_atom1->truth != tmp_atom2->truth)
    return EPI_FAILURE;

  if (ident_compare(*(EPI_ATOM_MEMB_ELEMENT(*tmp_atom1)), 
                    *(EPI_ATOM_MEMB_ELEMENT(*tmp_atom2))) != EPI_OK)
    return EPI_FAILURE;

  return EPI_OK;
}

/* returns 0 if the atoms pointed to by p1 and p2 are both memb and that
 * the group field matches */
int gnd_exp_cmp_memb_group(void *p1, void *p2)
{
  gnd_atom_type *tmp_atom1 = NULL;
  gnd_atom_type *tmp_atom2 = NULL;

  if (p1 == NULL || p2 == NULL)
    return EPI_NULLPTR;

  tmp_atom1 = (gnd_atom_type *) p1;
  tmp_atom2 = (gnd_atom_type *) p2;

  if (tmp_atom1->type != tmp_atom2->type || !EPI_ATOM_IS_MEMB(*tmp_atom1))
    return EPI_FAILURE;

  if (tmp_atom1->truth != tmp_atom2->truth)
    return EPI_FAILURE;

  if (ident_compare(*(EPI_ATOM_MEMB_GROUP(*tmp_atom1)), 
                    *(EPI_ATOM_MEMB_GROUP(*tmp_atom2))) != EPI_OK)
    return EPI_FAILURE;

  return EPI_OK;
}

/* returns a reference to the atom that matches atom, null if not found */
gnd_atom_type *gnd_exp_get_ref(gnd_atom_type atom, gnd_exp_type exp)
{
  gnd_atom_type *tmp_atom = NULL;
  gnd_exp_type tmp_exp;

  if (simplelist_get_data(exp, &tmp_exp, &atom, gnd_exp_compare) == EPI_OK) {
    simplelist_get_index(tmp_exp, 0, (void **) &tmp_atom);
    simplelist_purge(&tmp_exp, NULL);
  }

  return tmp_atom;
}

/* assumes list has been initialised. gives a list of all identifiers that
 * contains ident. if immediate == 0, do not get explicitly defined
 * supergroups */
int gnd_exp_get_supergroups(ident_type ident,
                            identlist_type *list,
                            gnd_exp_type exp,
                            int immediate)
{
  unsigned int i;
  gnd_atom_type tmp_atom;
  gnd_exp_type tmp_exp;
  ident_type *tmp_ident = NULL;
  gnd_atom_type *ptr_atom = NULL;
  int tmp_res;

  if (list == NULL)
    return EPI_NULLPTR;

  tmp_atom.truth = EPI_TRUE;
  simplelist_init(&tmp_exp);

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
    if ((tmp_res = simplelist_get_index(tmp_exp, i, (void **) &ptr_atom)) != EPI_OK)
      return tmp_res;

    /* get superset */
    if (EPI_IDENT_IS_GROUP(ident))
      tmp_ident = EPI_ATOM_SUBST_GROUP2(*ptr_atom);
    else
      tmp_ident = EPI_ATOM_MEMB_GROUP(*ptr_atom);

    if (immediate) {
      /* see if we already have this identifier */
      if (identlist_find(*list, tmp_ident->name) == EPI_OK)
        continue;

      /* no? so we add it to our list */
      if ((tmp_res = identlist_add(list, tmp_ident)) != EPI_OK)
        return tmp_res;
    }

    /* now see if we can find supersets for this identifier */
    if ((tmp_res = gnd_exp_get_supergroups(*tmp_ident, list, exp, 1)) != EPI_OK)
      return tmp_res;
  }

  simplelist_purge(&tmp_exp, NULL);

  return 0;
}

/* assumes list has been initialised. gives a list of all group identifiers 
 * that DOES NOT contain ident. if immediate == 0, do not get explicitly
 * defined non-supergroups */
int gnd_exp_get_non_supergroups(ident_type ident,
                                identlist_type *list,
                                gnd_exp_type exp,
                                int immediate)
{
  unsigned int i;
  gnd_atom_type tmp_atom;
  gnd_exp_type tmp_exp;
  ident_type *tmp_ident = NULL;
  gnd_atom_type *ptr_atom = NULL;
  int tmp_res;

  if (list == NULL)
    return EPI_NULLPTR;

  /* phase 1: 
   * get all groups that does not contain ident and all groups inside them */

  tmp_atom.truth = EPI_FALSE;
  simplelist_init(&tmp_exp);

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
    if ((tmp_res = simplelist_get_index(tmp_exp, i, (void **) &ptr_atom)) != EPI_OK)
      return tmp_res;

    /* get non-superset */
    if (EPI_IDENT_IS_GROUP(ident))
      tmp_ident = EPI_ATOM_SUBST_GROUP2(*ptr_atom);
    else
      tmp_ident = EPI_ATOM_MEMB_GROUP(*ptr_atom);

    if (immediate) {
      /* see if we already have this identifier */
      if (identlist_find(*list, tmp_ident->name) == EPI_OK)
        continue;

      /* no? so we add it to our list */
      if ((tmp_res = identlist_add(list, tmp_ident)) != EPI_OK)
        return tmp_res;
    }

    /* now see if we can find groups inside this non-supergroup */
    if ((tmp_res = gnd_exp_get_subgroups(*tmp_ident, list, exp, 1)) != EPI_OK)
      return tmp_res;
  }

  simplelist_purge(&tmp_exp, NULL);

  /* phase 2: 
   * get all groups (group 1) that contain ident then find all groups (group 2)
   * that do not contain them (group 1), plus all the groups (group 3) inside 
   * them (group 2) */

  tmp_atom.truth = EPI_TRUE;
  tmp_atom.type = EPI_ATOM_SUBST;
  EPI_ATOM_SUBST_GROUP1(tmp_atom) = &ident;

  /* now search for atoms where in ident is a superset */
  if (simplelist_get_data(exp, &tmp_exp, (void *) &tmp_atom, gnd_exp_cmp_subst_group1) == EPI_OK) {

    /* now go through the resulting list and find groups that do not contain them */
    for (i = 0; i < simplelist_length(tmp_exp); i++) {
      if ((tmp_res = simplelist_get_index(tmp_exp, i, (void **) &ptr_atom)) != EPI_OK)
        return tmp_res;
      if ((tmp_res = gnd_exp_get_non_supergroups(*(EPI_ATOM_SUBST_GROUP2(*ptr_atom)), list, exp, 1)) != EPI_OK)
       return tmp_res;
    }

    simplelist_purge(&tmp_exp, NULL);
  }

  return EPI_OK;
}

/* assumes list has been initialised. gives a list of all group identifiers 
 * that is contained in ident. if immediate == 0, do not include explicitly
 * defined subgroups */
int gnd_exp_get_subgroups(ident_type ident,
                          identlist_type *list,
                          gnd_exp_type exp,
                          int immediate)
{
  unsigned int i;
  gnd_atom_type tmp_atom;
  gnd_exp_type tmp_exp;
  ident_type *tmp_ident = NULL;
  gnd_atom_type *ptr_atom = NULL;
  int tmp_res;

  if (list == NULL)
    return EPI_NULLPTR;

  if (!EPI_IDENT_IS_GROUP(ident)) 
    return EPI_INVALIDINPUT;

  tmp_atom.truth = EPI_TRUE;
  tmp_atom.type = EPI_ATOM_SUBST;
  EPI_ATOM_SUBST_GROUP2(tmp_atom) = &ident;

  /* now search for atoms where in ident is a superset */
  if (simplelist_get_data(exp, &tmp_exp, (void *) &tmp_atom, gnd_exp_cmp_subst_group2) == EPI_OK) {
    /* now go through the resulting list and find their supersets */
    for (i = 0; i < simplelist_length(tmp_exp); i++) {
      if ((tmp_res = simplelist_get_index(tmp_exp, i, (void **) &ptr_atom)) != EPI_OK)
        return tmp_res;

      tmp_ident = EPI_ATOM_SUBST_GROUP1(*ptr_atom);

      if (immediate) {
        /* see if we already have this identifier */
        if (identlist_find(*list, tmp_ident->name) == EPI_OK)
          continue;

        /* no? so we add it to our list */
        if ((tmp_res = identlist_add(list, tmp_ident)) != EPI_OK)
          return tmp_res;
      }

      /* now see if we can find subsets for this identifier */
      if ((tmp_res = gnd_exp_get_subgroups(*tmp_ident, list, exp, 1)) != EPI_OK)
        return tmp_res;
    }

    simplelist_purge(&tmp_exp, NULL);
  }

  return EPI_OK;
}

/* assumes list has been initialised. gives a list of all group identifiers 
 * that IS NOT in ident. if immediate == 0, do not include explicitly
 * defined non-subgroups */
int gnd_exp_get_non_subgroups(ident_type ident,
                              identlist_type *list,
                              gnd_exp_type exp,
                              int immediate)
{
  unsigned int i;
  gnd_atom_type tmp_atom;
  gnd_exp_type tmp_exp;
  ident_type *tmp_ident = NULL;
  gnd_atom_type *ptr_atom = NULL;
  int tmp_res;

  if (list == NULL)
    return EPI_NULLPTR;

  if (!EPI_IDENT_IS_GROUP(ident))
    return EPI_INVALIDINPUT;

  /* phase 1: 
   * get all groups that does not contain ident and all groups inside them */

  tmp_atom.truth = EPI_FALSE;
  tmp_atom.type = EPI_ATOM_SUBST;
  EPI_ATOM_SUBST_GROUP2(tmp_atom) = &ident;

  /* now search for atoms where in ident is a subset */
  if (simplelist_get_data(exp, &tmp_exp, (void *) &tmp_atom, gnd_exp_cmp_subst_group2) == EPI_OK) {
    /* now go through the resulting list and find their non-subsets */
    for (i = 0; i < simplelist_length(tmp_exp); i++) {
      if ((tmp_res = simplelist_get_index(tmp_exp, i, (void **) &ptr_atom)) != EPI_OK)
        return tmp_res;

      /* get non-subset */
      tmp_ident = EPI_ATOM_SUBST_GROUP1(*ptr_atom);

      if (immediate) {
        /* see if we already have this identifier */
        if (identlist_find(*list, tmp_ident->name) == EPI_OK)
          continue;

        /* no? so we add it to our list */
        if ((tmp_res = identlist_add(list, tmp_ident)) != EPI_OK)
          return tmp_res;
      }

      /* now see if we can find groups inside this non-subgroup */
      if ((tmp_res = gnd_exp_get_subgroups(*tmp_ident, list, exp, 1)) != EPI_OK)
        return tmp_res;
    }
    simplelist_purge(&tmp_exp, NULL);
  }

  /* phase 2: 
   * get all groups (group 1) that are in ident then find all groups (group 2)
   * that are not in them (group 1), plus all the groups (group 3) inside 
   * them (group 2) */

  tmp_atom.truth = EPI_TRUE;
  tmp_atom.type = EPI_ATOM_SUBST;
  EPI_ATOM_SUBST_GROUP2(tmp_atom) = &ident;

  /* now search for atoms where in ident is a superset */
  if (simplelist_get_data(exp, &tmp_exp, (void *) &tmp_atom, gnd_exp_cmp_subst_group2) == EPI_OK) {
    /* now go through the resulting list and find groups that do not contain them */
    for (i = 0; i < simplelist_length(tmp_exp); i++) {
      if ((tmp_res = simplelist_get_index(tmp_exp, i, (void **) &ptr_atom)) != EPI_OK)
        return tmp_res;
      if ((tmp_res = gnd_exp_get_non_subgroups(*(EPI_ATOM_SUBST_GROUP1(*ptr_atom)), list, exp, 1)) != EPI_OK)
       return tmp_res;
    }
    simplelist_purge(&tmp_exp, NULL);
  }

  return 0;
}

/* assumes list has been initialised. gives a list of all element identifiers 
 * that is contained in ident. if immediate == 0, do not include explicitly
 * defined elements. */
int gnd_exp_get_elements(ident_type ident,
                         identlist_type *list,
                         gnd_exp_type exp,
                         int immediate)
{
  unsigned int i;
  gnd_atom_type tmp_atom;
  gnd_exp_type tmp_exp;
  gnd_atom_type *ptr_atom = NULL;
  int tmp_res;

  if (list == NULL)
    return EPI_NULLPTR;

  if (!EPI_IDENT_IS_GROUP(ident))
    return EPI_INVALIDINPUT;

  if (immediate) {
    tmp_atom.truth = EPI_TRUE;
    tmp_atom.type = EPI_ATOM_MEMB;
    EPI_ATOM_MEMB_GROUP(tmp_atom) = &ident;
  
    /* now search for atoms where in ident is a group */
    if (simplelist_get_data(exp, &tmp_exp, (void *) &tmp_atom, gnd_exp_cmp_memb_group) == EPI_OK) {
      /* now go through the resulting list and find the elements */
      for (i = 0; i < simplelist_length(tmp_exp); i++) {
        if ((tmp_res = simplelist_get_index(tmp_exp, i, (void **) &ptr_atom)) != EPI_OK)
          return tmp_res;
  
        /* see if we already have this identifier */
        if (identlist_find(*list, EPI_ATOM_MEMB_ELEMENT(*ptr_atom)->name) == EPI_OK)
          continue;
  
        /* no? so we add it to our list */
        if ((tmp_res = identlist_add(list, EPI_ATOM_MEMB_ELEMENT(*ptr_atom))) != EPI_OK)
          return tmp_res;
      }
      simplelist_purge(&tmp_exp, NULL);
    }
  }

  /* now see if we can find subsets for this identifier */
  tmp_atom.truth = EPI_TRUE;
  tmp_atom.type = EPI_ATOM_SUBST;
  EPI_ATOM_SUBST_GROUP2(tmp_atom) = &ident;

  /* search for atoms where in this ident is a sub group */
  if (simplelist_get_data(exp, &tmp_exp, (void *) &tmp_atom, gnd_exp_cmp_subst_group2) == EPI_OK) {
    /* now go through the resulting list and find the elements */
    for (i = 0; i < simplelist_length(tmp_exp); i++) {
      if ((tmp_res = simplelist_get_index(tmp_exp, i, (void **) &ptr_atom)) != EPI_OK)
        return tmp_res;

      /* finally, get the elements of this group */
      if ((tmp_res = gnd_exp_get_elements(*EPI_ATOM_SUBST_GROUP1(*ptr_atom), list, exp, 1)) != EPI_OK)
        return tmp_res;
    }
    simplelist_purge(&tmp_exp, NULL);
  }

  return EPI_OK;
}

/* assumes list has been initialised. gives a list of all element identifiers 
 * that is NOT contained in ident. if immediate == 0, do not include
 * explicitly defined non-elements */
int gnd_exp_get_non_elements(ident_type ident,
                             identlist_type *list,
                             gnd_exp_type exp,
                             int immediate)
{
  unsigned int i;
  gnd_atom_type tmp_atom;
  gnd_exp_type tmp_exp;
  gnd_atom_type *ptr_atom = NULL;
  identlist_type tmp_list;
  ident_type *ptr_ident = NULL;
  int tmp_res;

  if (list == NULL)
    return EPI_NULLPTR;

  if (!EPI_IDENT_IS_GROUP(ident))
    return EPI_INVALIDINPUT;

  if (immediate) {
    tmp_atom.truth = EPI_FALSE;
    tmp_atom.type = EPI_ATOM_MEMB;
    EPI_ATOM_MEMB_GROUP(tmp_atom) = &ident;

    /* now search for atoms where in ident is a group */
    if (simplelist_get_data(exp, &tmp_exp, (void *) &tmp_atom, gnd_exp_cmp_memb_group) == EPI_OK) {
      /* now go through the resulting list and find the elements */
      for (i = 0; i < simplelist_length(tmp_exp); i++) {
        if ((tmp_res = simplelist_get_index(tmp_exp, i, (void **) &ptr_atom)) != EPI_OK)
          return tmp_res;

        /* see if we already have this identifier */
        if (identlist_find(*list, EPI_ATOM_MEMB_ELEMENT(*ptr_atom)->name) == EPI_OK)
          continue;

        /* no? so we add it to our list */
        if ((tmp_res = identlist_add(list, EPI_ATOM_MEMB_ELEMENT(*ptr_atom))) != EPI_OK)
          return tmp_res;
      }
      simplelist_purge(&tmp_exp, NULL);
    }
  }

  /* now we need to find the groups that do not contain this set (all
   * elements of those groups will be non-elements of this group) */

  identlist_init(&tmp_list);

  if ((tmp_res = gnd_exp_get_non_supergroups(ident, &tmp_list, exp, 1)) != EPI_OK)
    return tmp_res;

  /* then we go through the list of non-groups to get their respective
   * elements. result of which are non-elements of this group */

  for (i = 0; i < identlist_length(tmp_list); i++) {
    if ((tmp_res = identlist_get(tmp_list, i, &ptr_ident)) != EPI_OK)
      return tmp_res;

    if ((tmp_res = gnd_exp_get_elements(*ptr_ident, list, exp, 1)) != EPI_OK)
      return tmp_res;
  }

  identlist_purge(&tmp_list);

  /* finally we need to find the groups that are not in this set (all
   * elements of those groups will be non-elements of this group) */

  identlist_init(&tmp_list);

  if ((tmp_res = gnd_exp_get_non_subgroups(ident, &tmp_list, exp, 1)) != EPI_OK)
    return tmp_res;

  /* then we go through the list of non-subgroups to get their respective
   * elements. result of which are non-elements of this group */

  for (i = 0; i < identlist_length(tmp_list); i++) {
    if ((tmp_res = identlist_get(tmp_list, i, &ptr_ident)) != EPI_OK)
      return tmp_res;

    if ((gnd_exp_get_elements(*ptr_ident, list, exp, 1)) != EPI_OK)
      return tmp_res;
  }

  identlist_purge(&tmp_list);

  return EPI_OK;
}

/* gives true, false or unknown depending on whether holds(sub, acc, obj)
 * can be derived from exp or not. */
int gnd_exp_eval_holds_atom(ident_type sub,
                            ident_type acc,
                            ident_type obj, 
                            gnd_exp_type exp,
                            unsigned short int *ans)
{
  unsigned int i;
  unsigned int j;
  unsigned int k;
  gnd_atom_type tmp_atom;
  identlist_type tmp_sub;
  identlist_type tmp_acc;
  identlist_type tmp_obj;
  int tmp_res;

  if (ans == NULL)
    return EPI_NULLPTR;

  *ans = EPI_RESULT_UNKNOWN;
  tmp_atom.truth = EPI_TRUE;
  tmp_atom.type = EPI_ATOM_HOLDS;

  identlist_init(&tmp_sub);
  identlist_init(&tmp_acc);
  identlist_init(&tmp_obj);

  /* get supergroups of all three identifiers */
  if ((tmp_res = gnd_exp_get_supergroups(sub, &tmp_sub, exp, 1)) != EPI_OK)
    return tmp_res;
  if ((tmp_res = gnd_exp_get_supergroups(acc, &tmp_acc, exp, 1)) != EPI_OK)
    return tmp_res;
  if ((tmp_res = gnd_exp_get_supergroups(obj, &tmp_obj, exp, 1)) != EPI_OK)
    return tmp_res;

  /* now go through all the possible combinations of all three lists to
   * see if we find a match */
  for (i = 0; i < identlist_length(tmp_sub); i++) {
    if ((tmp_res = identlist_get(tmp_sub, i, &(EPI_ATOM_HOLDS_SUBJECT(tmp_atom)))) != EPI_OK)
      return tmp_res;
    for (j = 0; j < identlist_length(tmp_acc); j++) {
      if ((tmp_res = identlist_get(tmp_acc, j, &(EPI_ATOM_HOLDS_ACCESS(tmp_atom)))) != EPI_OK)
        return tmp_res;
      for (k = 0; k < identlist_length(tmp_obj); k++) {
        if ((tmp_res = identlist_get(tmp_obj, k, &(EPI_ATOM_HOLDS_OBJECT(tmp_atom)))) != EPI_OK)
          return tmp_res;
        if (simplelist_find_data(exp, (void *) &tmp_atom, gnd_exp_compare) == EPI_OK) { 
          *ans = EPI_RESULT_TRUE;
          identlist_purge(&tmp_sub);
          identlist_purge(&tmp_acc);
          identlist_purge(&tmp_obj);
          return EPI_OK;
        }
        /* also try the negation while we're here */
        EPI_ATOM_NEGATE(tmp_atom);
        if (simplelist_find_data(exp, (void *) &tmp_atom, gnd_exp_compare) == EPI_OK) { 
          *ans = EPI_RESULT_FALSE;
          identlist_purge(&tmp_sub);
          identlist_purge(&tmp_acc);
          identlist_purge(&tmp_obj);
          return EPI_OK;
        }
        EPI_ATOM_NEGATE(tmp_atom);
      }
    }
  }
  identlist_purge(&tmp_sub);
  identlist_purge(&tmp_acc);
  identlist_purge(&tmp_obj);

  return EPI_OK;
}

/* check to see if we can derive the truth value of memb. */
int gnd_exp_eval_memb_atom(ident_type element,
                           ident_type group,
                           gnd_exp_type exp,
                           unsigned short int *ans)
{
  identlist_type tmp_grp;
  int tmp_res;

  if (ans == NULL)
    return EPI_NULLPTR;

  *ans = EPI_RESULT_UNKNOWN;
  identlist_init(&tmp_grp); 

  /* get all the elements of the group */
  if ((tmp_res = gnd_exp_get_elements(group, &tmp_grp, exp, 0)) != EPI_OK)
    return tmp_res;

  /* check for truthfulness */
  if (identlist_find(tmp_grp, element.name) == EPI_OK) {
    *ans = EPI_RESULT_TRUE;
    identlist_purge(&tmp_grp);
    return EPI_OK;
  }

  identlist_purge(&tmp_grp);

  /* get all the non-elements of the group */
  if ((tmp_res = gnd_exp_get_non_elements(group, &tmp_grp, exp, 0)) != EPI_OK)
    return tmp_res;

  /* check for falseness */
  if (identlist_find(tmp_grp, element.name) == EPI_OK) {
    *ans = EPI_RESULT_FALSE;
    identlist_purge(&tmp_grp);
    return EPI_OK;
  }

  identlist_purge(&tmp_grp);

  return EPI_OK;
}

int gnd_exp_eval_subst_atom(ident_type group1,
                            ident_type group2,
                            gnd_exp_type exp,
			    unsigned short int *ans)
{
  identlist_type tmp_grp;
  ident_type *ptr_ident;
  gnd_atom_type tmp_atom;
  unsigned short int tmp_ans;
  unsigned int i;
  unsigned int len;
  int tmp_res;

  if (ans == NULL)
    return EPI_NULLPTR;
  
  *ans = EPI_RESULT_UNKNOWN;
  identlist_init(&tmp_grp);

  /* check for truthfulness */
  if ((tmp_res = gnd_exp_get_supergroups(group1, &tmp_grp, exp, 0)) != EPI_OK)
    return tmp_res;

  if (identlist_find(tmp_grp, group2.name) == EPI_OK) {
    *ans = EPI_RESULT_TRUE;
    identlist_purge(&tmp_grp);
    return EPI_OK;
  }
  
  identlist_purge(&tmp_grp);

  /* check for falseness */
  if ((tmp_res = gnd_exp_get_non_supergroups(group1, &tmp_grp, exp, 0)) != EPI_OK)
      return tmp_res;

  if (identlist_find(tmp_grp, group2.name) == EPI_OK) {
    *ans = EPI_RESULT_FALSE;
    identlist_purge(&tmp_grp);
    return EPI_OK;
  }

  identlist_purge(&tmp_grp);

  /* now we check for the elements. if all the elements of group1 is also
   * in group2 (implied or otherwise), then it group1 is a subset of
   * group2 */

  if ((tmp_res = gnd_exp_get_elements(group1, &tmp_grp, exp, 1)) != EPI_OK)
    return tmp_res;

  if ((len = identlist_length(tmp_grp)) == 0)
    return EPI_OK;

  *ans = EPI_RESULT_TRUE;

  /* now for each element of group1, see if it is also an element of group2 */
  for (i = 0; i < len; i++) {
    identlist_get(tmp_grp, i, &ptr_ident);

    /* see if there is an atom that proves that this identifier is an element
     * of group2 */
    if ((tmp_res = gnd_atom_create_memb(&tmp_atom, ptr_ident, &group2, EPI_TRUE)) != EPI_OK) {
      identlist_purge(&tmp_grp);
      return tmp_res;
    }
   
    if (simplelist_find_data(exp, (void *) &tmp_atom, gnd_exp_compare) == EPI_OK)
      continue;

    EPI_ATOM_NEGATE(tmp_atom);

    if (simplelist_find_data(exp, (void *) &tmp_atom, gnd_exp_compare) == EPI_OK) {
      identlist_purge(&tmp_grp);
      *ans = EPI_RESULT_FALSE;
      return EPI_OK;
    }

   /* no explicit atoms that say the identifier is (not) an element of group2
    * so we try to see if it is implied */
    if ((tmp_res = gnd_exp_eval_memb_atom(*ptr_ident, group2, exp, &tmp_ans)) != EPI_OK) {
      identlist_purge(&tmp_grp);
      return tmp_res;
    }

    switch(tmp_ans) {
      case EPI_RESULT_FALSE :
        /* if we find an element of group1 that is not in group2, then group1
         * cannot be a subset of group2 */
        identlist_purge(&tmp_grp);
        *ans = EPI_RESULT_FALSE;
        return EPI_OK;
      case EPI_RESULT_UNKNOWN :
        /* we found an element of group1 that cannot be proven to be an
         * an element or not an element of group2 so the best answer we
         * can come up with is an unknown. continue with the list in case
         * we see a false */
         *ans = EPI_RESULT_UNKNOWN;
        break;
      default :
    }
  }

  /* if we've reached the end of the loop, we can conclude that either
   * all elements of group1 are in group2 or at least one of them couldn't
   * be proven to be in or out of group2 */

  identlist_purge(&tmp_grp);

  return EPI_OK;
}

/* gives true, false or unknown depending on whether atom or its negation 
 * can be derived from expression, or not. */
int gnd_exp_eval_atom(gnd_atom_type atom,
                      gnd_exp_type exp, 
                      unsigned short int *ans)
{
  int tmp_res;
  
  if (ans == NULL)
    return EPI_NULLPTR;

  *ans = EPI_RESULT_UNKNOWN;

  if (EPI_ATOM_IS_HOLDS(atom)) {
    if ((tmp_res = gnd_exp_eval_holds_atom(*(EPI_ATOM_HOLDS_SUBJECT(atom)),
                                           *(EPI_ATOM_HOLDS_ACCESS(atom)),
                                           *(EPI_ATOM_HOLDS_OBJECT(atom)),
                                           exp,
                                           ans)) != EPI_OK)
      return tmp_res;

    if (*ans != EPI_RESULT_UNKNOWN)
      *ans = (*ans == atom.truth) ? EPI_RESULT_TRUE : EPI_RESULT_FALSE;
  }
  else if (EPI_ATOM_IS_MEMB(atom)) {
    if ((tmp_res = gnd_exp_eval_memb_atom(*(EPI_ATOM_MEMB_ELEMENT(atom)),
                                          *(EPI_ATOM_MEMB_GROUP(atom)),
                                          exp,
                                          ans)) != EPI_OK)
      return tmp_res;

    if (*ans != EPI_RESULT_UNKNOWN)
      *ans = (*ans == atom.truth) ? EPI_RESULT_TRUE : EPI_RESULT_FALSE;
  }
  else if (EPI_ATOM_IS_SUBST(atom)) {
    if ((tmp_res = gnd_exp_eval_subst_atom(*(EPI_ATOM_SUBST_GROUP1(atom)),
                                           *(EPI_ATOM_SUBST_GROUP2(atom)),
                                           exp,
                                           ans)) != EPI_OK)
      return tmp_res;

    if (*ans != EPI_RESULT_UNKNOWN)
      *ans = (*ans == atom.truth) ? EPI_RESULT_TRUE : EPI_RESULT_FALSE;
  }
  else if (EPI_ATOM_IS_CONST(atom)) {
    /* true is implied on any expression that is not false */
    *ans = (atom.truth == EPI_TRUE) ? EPI_RESULT_TRUE : EPI_RESULT_FALSE;
  }

  return EPI_OK;
}

/* returns 0 if the ATOMS pointed to by p1 and p2 are equivalent */
int gnd_exp_compare(void *p1, void *p2)
{
  if (p1 == NULL || p2 == NULL)
    return EPI_NULLPTR;

  return gnd_atom_compare(* (gnd_atom_type *) p1, * (gnd_atom_type *) p2);
}

/* free memory */
void gnd_exp_destroy(void *p)
{
  gnd_atom_destroy((gnd_atom_type *) p);
}
