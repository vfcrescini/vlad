/*
 * atom.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <new>

#include <config.h>
#include <vlad.h>
#include <atom.h>

atom::atom()
{
  /* all atoms are initialised as the constant true */
  type = 0;
  truth = true;
  constant = true;
}

atom::~atom()
{
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

bool atom::cmp(list_item *item)
{
  atom *tmp = NULL;

  /* a NULL will not match anything */
  if (item == NULL)
    return false;

  if ((tmp = dynamic_cast<atom *> (item)) == NULL)
    return false;

  if (type != tmp->type)
    return false;

  switch(type) {
    case VLAD_ATOM_CONST :
      return 
        (truth == tmp->truth && constant == tmp->constant) ||
        (truth != tmp->truth && constant != tmp->constant);
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

unsigned char atom::get_type()
{
  return type;
}

bool atom::get_truth()
{
  return truth;
}

int atom::get_const(bool *c)
{
  if (c == NULL)
    return VLAD_NULLPTR;

  if (type != VLAD_ATOM_CONST)
    return VLAD_FAILURE;

  *c = constant;

  return VLAD_OK;
}

int atom::get_holds(char **s, char **a, char **o)
{
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
  if (g1 == NULL || g2 == NULL)
    return VLAD_NULLPTR;

  if (type != VLAD_ATOM_SUBSET)
    return VLAD_FAILURE;

  *g1 = subset.group1;
  *g2 = subset.group2;

  return VLAD_OK;
}

int atom::init_atom(atom *a)
{
  switch(a->type) {
    case VLAD_ATOM_CONST :
      return init_const(a->constant, a->truth);
    case VLAD_ATOM_HOLDS :
      return init_holds(a->holds.subject, a->holds.access, a->holds.object, a->truth);
    case VLAD_ATOM_MEMBER :
      return init_member(a->member.element, a->member.group, a->truth);
    case VLAD_ATOM_SUBSET :
      return init_subset(a->subset.group1, a->subset.group2, a->truth);
  }

  return VLAD_INVALIDINPUT;
}

int atom::init_const(bool c, bool t)
{
  type = VLAD_ATOM_CONST;
  truth = t;
  constant = c;

  return VLAD_OK;
}

int atom::init_holds(const char *s, const char *a, const char *o, bool t)
{
  if (s == NULL || a == NULL || o == NULL)
    return VLAD_NULLPTR;

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

  return VLAD_OK;
}

int atom::init_member(const char *e, const char *g, bool t)
{
  if (e == NULL || g == NULL)
    return VLAD_NULLPTR;

  type = VLAD_ATOM_MEMBER;
  truth = t;

  if ((member.element = VLAD_STRING_MALLOC(e)) == NULL)
    return VLAD_MALLOCFAILED;
  if ((member.group = VLAD_STRING_MALLOC(g)) == NULL)
    return VLAD_MALLOCFAILED;

  /* strcpy never fails */
  strcpy(member.element, e);
  strcpy(member.group, g);

  return VLAD_OK;
}

int atom::init_subset(const char *g1, const char *g2, bool t)
{
  if (g1 == NULL || g2 == NULL)
    return VLAD_NULLPTR;

  type = VLAD_ATOM_SUBSET;
  truth = t;

  if ((subset.group1 = VLAD_STRING_MALLOC(g1)) == NULL)
    return VLAD_MALLOCFAILED;
  if ((subset.group2 = VLAD_STRING_MALLOC(g2)) == NULL)
    return VLAD_MALLOCFAILED;

  /* strcpy never fails */
  strcpy(subset.group1, g1);
  strcpy(subset.group2, g2);

  return VLAD_OK;
}

void atom::negate()
{
  truth = truth ? false : true;
}

#ifdef DEBUG
/* assuming s has enough memory allocation */
void atom::print(char *s)
{
  switch(type) {
    case VLAD_ATOM_CONST :
      sprintf(s, "%s%s", truth ? "" : "!", constant ? "true" : "false");
      break;
    case VLAD_ATOM_HOLDS :
      sprintf(s,
              "%sholds(%s,%s,%s)",
              truth ? "" : "!",
              holds.subject,
              holds.access,
              holds.object);
      break;
    case VLAD_ATOM_MEMBER :
      sprintf(s,
              "%smemb(%s,%s)",
              truth ? "" : "!",
              member.element,
              member.group);
      break;
    case VLAD_ATOM_SUBSET :
      sprintf(s,
              "%ssubst(%s,%s)",
              truth ? "" : "!",
              subset.group1,
              subset.group2);
      break;
  }
}
#endif
