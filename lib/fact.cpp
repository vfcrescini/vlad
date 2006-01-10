/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004, 2005 University of Western Sydney
 * by Vino Fernando Crescini <jcrescin@cit.uws.edu.au>
 *
 * PolicyUpdater is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * PolicyUpdater is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PolicyUpdater; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <cstdlib>
#include <cstddef>
#include <cstring>

#include <vlad/vlad.h>
#include <vlad/mem.h>
#include <vlad/identifier.h>
#include <vlad/fact.h>

/* some extra functions */

/* return VLAD_OK if the given holds atom is valid */
static int vlad_verify_holds(const char *a_sub,
                             const char *a_acc,
                             const char *a_obj,
                             vlad_symtab *a_stab,
                             vlad_varlist *a_vlist1,
                             vlad_varlist *a_vlist2);

/* return VLAD_OK if the given memb atom is valid */
static int vlad_verify_memb(const char *a_elt,
                            const char *a_grp,
                            vlad_symtab *a_stab,
                            vlad_varlist *a_vlist1,
                            vlad_varlist *a_vlist2);


/* return VLAD_OK if the given subst atom is valid */
static int vlad_verify_subst(const char *a_grp1,
                             const char *a_grp2,
                             vlad_symtab *a_stab,
                             vlad_varlist *a_vlist1,
                             vlad_varlist *a_vlist2);

vlad_fact::vlad_fact()
{
  /* choose holds as a default type */
  m_type = VLAD_ATOM_HOLDS;
  m_holds.subject = NULL;
  m_holds.access = NULL;
  m_holds.object = NULL;
  m_truth = true;
  m_init = false;
}

vlad_fact::~vlad_fact()
{
  reset();
}

/* returns true if items are equal */
bool vlad_fact::cmp(vlad_list_item *a_item)
{
  vlad_fact *fact = NULL;

  if (a_item == NULL)
    return false;

  if ((fact = dynamic_cast<vlad_fact *>(a_item)) == NULL)
    return false;

  /* if both are uninitialised, return true. if only one, return false */
  if (!m_init)
    return !fact->m_init;
  if (!fact->m_init)
    return false;

  /* make sure the types are equal */
  if (m_type != fact->m_type)
    return false;

  switch(m_type) {
    case VLAD_ATOM_HOLDS :
      return
        (m_truth == fact->m_truth) &&
        !strcmp(m_holds.subject, fact->m_holds.subject) &&
        !strcmp(m_holds.access, fact->m_holds.access) &&
        !strcmp(m_holds.object, fact->m_holds.object);
    case VLAD_ATOM_MEMBER :
      return
        (m_truth == fact->m_truth) &&
        !strcmp(m_member.element, fact->m_member.element) &&
        !strcmp(m_member.group, fact->m_member.group);
    case VLAD_ATOM_SUBSET :
      return
        (m_truth == fact->m_truth) &&
        !strcmp(m_subset.group1, fact->m_subset.group1) &&
        !strcmp(m_subset.group2, fact->m_subset.group2);
  }

  return false;
}

/* get attributes from fact */
int vlad_fact::get(char **a_elt1,
                   char **a_elt2,
                   char **a_elt3,
                   unsigned char *a_type,
                   bool *a_truth)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_type == NULL || a_truth == NULL)
    return VLAD_NULLPTR;

  *a_type = m_type;
  *a_truth = m_truth;

  switch(m_type) {
    case VLAD_ATOM_HOLDS :
      return get_holds(a_elt1, a_elt2, a_elt3);
    case VLAD_ATOM_MEMBER :
      return get_member(a_elt1, a_elt2);
    case VLAD_ATOM_SUBSET :
      return get_subset(a_elt1, a_elt2);
  }

  /* type will never be anything but the 3 above so this will never happen */
  return VLAD_FAILURE;
}

