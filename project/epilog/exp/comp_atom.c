/*
 * comp_atom.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include "comp_atom.h"

/* creates a pointer to an atom of type const */
int comp_atom_create_const(comp_atom_type *atom, unsigned short int truth)
{
  if (atom == NULL)
    return EPI_NULLPTR;

  atom->type = EPI_ATOM_CONST;
  atom->truth = truth;

  return EPI_OK;
}

/* creates a pointer to an atom of type holds */
int comp_atom_create_holds(comp_atom_type *atom,
                           name_type sub,
                           name_type acc,
                           name_type obj,
                           unsigned short int truth)
{
  if (atom == NULL)
    return EPI_NULLPTR;

  /* must be subject, access, object (in that order) or variables. */
  if ((!EPI_NAME_IS_SUBJECT(sub) && !EPI_NAME_IS_VAR(sub)) ||
      (!EPI_NAME_IS_ACCESS(acc) && !EPI_NAME_IS_VAR(acc)) ||
      (!EPI_NAME_IS_OBJECT(obj) && !EPI_NAME_IS_VAR(obj)))
    return EPI_INVALIDINPUT;

  atom->type = EPI_ATOM_HOLDS;
  atom->truth = truth;
  atom->atom.holds.subject = sub; 
  atom->atom.holds.access = acc; 
  atom->atom.holds.object = obj; 

  return EPI_OK;
}

/* creates a pointer to an atom of type memb */
int comp_atom_create_memb(comp_atom_type *atom,
                          name_type element,
                          name_type group,
                          unsigned short int truth)
{
  if (atom == NULL)
    return EPI_NULLPTR;

  /* the assumption is that only subjects can be elements of subject-groups,
   * etc. */
  if ((EPI_NAME_IS_GROUP(element) && !EPI_NAME_IS_VAR(element)) ||
      (!EPI_NAME_IS_GROUP(group) && !EPI_NAME_IS_VAR(group)) ||
      (EPI_NAME_IS_IDENT(element) && EPI_NAME_IS_IDENT(group) &&
      EPI_NAME_BASETYPE(element) != EPI_NAME_BASETYPE(group)))
    return EPI_INVALIDINPUT;

  atom->type = EPI_ATOM_MEMB;
  atom->truth = truth;
  atom->atom.memb.element = element;
  atom->atom.memb.group = group;

  return EPI_OK;
}

/* creates a pointer to an atom of type subst */
int comp_atom_create_subst(comp_atom_type *atom,
                           name_type group1,
                           name_type group2,
                           unsigned short int truth)
{
  if (atom == NULL)
    return EPI_NULLPTR;

  /* the groups must be of the same type */
  if ((!EPI_NAME_IS_GROUP(group1) && !EPI_NAME_IS_VAR(group1)) ||
      (!EPI_NAME_IS_GROUP(group2) && !EPI_NAME_IS_VAR(group2)) ||
      (EPI_NAME_IS_IDENT(group1) && EPI_NAME_IS_IDENT(group2) &&
       group1.name.ident->type != group2.name.ident->type))
    return EPI_INVALIDINPUT;

  atom->type = EPI_ATOM_SUBST;
  atom->truth = truth;
  atom->atom.subst.group1 = group1;
  atom->atom.subst.group2 = group2;

  return EPI_OK;
}

/* destroys atom structure */
void comp_atom_destroy(comp_atom_type *atom)
{
  if (atom != NULL) {
    if (EPI_ATOM_IS_HOLDS(*atom)) {
      name_destroy(atom->atom.holds.subject);
      name_destroy(atom->atom.holds.access);
      name_destroy(atom->atom.holds.object);
    }
    else if (EPI_ATOM_IS_MEMB(*atom)) {
      name_destroy(atom->atom.memb.element);
      name_destroy(atom->atom.memb.group);
    }
    else if (EPI_ATOM_IS_SUBST(*atom)) {
      name_destroy(atom->atom.subst.group1);
      name_destroy(atom->atom.subst.group2);
    }

    free(atom);
  }
}

