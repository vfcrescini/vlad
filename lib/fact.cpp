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
#include <new>

#include <vlad/vlad.h>
#include <vlad/identifier.h>
#include <vlad/fact.h>

/* some extra functions */
static int verify_holds(const char *a_sub,
                        const char *a_acc,
                        const char *a_obj,
                        symtab *a_stab,
                        stringlist *a_vlist,
                        bool a_gndflag);
static int verify_memb(const char *a_elt,
                       const char *a_grp,
                       symtab *a_stab,
                       stringlist *a_vlist,
                       bool a_gndflag);
static int verify_subst(const char *a_grp1,
                        const char *a_grp2,
                        symtab *a_stab,
                        stringlist *a_vlist,
                        bool a_gndflag);

fact::fact()
{
  /* choose holds as a default type */
  m_type = VLAD_ATOM_HOLDS;
  m_holds.subject = NULL;
  m_holds.access = NULL;
  m_holds.object = NULL;
  m_truth = true;
  m_init = false;
}

fact::~fact()
{
  reset();
}

bool fact::cmp(list_item *a_item)
{
  fact *tmp = NULL;

  if (a_item == NULL)
    return false;

  if ((tmp = dynamic_cast<fact *>(a_item)) == NULL)
    return false;

  /* if both are uninitialised, return true. if only one, return false */
  if (!m_init)
    return !tmp->m_init;
  if (!tmp->m_init)
    return false;

  /* make sure the types are equal */
  if (m_type != tmp->m_type)
    return false;

  switch(m_type) {
    case VLAD_ATOM_HOLDS :
      return
        (m_truth == tmp->m_truth) &&
        !strcmp(m_holds.subject, tmp->m_holds.subject) &&
        !strcmp(m_holds.access, tmp->m_holds.access) &&
        !strcmp(m_holds.object, tmp->m_holds.object);
    case VLAD_ATOM_MEMBER :
      return
        (m_truth == tmp->m_truth) &&
        !strcmp(m_member.element, tmp->m_member.element) &&
        !strcmp(m_member.group, tmp->m_member.group);
    case VLAD_ATOM_SUBSET :
      return
        (m_truth == tmp->m_truth) &&
        !strcmp(m_subset.group1, tmp->m_subset.group1) &&
        !strcmp(m_subset.group2, tmp->m_subset.group2);
  }

  return false;
}