/* if holds, get attributes */
int vlad_fact::get_holds(char **a_sub, char **a_acc, char **a_obj)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_sub == NULL || a_acc == NULL || a_obj == NULL)
    return VLAD_NULLPTR;

  if (m_type != VLAD_ATOM_HOLDS)
    return VLAD_FAILURE;

  *a_sub = m_holds.subject;
  *a_acc = m_holds.access;
  *a_obj = m_holds.object;

  return VLAD_OK;
}

/* if member, get attributes */
int vlad_fact::get_member(char **a_elt, char **a_grp)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_elt == NULL || a_grp == NULL)
    return VLAD_NULLPTR;

  if (m_type != VLAD_ATOM_MEMBER)
    return VLAD_FAILURE;

  *a_elt = m_member.element;
  *a_grp = m_member.group;

  return VLAD_OK;
}

/* if subset, get attributes */
int vlad_fact::get_subset(char **a_grp1, char **a_grp2)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_grp1 == NULL || a_grp2 == NULL)
    return VLAD_NULLPTR;

  if (m_type != VLAD_ATOM_SUBSET)
    return VLAD_FAILURE;

  *a_grp1 = m_subset.group1;
  *a_grp2 = m_subset.group2;

  return VLAD_OK;
}

/* initialise facts */
int vlad_fact::init(const char *a_elt1,
                    const char *a_elt2,
                    const char *a_elt3,
                    unsigned char a_type,
                    bool a_truth)
{
  switch(a_type) {
    case VLAD_ATOM_HOLDS :
      return init_holds(a_elt1, a_elt2, a_elt3, a_truth);
    case VLAD_ATOM_MEMBER :
      return init_member(a_elt1, a_elt2, a_truth);
    case VLAD_ATOM_SUBSET :
      return init_subset(a_elt1, a_elt2, a_truth);
  }
  return VLAD_INVALIDINPUT;
}

/* initialise a holds fact */
int vlad_fact::init_holds(const char *a_sub,
                          const char *a_acc,
                          const char *a_obj,
                          bool a_truth)
{
  if (a_sub == NULL || a_acc == NULL || a_obj == NULL)
    return VLAD_NULLPTR;

  reset();

  m_type = VLAD_ATOM_HOLDS;
  m_truth = a_truth;

  if ((m_holds.subject = VLAD_MEM_STR_MALLOC(a_sub)) == NULL)
    return VLAD_MALLOCFAILED;
  if ((m_holds.access = VLAD_MEM_STR_MALLOC(a_acc)) == NULL)
    return VLAD_MALLOCFAILED;
  if ((m_holds.object = VLAD_MEM_STR_MALLOC(a_obj)) == NULL)
    return VLAD_MALLOCFAILED;

  /* strcpy never fails */
  strcpy(m_holds.subject, a_sub);
  strcpy(m_holds.access, a_acc);
  strcpy(m_holds.object, a_obj);

  m_init = true;

  return VLAD_OK;
}

/* initialise a memb fact */
int vlad_fact::init_member(const char *a_elt,
                           const char *a_grp,
                           bool a_truth)
{
  if (a_elt == NULL || a_grp == NULL)
    return VLAD_NULLPTR;

  reset();

  m_type = VLAD_ATOM_MEMBER;
  m_truth = a_truth;

  if ((m_member.element = VLAD_MEM_STR_MALLOC(a_elt)) == NULL)
    return VLAD_MALLOCFAILED;
  if ((m_member.group = VLAD_MEM_STR_MALLOC(a_grp)) == NULL)
    return VLAD_MALLOCFAILED;

  /* strcpy never fails */
  strcpy(m_member.element, a_elt);
  strcpy(m_member.group, a_grp);

  m_init = true;

  return VLAD_OK;
}

/* initialise a subst fact */
int vlad_fact::init_subset(const char *a_grp1,
                           const char *a_grp2,
                           bool a_truth)
{
  if (a_grp1 == NULL || a_grp2 == NULL)
    return VLAD_NULLPTR;

  reset();

  m_type = VLAD_ATOM_SUBSET;
  m_truth = a_truth;

  if ((m_subset.group1 = VLAD_MEM_STR_MALLOC(a_grp1)) == NULL)
    return VLAD_MALLOCFAILED;
  if ((m_subset.group2 = VLAD_MEM_STR_MALLOC(a_grp2)) == NULL)
    return VLAD_MALLOCFAILED;

  /* strcpy never fails */
  strcpy(m_subset.group1, a_grp1);
  strcpy(m_subset.group2, a_grp2);

  m_init = true;

  return VLAD_OK;
}

