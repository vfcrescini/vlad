/*
 * comp_atom.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_COMP_ATOM_H
#define __EPI_COMP_ATOM_H

#include "name.h"
#include "expression.h"

typedef struct 
{
  name_type subject;
  name_type access;
  name_type object;
} holds_type;

typedef struct
{
  name_type element;
  name_type group;
} memb_type;

typedef struct
{
  name_type group1;
  name_type group2;
} subst_type;

typedef struct
{
  unsigned short int type;
  truth_type truth;
  union {
    holds_type holds;
    memb_type memb; 
    subst_type subst;
  } atom;
} comp_atom_type;

/* creates a pointer to an comp_atom of type const */
int comp_atom_create_const(comp_atom_type **comp_atom, truth_type truth);

/* creates a pointer to an comp_atom of type holds */
int comp_atom_create_holds(comp_atom_type **comp_atom, 
                      name_type sub, 
                      name_type acc,
                      name_type obj,
                      truth_type truth);

/* creates a pointer to an comp_atom of type memb */
int comp_atom_create_memb(comp_atom_type **comp_atom,
                     name_type element,
                     name_type group,
                     truth_type truth);

/* creates a pointer to an comp_atom of type subst */
int comp_atom_create_subst(comp_atom_type **comp_atom,
                      name_type group1,
                      name_type group2,
                      truth_type truth);

/* return 0 if the comp_atom is valid */
int comp_atom_check(comp_atom_type comp_atom);

/* creates a copy of comp_atom1 */
int comp_atom_copy(comp_atom_type **comp_atom2, comp_atom_type comp_atom1);

/* destroys comp_atom structure */
void comp_atom_destroy(comp_atom_type *comp_atom);

/* returns 0 if the contents of comp_atom1 and comp_atom2 are identical */
int comp_atom_compare(comp_atom_type comp_atom1, comp_atom_type comp_atom2);

#endif
