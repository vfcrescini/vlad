/* 
 * comp_exp.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include "gnd_atom.h"
#include "comp_atom.h"
#include "comp_exp.h"

int comp_exp_replace_atom(comp_atom_type comp,
                          gnd_atom_type *ground,
                          stringlist_type varlist,
                          identlist_type identlist);
int comp_exp_get_ident(name_type name,
                       ident_type **ident,
                       stringlist_type varlist,
                       identlist_type identlist);
int comp_exp_compare(void *p1, void *p2);
int comp_exp_destroy(void *p);

/* initialise list */
int comp_exp_init(comp_exp_type *exp)
{
  return simplelist_init(exp);
}

/* gives the number of atoms in the expression */
int comp_exp_length(comp_exp_type exp, unsigned int *len)
{
  return simplelist_length(exp, len);
}

/* return 0 if the atom is in the expression */
int comp_exp_find(comp_exp_type exp, comp_atom_type atom)
{
  return simplelist_find_data(exp, (void *) &atom, comp_exp_compare);
}

/* gives a reference to the index'th atom in the expression */
int comp_exp_get(comp_exp_type exp, unsigned int index, comp_atom_type **atom)
{
  return simplelist_get_index(exp, index, (void **) atom);
}

/* add an atom into the expression */
int comp_exp_add(comp_exp_type *exp, comp_atom_type atom)
{
  comp_atom_type *new_atom = NULL;
  comp_atom_type false_atom;
  
  if (exp == NULL)
    return -1;
  
  /* if an atom is already in simply return success */
  if (comp_exp_find(*exp, atom) == 0)
    return 0;

  /* if the expression contains a FALSE constant, just return success */
  false_atom.type = EPI_ATOM_CONST;
  false_atom.truth = epi_false;

  if (comp_exp_find(*exp, false_atom) == 0)
    return 0;

  /* if the negation of an atom is aleady in, or if the atom is a FALSE
   * constant, we replace the the whole expression with a constant false */
  
  EPI_ATOM_NEGATE(atom);
  if ((EPI_ATOM_IS_CONST(atom) && atom.truth == epi_true) ||
      comp_exp_find(*exp, atom) == 0) {
    if (comp_exp_purge(exp) != 0)
      return -1;
    if (comp_atom_create_const(&new_atom, epi_false) != 0)
      return -1;
  }
  else {
    if (comp_atom_copy(&new_atom, atom) != 0)
      return -1;

    EPI_ATOM_NEGATE(*new_atom);
  }

  return simplelist_add(exp, (void *) new_atom);
}

/* replaces all variable instances in the comp expression with ground atoms */
int comp_exp_replace(comp_exp_type comp,
                     gnd_exp_type *ground,
                     stringlist_type varlist,
                     identlist_type identlist)
{
  unsigned int i;
  unsigned int len_exp;
  unsigned int len_vlist;
  unsigned int len_ilist;
  gnd_atom_type *tmp_gnd_atom = NULL;
  comp_atom_type *tmp_comp_atom = NULL;

  if (ground == NULL)
    return -1;

  /* first we ensure that the varlist and the identlist are of equal length */
  if (stringlist_length(varlist, &len_vlist) != 0 ||
      identlist_length(identlist, &len_ilist) != 0 ||
      len_vlist != len_ilist)
    return -1;
  
  if (gnd_exp_init(ground) != 0)
    return -1;

  if (comp_exp_length(comp, &len_exp) != 0)
    return -1;

  for (i = 0; i < len_exp; i++) {
    if (comp_exp_get(comp, i, &tmp_comp_atom) != 0)
      return -1;

    if (comp_exp_replace_atom(*tmp_comp_atom,
                              tmp_gnd_atom,
                              varlist,
                              identlist) != 0)
      return -1;

    if (gnd_exp_add(ground, *tmp_gnd_atom) != 0)
      return -1;
  }

  return 0;
}

/* delete an atom from the expression */
int comp_exp_del(comp_exp_type *exp, comp_atom_type atom)
{
  if (exp == NULL)
    return -1;

  return simplelist_del_data(exp,
                             (void *) &atom, 
                             comp_exp_compare,
                             comp_exp_destroy);
}

/* delete all atoms from this expression */
int comp_exp_purge(comp_exp_type *exp)
{
  unsigned int len;
  unsigned int i;

  if (exp == NULL)
    return -1;

  if (simplelist_length(*exp, &len) != 0)
    return -1;
  
  for (i = 0; i < len; i++)
    if (simplelist_del_index(exp, 0, comp_exp_destroy) != 0)
      return -1;

  return 0;
}

/* replace variables in comp with actual identifiers from identlist */
int comp_exp_replace_atom(comp_atom_type comp,
                          gnd_atom_type *ground,
                          stringlist_type varlist,
                          identlist_type identlist)
{
  if (ground == NULL)
    return -1;

  ground->type = comp.type;
  ground->truth = comp.truth;

  if (EPI_ATOM_IS_HOLDS(comp)) {
    if (comp_exp_get_ident(comp.atom.holds.subject,
                           &(ground->atom.holds.subject),
                           varlist,
                           identlist) != 0)
      return -1;
    if (comp_exp_get_ident(comp.atom.holds.access,
                           &(ground->atom.holds.access),
                           varlist,
                           identlist) != 0)
      return -1;
    if (comp_exp_get_ident(comp.atom.holds.object,
                           &(ground->atom.holds.object),
                           varlist,
                           identlist) != 0)
      return -1;

    return 0;
  }

  if (EPI_ATOM_IS_MEMB(comp)) {
    if (comp_exp_get_ident(comp.atom.memb.element,
                           &(ground->atom.memb.element),
                           varlist,
                           identlist) != 0)
      return -1;
    if (comp_exp_get_ident(comp.atom.memb.group,
                           &(ground->atom.memb.group),
                           varlist,
                           identlist) != 0)
      return -1;

    return 0;
  }

  if (EPI_ATOM_IS_SUBST(comp)) {
    if (comp_exp_get_ident(comp.atom.subst.group1,
                           &(ground->atom.subst.group1),
                           varlist,
                           identlist) != 0)
      return -1;
    if (comp_exp_get_ident(comp.atom.subst.group2,
                           &(ground->atom.subst.group2),
                           varlist,
                           identlist) != 0)
      return -1;

    return 0;
  }

  if (EPI_ATOM_IS_CONST(comp))
   return 0;

  return -1;
}

/* if name is an identifier, simply copy, if variable, replace */
int comp_exp_get_ident(name_type name,
                       ident_type **ident,
                       stringlist_type varlist,
                       identlist_type identlist)
{
  unsigned int tmp_index;

  if (ident == NULL)
    return -1;

  if (EPI_NAME_IS_VAR(name)) {
    if (stringlist_index(varlist, EPI_NAME_STRING(name), &tmp_index) != 0)
      return -1;
    if (identlist_get(identlist, tmp_index, ident) != 0)
      return -1;

    return 0;
  }
  if (EPI_NAME_IS_IDENT(name)) {
    *ident = name.name.ident;
    return 0;
  }

  return -1;
}

/* returns 0 if the ATOMS pointed to by p1 and p2 are equivalent */
int comp_exp_compare(void *p1, void *p2)
{
  if (p1 == NULL || p2 == NULL)
    return -1;
  
  return comp_atom_compare(* (comp_atom_type *) p1, * (comp_atom_type *) p2);
}

/* free memory */
int comp_exp_destroy(void *p)
{
  return comp_atom_destroy((comp_atom_type *) p);
}