/* create a new instance of this fact */
int vlad_fact::copy(vlad_fact **a_fact)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_fact == NULL)
    return VLAD_NULLPTR;

  if ((*a_fact = VLAD_MEM_NEW(vlad_fact())) == NULL)
    return VLAD_MALLOCFAILED;

  switch(m_type) {
    case VLAD_ATOM_HOLDS :
      return (*a_fact)->init_holds(m_holds.subject,
                                   m_holds.access,
                                   m_holds.object,
                                   m_truth);
    case VLAD_ATOM_MEMBER :
      return (*a_fact)->init_member(m_member.element,
                                    m_member.group,
                                    m_truth);
    case VLAD_ATOM_SUBSET :
      return (*a_fact)->init_subset(m_subset.group1,
                                    m_subset.group2,
                                    m_truth);
  }

  return VLAD_INVALIDINPUT;
}

/* verify and copy */
int vlad_fact::vcopy(vlad_symtab *a_stab,
                     vlad_varlist *a_vlist,
                     vlad_fact **a_fact)
{
  int retval;

  /* first we verify */
  if ((retval = verify(a_stab, a_vlist)) != VLAD_OK)
    return retval;

  /* then we copy */
  return copy(a_fact);
}

/* replaces vars in a_vlist with idents in a_ilist. gives a new fact */
int vlad_fact::replace(vlad_varlist *a_vlist,
                       vlad_stringlist *a_ilist,
                       vlad_fact **a_fact)
{
  int retval;
  unsigned int index;
  char *name[3];

  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_fact == NULL)
    return VLAD_NULLPTR;

  /* if both lists are null or if the lengths are zero, return the fact */
  if (((a_vlist == NULL) && (a_ilist == NULL)) || ((a_vlist->length() == 0) && (a_ilist->length() == 0)))
    return copy(a_fact);

  /* make sure lists are non-NULL and their lengths are equal */
  if (a_vlist == NULL || a_ilist == NULL || a_vlist->length() != a_ilist->length())
    return VLAD_INVALIDINPUT;

  /* all clear */
  if ((*a_fact = VLAD_MEM_NEW(vlad_fact())) == NULL)
    return VLAD_MALLOCFAILED;

  /*
   * at this level we don't really care if the entities are valid or not.
   * we simply replace strings from the first list with strings from the
   * second list.
   */

  switch(m_type) {
    case VLAD_ATOM_HOLDS :
      /* subject */
      if ((retval = a_vlist->get(m_holds.subject, &index)) == VLAD_NOTFOUND)
        name[0] = m_holds.subject;
      else if (retval == VLAD_OK)
        a_ilist->get(index, &(name[0]));
      else
        return retval;
      /* access */
      if ((retval = a_vlist->get(m_holds.access, &index)) == VLAD_NOTFOUND)
        name[1] = m_holds.access;
      else if (retval == VLAD_OK)
        a_ilist->get(index, &(name[1]));
      else
        return retval;
      /* object */
      if ((retval = a_vlist->get(m_holds.object, &index)) == VLAD_NOTFOUND)
        name[2] = m_holds.object;
      else if (retval == VLAD_OK)
        a_ilist->get(index, &(name[2]));
      else
        return retval;

      return (*a_fact)->init_holds(name[0], name[1], name[2], m_truth);
    case VLAD_ATOM_MEMBER :
      /* element */
      if ((retval = a_vlist->get(m_member.element, &index)) == VLAD_NOTFOUND)
        name[0] = m_member.element;
      else if (retval == VLAD_OK)
        a_ilist->get(index, &(name[0]));
      else
        return retval;
      /* group */
      if ((retval = a_vlist->get(m_member.group, &index)) == VLAD_NOTFOUND)
        name[1] = m_member.group;
      else if (retval == VLAD_OK)
        a_ilist->get(index, &(name[1]));
      else
        return retval;

      return (*a_fact)->init_member(name[0], name[1], m_truth);
    case VLAD_ATOM_SUBSET :
      /* group1 */
      if ((retval = a_vlist->get(m_subset.group1, &index)) == VLAD_NOTFOUND)
        name[0] = m_subset.group1;
      else if (retval == VLAD_OK)
        a_ilist->get(index, &(name[0]));
      else
        return retval;
      /* group2 */
      if ((retval = a_vlist->get(m_subset.group2, &index)) == VLAD_NOTFOUND)
        name[1] = m_subset.group2;
      else if (retval == VLAD_OK)
        a_ilist->get(index, &(name[1]));
      else
        return retval;

      return (*a_fact)->init_subset(name[0], name[1], m_truth);
  }

  return VLAD_FAILURE;
}

