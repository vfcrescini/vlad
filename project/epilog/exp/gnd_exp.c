/* 
 * gnd_exp.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <identlist.h>
#include "gnd_exp.h"

int gnd_exp_cmp_subst(void *p1, void *p2);
int gnd_exp_cmp_memb(void *p1, void *p2);
int gnd_exp_get_superset(ident_type ident, 
                         identlist_type *list,
                         gnd_exp_type exp);
int gnd_exp_compare(void *p1, void *p2);
int gnd_exp_destroy(void *p);

/* initialise list */
int gnd_exp_init(gnd_exp_type *exp)
{
  return simplelist_init(exp);
}

/* gives the number of atoms in the gnd_exp */
int gnd_exp_length(gnd_exp_type exp, unsigned int *len)
{
  return simplelist_length(exp, len);
}

/* return 0 if the atom is in the gnd_exp */
int gnd_exp_find(gnd_exp_type exp, gnd_atom_type atom)
{
  return simplelist_find_data(exp, (void *) &atom, gnd_exp_compare);
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
    if (gnd_exp_purge(exp) != 0)
      return -1;
    if (gnd_atom_create_const(&new_atom, epi_false) != 0)
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

/* delete all atoms from this gnd_exp */
int gnd_exp_purge(gnd_exp_type *exp)
{
  unsigned int len;
  unsigned int i;

  if (exp == NULL)
    return -1;

  if (simplelist_length(*exp, &len) != 0)
    return -1;
  
  for (i = 0; i < len; i++)
    if (simplelist_del_index(exp, 0, gnd_exp_destroy) != 0)
      return -1;

  return 0;
}

/* returns 0 if the atoms pointed to by p1 and p2 are both subst and that
 * the group1 field matches */
int gnd_exp_cmp_subst(void *p1, void *p2)
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

  if (ident_compare(*(tmp_atom1->atom.subst.group1), *(tmp_atom2->atom.subst.group1)) != 0)
    return -1;

  return 0;
}

/* returns 0 if the atoms pointed to by p1 and p2 are both memb and that
 * the element field matches */
int gnd_exp_cmp_memb(void *p1, void *p2)
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

  if (ident_compare(*(tmp_atom1->atom.memb.element), *(tmp_atom2->atom.memb.element)) != 0)
    return -1;

  return 0;
}

/* we assume that list has already been initialised. gives out a list of 
 * identifiers that is a superset of ident. these include the following:
 *   - groups of which ident is a member (if ident is a non-group)
 *   - supersets of ident (if ident is a group)
 *   - all other groups that contain the groups above */
int gnd_exp_get_superset(ident_type ident,
                         identlist_type *list,
                         gnd_exp_type exp)
{
  unsigned int i;
  unsigned int len;
  gnd_atom_type tmp_atom;
  gnd_exp_type tmp_exp;
  ident_type *tmp_ident = NULL;

  if (list == NULL)
    return -1;

  tmp_atom.truth = epi_true;
  if (EPI_IDENT_IS_GROUP(ident)) {
    /* if it is a group, we have to look for subset atoms */
    tmp_atom.type = EPI_ATOM_SUBST;
    tmp_atom.atom.subst.group1 = &ident;

    /* now search for atoms where in ident is a subset */
    if (simplelist_get_data(exp, &tmp_exp, (void *) &tmp_atom, gnd_exp_cmp_subst) != 0)
      return -1;
  }
  else {
   /* if it is a non-group, we have to look for memb atoms */
   tmp_atom.type = EPI_ATOM_MEMB;
   tmp_atom.atom.memb.element = &ident;

    /* now search for atoms where in ident is a memb */
    if (simplelist_get_data(exp, &tmp_exp, (void *) &tmp_atom, gnd_exp_cmp_memb) != 0)
      return -1;
  }

  if (simplelist_length(tmp_exp, &len) != 0)
    return -1;

  /* if we did not find any atoms that matches, return. the all-important
   * recursion breakpoint */
  if (len == 0)
    return 0;

  /* now go through the resulting list and find their supersets */
  for (i = 0; i < len; i++) {
    if (simplelist_get_index(tmp_exp, i, (void **) &tmp_atom) != 0)
      return -1;

    /* get superset */
    if (EPI_IDENT_IS_GROUP(ident))
      tmp_ident = tmp_atom.atom.subst.group2;
    else
      tmp_ident = tmp_atom.atom.memb.group;

    /* see if we already have this identifier */
    if (identlist_find(*list, tmp_ident->name) == 0)
      continue;

    if (identlist_add(list, tmp_ident) != 0)
      return -1;

    /* now see if we can find supersets for this identifier */
    if (gnd_exp_get_superset(*tmp_ident, list, exp) != 0)
      return -1;
  }

  return gnd_exp_purge(&tmp_exp);
}

/* returns 0 if the ATOMS pointed to by p1 and p2 are equivalent */
int gnd_exp_compare(void *p1, void *p2)
{
  if (p1 == NULL || p2 == NULL)
    return -1;
  
  return gnd_atom_compare(* (gnd_atom_type *) p1, * (gnd_atom_type *) p2);
}

/* free memory */
int gnd_exp_destroy(void *p)
{
  return gnd_atom_destroy((gnd_atom_type *) p);
}