int fact::get(char **a_elt1,
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

int fact::get_holds(char **a_sub, char **a_acc, char **a_obj)
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

int fact::get_member(char **a_elt, char **a_grp)
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

int fact::get_subset(char **a_grp1, char **a_grp2)
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

int fact::init(const char *a_elt1,
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

int fact::init_holds(const char *a_sub,
                     const char *a_acc,
                     const char *a_obj,
                     bool a_truth)
{
  if (a_sub == NULL || a_acc == NULL || a_obj == NULL)
    return VLAD_NULLPTR;

  reset();

  m_type = VLAD_ATOM_HOLDS;
  m_truth = a_truth;

  if ((m_holds.subject = VLAD_STRING_MALLOC(a_sub)) == NULL)
    return VLAD_MALLOCFAILED;
  if ((m_holds.access = VLAD_STRING_MALLOC(a_acc)) == NULL)
    return VLAD_MALLOCFAILED;
  if ((m_holds.object = VLAD_STRING_MALLOC(a_obj)) == NULL)
    return VLAD_MALLOCFAILED;

  /* strcpy never fails */
  strcpy(m_holds.subject, a_sub);
  strcpy(m_holds.access, a_acc);
  strcpy(m_holds.object, a_obj);

  m_init = true;

  return VLAD_OK;
}

int fact::init_member(const char *a_elt,
                      const char *a_grp,
                      bool a_truth)
{
  if (a_elt == NULL || a_grp == NULL)
    return VLAD_NULLPTR;

  reset();

  m_type = VLAD_ATOM_MEMBER;
  m_truth = a_truth;

  if ((m_member.element = VLAD_STRING_MALLOC(a_elt)) == NULL)
    return VLAD_MALLOCFAILED;
  if ((m_member.group = VLAD_STRING_MALLOC(a_grp)) == NULL)
    return VLAD_MALLOCFAILED;

  /* strcpy never fails */
  strcpy(m_member.element, a_elt);
  strcpy(m_member.group, a_grp);

  m_init = true;

  return VLAD_OK;
}

int fact::init_subset(const char *a_grp1, const char *a_grp2, bool a_truth)
{
  if (a_grp1 == NULL || a_grp2 == NULL)
    return VLAD_NULLPTR;

  reset();

  m_type = VLAD_ATOM_SUBSET;
  m_truth = a_truth;

  if ((m_subset.group1 = VLAD_STRING_MALLOC(a_grp1)) == NULL)
    return VLAD_MALLOCFAILED;
  if ((m_subset.group2 = VLAD_STRING_MALLOC(a_grp2)) == NULL)
    return VLAD_MALLOCFAILED;

  /* strcpy never fails */
  strcpy(m_subset.group1, a_grp1);
  strcpy(m_subset.group2, a_grp2);

  m_init = true;

  return VLAD_OK;
}

/* create a new instance of this fact */
int fact::copy(fact **a_fact)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_fact == NULL)
    return VLAD_NULLPTR;

  if ((*a_fact = VLAD_NEW(fact())) == NULL)
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

/* replaces instances of var with ident, gives a new fact */
int fact::replace(const char *a_var, const char *a_ident, fact **a_fact)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_var == NULL || a_ident == NULL || a_fact == NULL)
    return VLAD_NULLPTR;

  /* create a new fact */
  if ((*a_fact = VLAD_NEW(fact())) == NULL)
    return VLAD_MALLOCFAILED;

  /*
   * at this level we do not care if the entities are valid or not. we blindy
   * replace var with ident. nothing more, nothing less.
   */

  switch(m_type) {
    case VLAD_ATOM_HOLDS :
      return (*a_fact)->init_holds((strcmp(m_holds.subject, a_var) ? m_holds.subject : a_ident),
                                   (strcmp(m_holds.access, a_var) ? m_holds.access : a_ident),
                                   (strcmp(m_holds.object, a_var) ? m_holds.object : a_ident),
                                   m_truth);
    case VLAD_ATOM_MEMBER :
      return (*a_fact)->init_member((strcmp(m_member.element, a_var) ? m_member.element : a_ident),
                                    (strcmp(m_member.group, a_var) ? m_member.group : a_ident),
                                    m_truth);
    case VLAD_ATOM_SUBSET :
      return (*a_fact)->init_subset((strcmp(m_subset.group1, a_var) ? m_subset.group1 : a_ident),
                                    (strcmp(m_subset.group2, a_var) ? m_subset.group2 : a_ident),
                                    m_truth);
  }

  return VLAD_FAILURE;
}

