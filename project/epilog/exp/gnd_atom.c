/*
 * gnd_atom.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include "gnd_atom.h"

/* creates a pointer to an atom of type const */
int gnd_atom_create_const(gnd_atom_type *atom, unsigned short int truth)
{
  if (atom == NULL)
    return EPI_NULLPTR;

  atom->type = EPI_ATOM_CONST;
  atom->truth = truth;

  return EPI_OK;
}

/* creates a pointer to an atom of type holds */
int gnd_atom_create_holds(gnd_atom_type *atom,
                          ident_type *sub,
                          ident_type *acc,
                          ident_type *obj,
                          unsigned short int truth)
{
  if (atom == NULL)
    return EPI_NULLPTR;

  /* must be subject, access, object (in that order) */
  if (!EPI_IDENT_IS_SUBJECT(*sub) ||
      !EPI_IDENT_IS_ACCESS(*acc) ||
      !EPI_IDENT_IS_OBJECT(*obj))
    return EPI_INVALIDINPUT;

  atom->type = EPI_ATOM_HOLDS;
  atom->truth = truth;
  atom->atom.holds.subject = sub; 
  atom->atom.holds.access = acc; 
  atom->atom.holds.object = obj; 

  return EPI_OK;
}

/* creates a pointer to an atom of type memb */
int gnd_atom_create_memb(gnd_atom_type *atom,
                         ident_type *element,
                         ident_type *group,
                         unsigned short int truth)
{
  if (atom == NULL)
    return EPI_NULLPTR;

  /* the assumption is that only subjects can be elements of subject-groups,
   * etc. */
  if (EPI_IDENT_IS_GROUP(*element) ||
      !EPI_IDENT_IS_GROUP(*group) ||
      EPI_IDENT_BASETYPE(*element) != EPI_IDENT_BASETYPE(*group))
    return EPI_INVALIDINPUT;

  atom->type = EPI_ATOM_MEMB;
  atom->truth = truth;
  atom->atom.memb.element = element;
  atom->atom.memb.group = group;

  return EPI_OK;
}

/* creates a pointer to an atom of type subst */
int gnd_atom_create_subst(gnd_atom_type *atom,
                          ident_type *group1,
                          ident_type *group2,
                          unsigned short int truth)
{
  if (atom == NULL)
    return EPI_NULLPTR;

  /* the groups must be of the same type */
  if (!EPI_IDENT_IS_GROUP(*group1) ||
      !EPI_IDENT_IS_GROUP(*group2) ||
      EPI_IDENT_BASETYPE(*group1) != EPI_IDENT_BASETYPE(*group2))
    return EPI_INVALIDINPUT;

  atom->type = EPI_ATOM_SUBST;
  atom->truth = truth;
  atom->atom.subst.group1 = group1;
  atom->atom.subst.group2 = group2;

  return EPI_OK;
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
    return EPI_OK;

  if (EPI_ATOM_IS_HOLDS(atom)) {
    /* subject field must be subject */
    if (!EPI_IDENT_IS_SUBJECT(*(atom.atom.holds.subject))) 
      return EPI_INVALIDINPUT;
    /* access field must be access */
    if (!EPI_IDENT_IS_ACCESS(*(atom.atom.holds.access)))
      return EPI_INVALIDINPUT;
    /* object field must be object */
    if (!EPI_IDENT_IS_OBJECT(*(atom.atom.holds.object)))
      return EPI_INVALIDINPUT;
  }
  else if (EPI_ATOM_IS_MEMB(atom)) {
    /* first field must not be a group */
    if (EPI_IDENT_IS_GROUP(*(atom.atom.memb.element)))
      return EPI_INVALIDINPUT;
    /* second field must be a group */
    if (!EPI_IDENT_IS_GROUP(*(atom.atom.memb.group)))
      return EPI_INVALIDINPUT;
    /* base type must be the same */
    if (EPI_IDENT_BASETYPE(*(atom.atom.memb.element)) != 
	EPI_IDENT_BASETYPE(*(atom.atom.memb.group)))
      return EPI_INVALIDINPUT;
  }
  else if (EPI_ATOM_IS_SUBST(atom)) {
    /* first field must be a group */
    if (!EPI_IDENT_IS_GROUP(*(atom.atom.subst.group1)))
      return EPI_INVALIDINPUT;
    /* second field must be a group */
    if (!EPI_IDENT_IS_GROUP(*(atom.atom.subst.group2)))
      return EPI_INVALIDINPUT;
    /* base type must be the same */
    if (EPI_IDENT_BASETYPE(*(atom.atom.subst.group1)) != 
	EPI_IDENT_BASETYPE(*(atom.atom.subst.group2)))
      return EPI_INVALIDINPUT;
  }
  else
    return EPI_INVALIDINPUT;
 
  return EPI_OK;
}

/* creates a copy of atom1 (no malloc) */
int gnd_atom_copy(gnd_atom_type *atom2, gnd_atom_type atom1)
{
  if (atom2 == NULL)
    return EPI_NULLPTR;

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
    return EPI_INVALIDINPUT;
}

/* creates a duplicate of atom1 (with malloc) */
int gnd_atom_dup(gnd_atom_type **atom2, gnd_atom_type atom1)
{
  if (atom2 == NULL)
    return EPI_NULLPTR;

  if ((*atom2 = EPI_GNDATOM_MALLOC) == NULL)
    return EPI_MALLOCFAILED;

  return gnd_atom_copy(*atom2, atom1);
}

/* returns 0 if the contents of atom1 and atom2 are identical */
int gnd_atom_compare(gnd_atom_type atom1, gnd_atom_type atom2)
{
  if (atom1.type != atom2.type)
    return EPI_FAILURE;

  if (atom1.truth != atom2.truth)
    return EPI_FAILURE;
  
  if (EPI_ATOM_IS_HOLDS(atom1)) {
    if (ident_compare(*(atom1.atom.holds.subject), *(atom2.atom.holds.subject)) != EPI_OK)
      return EPI_FAILURE;
    if (ident_compare(*(atom1.atom.holds.access), *(atom2.atom.holds.access)) != EPI_OK)
      return EPI_FAILURE;
    if (ident_compare(*(atom1.atom.holds.object), *(atom2.atom.holds.object)) != EPI_OK)
      return EPI_FAILURE;
  }
  else if (EPI_ATOM_IS_MEMB(atom1)) {
    if (ident_compare(*(atom1.atom.memb.element), *(atom2.atom.memb.element)) != EPI_OK)
      return EPI_FAILURE;
    if (ident_compare(*(atom1.atom.memb.group), *(atom2.atom.memb.group)) != EPI_OK)
      return EPI_FAILURE;
  }
  else if (EPI_ATOM_IS_SUBST(atom1)) {
    if (ident_compare(*(atom1.atom.subst.group1), *(atom2.atom.subst.group1)) != EPI_OK)
      return EPI_FAILURE;
    if (ident_compare(*(atom1.atom.subst.group2), *(atom2.atom.subst.group2)) != EPI_OK)
      return EPI_FAILURE;
  }

  return EPI_OK;
}
