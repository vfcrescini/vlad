/*
 * fact.cpp
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <new>

#include <vlad/vlad.h>
#include <vlad/fact.h>

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

/* replaces vars in a_vlist with idents in a_ilist. gives a new fact */
int fact::replace(stringlist *a_vlist, stringlist *a_ilist, fact **a_fact)
{
  int retval;
  unsigned int index;

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
    case VLAD_ATOM_HOLDS : {
      char *tmp_s;
      char *tmp_a;
      char *tmp_o;

      /* subject */
      if ((retval = a_vlist->get(m_holds.subject, &index)) == VLAD_NOTFOUND)
        tmp_s = m_holds.subject;
      else if (retval == VLAD_OK)
        a_ilist->get(index, &tmp_s);
      else
        return retval;
      /* access */
      if ((retval = a_vlist->get(m_holds.access, &index)) == VLAD_NOTFOUND)
        tmp_a = m_holds.access;
      else if (retval == VLAD_OK)
        a_ilist->get(index, &tmp_a);
      else
        return retval;
      /* object */
      if ((retval = a_vlist->get(m_holds.object, &index)) == VLAD_NOTFOUND)
        tmp_o = m_holds.object;
      else if (retval == VLAD_OK)
        a_ilist->get(index, &tmp_o);
      else
        return retval;

      return (*a_fact)->init_holds(tmp_s, tmp_a, tmp_o, m_truth);
    }
    case VLAD_ATOM_MEMBER : {
      char *tmp_e;
      char *tmp_g;

      /* element */
      if ((retval = a_vlist->get(m_member.element, &index)) == VLAD_NOTFOUND)
        tmp_e = m_member.element;
      else if (retval == VLAD_OK)
        a_ilist->get(index, &tmp_e);
      else
        return retval;
      /* group */
      if ((retval = a_vlist->get(m_member.group, &index)) == VLAD_NOTFOUND)
        tmp_g = m_member.group;
      else if (retval == VLAD_OK)
        a_ilist->get(index, &tmp_g);
      else
        return retval;

      return (*a_fact)->init_member(tmp_e, tmp_g, m_truth);
    }
    case VLAD_ATOM_SUBSET : {
      char *tmp_g1;
      char *tmp_g2;

      /* group1 */
      if ((retval = a_vlist->get(m_subset.group1, &index)) == VLAD_NOTFOUND)
        tmp_g1 = m_subset.group1;
      else if (retval == VLAD_OK)
        a_ilist->get(index, &tmp_g1);
      else
        return retval;
      /* group2 */
      if ((retval = a_vlist->get(m_subset.group2, &index)) == VLAD_NOTFOUND)
        tmp_g2 = m_subset.group2;
      else if (retval == VLAD_OK)
        a_ilist->get(index, &tmp_g2);
      else
        return retval;

      return (*a_fact)->init_subset(tmp_g1, tmp_g2, m_truth);
    }
  }

  return VLAD_INVALIDINPUT;
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
