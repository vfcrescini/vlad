/* 
 * expression.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_EXPRESSION_H
#define __EPI_EXPRESSION_H

#include <simplelist.h>
#include "atom.h"

#define expression_type simplelist_type

/* initialise list */
int expression_init(expression_type *exp);

/* return 0 if the atom is in the expression */
int expression_find(expression_type exp, atom_type atom);

/* add an atom into the expression */
int expression_add(expression_type *exp, atom_type atom);

/* delete an atom from the expression */
int expression_del(expression_type *exp, atom_type atom);

/* delete all atoms from this expression */
int expression_purge();

#endif

