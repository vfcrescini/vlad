/*
 * gnd_atom.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_GND_ATOM_H
#define __EPI_GND_ATOM_H

#include "atom_type.h"

/* positive holds structure */
typedef struct
{
  unsigned int subject;
  unsigned int access;
  unsigned int object;
} gnd_atom_holds_type;

/* positive memb structure */
typedef struct
{
  unsigned int member;
  unsigned int group;
} gnd_atom_memb_type;

/* positive subst structure */
typedef struct
{
  unsigned int group1;
  unsigned int group2;
} gnd_atom_subst_type;

/* positive union of the three atoms */
typedef union
{
  gnd_atom_holds_type holds;
  gnd_atom_memb_type memb;
  gnd_atom_subst_type subst;
} gnd_positive_atom_type;

/* atom type */
typedef struct
{
  gnd_positive_atom_type atom;
  atom_type_type type;
  int truth;
} gnd_atom_type;

/* some convenience structure to compare atoms */
int gnd_atom_holds_cmp(gnd_atom_holds_type atom1, gnd_atom_holds_type atom2);
int gnd_atom_memb_cmp(gnd_atom_memb_type atom1, gnd_atom_memb_type atom2);
int gnd_atom_subst_cmp(gnd_atom_subst_type atom1, gnd_atom_subst_type atom2);
int gnd_atom_cmp(gnd_atom_type atom1, gnd_atom_type atom2);

#endif
