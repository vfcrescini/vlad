/* 
 * expression.h
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

/* return 0 if the atom is in the expression */
int expression_find(expression_type exp, atom_type atom)
{
  return simplelist_find_data(exp, (void *) &atom, expression_compare);
}

/* add an atom into the expression */
int expression_add(expression_type *exp, atom_type atom)
{
  atom_type *new_atom = NULL;

  if (exp == NULL)
    return -1;

  if (atom_copy(&new_atom, atom))
    return -1;

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
    if (simplelist_del_index(exp, i, expression_destroy) != 0)
      return -1;

  return 0;
}

/* returns 0 if the ATOMS pointed to by p1 and p2 are equivalent */
int expression_compare(void *p1, void *p2)
{
  return atom_compare(* (atom_type *) p1, * (atom_type *) p2);
}

/* free memory */
int expression_destroy(void *p)
{
  return atom_destroy((atom_type *) p);
}
