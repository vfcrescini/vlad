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

transdef::transdef(char *n, stringlist *v, expression *pr, expression *po)
{
  name = n;
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

  if (tmp->name == NULL && name != NULL)
    return false;
  else if (tmp->name != NULL && name == NULL)
    return false;
  else if (tmp->name != NULL && name != NULL && strcmp(name, tmp->name))
    return false;
  
  if (tmp->vtable == NULL && vtable != NULL)
    return false;
  else if (tmp->vtable != NULL && vtable == NULL)
    return false;
  else if (tmp->vtable != NULL && vtable != NULL && !vtable->cmp(tmp->vtable))
    return false;

  if (tmp->precond == NULL && precond != NULL)
    return false;
  else if (tmp->precond != NULL && precond == NULL)
    return false;
  else if (tmp->precond != NULL && precond != NULL && !precond->cmp(tmp->precond))
    return false;

  if (tmp->postcond == NULL && postcond != NULL)
    return false;
  else if (tmp->postcond != NULL && postcond == NULL)
    return false;
  else if (tmp->postcond != NULL && postcond != NULL && !postcond->cmp(tmp->postcond))
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

int transtab::add(char *n, stringlist *v, expression *pr, expression *po)
{
  transdef *tmp;

  if (n == NULL || v == NULL || pr == NULL || po == NULL)
    return VLAD_NULLPTR;

  if ((tmp = VLAD_NEW(transdef(n, v, pr, po))) == NULL)
    return VLAD_MALLOCFAILED;

  return list::add((list_item *) tmp);
}
