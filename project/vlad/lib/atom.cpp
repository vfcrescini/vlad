/*
 * atom.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <new>

#include <config.h>
#include <vlad/vlad.h>
#include <vlad/atom.h>

atom::atom()
{
  type = VLAD_ATOM_HOLDS;
  holds.subject = NULL;
  holds.access = NULL;
  holds.object = NULL;
  truth = true;
  initialised = false;
}

atom::~atom()
{
  reset();
}

bool atom::cmp(list_item *item)
{
  atom *tmp = NULL;

  if (item == NULL)
    return false;

  if ((tmp = dynamic_cast<atom *> (item)) == NULL)
    return false;

  /* if both are uninitialised, return true. if only one, return false */
  if (!initialised)
    return !tmp->initialised;

  if (!tmp->initialised)
    return false;

  /* make sure the types are equal */
  if (type != tmp->type)
    return false;

  switch(type) {
    case VLAD_ATOM_HOLDS :
      return
        (truth == tmp->truth) &&
        !strcmp(holds.subject, tmp->holds.subject) &&
        !strcmp(holds.access, tmp->holds.access) &&
        !strcmp(holds.object, tmp->holds.object);
    case VLAD_ATOM_MEMBER :
      return
        (truth == tmp->truth) &&
        !strcmp(member.element, tmp->member.element) &&
        !strcmp(member.group, tmp->member.group);
    case VLAD_ATOM_SUBSET :
      return
        (truth == tmp->truth) &&
        !strcmp(subset.group1, tmp->subset.group1) &&
        !strcmp(subset.group2, tmp->subset.group2);
  }

  return false;
}

int atom::get(char **n1, char **n2, char **n3, unsigned char *ty, bool *tr)
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  if (ty == NULL || tr == NULL)
    return VLAD_NULLPTR;

  *ty = type;
  *tr = truth;

  switch(type) {
    case VLAD_ATOM_HOLDS :
      return get_holds(n1, n2, n3);
    case VLAD_ATOM_MEMBER :
      return get_member(n1, n2);
    case VLAD_ATOM_SUBSET :
      return get_subset(n1, n2);
  }

  /* type will never be anything but the 3 above so this will never happen */
  return VLAD_FAILURE;
}

int atom::get_holds(char **s, char **a, char **o)
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  if (s == NULL || a == NULL || o == NULL)
    return VLAD_NULLPTR;

  if (type != VLAD_ATOM_HOLDS)
    return VLAD_FAILURE;

  *s = holds.subject;
  *a = holds.access;
  *o = holds.object;

  return VLAD_OK;
}

int atom::get_member(char **e, char **g)
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  if (e == NULL || g == NULL)
    return VLAD_NULLPTR;

  if (type != VLAD_ATOM_MEMBER)
    return VLAD_FAILURE;

  *e = member.element;
  *g = member.group;

  return VLAD_OK;
}

int atom::get_subset(char **g1, char **g2)
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  if (g1 == NULL || g2 == NULL)
    return VLAD_NULLPTR;

  if (type != VLAD_ATOM_SUBSET)
    return VLAD_FAILURE;

  *g1 = subset.group1;
  *g2 = subset.group2;

  return VLAD_OK;
}

int atom::init(char *n1, char *n2, char *n3, unsigned char ty, bool tr)
{
  switch(ty) {
    case VLAD_ATOM_HOLDS :
      return init_holds(n1, n2, n3, tr);
    case VLAD_ATOM_MEMBER :
      return init_member(n1, n2, tr);
    case VLAD_ATOM_SUBSET :
      return init_subset(n1, n2, tr);
  }
  return VLAD_INVALIDINPUT;
}

int atom::init_holds(const char *s, const char *a, const char *o, bool t)
{
  if (s == NULL || a == NULL || o == NULL)
    return VLAD_NULLPTR;

  reset();

  type = VLAD_ATOM_HOLDS;
  truth = t;

  if ((holds.subject = VLAD_STRING_MALLOC(s)) == NULL)
    return VLAD_MALLOCFAILED;
  if ((holds.access = VLAD_STRING_MALLOC(a)) == NULL)
    return VLAD_MALLOCFAILED;
  if ((holds.object = VLAD_STRING_MALLOC(o)) == NULL)
    return VLAD_MALLOCFAILED;

  /* strcpy never fails */
  strcpy(holds.subject, s);
  strcpy(holds.access, a);
  strcpy(holds.object, o);

  initialised = true;

  return VLAD_OK;
}

int atom::init_member(const char *e, const char *g, bool t)
{
  if (e == NULL || g == NULL)
    return VLAD_NULLPTR;

  reset();

  type = VLAD_ATOM_MEMBER;
  truth = t;

  if ((member.element = VLAD_STRING_MALLOC(e)) == NULL)
    return VLAD_MALLOCFAILED;
  if ((member.group = VLAD_STRING_MALLOC(g)) == NULL)
    return VLAD_MALLOCFAILED;

  /* strcpy never fails */
  strcpy(member.element, e);
  strcpy(member.group, g);

  initialised = true;

  return VLAD_OK;
}

int atom::init_subset(const char *g1, const char *g2, bool t)
{
  if (g1 == NULL || g2 == NULL)
    return VLAD_NULLPTR;

  reset();

  type = VLAD_ATOM_SUBSET;
  truth = t;

  if ((subset.group1 = VLAD_STRING_MALLOC(g1)) == NULL)
    return VLAD_MALLOCFAILED;
  if ((subset.group2 = VLAD_STRING_MALLOC(g2)) == NULL)
    return VLAD_MALLOCFAILED;

  /* strcpy never fails */
  strcpy(subset.group1, g1);
  strcpy(subset.group2, g2);

  initialised = true;

  return VLAD_OK;
}