/* replace then verify */
int vlad_fact::vreplace(vlad_symtab *a_stab,
                        vlad_varlist *a_vlist,
                        vlad_stringlist *a_ilist,
                        vlad_fact **a_fact)
{
  int retval;

  if ((retval = replace(a_vlist, a_ilist, a_fact)) != VLAD_OK)
    return retval;

  if ((retval = (*a_fact)->verify(a_stab, NULL)) != VLAD_OK) {
    VLAD_MEM_DELETE(*a_fact);
    return retval;
  }

  return VLAD_OK;
}

/* gives a list of vars occuring in the fact. assumes list is init'ed */
int vlad_fact::varlist(vlad_varlist *a_list)
{
  int retval;

  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_list == NULL)
    return VLAD_NULLPTR;

  switch(m_type) {
    case VLAD_ATOM_HOLDS :
      if (vlad_identifier::validate_var_ident(m_holds.subject) == VLAD_OK)
        if ((retval = a_list->add(m_holds.subject)) != VLAD_OK)
          if (retval != VLAD_DUPLICATE)
            return retval;
      if (vlad_identifier::validate_var_ident(m_holds.access) == VLAD_OK)
        if ((retval = a_list->add(m_holds.access)) != VLAD_OK)
          if (retval != VLAD_DUPLICATE)
            return retval;
      if (vlad_identifier::validate_var_ident(m_holds.object) == VLAD_OK)
        if ((retval = a_list->add(m_holds.object)) != VLAD_OK)
          if (retval != VLAD_DUPLICATE)
            return retval;

      return VLAD_OK;
    case VLAD_ATOM_MEMBER :
      if (vlad_identifier::validate_var_ident(m_member.element) == VLAD_OK)
        if ((retval = a_list->add(m_member.element)) != VLAD_OK)
          if (retval != VLAD_DUPLICATE)
            return retval;
      if (vlad_identifier::validate_var_ident(m_member.group) == VLAD_OK)
        if ((retval = a_list->add(m_member.group)) != VLAD_OK)
          if (retval != VLAD_DUPLICATE)
            return retval;

      return VLAD_OK;
    case VLAD_ATOM_SUBSET :
      if (vlad_identifier::validate_var_ident(m_subset.group1) == VLAD_OK)
        if ((retval = a_list->add(m_subset.group1)) != VLAD_OK)
          if (retval != VLAD_DUPLICATE)
            return retval;
      if (vlad_identifier::validate_var_ident(m_subset.group2) == VLAD_OK)
        if ((retval = a_list->add(m_subset.group2)) != VLAD_OK)
          if (retval != VLAD_DUPLICATE)
            return retval;

      return VLAD_OK;
  }

  return VLAD_FAILURE;
}

