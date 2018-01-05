/*
 * querytab.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <new>

#include <config.h>
#include <vlad.h>
#include <querytab.h>

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

transreflist::transreflist() : list(false)
{
}

transreflist::~transreflist()
{
  purge(true);
}

/* add pre-malloc'ed transref */
int transreflist::add(transref *t)
{
  if (t == NULL)
    return VLAD_NULLPTR;

  return list::add((list_item *) t);
}

int transreflist::get(unsigned int i, transref **t)
{
  return list::get(i, (list_item **) t);
}

#ifdef DEBUG
void transreflist::print(char *s)
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

query::query(expression *p, transreflist *r)
{
  prop = p;
  reflist = r;
}

query::~query()
{
  if (prop != NULL)
    delete prop;
  if (reflist != NULL)
    delete reflist;
}

bool query::cmp(list_item *item)
{
  query *tmp = NULL;

  if (item == NULL)
    return false;

  if ((tmp = dynamic_cast<query *> (item)) == NULL)
    return false;

  if (tmp->prop == NULL && prop != NULL)
    return false;
  else if (tmp->prop != NULL && prop == NULL)
    return false;
  else if (tmp->prop != NULL && prop != NULL && !prop->cmp(tmp->prop))
    return false;

  if (tmp->reflist == NULL && reflist != NULL)
    return false;
  else if (tmp->reflist != NULL && reflist == NULL)
    return false;
  else if (tmp->reflist != NULL && reflist != NULL && !reflist->cmp(tmp->reflist))
    return false;

  return true;
}

querytab::querytab() : list(true)
{
}

querytab::~querytab()
{
  purge(true);
}

int querytab::add(expression *p, transreflist *r)
{
  query *tmp;

  if (p == NULL || r == NULL)
    return VLAD_NULLPTR;

  if ((tmp = VLAD_NEW(query(p, r))) == NULL)
    return VLAD_MALLOCFAILED;

  return list::add((list_item *) tmp);

}