/* create a new instance of this atom */
int atom::copy(atom **a)
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  if (a == NULL)
    return VLAD_NULLPTR;

  if ((*a = VLAD_NEW(atom())) == NULL)
    return VLAD_MALLOCFAILED;

  switch(type) {
    case VLAD_ATOM_HOLDS :
      return (*a)->init_holds(holds.subject, holds.access, holds.object, truth);
    case VLAD_ATOM_MEMBER :
      return (*a)->init_member(member.element, member.group, truth);
    case VLAD_ATOM_SUBSET :
      return (*a)->init_subset(subset.group1, subset.group2, truth);
  }

  return VLAD_INVALIDINPUT;
}

/* replaces vars in vlist with idents in ilist. new atom a */
int atom::replace(stringlist *vlist, stringlist *ilist, atom **a)
{
  int retval;
  unsigned int index;

  if (!initialised)
    return VLAD_UNINITIALISED;

  if (a == NULL)
    return VLAD_NULLPTR;

  /* if both lists are null or if the lengths are zero, return the atom */
  if ((vlist == NULL && ilist == NULL) || VLAD_LIST_LENGTH(vlist) == VLAD_LIST_LENGTH(ilist) == 0)
    return copy(a);

  /* make sure lists are non-NULL and their lengths are equal */
  if (vlist == NULL || ilist == NULL || vlist->length() != ilist->length())
    return VLAD_INVALIDINPUT;

  /* all clear */
  if ((*a = VLAD_NEW(atom())) == NULL)
    return VLAD_MALLOCFAILED;

  /*
   * at this level we don't really care if the identifiers are valid or not.
   * we simply replace strings from the first list with strings from the
   * second list.
   */

  switch(type) {
    case VLAD_ATOM_HOLDS : {
      char *tmp_s;
      char *tmp_a;
      char *tmp_o;

      /* subject */
      if ((retval = vlist->get(holds.subject, &index)) == VLAD_NOTFOUND)
        tmp_s = holds.subject;
      else if (retval == VLAD_OK)
        ilist->get(index, &tmp_s);
      else
        return retval;
      /* access */
      if ((retval = vlist->get(holds.access, &index)) == VLAD_NOTFOUND)
        tmp_a = holds.access;
      else if (retval == VLAD_OK)
        ilist->get(index, &tmp_a);
      else
        return retval;
      /* object */
      if ((retval = vlist->get(holds.object, &index)) == VLAD_NOTFOUND)
        tmp_o = holds.object;
      else if (retval == VLAD_OK)
        ilist->get(index, &tmp_o);
      else
        return retval;

      return (*a)->init_holds(tmp_s, tmp_a, tmp_o, truth);
    }
    case VLAD_ATOM_MEMBER : {
      char *tmp_e;
      char *tmp_g;

      /* element */
      if ((retval = vlist->get(member.element, &index)) == VLAD_NOTFOUND)
        tmp_e = member.element;
      else if (retval == VLAD_OK)
        ilist->get(index, &tmp_e);
      else
        return retval;
      /* group */
      if ((retval = vlist->get(member.group, &index)) == VLAD_NOTFOUND)
        tmp_g = member.group;
      else if (retval == VLAD_OK)
        ilist->get(index, &tmp_g);
      else
        return retval;

      return (*a)->init_member(tmp_e, tmp_g, truth);
    }
    case VLAD_ATOM_SUBSET : {
      char *tmp_g1;
      char *tmp_g2;

      /* group1 */
      if ((retval = vlist->get(subset.group1, &index)) == VLAD_NOTFOUND)
        tmp_g1 = subset.group1;
      else if (retval == VLAD_OK)
        ilist->get(index, &tmp_g1);
      else
        return retval;
      /* group2 */
      if ((retval = vlist->get(subset.group2, &index)) == VLAD_NOTFOUND)
        tmp_g2 = subset.group2;
      else if (retval == VLAD_OK)
        ilist->get(index, &tmp_g2);
      else
        return retval;

      return (*a)->init_subset(tmp_g1, tmp_g2, truth);
    }
  }

  return VLAD_INVALIDINPUT;
}

void atom::negate()
{
  truth = truth ? false : true;
}

#ifdef DEBUG
/* assuming s has enough memory allocation */
void atom::print(char *s)
{
  if (initialised) {
    switch(type) {
      case VLAD_ATOM_HOLDS :
        sprintf(s,
                "%s%s(%s,%s,%s)",
                truth ? "" : "!",
                VLAD_STR_HOLDS,
                holds.subject,
                holds.access,
                holds.object);
        break;
      case VLAD_ATOM_MEMBER :
        sprintf(s,
                "%s%s(%s,%s)",
                truth ? "" : "!",
                VLAD_STR_MEMB,
                member.element,
                member.group);
        break;
      case VLAD_ATOM_SUBSET :
        sprintf(s,
                "%s%s(%s,%s)",
                truth ? "" : "!",
                VLAD_STR_SUBSET,
                subset.group1,
                subset.group2);
        break;
    }
  }
}
#endif

int atom::reset()
{
  if (initialised) {
    switch(type) {
      case VLAD_ATOM_HOLDS :
        if (holds.subject != NULL)
          free(holds.subject);
        if (holds.access != NULL)
          free(holds.access);
        if (holds.object != NULL)
          free(holds.object);
        break;
      case VLAD_ATOM_MEMBER :
        if (member.element != NULL)
          free(member.element);
        if (member.group != NULL)
          free(member.group);
        break;
      case VLAD_ATOM_SUBSET :
        if (subset.group1 != NULL)
          free(subset.group1);
        if (subset.group2 != NULL)
          free(subset.group2);
        break;
    }
  }

  return VLAD_OK;
}
