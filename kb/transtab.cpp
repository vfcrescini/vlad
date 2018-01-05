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
  vtable = v;
  precond = pr;
  postcond = po;
}

transdef::~transdef()
{
  if (name != NULL)
    free(name);
  if (vtable != NULL)
    delete vtable;
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

transtab::transtab(const char *n) : list(n, true)
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
