/* 
 * comp_exp.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_COMP_EXP_H
#define __EPI_COMP_EXP_H

#include <simplelist.h>
#include "comp_atom.h"

#define comp_exp_type simplelist_type

/* initialise list */
int comp_exp_init(comp_exp_type *exp);

/* gives the number of atoms in the expression */
int comp_exp_length(comp_exp_type exp, unsigned int *len);

/* return 0 if the atom is in the expression */
int comp_exp_find(comp_exp_type exp, comp_atom_type atom);

/* gives a reference to the index'th atom in the expression */
int comp_exp_get(comp_exp_type exp, unsigned int index, comp_atom_type **atom);

/* add an atom into the expression */
int comp_exp_add(comp_exp_type *exp, comp_atom_type atom);

/* delete an atom from the expression */
int comp_exp_del(comp_exp_type *exp, comp_atom_type atom);

/* delete all atoms from this expression */
int comp_exp_purge(comp_exp_type *exp);

#endif
