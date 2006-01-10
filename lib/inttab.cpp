/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004, 2005, 2006 University of Western Sydney
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
#include <cstring>

#include <vlad/vlad.h>
#include <vlad/mem.h>
#include <vlad/inttab.h>

vlad_inttab_item::vlad_inttab_item()
{
  m_name = NULL;
  m_ep1 = 0;
  m_ep2 = 0;
  m_init = false;
}

vlad_inttab_item::~vlad_inttab_item()
{
  if (m_name != NULL)
    VLAD_MEM_FREE(m_name);
}

/* compare item with this */
bool vlad_inttab_item::cmp(vlad_list_item *a_item)
{
  vlad_inttab_item *iitem = NULL;

  /* a NULL will not match anything */
  if (a_item == NULL)
    return false;

  if ((iitem = dynamic_cast<vlad_inttab_item *>(a_item)) == NULL)
    return false;

  /* only compare the names */
  return (strcmp(iitem->m_name, m_name) == 0);
}

/* return an exact duplicate of this item */
vlad_list_item *vlad_inttab_item::copy()
{
  vlad_inttab_item *iitem;

  if ((iitem = VLAD_MEM_NEW(vlad_inttab_item())) == NULL)
    return NULL;

  if (!m_init)
    return (vlad_list_item *) iitem;

  if (iitem->init(m_name, m_ep1, m_ep2) != VLAD_OK)
    return NULL;

  return (vlad_list_item *) iitem;
}

/* initialise */
int vlad_inttab_item::init(const char *a_name,
                            unsigned int a_ep1,
                            unsigned int a_ep2)
{
  if (a_name == NULL)
    return VLAD_NULLPTR;

  /* obviously, ep1 should be less than ep2 */
  if (a_ep1 >= a_ep2)
    return VLAD_INVALIDINPUT;

  if (m_name != NULL)
    VLAD_MEM_FREE(m_name);

  if ((m_name = VLAD_MEM_STR_MALLOC(a_name)) == NULL)
    return VLAD_MALLOCFAILED;

  strcpy(m_name, a_name);
  m_ep1 = a_ep1;
  m_ep2 = a_ep2;
  m_init = true;

  return VLAD_OK;
}

/* get values */
int vlad_inttab_item::get(char **a_name,
                           unsigned int *a_ep1,
                           unsigned int *a_ep2)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_name == NULL || a_ep1 == NULL || a_ep2 == NULL)
    return VLAD_NULLPTR;

  /* give reference to name, not a copy */
  *a_name = m_name;
  *a_ep1 = m_ep1;
  *a_ep2 = m_ep2;

  return VLAD_OK;
}

vlad_inttab::vlad_inttab() : vlad_list(true)
{
}

vlad_inttab::~vlad_inttab()
{
  purge(true);
}

/* add an interval */
int vlad_inttab::add(const char *a_name,
                     unsigned int a_ep1,
                     unsigned int a_ep2)
{
  int retval;
  vlad_inttab_item *iitem = NULL;

  if (a_name == NULL)
    return VLAD_NULLPTR;

  if ((iitem = VLAD_MEM_NEW(vlad_inttab_item())) == NULL)
    return VLAD_MALLOCFAILED;

  retval = VLAD_OK;

  if (retval == VLAD_OK)
    retval = iitem->init(a_name, a_ep1, a_ep2);

  if (retval == VLAD_OK)
    retval = vlad_list::add(iitem);

  if (retval != VLAD_OK)
    VLAD_MEM_DELETE(iitem);

  return retval;
}

/* gives the i'th item in the table, a_ep is set if endpoins are defined */
int vlad_inttab::get(unsigned int a_index,
                     char **a_name,
                     unsigned int *a_ep1,
                     unsigned int *a_ep2)
{
  int retval;
  vlad_inttab_item *iitem = NULL;

  if (a_name == NULL)
    return VLAD_NULLPTR;

  if ((retval = vlad_list::get(a_index, (vlad_list_item **) &iitem)) != VLAD_OK)
    return retval;

  if ((retval = iitem->get(a_name, a_ep1, a_ep2)) != VLAD_OK)
    return retval;

  return VLAD_OK;
}

/* gives the endpoints of the interval of the given name */
int vlad_inttab::get(char *a_name, unsigned int *a_ep1, unsigned int *a_ep2)
{
  int retval;
  unsigned int i;

  if (a_name == NULL || a_ep1 == NULL || a_ep2 == NULL)
    return VLAD_NULLPTR;

  for (i = 0; i < vlad_list::length(); i++) {
    char *name;
    unsigned int ep1;
    unsigned int ep2;

    if ((retval = get(i, &name, &ep1, &ep2)) != VLAD_OK)
      return retval;

    if (!strcmp(a_name, name)) {
      *a_ep1 = ep1;
      *a_ep2 = ep2;
      return VLAD_OK;
    }
  }

  return VLAD_NOTFOUND;
}