/* same as above, but verifies the fact too */
int vlad_fact::vvarlist(vlad_symtab *a_stab, vlad_varlist *a_list)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_stab == NULL || a_list == NULL)
    return VLAD_NULLPTR;

  switch(m_type) {
    case VLAD_ATOM_HOLDS :
      return vlad_verify_holds(m_holds.subject,
                               m_holds.access,
                               m_holds.object,
                               a_stab,
                               NULL,
                               a_list);
    case VLAD_ATOM_MEMBER :
      return vlad_verify_memb(m_member.element,
                              m_member.group,
                              a_stab,
                              NULL,
                              a_list);

    case VLAD_ATOM_SUBSET :
      return vlad_verify_subst(m_subset.group1,
                               m_subset.group2,
                               a_stab,
                               NULL,
                               a_list);
  }

  return VLAD_INVALIDINPUT;
}

/* check if fact is valid, any variables that occur must be in a_vlist */
int vlad_fact::verify(vlad_symtab *a_stab, vlad_varlist *a_vlist)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_stab == NULL)
    return VLAD_NULLPTR;

  switch(m_type) {
    case VLAD_ATOM_HOLDS :
      return vlad_verify_holds(m_holds.subject,
                               m_holds.access,
                               m_holds.object,
                               a_stab,
                               a_vlist,
                               NULL);
    case VLAD_ATOM_MEMBER :
      return vlad_verify_memb(m_member.element,
                              m_member.group,
                              a_stab,
                              a_vlist,
                              NULL);

    case VLAD_ATOM_SUBSET :
      return vlad_verify_subst(m_subset.group1,
                               m_subset.group2,
                               a_stab,
                               a_vlist,
                               NULL);
  }

  return VLAD_INVALIDINPUT;
}

/* reverses the truth value */
void vlad_fact::negate()
{
  m_truth = m_truth ? false : true;
}

/* gives the type of the fact */
int vlad_fact::type(unsigned char *a_type)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_type == NULL)
    return VLAD_NULLPTR;

  *a_type = m_type;

  return VLAD_OK;
}

/* gives the truth value of the fact */
int vlad_fact::truth(bool *a_truth)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_truth == NULL)
    return VLAD_NULLPTR;

  *a_truth = m_truth;

  return VLAD_OK;
}

/* returns VLAD_OK if none of the entities are variables */
int vlad_fact::is_ground()
{
  int retval;

  if (!m_init)
    return VLAD_UNINITIALISED;

  switch(m_type) {
    case VLAD_ATOM_HOLDS :
      if ((retval = vlad_identifier::validate_ent_ident(m_holds.subject)) != VLAD_OK)
        return retval;
      if ((retval = vlad_identifier::validate_ent_ident(m_holds.access)) != VLAD_OK)
        return retval;
      if ((retval = vlad_identifier::validate_ent_ident(m_holds.object)) != VLAD_OK)
        return retval;
      return VLAD_OK;
    case VLAD_ATOM_MEMBER :
      if ((retval = vlad_identifier::validate_ent_ident(m_member.element)) != VLAD_OK)
        return retval;
      if ((retval = vlad_identifier::validate_ent_ident(m_member.group)) != VLAD_OK)
        return retval;
      return VLAD_OK;
    case VLAD_ATOM_SUBSET :
      if ((retval = vlad_identifier::validate_ent_ident(m_subset.group1)) != VLAD_OK)
        return retval;
      if ((retval = vlad_identifier::validate_ent_ident(m_subset.group2)) != VLAD_OK)
        return retval;
      return VLAD_OK;
  }

  return VLAD_INVALIDINPUT;
}

#ifdef VLAD_DEBUG
/* assuming s has enough memory allocation */
void vlad_fact::print(char *a_str)
{
  if (a_str) {
    switch(m_type) {
      case VLAD_ATOM_HOLDS :
        sprintf(a_str,
                "%s%s(%s,%s,%s)",
                m_truth ? "" : "!",
                VLAD_STR_HOLDS,
                m_holds.subject,
                m_holds.access,
                m_holds.object);
        break;
      case VLAD_ATOM_MEMBER :
        sprintf(a_str,
                "%s%s(%s,%s)",
                m_truth ? "" : "!",
                VLAD_STR_MEMBER,
                m_member.element,
                m_member.group);
        break;
      case VLAD_ATOM_SUBSET :
        sprintf(a_str,
                "%s%s(%s,%s)",
                m_truth ? "" : "!",
                VLAD_STR_SUBSET,
                m_subset.group1,
                m_subset.group2);
        break;
    }
  }
}
#endif

