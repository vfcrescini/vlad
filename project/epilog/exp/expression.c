/* 
 * expression.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include "expression.h"

int expression_compare(void *p1, void *p2);
int expression_destroy(void *p);

/* initialise list */
int expression_init(expression_type *exp)
{
  return simplelist_init(exp);
}

/* gives the number of atoms in the expression */
int expression_length(expression_type exp, unsigned int *len)
{
  return simplelist_length(exp, len);
}

/* return 0 if the atom is in the expression */
int expression_find(expression_type exp, atom_type atom)
{
  return simplelist_find_data(exp, (void *) &atom, expression_compare);
}

/* gives a reference to the index'th atom in the expression */
int expression_get(expression_type exp, unsigned int index, atom_type **atom)
{
  return simplelist_get_index(exp, index, (void **) atom);
}

/* add an atom into the expression */
int expression_add(expression_type *exp, atom_type atom)
{
  atom_type *new_atom = NULL;
  atom_type false_atom;
  
  if (exp == NULL)
    return -1;
  
  /* if an atom is already in simply return success */
  if (expression_find(*exp, atom) == 0)
    return 0;

  /* if the expression contains a FALSE constant, just return success */
  false_atom.type = EPI_ATOM_CONST;
  false_atom.truth = epi_false;

  if (expression_find(*exp, false_atom) == 0)
    return 0;

  /* if the negation of an atom is aleady in, or if the atom is a FALSE
   * constant, we replace the the whole expression with a constant false */
  
  EPI_ATOM_NEGATE(atom);
  if ((EPI_ATOM_IS_CONST(atom) && atom.truth == epi_true) ||
      expression_find(*exp, atom) == 0) {
    if (expression_purge(exp) != 0)
      return -1;
    if (atom_create_const(&new_atom, epi_false) != 0)
      return -1;
  }
  else {
    if (atom_copy(&new_atom, atom) != 0)
      return -1;

    EPI_ATOM_NEGATE(*new_atom);
  }

  return simplelist_add(exp, (void *) new_atom);
}

/* delete an atom from the expression */
int expression_del(expression_type *exp, atom_type atom)
{
  if (exp == NULL)
    return -1;

  return simplelist_del_data(exp,
                             (void *) &atom, 
                             expression_compare,
                             expression_destroy);
}

/* delete all atoms from this expression */
int expression_purge(expression_type *exp)
{
  unsigned int len;
  unsigned int i;

  if (exp == NULL)
    return -1;

  if (simplelist_length(*exp, &len) != 0)
    return -1;
  
  for (i = 0; i < len; i++)
    if (simplelist_del_index(exp, 0, expression_destroy) != 0)
      return -1;

  return 0;
}

/* return 0 if the lists are equivalent */
int expression_cmp(expression_type e1, expression_type e2)
{
  atom_type *a1;
  atom_type *a2;
  unsigned int len1;
  unsigned int len2;
  unsigned int i;

  if (simplelist_length(e1, &len1) != 0 ||
      simplelist_length(e2, &len2) != 0 ||
      len1 != len2)
    return -1;

  for (i = 0; i < len1; i++) {
    if (simplelist_get_index(e1, i, (void **) &a1) != 0 ||
        simplelist_get_index(e2, i, (void **) &a2) != 0)
      return -1;

    if (atom_compare(*a1, *a2) != 0)
      return -1;
  }
  
  return 0;
}

/* returns 0 if the ATOMS pointed to by p1 and p2 are equivalent */
int expression_compare(void *p1, void *p2)
{
  if (p1 == NULL || p2 == NULL)
    return -1;
  
  return atom_compare(* (atom_type *) p1, * (atom_type *) p2);
}

/* free memory */
int expression_destroy(void *p)
{
  return atom_destroy((atom_type *) p);
}
