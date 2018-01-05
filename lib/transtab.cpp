/*
 * transtab.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <new>

#include <vlad/vlad.h>
#include <vlad/transtab.h>

transdef::transdef()
{
  name = NULL;
  vlist = NULL;
  precond = NULL;
  postcond = NULL;
  initialised = false;
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

  /* if both are uninitialised return true. if only one -- false */
  if (!initialised)
    return !tmp->initialised;

  if (!tmp->initialised)
    return false;

  /*
   * here we are only concerned about the name. if the names match,
   * then we consider the transformations equivalent.
   */

  return !strcmp(name, tmp->name);
}

int transdef::init(const char *n, stringlist *v, expression *pr, expression *po)
{
  if (initialised) {
    if (name != NULL)
      free(name);
    if (vlist != NULL)
      delete vlist;
    if (precond != NULL)
      delete precond;
    if (postcond != NULL)
      delete postcond;
  }

  if (n == NULL)
    return VLAD_NULLPTR;

  name = (char *) n;
  vlist = v;
  precond = pr;
  postcond = po;

  initialised = true;

  return VLAD_OK;
}

int transdef::get(char **n, stringlist **v, expression **pr, expression **po)
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  if (n == NULL || v == NULL || pr == NULL || po == NULL)
    return VLAD_NULLPTR;

  *n = name;
  *v = vlist;
  *pr = precond;
  *po = postcond;

  return VLAD_OK;
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
  int retval;
  transdef *tmp;

  if ((tmp = VLAD_NEW(transdef())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = tmp->init(n, v, pr, po)) != VLAD_OK) {
    delete tmp;
    return retval;
  }

  return list::add((list_item *) tmp);
}

int transtab::get(const char *n, stringlist **v, expression **pr, expression **po)
{
  int retval;
  char *tmp_name;
  transdef *tmp_def;
  transdef **lst;
  unsigned int size;

  if (n == NULL || v == NULL || pr == NULL || po == NULL)
    return VLAD_NULLPTR;

  /* create a dummy transdef to search */
  if ((tmp_name = VLAD_STRING_MALLOC(n)) == NULL)
    return VLAD_MALLOCFAILED;

  strcpy(tmp_name, n);

  if ((tmp_def = VLAD_NEW(transdef())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = tmp_def->init(tmp_name, NULL, NULL, NULL)) != VLAD_OK)
    return retval;

  if ((retval = list::get((list_item *) tmp_def, (list_item ***) &lst, &size)) != VLAD_OK)
    return retval;

  /* only one. name is already copied and will be deleted with tmp */
  if ((retval = lst[0]->get(&tmp_name, v, pr, po)) != VLAD_OK)
    return retval;

  free(lst);
  delete tmp_def;

  return VLAD_OK;
}

/* replace variables with identifiers in v, then get pr and pp */
int transtab::replace(const char *n, stringlist *ilist, expression **pr, expression **po)
{
  int retval;
  expression *tmp_precond;
  expression *tmp_postcond;
  stringlist *tmp_vlist;

  if (n == NULL || pr == NULL || po == NULL)
    return VLAD_NULLPTR;

  if ((retval = get(n, &tmp_vlist, &tmp_precond, &tmp_postcond)) != VLAD_OK)
    return retval;

  if (tmp_precond != NULL) {
    if ((retval = tmp_precond->replace(tmp_vlist, ilist, pr)) != VLAD_OK)
      return retval;
  }
  else
    *pr = NULL;

  if (tmp_postcond != NULL) {
    if ((retval = tmp_postcond->replace(tmp_vlist, ilist, po)) != VLAD_OK)
      return retval;
  }
  else
    *po = NULL;

  return VLAD_OK;
}