int vlad_fact::reset()
{
  if (m_init) {
    switch(m_type) {
      case VLAD_ATOM_HOLDS :
        if (m_holds.subject != NULL)
          VLAD_MEM_FREE(m_holds.subject);
        if (m_holds.access != NULL)
          VLAD_MEM_FREE(m_holds.access);
        if (m_holds.object != NULL)
          VLAD_MEM_FREE(m_holds.object);
        break;
      case VLAD_ATOM_MEMBER :
        if (m_member.element != NULL)
          VLAD_MEM_FREE(m_member.element);
        if (m_member.group != NULL)
          VLAD_MEM_FREE(m_member.group);
        break;
      case VLAD_ATOM_SUBSET :
        if (m_subset.group1 != NULL)
          VLAD_MEM_FREE(m_subset.group1);
        if (m_subset.group2 != NULL)
          VLAD_MEM_FREE(m_subset.group2);
        break;
    }
  }

  return VLAD_OK;
}

/* return VLAD_OK if the given holds atom is valid */
static int vlad_verify_holds(const char *a_sub,
                             const char *a_acc,
                             const char *a_obj,
                             vlad_symtab *a_stab,
                             vlad_varlist *a_vlist1,
                             vlad_varlist *a_vlist2)
{
  int retval;
  unsigned char type;

  /* check subject */
  if (vlad_identifier::validate_ent_ident(a_sub) == VLAD_OK) {
    /* an entity, so we get the type from the symtab and make sure it's sub */
    if ((retval = a_stab->type(a_sub, &type)) != VLAD_OK)
      return retval;
    if (!VLAD_IDENT_TYPE_IS_SUB(type))
      return VLAD_INVALIDINPUT;
  }
  else if (vlad_identifier::validate_var_ident(a_sub) == VLAD_OK) {
    /* a variable, so we check it's type */
    if (!VLAD_IDENT_TYPE_IS_SUB(vlad_identifier::get_var_type(a_sub)))
      return VLAD_INVALIDINPUT;
    /* all good, so we check if it's in the varlist */
    if (a_vlist1 != NULL && (retval = a_vlist1->find(a_sub)) != VLAD_OK)
      return VLAD_INVALIDINPUT;
    /* finally, we add it in vlist2 */
    if (a_vlist2 != NULL) {
      retval = a_vlist2->add(a_sub);
      if (retval != VLAD_OK && retval != VLAD_DUPLICATE)
        return retval;
    }
  }
  else
    return VLAD_INVALIDINPUT;

  /* check access */
  if (vlad_identifier::validate_ent_ident(a_acc) == VLAD_OK) {
    /* an entity, so we get the type from the symtab and make sure it's acc */
    if ((retval = a_stab->type(a_acc, &type)) != VLAD_OK)
      return retval;
    if (!VLAD_IDENT_TYPE_IS_ACC(type))
      return VLAD_INVALIDINPUT;
  }
  else if (vlad_identifier::validate_var_ident(a_acc) == VLAD_OK) {
    /* a variable, so we check it's type */
    if (!VLAD_IDENT_TYPE_IS_ACC(vlad_identifier::get_var_type(a_acc)))
      return VLAD_INVALIDINPUT;
    /* all good, so we check if it's in the varlist */
    if (a_vlist1 != NULL && (retval = a_vlist1->find(a_acc)) != VLAD_OK)
      return VLAD_INVALIDINPUT;
    /* finally, we add it in vlist2 */
    if (a_vlist2 != NULL) {
      retval = a_vlist2->add(a_acc);
      if (retval != VLAD_OK && retval != VLAD_DUPLICATE)
        return retval;
    }
  }
  else
    return VLAD_INVALIDINPUT;

  /* check object */
  if (vlad_identifier::validate_ent_ident(a_obj) == VLAD_OK) {
    /* an entity, so we get the type from the symtab and make sure it's obj */
    if ((retval = a_stab->type(a_obj, &type)) != VLAD_OK)
      return retval;
    if (!VLAD_IDENT_TYPE_IS_OBJ(type))
      return VLAD_INVALIDINPUT;
  }
  else if (vlad_identifier::validate_var_ident(a_obj) == VLAD_OK) {
    /* a variable, so we check it's type */
    if (!VLAD_IDENT_TYPE_IS_OBJ(vlad_identifier::get_var_type(a_obj)))
      return VLAD_INVALIDINPUT;
    /* all good, so we check if it's in the varlist */
    if (a_vlist1 != NULL && (retval = a_vlist1->find(a_obj)) != VLAD_OK)
      return VLAD_INVALIDINPUT;
    /* finally, we add it in vlist2 */
    if (a_vlist2 != NULL) {
      retval = a_vlist2->add(a_obj);
      if (retval != VLAD_OK && retval != VLAD_DUPLICATE)
        return retval;
    }
  }
  else
    return VLAD_INVALIDINPUT;

  return VLAD_OK;
}