/* replaces vars in a_vlist with idents in a_ilist. gives a new fact */
int fact::replace(stringlist *a_vlist, stringlist *a_ilist, fact **a_fact)
{
  int retval;
  unsigned int index;
  char *tmp[3];

  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_fact == NULL)
    return VLAD_NULLPTR;

  /* if both lists are null or if the lengths are zero, return the fact */
  if ((a_vlist == NULL && a_ilist == NULL) ||
      VLAD_LIST_LENGTH(a_vlist) == VLAD_LIST_LENGTH(a_ilist) == 0)
    return copy(a_fact);

  /* make sure lists are non-NULL and their lengths are equal */
  if (a_vlist == NULL || a_ilist == NULL || a_vlist->length() != a_ilist->length())
    return VLAD_INVALIDINPUT;

  /* all clear */
  if ((*a_fact = VLAD_NEW(fact())) == NULL)
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
        tmp[0] = m_holds.subject;
      else if (retval == VLAD_OK)
        a_ilist->get(index, &(tmp[0]));
      else
        return retval;
      /* access */
      if ((retval = a_vlist->get(m_holds.access, &index)) == VLAD_NOTFOUND)
        tmp[1] = m_holds.access;
      else if (retval == VLAD_OK)
        a_ilist->get(index, &(tmp[1]));
      else
        return retval;
      /* object */
      if ((retval = a_vlist->get(m_holds.object, &index)) == VLAD_NOTFOUND)
        tmp[2] = m_holds.object;
      else if (retval == VLAD_OK)
        a_ilist->get(index, &(tmp[2]));
      else
        return retval;

      return (*a_fact)->init_holds(tmp[0], tmp[1], tmp[2], m_truth);
    case VLAD_ATOM_MEMBER :
      /* element */
      if ((retval = a_vlist->get(m_member.element, &index)) == VLAD_NOTFOUND)
        tmp[0] = m_member.element;
      else if (retval == VLAD_OK)
        a_ilist->get(index, &(tmp[0]));
      else
        return retval;
      /* group */
      if ((retval = a_vlist->get(m_member.group, &index)) == VLAD_NOTFOUND)
        tmp[1] = m_member.group;
      else if (retval == VLAD_OK)
        a_ilist->get(index, &(tmp[1]));
      else
        return retval;

      return (*a_fact)->init_member(tmp[0], tmp[1], m_truth);
    case VLAD_ATOM_SUBSET :
      /* group1 */
      if ((retval = a_vlist->get(m_subset.group1, &index)) == VLAD_NOTFOUND)
        tmp[0] = m_subset.group1;
      else if (retval == VLAD_OK)
        a_ilist->get(index, &(tmp[0]));
      else
        return retval;
      /* group2 */
      if ((retval = a_vlist->get(m_subset.group2, &index)) == VLAD_NOTFOUND)
        tmp[1] = m_subset.group2;
      else if (retval == VLAD_OK)
        a_ilist->get(index, &(tmp[1]));
      else
        return retval;

      return (*a_fact)->init_subset(tmp[0], tmp[1], m_truth);
  }

  return VLAD_FAILURE;
}

/* reverses the truth value */
void fact::negate()
{
  m_truth = m_truth ? false : true;
}

/* gives the type of the fact */
int fact::type(unsigned char *a_type)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_type == NULL)
    return VLAD_NULLPTR;

  *a_type = m_type;

  return VLAD_OK;
}

/* gives the truth value of the fact */
int fact::truth(bool *a_truth)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_truth == NULL)
    return VLAD_NULLPTR;

  *a_truth = m_truth;

  return VLAD_OK;
}

/* gives a list of vars occuring in the fact. assumes list is init'ed */
int fact::varlist(stringlist **a_list)
{
  int retval;

  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_list == NULL || *a_list == NULL)
    return VLAD_NULLPTR;

  switch(m_type) {
    case VLAD_ATOM_HOLDS :
      if (VLAD_IDENT_IS_VAR(m_holds.subject))
        if ((retval = (*a_list)->add(m_holds.subject)) != VLAD_OK)
          if (retval != VLAD_DUPLICATE)
            return retval;
      if (VLAD_IDENT_IS_VAR(m_holds.access))
        if ((retval = (*a_list)->add(m_holds.access)) != VLAD_OK)
          if (retval != VLAD_DUPLICATE)
            return retval;
      if (VLAD_IDENT_IS_VAR(m_holds.object))
        if ((retval = (*a_list)->add(m_holds.object)) != VLAD_OK)
          if (retval != VLAD_DUPLICATE)
            return retval;

      return VLAD_OK;
    case VLAD_ATOM_MEMBER :
      if (VLAD_IDENT_IS_VAR(m_member.element))
        if ((retval = (*a_list)->add(m_member.element)) != VLAD_OK)
          if (retval != VLAD_DUPLICATE)
            return retval;
      if (VLAD_IDENT_IS_VAR(m_member.group))
        if ((retval = (*a_list)->add(m_member.group)) != VLAD_OK)
          if (retval != VLAD_DUPLICATE)
            return retval;

      return VLAD_OK;
    case VLAD_ATOM_SUBSET :
      if (VLAD_IDENT_IS_VAR(m_subset.group1))
        if ((retval = (*a_list)->add(m_subset.group1)) != VLAD_OK)
          if (retval != VLAD_DUPLICATE)
            return retval;
      if (VLAD_IDENT_IS_VAR(m_subset.group2))
        if ((retval = (*a_list)->add(m_subset.group2)) != VLAD_OK)
          if (retval != VLAD_DUPLICATE)
            return retval;

      return VLAD_OK;
  }

  return VLAD_FAILURE;
}

