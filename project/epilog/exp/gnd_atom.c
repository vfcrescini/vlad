/*
 * gnd_atom.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include "gnd_atom.h"

/* creates a pointer to an atom of type const */
int gnd_atom_create_const(gnd_atom_type **atom, truth_type truth)
{
  if (atom == NULL)
    return -1;

  if ((*atom = (gnd_atom_type *) malloc(sizeof(gnd_atom_type))) == NULL)
    return -1;

  (*atom)->type = EPI_ATOM_CONST;
  (*atom)->truth = truth;

  return 0;
}

/* creates a pointer to an atom of type holds */
int gnd_atom_create_holds(gnd_atom_type **atom,
                      ident_type *sub,
                      ident_type *acc,
                      ident_type *obj,
                      truth_type truth)
{
  if (atom == NULL)
    return -1;

  /* must be subject, access, object (in that order) */
  if (!EPI_IDENT_IS_SUBJECT(*sub) ||
      !EPI_IDENT_IS_ACCESS(*acc) ||
      !EPI_IDENT_IS_OBJECT(*obj))
    return -1;

  if ((*atom = (gnd_atom_type *) malloc(sizeof(gnd_atom_type))) == NULL)
    return -1;

  (*atom)->type = EPI_ATOM_HOLDS;
  (*atom)->truth = truth;
  (*atom)->atom.holds.subject = sub; 
  (*atom)->atom.holds.access = acc; 
  (*atom)->atom.holds.object = obj; 

  return 0;
}

/* creates a pointer to an atom of type memb */
int gnd_atom_create_memb(gnd_atom_type **atom,
                         ident_type *element,
                         ident_type *group,
                         truth_type truth)
{
  if (atom == NULL)
    return -1;

  /* the assumption is that only subjects can be elements of subject-groups,
   * etc. */
  if (EPI_IDENT_IS_GROUP(*element) ||
      !EPI_IDENT_IS_GROUP(*group) ||
      EPI_IDENT_BASETYPE(*element) != EPI_IDENT_BASETYPE(*group))
    return -1;

  if ((*atom = (gnd_atom_type *) malloc(sizeof(gnd_atom_type))) == NULL)
    return -1;

  (*atom)->type = EPI_ATOM_MEMB;
  (*atom)->truth = truth;
  (*atom)->atom.memb.element = element;
  (*atom)->atom.memb.group = group;

  return 0;
}

/* creates a pointer to an atom of type subst */
int gnd_atom_create_subst(gnd_atom_type **atom,
                      ident_type *group1,
                      ident_type *group2,
                      truth_type truth)
{
  if (atom == NULL)
    return -1;

  /* the groups must be of the same type */
  if (!EPI_IDENT_IS_GROUP(*group1) ||
      !EPI_IDENT_IS_GROUP(*group2) ||
      EPI_IDENT_BASETYPE(*group1) != EPI_IDENT_BASETYPE(*group2))
    return -1;

  if ((*atom = (gnd_atom_type *) malloc(sizeof(gnd_atom_type))) == NULL)
    return -1;

  (*atom)->type = EPI_ATOM_SUBST;
  (*atom)->truth = truth;
  (*atom)->atom.subst.group1 = group1;
  (*atom)->atom.subst.group2 = group2;

  return 0;
}

/* destroys atom structure */
void gnd_atom_destroy(gnd_atom_type *atom)
{
  if (atom != NULL)
    free(atom);
}

/* return 0 if the atom is valid */
int gnd_atom_check(gnd_atom_type atom)
{
  if (EPI_ATOM_IS_CONST(atom))
    return 0;

  if (EPI_ATOM_IS_HOLDS(atom)) {
    /* subject field must be subject */
    if (!EPI_IDENT_IS_SUBJECT(*(atom.atom.holds.subject))) 
      return -1;
    /* access field must be access */
    if (!EPI_IDENT_IS_ACCESS(*(atom.atom.holds.access)))
      return -1;
    /* object field must be object */
    if (!EPI_IDENT_IS_OBJECT(*(atom.atom.holds.object)))
      return -1;
  }
  else if (EPI_ATOM_IS_MEMB(atom)) {
    /* first field must not be a group */
    if (EPI_IDENT_IS_GROUP(*(atom.atom.memb.element)))
      return -1;
    /* second field must be a group */
    if (!EPI_IDENT_IS_GROUP(*(atom.atom.memb.group)))
      return -1;
    /* base type must be the same */
    if (EPI_IDENT_BASETYPE(*(atom.atom.memb.element)) != 
	EPI_IDENT_BASETYPE(*(atom.atom.memb.group)))
      return -1;
  }
  else if (EPI_ATOM_IS_SUBST(atom)) {
    /* first field must be a group */
    if (!EPI_IDENT_IS_GROUP(*(atom.atom.subst.group1)))
      return -1;
    /* second field must be a group */
    if (!EPI_IDENT_IS_GROUP(*(atom.atom.subst.group2)))
      return -1;
    /* base type must be the same */
    if (EPI_IDENT_BASETYPE(*(atom.atom.subst.group1)) != 
	EPI_IDENT_BASETYPE(*(atom.atom.subst.group2)))
      return -1;
  }
  else
    return -1;
 
  return 0;
}

/* creates a copy of atom1 */
int gnd_atom_copy(gnd_atom_type **atom2, gnd_atom_type atom1)
{
  if (atom2 == NULL)
    return -1;

  if (EPI_ATOM_IS_CONST(atom1))
    return gnd_atom_create_const(atom2, atom1.truth);
  else if (EPI_ATOM_IS_HOLDS(atom1))
    return gnd_atom_create_holds(atom2,
                                 atom1.atom.holds.subject,
                                 atom1.atom.holds.access,
                                 atom1.atom.holds.object,
                                 atom1.truth);
  else if (EPI_ATOM_IS_MEMB(atom1))
    return gnd_atom_create_memb(atom2,
                                atom1.atom.memb.element,
                                atom1.atom.memb.group,
                                atom1.truth);
  else if (EPI_ATOM_IS_SUBST(atom1))
    return gnd_atom_create_subst(atom2,
                                 atom1.atom.subst.group1,
                                 atom1.atom.subst.group2,
                                 atom1.truth);
  else
    return -1;
}

/* returns 0 if the contents of atom1 and atom2 are identical */
int gnd_atom_compare(gnd_atom_type atom1, gnd_atom_type atom2)
{
  if (atom1.type != atom2.type)
    return -1;

  if (atom1.truth != atom2.truth)
    return -1;
  
  if (EPI_ATOM_IS_HOLDS(atom1)) {
    if (ident_compare(*(atom1.atom.holds.subject), *(atom2.atom.holds.subject)) != 0)
      return -1;
    if (ident_compare(*(atom1.atom.holds.access), *(atom2.atom.holds.access)) != 0)
      return -1;
    if (ident_compare(*(atom1.atom.holds.object), *(atom2.atom.holds.object)) != 0)
      return -1;
  }
  else if (EPI_ATOM_IS_MEMB(atom1)) {
    if (ident_compare(*(atom1.atom.memb.element), *(atom2.atom.memb.element)) != 0)
      return -1;
    if (ident_compare(*(atom1.atom.memb.group), *(atom2.atom.memb.group)) != 0)
      return -1;
  }
  else if (EPI_ATOM_IS_SUBST(atom1)) {
    if (ident_compare(*(atom1.atom.subst.group1), *(atom2.atom.subst.group1)) != 0)
      return -1;
    if (ident_compare(*(atom1.atom.subst.group2), *(atom2.atom.subst.group2)) != 0)
      return -1;
  }

  return 0;
}