/* return VLAD_OK if the given memb atom is valid */
static int vlad_verify_memb(const char *a_elt,
                            const char *a_grp,
                            vlad_symtab *a_stab,
                            vlad_varlist *a_vlist1,
                            vlad_varlist *a_vlist2)
{
  int retval;
  unsigned char type[2];

  /* check element */
  if (vlad_identifier::validate_ent_ident(a_elt) == VLAD_OK) {
    /* it's an entity, so check the type */
    if ((retval = a_stab->type(a_elt, &(type[0]))) != VLAD_OK)
      return retval;
    /* make sure it's a single */
    if (!VLAD_IDENT_TYPE_IS_SIN(type[0]))
      return VLAD_INVALIDINPUT;
  }
  else if (vlad_identifier::validate_var_ident(a_elt) == VLAD_OK) {
    /* it's a variable, so get the type */
    type[0] = vlad_identifier::get_var_type(a_elt);
    /* check for both types as they are not mutually exclusive */
    if (!VLAD_IDENT_TYPE_IS_SIN(type[0]) || VLAD_IDENT_TYPE_IS_GRP(type[0]))
      return VLAD_INVALIDINPUT;
    /* all good, now check if it's in the varlist */
    if (a_vlist1 != NULL && (retval = a_vlist1->find(a_elt)) != VLAD_OK)
      return VLAD_INVALIDINPUT;
  }
  else
    return VLAD_INVALIDINPUT;

  /* check group */
  if (vlad_identifier::validate_ent_ident(a_grp) == VLAD_OK) {
    /* it's an entity, so get the type */
    if ((retval = a_stab->type(a_grp, &(type[1]))) != VLAD_OK)
      return retval;
    /* make sure it's a group */
    if (!VLAD_IDENT_TYPE_IS_GRP(type[1]))
      return VLAD_INVALIDINPUT;
  }
  else if (vlad_identifier::validate_var_ident(a_grp) == VLAD_OK) {
    /* it's a variable, so get the type */
    type[1] = vlad_identifier::get_var_type(a_grp);
    /* check for both types as they are not mutually exclusive */
    if (VLAD_IDENT_TYPE_IS_SIN(type[1]) || !VLAD_IDENT_TYPE_IS_GRP(type[1]))
      return VLAD_INVALIDINPUT;
    /* all good, now check if it's in the varlist */
    if (a_vlist1 != NULL && (retval = a_vlist1->find(a_grp)) != VLAD_OK)
      return VLAD_INVALIDINPUT;
  }
  else
    return VLAD_INVALIDINPUT;

  /* check types */
  if (VLAD_IDENT_TYPE_BASE(type[0]) != VLAD_IDENT_TYPE_BASE(type[1]))
    return VLAD_INVALIDINPUT;

  /* add to vlist2 */
  if (a_vlist2 != NULL) {
    if (VLAD_IDENT_TYPE_IS_VAR(type[0])) {
      retval = a_vlist2->add(a_elt);
      if (retval != VLAD_OK && retval != VLAD_DUPLICATE)
        return retval;
    }
    if (VLAD_IDENT_TYPE_IS_VAR(type[1])) {
      retval = a_vlist2->add(a_grp);
      if (retval != VLAD_OK && retval != VLAD_DUPLICATE)
        return retval;
    }
  }

  return VLAD_OK;
}