/*
 * verify if fact is valid, if vlist is non-null, check if variables
 * occur within this list. if gnd_flag is true, ensure that the fact
 * is ground.
 */
int fact::verify(symtab *a_stab, stringlist *a_vlist, bool a_gndflag)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_stab == NULL)
    return VLAD_NULLPTR;

  switch(m_type) {
    case VLAD_ATOM_HOLDS :
      return verify_holds(m_holds.subject,
                          m_holds.access,
                          m_holds.object,
                          a_stab,
                          a_vlist,
                          a_gndflag);
    case VLAD_ATOM_MEMBER :
      return verify_memb(m_member.element,
                         m_member.group,
                         a_stab,
                         a_vlist,
                         a_gndflag);

    case VLAD_ATOM_SUBSET :
      return verify_subst(m_subset.group1,
                          m_subset.group2,
                          a_stab,
                          a_vlist,
                          a_gndflag);
  }

  return VLAD_INVALIDINPUT;
}

#ifdef VLAD_DEBUG
/* assuming s has enough memory allocation */
void fact::print(char *a_str)
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

int fact::reset()
{
  if (m_init) {
    switch(m_type) {
      case VLAD_ATOM_HOLDS :
        if (m_holds.subject != NULL)
          free(m_holds.subject);
        if (m_holds.access != NULL)
          free(m_holds.access);
        if (m_holds.object != NULL)
          free(m_holds.object);
        break;
      case VLAD_ATOM_MEMBER :
        if (m_member.element != NULL)
          free(m_member.element);
        if (m_member.group != NULL)
          free(m_member.group);
        break;
      case VLAD_ATOM_SUBSET :
        if (m_subset.group1 != NULL)
          free(m_subset.group1);
        if (m_subset.group2 != NULL)
          free(m_subset.group2);
        break;
    }
  }

  return VLAD_OK;
}

/* some extra functions */
static int verify_holds(const char *a_sub,
                        const char *a_acc,
                        const char *a_obj,
                        symtab *a_stab,
                        stringlist *a_vlist,
                        bool a_gndflag)
{
  int retval;
  unsigned char type;

  /* check subject */
  if (VLAD_IDENT_IS_IDENT(a_sub)) {
    if ((retval = a_stab->type(a_sub, &type)) != VLAD_OK)
      return retval;
    if (type != VLAD_IDENT_SUB_SIN && type != VLAD_IDENT_SUB_GRP)
      return VLAD_INVALIDINPUT;
  }
  else if (VLAD_IDENT_IS_VAR(a_sub)) {
    if (a_gndflag)
      return VLAD_INVALIDINPUT;
    if (a_vlist != NULL)
      if ((retval = a_vlist->find(a_sub)) != VLAD_OK)
        return VLAD_INVALIDINPUT;
  }
  else
    return VLAD_INVALIDINPUT;

  /* check access */
  if (VLAD_IDENT_IS_IDENT(a_acc)) {
    if ((retval = a_stab->type(a_acc, &type)) != VLAD_OK)
      return retval;
    if (type != VLAD_IDENT_ACC_SIN && type != VLAD_IDENT_ACC_GRP)
      return VLAD_INVALIDINPUT;
  }
  else if (VLAD_IDENT_IS_VAR(a_acc)) {
    if (a_gndflag)
      return VLAD_INVALIDINPUT;
    if (a_vlist != NULL)
      if ((retval = a_vlist->find(a_acc)) != VLAD_OK)
        return VLAD_INVALIDINPUT;
  }
  else
    return VLAD_INVALIDINPUT;

  /* check object */
  if (VLAD_IDENT_IS_IDENT(a_obj)) {
    if ((retval = a_stab->type(a_obj, &type)) != VLAD_OK)
      return retval;
    if (type != VLAD_IDENT_OBJ_SIN && type != VLAD_IDENT_OBJ_GRP)
      return VLAD_INVALIDINPUT;
  }
  else if (VLAD_IDENT_IS_VAR(a_obj)) {
    if (a_gndflag)
      return VLAD_INVALIDINPUT;
    if (a_vlist != NULL)
      if ((retval = a_vlist->find(a_obj)) != VLAD_OK)
        return VLAD_INVALIDINPUT;
  }
  else
    return VLAD_INVALIDINPUT;

  return VLAD_OK;
}

