/*
 * atom.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include "atom.h"

/* creates a pointer to an atom of type const */
int atom_create_const(atom_type **atom, truth_type truth)
{
  if (atom == NULL)
    return -1;

  if ((*atom = (atom_type *) malloc(sizeof(atom_type))) == NULL)
    return -1;

  (*atom)->type = EPI_ATOM_CONST;
  (*atom)->truth = truth;

  return 0;
}

/* creates a pointer to an atom of type holds */
int atom_create_holds(atom_type **atom,
                      name_type sub,
                      name_type acc,
                      name_type obj,
                      truth_type truth)
{
  if (atom == NULL)
    return -1;

  /* must be subject, access, object (in that order) or variables. */
  if (!(EPI_NAME_IS_SUBJECT(sub) || EPI_NAME_IS_VAR(sub)) ||
      !(EPI_NAME_IS_ACCESS(acc) || EPI_NAME_IS_VAR(acc)) ||
      !(EPI_NAME_IS_OBJECT(obj) || EPI_NAME_IS_VAR(obj)))
    return -1;

  if ((*atom = (atom_type *) malloc(sizeof(atom_type))) == NULL)
    return -1;

  (*atom)->type = EPI_ATOM_HOLDS;
  (*atom)->truth = truth;
  (*atom)->atom.holds.subject = sub; 
  (*atom)->atom.holds.access = acc; 
  (*atom)->atom.holds.object = obj; 

  return 0;
}

/* creates a pointer to an atom of type memb */
int atom_create_memb(atom_type **atom,
                     name_type element,
                     name_type group,
                     truth_type truth)
{
  if (atom == NULL)
    return -1;

  /* the assumption is that only subjects can be elements of subject-groups,
   * etc. */
  if ((EPI_NAME_IS_GROUP(element) && !EPI_NAME_IS_VAR(element)) ||
      (!EPI_NAME_IS_GROUP(group) && !EPI_NAME_IS_VAR(group)) ||
      (EPI_NAME_IS_IDENT(element) && EPI_NAME_IS_IDENT(group) &&
      EPI_NAME_BASETYPE(element) != EPI_NAME_BASETYPE(group)))
    return -1;

  if ((*atom = (atom_type *) malloc(sizeof(atom_type))) == NULL)
    return -1;

  (*atom)->type = EPI_ATOM_MEMB;
  (*atom)->truth = truth;
  (*atom)->atom.memb.element = element;
  (*atom)->atom.memb.group = group;

  return 0;
}

/* creates a pointer to an atom of type subst */
int atom_create_subst(atom_type **atom,
                      name_type group1,
                      name_type group2,
                      truth_type truth)
{
  if (atom == NULL)
    return -1;

  /* the groups must be of the same type */
  if ((!EPI_NAME_IS_GROUP(group1) && !EPI_NAME_IS_VAR(group1)) ||
      (!EPI_NAME_IS_GROUP(group2) && !EPI_NAME_IS_VAR(group2)) ||
      (EPI_NAME_IS_IDENT(group1) && EPI_NAME_IS_IDENT(group2) &&
       group1.name.ident->type != group2.name.ident->type))
    return -1;

  if ((*atom = (atom_type *) malloc(sizeof(atom_type))) == NULL)
    return -1;

  (*atom)->type = EPI_ATOM_SUBST;
  (*atom)->truth = truth;
  (*atom)->atom.subst.group1 = group1;
  (*atom)->atom.subst.group2 = group2;

  return 0;
}

/* destroys atom structure */
int atom_destroy(atom_type *atom)
{
  if (atom == NULL)
    return -1;

  free(atom);
  atom = NULL;

  return 0;
}