/* return 0 if the atom is valid */
int comp_atom_check(comp_atom_type atom)
{
  if (EPI_ATOM_IS_CONST(atom))
    return EPI_OK;

  if (EPI_ATOM_IS_HOLDS(atom)) {
    /* subject field must be subject or variable */
    if (!EPI_NAME_IS_SUBJECT(atom.atom.holds.subject) &&
        !EPI_NAME_IS_VAR(atom.atom.holds.subject))
      return EPI_FAILURE;
    /* access field must be access or variable */
    if (!EPI_NAME_IS_ACCESS(atom.atom.holds.access) && 
        !EPI_NAME_IS_VAR(atom.atom.holds.access))
      return EPI_FAILURE;
    /* object field must be access or variable */
    if (!EPI_NAME_IS_OBJECT(atom.atom.holds.object) && 
        !EPI_NAME_IS_VAR(atom.atom.holds.object))
      return EPI_FAILURE;
  }
  else if (EPI_ATOM_IS_MEMB(atom)) {
    /* first field must not be a group or a variable */
    if (EPI_NAME_IS_GROUP(atom.atom.memb.element) && 
        !EPI_NAME_IS_VAR(atom.atom.memb.element))
      return EPI_FAILURE;
    /* second field must be a group or a variable */
    if (!EPI_NAME_IS_GROUP(atom.atom.memb.group) && 
        !EPI_NAME_IS_VAR(atom.atom.memb.group))
      return EPI_FAILURE;
    /* if both non-variables, base type must be the same */
    if (!EPI_NAME_IS_VAR(atom.atom.memb.element) && 
        !EPI_NAME_IS_VAR(atom.atom.memb.group) &&
        EPI_NAME_BASETYPE(atom.atom.memb.element) != 
	EPI_NAME_BASETYPE(atom.atom.memb.group))
      return EPI_FAILURE;
  }
  else if (EPI_ATOM_IS_SUBST(atom)) {
    /* first field must be a group or a variable */
    if (!EPI_NAME_IS_GROUP(atom.atom.subst.group1) && 
        !EPI_NAME_IS_VAR(atom.atom.subst.group1))
      return EPI_FAILURE;
    /* second field must be a group or a variable */
    if (!EPI_NAME_IS_GROUP(atom.atom.subst.group2) && 
        !EPI_NAME_IS_VAR(atom.atom.subst.group2))
      return EPI_FAILURE;
    /* if both non-variables, base type must be the same */
    if (!EPI_NAME_IS_VAR(atom.atom.subst.group1) && 
        !EPI_NAME_IS_VAR(atom.atom.subst.group2) &&
        EPI_NAME_BASETYPE(atom.atom.subst.group1) != 
	EPI_NAME_BASETYPE(atom.atom.subst.group2))
      return EPI_FAILURE;
  }
  else
    return EPI_FAILURE;
 
  return EPI_OK;
}

/* creates a copy of atom1 */
int comp_atom_copy(comp_atom_type **atom2, comp_atom_type atom1)
{
  if (atom2 == NULL)
    return EPI_NULLPTR;

  if ((*atom2 = EPI_COMPATOM_MALLOC) == NULL)
    return EPI_MALLOCFAILED;

  if (EPI_ATOM_IS_CONST(atom1))
    return comp_atom_create_const(*atom2, atom1.truth);
  else if (EPI_ATOM_IS_HOLDS(atom1))
    return comp_atom_create_holds(*atom2,
                                  atom1.atom.holds.subject,
                                  atom1.atom.holds.access,
                                  atom1.atom.holds.object,
                                  atom1.truth);
  else if (EPI_ATOM_IS_MEMB(atom1))
    return comp_atom_create_memb(*atom2,
                                 atom1.atom.memb.element,
                                 atom1.atom.memb.group,
                                 atom1.truth);
  else if (EPI_ATOM_IS_SUBST(atom1))
    return comp_atom_create_subst(*atom2,
                                  atom1.atom.subst.group1,
                                  atom1.atom.subst.group2,
                                  atom1.truth);
  else
    return EPI_FAILURE;
}

/* returns 0 if the contents of atom1 and atom2 are identical */
int comp_atom_compare(comp_atom_type atom1, comp_atom_type atom2)
{
  if (atom1.type != atom2.type)
    return EPI_FAILURE;

  if (atom1.truth != atom2.truth)
    return EPI_FAILURE;
  
  if (EPI_ATOM_IS_HOLDS(atom1)) {
    if (name_compare(atom1.atom.holds.subject, atom2.atom.holds.subject) != EPI_OK)
      return EPI_FAILURE;
    if (name_compare(atom1.atom.holds.access, atom2.atom.holds.access) != EPI_OK)
      return EPI_FAILURE;
    if (name_compare(atom1.atom.holds.object, atom2.atom.holds.object) != EPI_OK)
      return EPI_FAILURE;
  }
  else if (EPI_ATOM_IS_MEMB(atom1)) {
    if (name_compare(atom1.atom.memb.element, atom2.atom.memb.element) != EPI_OK)
      return EPI_FAILURE;
    if (name_compare(atom1.atom.memb.group, atom2.atom.memb.group) != EPI_OK)
      return EPI_FAILURE;
  }
  else if (EPI_ATOM_IS_SUBST(atom1)) {
    if (name_compare(atom1.atom.subst.group1, atom2.atom.subst.group1) != EPI_OK)
      return EPI_FAILURE;
    if (name_compare(atom1.atom.subst.group2, atom2.atom.subst.group2) != EPI_OK)
      return EPI_FAILURE;
  }

  return EPI_OK;
}