/* return VLAD_OK if the given subst atom is valid */
static int vlad_verify_subst(const char *a_grp1,
                             const char *a_grp2,
                             vlad_symtab *a_stab,
                             vlad_varlist *a_vlist1,
                             vlad_varlist *a_vlist2)
{
  int retval;
  unsigned char type[2];

  /* check group1 */
  if (vlad_identifier::validate_ent_ident(a_grp1) == VLAD_OK) {
    /* it's an entity, so check the type */
    if ((retval = a_stab->type(a_grp1, &(type[0]))) != VLAD_OK)
      return retval;
    /* make sure it's a group */
    if (!VLAD_IDENT_TYPE_IS_GRP(type[0]))
      return VLAD_INVALIDINPUT;
  }
  else if (vlad_identifier::validate_var_ident(a_grp1) == VLAD_OK) {
    /* it's a variable, so get the type */
    type[0] = vlad_identifier::get_var_type(a_grp1);
    /* check for both types as they are not mutually exclusive */
    if (VLAD_IDENT_TYPE_IS_SIN(type[0]) || !VLAD_IDENT_TYPE_IS_GRP(type[0]))
      return VLAD_INVALIDINPUT;
    /* all good, now check if it's in the varlist */
    if (a_vlist1 != NULL && (retval = a_vlist1->find(a_grp1)) != VLAD_OK)
      return VLAD_INVALIDINPUT;
  }
  else
    return VLAD_INVALIDINPUT;

  /* check group */
  if (vlad_identifier::validate_ent_ident(a_grp2) == VLAD_OK) {
    /* it's an entity, so get the type */
    if ((retval = a_stab->type(a_grp2, &(type[1]))) != VLAD_OK)
      return retval;
    /* make sure it's a group */
    if (!VLAD_IDENT_TYPE_IS_GRP(type[1]))
      return VLAD_INVALIDINPUT;
  }
  else if (vlad_identifier::validate_var_ident(a_grp2) == VLAD_OK) {
    /* it's a variable, so get the type */
    type[1] = vlad_identifier::get_var_type(a_grp2);
    /* check for both types as they are not mutually exclusive */
    if (VLAD_IDENT_TYPE_IS_SIN(type[1]) || !VLAD_IDENT_TYPE_IS_GRP(type[1]))
      return VLAD_INVALIDINPUT;
    /* all good, now check if it's in the varlist */
    if (a_vlist1 != NULL && (retval = a_vlist1->find(a_grp2)) != VLAD_OK)
      return VLAD_INVALIDINPUT;
  }
  else
    return VLAD_INVALIDINPUT;

  /* check types */
  if (VLAD_IDENT_TYPE_BASE(type[0]) != VLAD_IDENT_TYPE_BASE(type[1]))
    return VLAD_INVALIDINPUT;

  /* add to vlist2 */
  if (a_vlist2 != NULL) {
    if (VLAD_IDENT_TYPE_IS_VAR(type[0])) {
      retval = a_vlist2->add(a_grp1);
      if (retval != VLAD_OK && retval != VLAD_DUPLICATE)
        return retval;
    }
    if (VLAD_IDENT_TYPE_IS_VAR(type[1])) {
      retval = a_vlist2->add(a_grp2);
      if (retval != VLAD_OK && retval != VLAD_DUPLICATE)
        return retval;
    }
  }

  return VLAD_OK;
}
