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
void comp_exp_destroy(void *p);

/* initialise list */
void comp_exp_init(comp_exp_type *exp)
{
  simplelist_init(exp);
}

/* gives the number of atoms in the expression */
unsigned int comp_exp_length(comp_exp_type exp)
{
  return simplelist_length(exp);
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

    comp_exp_purge(exp);

    if ((new_atom = EPI_COMPATOM_MALLOC) == NULL)
      return -1;

    if (comp_atom_create_const(new_atom, epi_false) != 0)
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
  gnd_atom_type tmp_gnd_atom;
  comp_atom_type *tmp_comp_atom = NULL;

  if (ground == NULL)
    return -1;

  /* first we ensure that the varlist and the identlist are of equal length */
  if (stringlist_length(varlist) != identlist_length(identlist))
    return -1;
  
  gnd_exp_init(ground);

  for (i = 0; i < comp_exp_length(comp); i++) {
    if (comp_exp_get(comp, i, &tmp_comp_atom) != 0)
      return -1;

    if (comp_exp_replace_atom(*tmp_comp_atom,
                              &tmp_gnd_atom,
                              varlist,
                              identlist) != 0)
      return -1;

    if (gnd_exp_add(ground, tmp_gnd_atom) != 0)
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
void comp_exp_purge(comp_exp_type *exp)
{
  unsigned int i;

  if (exp != NULL) {
    for (i = 0; i < simplelist_length(*exp); i++)
      simplelist_del_index(exp, 0, comp_exp_destroy);
  }
  simplelist_init(exp);
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
    if (comp_exp_get_ident(EPI_ATOM_HOLDS_SUBJECT(comp),
                           &(EPI_ATOM_HOLDS_SUBJECT(*ground)),
                           varlist,
                           identlist) != 0)
      return -1;
    if (comp_exp_get_ident(EPI_ATOM_HOLDS_ACCESS(comp),
                           &(EPI_ATOM_HOLDS_ACCESS(*ground)),
                           varlist,
                           identlist) != 0)
      return -1;
    if (comp_exp_get_ident(EPI_ATOM_HOLDS_OBJECT(comp),
                           &(EPI_ATOM_HOLDS_OBJECT(*ground)),
                           varlist,
                           identlist) != 0)
      return -1;

    return 0;
  }

  if (EPI_ATOM_IS_MEMB(comp)) {
    if (comp_exp_get_ident(EPI_ATOM_MEMB_ELEMENT(comp),
                           &(EPI_ATOM_MEMB_ELEMENT(*ground)),
                           varlist,
                           identlist) != 0)
      return -1;
    if (comp_exp_get_ident(EPI_ATOM_MEMB_GROUP(comp),
                           &(EPI_ATOM_MEMB_GROUP(*ground)),
                           varlist,
                           identlist) != 0)
      return -1;

    return 0;
  }

  if (EPI_ATOM_IS_SUBST(comp)) {
    if (comp_exp_get_ident(EPI_ATOM_SUBST_GROUP1(comp),
                           &(EPI_ATOM_SUBST_GROUP1(*ground)),
                           varlist,
                           identlist) != 0)
      return -1;
    if (comp_exp_get_ident(EPI_ATOM_SUBST_GROUP2(comp),
                           &(EPI_ATOM_SUBST_GROUP2(*ground)),
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
void comp_exp_destroy(void *p)
{
  comp_atom_destroy((comp_atom_type *) p);
}
