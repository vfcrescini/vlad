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

transref::transref(const char *n, stringlist *i)
{
  name = (char *) n;
  ilist = i;
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

  if (tmp->name == NULL && name != NULL)
    return false;
  else if (tmp->name != NULL && name == NULL)
    return false;
  else if (tmp->name != NULL && name != NULL && strcmp(name, tmp->name))
    return false;

  if (tmp->ilist == NULL && ilist != NULL)
    return false;
  else if (tmp->ilist != NULL && ilist == NULL)
    return false;
  else if (tmp->ilist != NULL && ilist != NULL && !ilist->cmp(tmp->ilist))
    return false;

  return true;
}

char *transref::get_name()
{
  return name;
}

stringlist *transref::get_ilist()
{
  return ilist;
}

#ifdef DEBUG
void transref::print(char *s)
{
  char tmps[1024];

  strcpy(tmps, "");

  if (ilist != NULL)
    ilist->print(tmps);

  sprintf(s, "%s(%s)", name, tmps);
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

int sequence::get(unsigned int i, transref **t)
{
  return list::get(i, (list_item **) t);
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

    tmpr->print(tmps);
    sprintf(s, "%s %s", s, tmps);
  }
}
#endif