/* return 0 if the atom is valid */
int atom_check(atom_type atom)
{
  if (EPI_ATOM_IS_CONST(atom))
    return 0;

  if (EPI_ATOM_IS_HOLDS(atom)) {
    /* subject field must be subject or variable */
    if (!EPI_NAME_IS_SUBJECT(atom.atom.holds.subject) && !EPI_NAME_IS_VAR(atom.atom.holds.subject))
      return -1;
    /* access field must be access or variable */
    if (!EPI_NAME_IS_ACCESS(atom.atom.holds.access) && !EPI_NAME_IS_VAR(atom.atom.holds.access))
      return -1;
    /* object field must be access or variable */
    if (!EPI_NAME_IS_OBJECT(atom.atom.holds.object) && !EPI_NAME_IS_VAR(atom.atom.holds.object))
      return -1;
  }
  else if (EPI_ATOM_IS_MEMB(atom)) {
    /* first field must not be a group or a variable */
    if (EPI_NAME_IS_GROUP(atom.atom.memb.element) && !EPI_NAME_IS_VAR(atom.atom.memb.element))
      return -1;
    /* second field must be a group or a variable */
    if (!EPI_NAME_IS_GROUP(atom.atom.memb.group) && !EPI_NAME_IS_VAR(atom.atom.memb.group))
      return -1;
    /* if both non-variables, base type must be the same */
    if (!EPI_NAME_IS_VAR(atom.atom.memb.element) && !EPI_NAME_IS_VAR(atom.atom.memb.group) &&
        EPI_NAME_BASETYPE(atom.atom.memb.element) != EPI_NAME_BASETYPE(atom.atom.memb.group))
      return -1;
  }
  else if (EPI_ATOM_IS_SUBST(atom)) {
    /* first field must be a group or a variable */
    if (!EPI_NAME_IS_GROUP(atom.atom.subst.group1) && !EPI_NAME_IS_VAR(atom.atom.subst.group1))
      return -1;
    /* second field must be a group or a variable */
    if (!EPI_NAME_IS_GROUP(atom.atom.subst.group2) && !EPI_NAME_IS_VAR(atom.atom.subst.group2))
      return -1;
    /* if both non-variables, base type must be the same */
    if (!EPI_NAME_IS_VAR(atom.atom.subst.group1) && !EPI_NAME_IS_VAR(atom.atom.subst.group2) &&
        EPI_NAME_BASETYPE(atom.atom.subst.group1) != EPI_NAME_BASETYPE(atom.atom.subst.group2))
      return -1;
  }
  else
    return -1;
 
  return 0;
}

/* creates a copy of atom1 */
int atom_copy(atom_type **atom2, atom_type atom1)
{
  if (atom2 == NULL)
    return -1;

  if (EPI_ATOM_IS_CONST(atom1))
    return atom_create_const(atom2, atom1.truth);
  else if (EPI_ATOM_IS_HOLDS(atom1))
    return atom_create_holds(atom2,
                             atom1.atom.holds.subject,
                             atom1.atom.holds.access,
                             atom1.atom.holds.object,
                             atom1.truth);
  else if (EPI_ATOM_IS_MEMB(atom1))
    return atom_create_memb(atom2,
                            atom1.atom.memb.element,
                            atom1.atom.memb.group,
                            atom1.truth);
  else if (EPI_ATOM_IS_SUBST(atom1))
    return atom_create_subst(atom2,
                             atom1.atom.subst.group1,
                             atom1.atom.subst.group2,
                             atom1.truth);
  else
    return -1;
}

/* returns 0 if the contents of atom1 and atom2 are identical */
int atom_compare(atom_type atom1, atom_type atom2)
{
  if (atom1.type != atom2.type)
    return -1;

  if (atom1.truth != atom2.truth)
    return -1;
  
  if (EPI_ATOM_IS_HOLDS(atom1)) {
    if (name_compare(atom1.atom.holds.subject, atom2.atom.holds.subject) != 0)
      return -1;
    if (name_compare(atom1.atom.holds.access, atom2.atom.holds.access) != 0)
      return -1;
    if (name_compare(atom1.atom.holds.object, atom2.atom.holds.object) != 0)
      return -1;
  }
  else if (EPI_ATOM_IS_MEMB(atom1)) {
    if (name_compare(atom1.atom.memb.element, atom2.atom.memb.element) != 0)
      return -1;
    if (name_compare(atom1.atom.memb.group, atom2.atom.memb.group) != 0)
      return -1;
  }
  else if (EPI_ATOM_IS_SUBST(atom1)) {
    if (name_compare(atom1.atom.subst.group1, atom2.atom.subst.group1) != 0)
      return -1;
    if (name_compare(atom1.atom.subst.group2, atom2.atom.subst.group2) != 0)
      return -1;
  }

  return 0;
}