static int verify_memb(const char *a_elt,
                       const char *a_grp,
                       symtab *a_stab,
                       stringlist *a_vlist,
                       bool a_gndflag)
{
  int retval;
  unsigned char type[2];

  /* check element */
  if (VLAD_IDENT_IS_IDENT(a_elt)) {
    if ((retval = a_stab->type(a_elt, &(type[0]))) != VLAD_OK)
      return retval;
    if (VLAD_IDENT_IS_GROUP(type[0]))
      return VLAD_INVALIDINPUT;
  }
  else if (VLAD_IDENT_IS_VAR(a_elt)) {
    if (a_gndflag)
      return VLAD_INVALIDINPUT;
    if (a_vlist != NULL)
      if ((retval = a_vlist->find(a_elt)) != VLAD_OK)
         return VLAD_INVALIDINPUT;
  }
  else
    return VLAD_INVALIDINPUT;

  /* check group */
  if (VLAD_IDENT_IS_IDENT(a_grp)) {
    if ((retval = a_stab->type(a_grp, &(type[1]))) != VLAD_OK)
      return retval;
    if (!VLAD_IDENT_IS_GROUP(type[1]))
      return VLAD_INVALIDINPUT;
  }
  else if (VLAD_IDENT_IS_VAR(a_grp)) {
    if (a_gndflag)
      return VLAD_INVALIDINPUT;
    if (a_vlist != NULL)
      if ((retval = a_vlist->find(a_grp)) != VLAD_OK)
        return VLAD_INVALIDINPUT;
  }
  else
    return VLAD_INVALIDINPUT;

  /* check types */
  if (type[0] != VLAD_IDENT_BASETYPE(type[1]))
    return VLAD_INVALIDINPUT;

  return VLAD_OK;
}

static int verify_subst(const char *a_grp1,
                        const char *a_grp2,
                        symtab *a_stab,
                        stringlist *a_vlist,
                        bool a_gndflag)
{
  int retval;
  unsigned char type[2];

  /* check group1 */
  if (VLAD_IDENT_IS_IDENT(a_grp1)) {
    if ((retval = a_stab->type(a_grp2, &(type[0]))) != VLAD_OK)
      return retval;
    if (!VLAD_IDENT_IS_GROUP(type[0]))
      return VLAD_INVALIDINPUT;
  }
  else if (VLAD_IDENT_IS_VAR(a_grp1)) {
    if (a_vlist != NULL)
      if ((retval = a_vlist->find(a_grp1)) != VLAD_OK)
         return VLAD_INVALIDINPUT;
  }
  else
    return VLAD_INVALIDINPUT;

  /* check group2 */
  if (VLAD_IDENT_IS_IDENT(a_grp2)) {
    if ((retval = a_stab->type(a_grp2, &(type[1]))) != VLAD_OK)
      return retval;
    if (!VLAD_IDENT_IS_GROUP(type[1]))
      return VLAD_INVALIDINPUT;
  }
  else if (VLAD_IDENT_IS_VAR(a_grp2)) {
    if (a_vlist != NULL)
      if ((retval = a_vlist->find(a_grp2)) != VLAD_OK)
        return VLAD_INVALIDINPUT;
  }
  else
    return VLAD_INVALIDINPUT;

  /* check types */
  if (type[0] != type[1])
    return VLAD_INVALIDINPUT;

  return VLAD_OK;
}
