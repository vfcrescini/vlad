/*
 * gnd_atom.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include "gnd_atom.h"

/* returns 0 if the two are equal, -1 otherwise */
int gnd_atom_holds_cmp(gnd_atom_holds_type atom1, gnd_atom_holds_type atom2)
{
  return (atom1.subject == atom2.subject) &&
         (atom1.access == atom2.access) &&
         (atom1.object == atom2.object) ? 0 : -1;
}

/* returns 0 if the two are equal, -1 otherwise */
int gnd_atom_memb_cmp(gnd_atom_memb_type atom1, gnd_atom_memb_type atom2)
{
  return (atom1.member == atom2.member) &&
         (atom1.group == atom2.group) ? 0 : -1;
}

/* returns 0 if the two are equal, -1 otherwise */
int gnd_atom_subst_cmp(gnd_atom_subst_type atom1, gnd_atom_subst_type atom2)
{
  return (atom1.group1 == atom2.group1) &&
         (atom1.group2 == atom2.group2) ? 0 : -1;
}

/* returns 0 if the two are equal, -1 otherwise */
int gnd_atom_cmp(gnd_atom_type atom1, gnd_atom_type atom2)
{
  if (atom1.truth != atom2.truth)
    return -1;

  if (atom1.type != atom2.type)
    return -1;

  switch (atom1.type) {
    case atom_holds :
      return gnd_atom_holds_cmp(atom1.atom.holds, atom2.atom.holds);
    case atom_memb :
      return gnd_atom_memb_cmp(atom1.atom.memb, atom2.atom.memb);
    case atom_subst :
      return gnd_atom_subst_cmp(atom1.atom.subst, atom2.atom.subst);
  }

  return -1;
}
