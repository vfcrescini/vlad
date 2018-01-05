/*
 * sequence.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <new>

#include <config.h>
#include <vlad.h>
#include <sequence.h>

transref::transref()
{
  name = NULL;
  ilist = NULL;
  initialised = false;	
}

transref::~transref()
{
  if (name != NULL)
    free(name);
  if (ilist != NULL)
    delete ilist;
}

bool transref::cmp(list_item *item)
{
  transref *tmp = NULL;

  if (item == NULL)
    return false;

  if ((tmp = dynamic_cast<transref *> (item)) == NULL)
    return false;

  /* if both are uninitialised return true. if only one -- false */
  if (!initialised)
    return !tmp->initialised;

  if (!tmp->initialised)
    return false;

  /* name cannot be NULL */
  if (strcmp(name, tmp->name))
    return false;

  return VLAD_LIST_ITEMCMP(ilist, tmp->ilist);
}

int transref::init(const char *n, stringlist *il)
{
  if (n == NULL)
    return VLAD_NULLPTR;

  name = (char *) n;
  ilist = il;
  initialised = true;

  return VLAD_OK;
}

int transref::get(char **n, stringlist **il)
{
  if (n == NULL || il == NULL)
    return VLAD_NULLPTR;

  if (!initialised)
    return VLAD_UNINITIALISED;

  *n = name;
  *il = ilist;

  return VLAD_OK;
}

#ifdef DEBUG
void transref::print(char *s)
{
  char tmps[1024];

  if (initialised) {
    strcpy(tmps, "");

    if (ilist != NULL)
      ilist->print(tmps);

    sprintf(s, "%s(%s)", name, tmps);
  }
}
#endif

sequence::sequence() : list(false)
{
}

sequence::~sequence()
{
  purge(true);
}

/* add pre-malloc'ed transref */
int sequence::add(transref *t)
{
  if (t == NULL)
    return VLAD_NULLPTR;

  return list::add((list_item *) t);
}

/* add pre-malloc'ed name and ilist */
int sequence::add(const char *n, stringlist *il)
{
  int retval;
  transref *tmp_ref;

  if ((tmp_ref = VLAD_NEW(transref())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = tmp_ref->init(n, il)) != VLAD_OK)
    return retval;

  return list::add((list_item *) tmp_ref);
}

/* get i'th name and ilist */
int sequence::get(unsigned int i, char **n, stringlist **il)
{
  int retval;
  transref *tmp_ref;

  if (n == NULL || il == NULL)
    return VLAD_NULLPTR;

  if ((retval = list::get(i, (list_item **) &tmp_ref)) != VLAD_OK)
    return retval;

  return tmp_ref->get(n, il);
}

#ifdef DEBUG
void sequence::print(char *s)
{
  unsigned int i;
  char tmps[1024];
  transref *tmpr;

  strcpy(s, "");

  for (i = 0; i < list::length(); i++) {
    if (list::get(i, (list_item **) &tmpr) != VLAD_OK)
      break;

    strcpy(tmps, "");
    if (tmpr != NULL)
      tmpr->print(tmps);
    sprintf(s, "%s %s", s, tmps);
  }
}
#endif
