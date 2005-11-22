/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004, 2005 University of Western Sydney
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
#include <vlad/seqtab.h>

updateref::updateref()
{
  m_name = NULL;
  m_list = NULL;
  m_init = false;	
}

updateref::~updateref()
{
  if (m_name != NULL)
    free(m_name);
  if (m_list != NULL)
    delete m_list;
}

bool updateref::cmp(list_item *a_item)
{
  updateref *tmp = NULL;

  if (a_item == NULL)
    return false;

  if ((tmp = dynamic_cast<updateref *>(a_item)) == NULL)
    return false;

  /* if both are uninit return true. if only one -- false */
  if (!m_init)
    return !tmp->m_init;

  if (!tmp->m_init)
    return false;

  /* m_name cannot be NULL */
  if (strcmp(m_name, tmp->m_name))
    return false;

  return VLAD_LIST_ITEMCMP(m_list, tmp->m_list);
}

int updateref::init(const char *a_name, stringlist *a_list)
{
  if (a_name == NULL)
    return VLAD_NULLPTR;

  m_name = (char *) a_name;
  m_list = a_list;
  m_init = true;

  return VLAD_OK;
}

int updateref::get(char **a_name, stringlist **a_list)
{
  if (a_name == NULL || a_list == NULL)
    return VLAD_NULLPTR;

  if (!m_init)
    return VLAD_UNINITIALISED;

  *a_name = m_name;
  *a_list = m_list;

  return VLAD_OK;
}

#ifdef VLAD_DEBUG
void updateref::print(char *a_str)
{
  char tmp_str[VLAD_MAXLEN_STR];

  if (m_init) {
    memset(tmp_str, 0, VLAD_MAXLEN_STR);

    if (m_list != NULL)
      m_list->print(tmp_str);

    sprintf(a_str, "%s(%s)", m_name, tmp_str);
  }
}
#endif

seqtab::seqtab() : list(false)
{
}

seqtab::~seqtab()
{
  purge(true);
}

/* add pre-malloc'ed updateref */
int seqtab::add(updateref *a_uref)
{
  if (a_uref == NULL)
    return VLAD_NULLPTR;

  return list::add((list_item *) a_uref);
}

/* add pre-malloc'ed m_name and ilist */
int seqtab::add(const char *a_name, stringlist *a_list)
{
  int retval;
  updateref *tmp_ref;

  if ((tmp_ref = VLAD_NEW(updateref())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = tmp_ref->init(a_name, a_list)) != VLAD_OK)
    return retval;

  return list::add((list_item *) tmp_ref);
}

/* delete i'th item */
int seqtab::del(unsigned int a_index)
{
  return list::del(a_index, true);
}

/* get i'th m_name and ilist */
int seqtab::get(unsigned int a_index, char **a_name, stringlist **a_list)
{
  int retval;
  updateref *tmp_ref;

  if (a_name == NULL || a_list == NULL)
    return VLAD_NULLPTR;

  if ((retval = list::get(a_index, (list_item **) &tmp_ref)) != VLAD_OK)
    return retval;

  return tmp_ref->get(a_name, a_list);
}

#ifdef VLAD_DEBUG
void seqtab::print(char *a_str)
{
  unsigned int i;
  char tmp_str[VLAD_MAXLEN_STR];
  updateref *tmp_obj;

  strcpy(a_str, "");

  for (i = 0; i < list::length(); i++) {
    if (list::get(i, (list_item **) &tmp_obj) != VLAD_OK)
      break;

    memset(tmp_str, 0, VLAD_MAXLEN_STR);
    if (tmp_obj != NULL)
      tmp_obj->print(tmp_str);
    sprintf(a_str, "%s %s", a_str, tmp_str);
  }
}
#endif
