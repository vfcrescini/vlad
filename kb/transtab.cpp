/*
 * transtab.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <new>

#include <config.h>
#include <vlad.h>
#include <transtab.h>

transdef::transdef(const char *n, stringlist *v, expression *pr, expression *po)
{
  name = (char *) n;
  vlist = v;
  precond = pr;
  postcond = po;
}

transdef::~transdef()
{
  if (name != NULL)
    free(name);
  if (vlist != NULL)
    delete vlist;
  if (precond != NULL)
    delete precond;
  if (postcond != NULL)
    delete postcond;
}

bool transdef::cmp(list_item *item)
{
  transdef *tmp = NULL;

  if (item == NULL)
    return false;

  if ((tmp = dynamic_cast<transdef *> (item)) == NULL)
    return false;

  /* 
   * here we are only concerned about the name. if the names match,
   * then we consider the transformations equivalent.
   */

  if (tmp->name == NULL && name != NULL)
    return false;
  else if (tmp->name != NULL && name == NULL)
    return false;
  else if (tmp->name != NULL && name != NULL && strcmp(name, tmp->name))
    return false;

  return true;
}

char *transdef::get_name()
{
  return name;
}

stringlist *transdef::get_vlist()
{
  return vlist;
}

transtab::transtab() : list(true)
{
}

transtab::~transtab()
{
  purge(true);
}

int transtab::add(const char *n, stringlist *v, expression *pr, expression *po)
{
  transdef *tmp;

  if (n == NULL || v == NULL || pr == NULL || po == NULL)
    return VLAD_NULLPTR;

  if ((tmp = VLAD_NEW(transdef(n, v, pr, po))) == NULL)
    return VLAD_MALLOCFAILED;

  return list::add((list_item *) tmp);
}

int transtab::get(const char *n, transdef **t)
{
  int retval;
  char *name;
  transdef *tmp;
  transdef **list;
  unsigned int s;

  if (n == NULL)
    return VLAD_NULLPTR;

  if ((name = VLAD_STRING_MALLOC(n)) == NULL)
    return VLAD_MALLOCFAILED;

  strcpy(name, n); 

  if ((tmp = VLAD_NEW(transdef(name, NULL, NULL, NULL))) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = list::get((list_item *) tmp, (list_item ***) &list, &s)) != VLAD_OK)
    return retval;

  /* only one */
  *t = list[0];

  free(list);
  delete tmp;

  return VLAD_OK;
}
