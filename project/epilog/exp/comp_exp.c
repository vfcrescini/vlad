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
int comp_exp_init(comp_exp_type *exp)
{
  return simplelist_init(exp);
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
  int tmp_res;
  
  if (exp == NULL)
    return EPI_NULLPTR;
  
  /* if an atom is already in simply return success */
  if (comp_exp_find(*exp, atom) == EPI_OK)
    return EPI_OK;

  /* if the expression contains a FALSE constant, just return success */
  false_atom.type = EPI_ATOM_CONST;
  false_atom.truth = EPI_FALSE;

  if (comp_exp_find(*exp, false_atom) == EPI_OK)
    return EPI_OK;

  /* if the negation of an atom is aleady in, or if the atom is a FALSE
   * constant, we replace the the whole expression with a constant false */
  EPI_ATOM_NEGATE(atom);
  if ((EPI_ATOM_IS_CONST(atom) && atom.truth == EPI_TRUE) ||
      comp_exp_find(*exp, atom) == EPI_OK) {

    comp_exp_purge(exp);

    if ((new_atom = EPI_COMPATOM_MALLOC) == NULL)
      return EPI_MALLOCFAILED;

    if ((tmp_res = comp_atom_create_const(new_atom, EPI_FALSE)) != EPI_OK)
      return tmp_res;
  }
  else {
    if ((tmp_res = comp_atom_copy(&new_atom, atom)) != EPI_OK)
      return tmp_res;

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
  int tmp_res;

  if (ground == NULL)
    return EPI_NULLPTR;

  /* first we ensure that the varlist and the identlist are of equal length */
  if (stringlist_length(varlist) != identlist_length(identlist))
    return EPI_INVALIDINPUT;
  
  gnd_exp_init(ground);

  for (i = 0; i < comp_exp_length(comp); i++) {
    if ((tmp_res = comp_exp_get(comp, i, &tmp_comp_atom)) != EPI_OK)
      return tmp_res;

    if ((tmp_res = comp_exp_replace_atom(*tmp_comp_atom,
                                         &tmp_gnd_atom,
                                         varlist,
                                         identlist)) != EPI_OK)
      return tmp_res;

    if ((tmp_res = gnd_exp_add(ground, tmp_gnd_atom)) != EPI_OK)
      return tmp_res;
  }

  return EPI_OK;
}

/* delete all atoms from this expression */
void comp_exp_purge(comp_exp_type *exp)
{
  simplelist_purge(exp, comp_exp_destroy);
}

/* replace variables in comp with actual identifiers from identlist */
int comp_exp_replace_atom(comp_atom_type comp,
                          gnd_atom_type *ground,
                          stringlist_type varlist,
                          identlist_type identlist)
{
  int tmp_res;
  
  if (ground == NULL)
    return EPI_NULLPTR;

  ground->type = comp.type;
  ground->truth = comp.truth;

  if (EPI_ATOM_IS_HOLDS(comp)) {
    if ((tmp_res = comp_exp_get_ident(EPI_ATOM_HOLDS_SUBJECT(comp),
                                      &(EPI_ATOM_HOLDS_SUBJECT(*ground)),
                                      varlist,
                                      identlist)) != EPI_OK)
      return tmp_res;
    if ((tmp_res = comp_exp_get_ident(EPI_ATOM_HOLDS_ACCESS(comp),
                                      &(EPI_ATOM_HOLDS_ACCESS(*ground)),
                                      varlist,
                                      identlist)) != EPI_OK)
      return tmp_res;
    if ((tmp_res = comp_exp_get_ident(EPI_ATOM_HOLDS_OBJECT(comp),
                                      &(EPI_ATOM_HOLDS_OBJECT(*ground)),
                                      varlist,
                                      identlist)) != EPI_OK)
      return tmp_res;

    return EPI_OK;
  }

  if (EPI_ATOM_IS_MEMB(comp)) {
    if ((tmp_res = comp_exp_get_ident(EPI_ATOM_MEMB_ELEMENT(comp),
                                      &(EPI_ATOM_MEMB_ELEMENT(*ground)),
                                      varlist,
                                      identlist)) != EPI_OK)
      return tmp_res;
    if ((tmp_res = comp_exp_get_ident(EPI_ATOM_MEMB_GROUP(comp),
                                      &(EPI_ATOM_MEMB_GROUP(*ground)),
                                      varlist,
                                      identlist)) != EPI_OK)
      return tmp_res;

    return EPI_OK;
  }

  if (EPI_ATOM_IS_SUBST(comp)) {
    if ((tmp_res = comp_exp_get_ident(EPI_ATOM_SUBST_GROUP1(comp),
                                      &(EPI_ATOM_SUBST_GROUP1(*ground)),
                                      varlist,
                                      identlist)) != EPI_OK)
      return tmp_res;
    if ((tmp_res = comp_exp_get_ident(EPI_ATOM_SUBST_GROUP2(comp),
                                      &(EPI_ATOM_SUBST_GROUP2(*ground)),
                                      varlist,
                                      identlist)) != EPI_OK)
      return tmp_res;

    return EPI_OK;
  }

  if (EPI_ATOM_IS_CONST(comp))
   return EPI_OK;

  return EPI_FAILURE;
}

/* if name is an identifier, simply copy, if variable, replace */
int comp_exp_get_ident(name_type name,
                       ident_type **ident,
                       stringlist_type varlist,
                       identlist_type identlist)
{
  unsigned int tmp_index;
  int tmp_res;

  if (ident == NULL)
    return EPI_NULLPTR;

  if (EPI_NAME_IS_VAR(name)) {
    if ((tmp_res = stringlist_index(varlist, EPI_NAME_STRING(name), &tmp_index)) != EPI_OK)
      return tmp_res;
    if ((tmp_res = identlist_get(identlist, tmp_index, ident)) != EPI_OK)
      return tmp_res;

    return EPI_OK;
  }

  if (EPI_NAME_IS_IDENT(name)) {
    *ident = name.name.ident;
    return EPI_OK;
  }

  return EPI_FAILURE;
}

/* returns 0 if the ATOMS pointed to by p1 and p2 are equivalent */
int comp_exp_compare(void *p1, void *p2)
{
  if (p1 == NULL || p2 == NULL)
    return EPI_NULLPTR;
  
  return comp_atom_compare(* (comp_atom_type *) p1, * (comp_atom_type *) p2);
}

/* free memory */
void comp_exp_destroy(void *p)
{
  comp_atom_destroy((comp_atom_type *) p);
}