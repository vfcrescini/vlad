/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004, 2005, 2006 University of Western Sydney
 * by Vino Fernando Crescini <jcrescin@scm.uws.edu.au>
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

#include <vlad/vlad.h>
#include <vlad/mem.h>
#include <vlad/stringlist.h>

vlad_stringlist_item::vlad_stringlist_item()
{
  m_string = NULL;
}

vlad_stringlist_item::~vlad_stringlist_item()
{
  if (m_string != NULL)
    VLAD_MEM_FREE(m_string);
}

/* compare item with this string */
bool vlad_stringlist_item::cmp(vlad_list_item *a_item)
{
  vlad_stringlist_item *sitem = NULL;

  /* a NULL will not match anything */
  if (a_item == NULL)
    return false;

  if ((sitem = dynamic_cast<vlad_stringlist_item *>(a_item)) == NULL)
    return false;

  /* only return true if they are both NULL */
  if (sitem->m_string == NULL)
      return (m_string == NULL);

  return (strcmp(sitem->m_string, m_string) == 0);
}

/* return an exact duplicate of this item */
vlad_list_item *vlad_stringlist_item::copy()
{
  vlad_stringlist_item *sitem;

  if ((sitem = VLAD_MEM_NEW(vlad_stringlist_item())) == NULL)
    return NULL;

  if (sitem->init(m_string) != VLAD_OK)
    return NULL;

  return (vlad_list_item *) sitem;
}

/* init with str */
int vlad_stringlist_item::init(const char *a_str)
{
  if (a_str == NULL)
    return VLAD_NULLPTR;

  if (m_string != NULL)
    VLAD_MEM_FREE(m_string);

  if ((m_string = VLAD_MEM_STR_MALLOC(a_str)) == NULL)
    return VLAD_MALLOCFAILED;

  return ((strcpy(m_string, a_str) == NULL) ? VLAD_FAILURE : VLAD_OK);
}

/* get value */
char *vlad_stringlist_item::get()
{
  return m_string;
}

vlad_stringlist::vlad_stringlist() : vlad_list(true)
{
  m_uniq = true;
}

vlad_stringlist::vlad_stringlist(bool a_uniq) : vlad_list(a_uniq)
{
  m_uniq = a_uniq;
}

vlad_stringlist::~vlad_stringlist()
{
  purge(true);
}

/* compares 2 lists */
bool vlad_stringlist::cmp(vlad_list_item *a_item)
{
  vlad_list *list;

  if ((list = dynamic_cast<vlad_list *>(a_item)) == NULL)
    return false;

  return vlad_list::cmp(list);
}

/* add a string in the list */
int vlad_stringlist::add(const char *a_str)
{
  int retval;
  vlad_stringlist_item *sitem = NULL;

  if (a_str == NULL)
    return VLAD_NULLPTR;

  if ((sitem = VLAD_MEM_NEW(vlad_stringlist_item())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = sitem->init(a_str)) != VLAD_OK)
    return retval;

  if ((retval = vlad_list::add(sitem)) != VLAD_OK) {
    VLAD_MEM_DELETE(sitem);
    return retval;
  }
  return VLAD_OK;
}

/* add the contents of the given list to this list */
int vlad_stringlist::add(vlad_stringlist *a_slist)
{
  return vlad_list::add(dynamic_cast<vlad_list *>(a_slist));
}

/* delete all items that matches the items in the given list */
int vlad_stringlist::del(vlad_stringlist *a_slist)
{
  return vlad_list::del(dynamic_cast<vlad_list *>(a_slist), true);
}

/* get the index of the string (will only work if m_uniq is true) */
int vlad_stringlist::get(const char *a_str, unsigned int *a_index)
{
  int retval;
  vlad_stringlist_item sitem;
  unsigned int size;
  unsigned int *array;

  if (a_str == NULL || a_index == NULL)
    return VLAD_NULLPTR;

  if (!m_uniq)
    return VLAD_INVALIDOP;

  if ((retval = sitem.init(a_str)) != VLAD_OK)
    return retval;

  if ((retval = vlad_list::get(&sitem, &array, &size)) != VLAD_OK)
    return retval;

  /* there should be exactly one in the array */
  *a_index = array[0];

  VLAD_MEM_FREE(array);

  return VLAD_OK;
}

/* get the ith string in the list */
int vlad_stringlist::get(unsigned int a_index, char **a_str)
{
  int retval;
  vlad_stringlist_item *sitem = NULL;

  /*
   * this will give a reference to the actual string and not a copy,
   * so care must be taken to ensure that s is not changed.
   */

  if ((retval = vlad_list::get(a_index, (vlad_list_item **) &sitem)) != VLAD_OK)
    return retval;

  *a_str = sitem->get();

  return VLAD_OK;
}

/* make a copy of this stringlist */
int vlad_stringlist::copy(vlad_stringlist **a_slist)
{
  int retval;

  if (a_slist == NULL)
    return VLAD_NULLPTR;

  if ((*a_slist = VLAD_MEM_NEW(vlad_stringlist(m_uniq))) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = (*a_slist)->vlad_list::add((vlad_list *) this)) != VLAD_OK) {
    VLAD_MEM_DELETE(*a_slist);
    return retval;
  }

  return VLAD_OK;
}

/* return true if string is in the list */
int vlad_stringlist::find(const char *a_str)
{
  int retval;
  vlad_stringlist_item sitem;

  if ((retval = sitem.init(a_str)) != VLAD_OK)
    return retval;

  return vlad_list::find(&sitem);
}

/* called by vlad_list::traverse() */
int vlad_stringlist::trav(vlad_list_item *a_item, vlad_list_trav *a_trav)
{
  vlad_stringlist_item *sitem;
  vlad_stringlist_trav *strav;

  if ((sitem = dynamic_cast<vlad_stringlist_item *>(a_item)) == NULL)
    return VLAD_NULLPTR;

  if ((strav = dynamic_cast<vlad_stringlist_trav *>(a_trav)) == NULL)
    return VLAD_NULLPTR;

  return strav->trav((const char *)sitem->get());
}
