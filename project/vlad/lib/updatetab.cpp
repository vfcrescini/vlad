/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004 University of Western Sydney
 * by Vino Fernando Crescini <jcrescin@cit.uws.edu.au>
 *
 * PolicyUpdater is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * PolicyUpdater is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PolicyUpdater; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <new>

#include <vlad/vlad.h>
#include <vlad/updatetab.h>

updatedef::updatedef()
{
  m_name = NULL;
  m_vlist = NULL;
  m_precond = NULL;
  m_postcond = NULL;
  m_init = false;
}

updatedef::~updatedef()
{
  if (m_name != NULL)
    free(m_name);
  if (m_vlist != NULL)
    delete m_vlist;
  if (m_precond != NULL)
    delete m_precond;
  if (m_postcond != NULL)
    delete m_postcond;
}

bool updatedef::cmp(list_item *a_item)
{
  updatedef *tmp = NULL;

  if (a_item == NULL)
    return false;

  if ((tmp = dynamic_cast<updatedef *>(a_item)) == NULL)
    return false;

  /* if both are uninit return true. if only one -- false */
  if (!m_init)
    return !tmp->m_init;

  if (!tmp->m_init)
    return false;

  /*
   * here we are only concerned about the m_name. if the m_names match,
   * then we consider the update equivalent.
   */

  return !strcmp(m_name, tmp->m_name);
}

int updatedef::init(const char *a_name,
                   stringlist *a_vlist,
                   expression *a_precond,
                   expression *a_postcond)
{
  if (m_init) {
    if (m_name != NULL)
      free(m_name);
    if (m_vlist != NULL)
      delete m_vlist;
    if (m_precond != NULL)
      delete m_precond;
    if (m_postcond != NULL)
      delete m_postcond;
  }

  if (a_name == NULL)
    return VLAD_NULLPTR;

  m_name = (char *) a_name;
  m_vlist = a_vlist;
  m_precond = a_precond;
  m_postcond = a_postcond;

  m_init = true;

  return VLAD_OK;
}

int updatedef::get(char **a_name,
                  stringlist **a_vlist,
                  expression **a_precond,
                  expression **a_postcond)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_name == NULL || a_vlist == NULL || a_precond == NULL || a_postcond == NULL)
    return VLAD_NULLPTR;

  *a_name = m_name;
  *a_vlist = m_vlist;
  *a_precond = m_precond;
  *a_postcond = m_postcond;

  return VLAD_OK;
}

updatetab::updatetab() : list(true)
{
}

updatetab::~updatetab()
{
  purge(true);
}

int updatetab::add(const char *a_name,
                  stringlist *a_vlist,
                  expression *a_precond,
                  expression *a_postcond)
{
  int retval;
  updatedef *tmp;

  if ((tmp = VLAD_NEW(updatedef())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = tmp->init(a_name, a_vlist, a_precond, a_postcond)) != VLAD_OK) {
    delete tmp;
    return retval;
  }

  return list::add((list_item *) tmp);
}

/* get update by m_name */
int updatetab::get(const char *a_name,
                  stringlist **a_vlist,
                  expression **a_precond,
                  expression **a_postcond)
{
  int retval;
  char *tmp_name;
  updatedef *tmp_def;
  updatedef **lst;
  unsigned int size;

  if (a_name == NULL || a_vlist == NULL || a_precond == NULL || a_postcond == NULL)
    return VLAD_NULLPTR;

  /* create a dummy updatedef to search */
  if ((tmp_name = VLAD_STRING_MALLOC(a_name)) == NULL)
    return VLAD_MALLOCFAILED;

  strcpy(tmp_name, a_name);

  if ((tmp_def = VLAD_NEW(updatedef())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = tmp_def->init(tmp_name, NULL, NULL, NULL)) != VLAD_OK)
    return retval;

  if ((retval = list::get((list_item *) tmp_def, (list_item ***) &lst, &size)) != VLAD_OK)
    return retval;

  /* only one. m_name is already copied and will be deleted with tmp */
  if ((retval = lst[0]->get(&tmp_name, a_vlist, a_precond, a_postcond)) != VLAD_OK)
    return retval;

  free(lst);
  delete tmp_def;

  return VLAD_OK;
}

/* get update by index */
int updatetab::get(unsigned int a_index,
                  char **a_name,
                  stringlist **a_vlist,
                  expression **a_precond,
                  expression **a_postcond)
{
  int retval;
  updatedef *tmp_def;

  if (a_name == NULL || a_vlist == NULL || a_precond == NULL || a_postcond == NULL)
    return VLAD_NULLPTR;

  if ((retval = list::get(a_index, (list_item **) &tmp_def)) != VLAD_OK)
    return retval;

  if ((retval = tmp_def->get(a_name, a_vlist, a_precond, a_postcond)) != VLAD_OK)
    return retval;

  return VLAD_OK;
}

/* replace variables with identifiers in v, then get pr and pp */
int updatetab::replace(const char *a_name,
                      stringlist *a_ilist,
                      expression **a_precond,
                      expression **a_postcond)
{
  int retval;
  expression *tmp_precond;
  expression *tmp_postcond;
  stringlist *tmp_vlist;

  if (a_name == NULL || a_precond == NULL || a_postcond == NULL)
    return VLAD_NULLPTR;

  if ((retval = get(a_name, &tmp_vlist, &tmp_precond, &tmp_postcond)) != VLAD_OK)
    return retval;

  if (tmp_precond != NULL) {
    if ((retval = tmp_precond->replace(tmp_vlist, a_ilist, a_precond)) != VLAD_OK)
      return retval;
  }
  else
    *a_precond = NULL;

  if (tmp_postcond != NULL) {
    if ((retval = tmp_postcond->replace(tmp_vlist, a_ilist, a_postcond)) != VLAD_OK)
      return retval;
  }
  else
    *a_postcond = NULL;

  return VLAD_OK;
}
