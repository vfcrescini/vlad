/* 
 * comp_exp.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include "comp_exp.h"

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
