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

#include <vlad/vlad.h>
#include <vlad/mem.h>
#include <vlad/numberlist.h>

vlad_numberlist_item::vlad_numberlist_item()
{
  m_number = 0;
}

vlad_numberlist_item::~vlad_numberlist_item()
{
}

/* compare item with this number */
bool vlad_numberlist_item::cmp(vlad_list_item *a_item)
{
  vlad_numberlist_item *nitem = NULL;

  /* a NULL will not match anything */
  if (a_item == NULL)
    return false;

  if ((nitem = dynamic_cast<vlad_numberlist_item *>(a_item)) == NULL)
    return false;

  return (nitem->m_number == m_number);
}

/* initialise with num */
int vlad_numberlist_item::init(unsigned int a_num)
{
  m_number = a_num;
  return VLAD_OK;
}

/* get value */
unsigned int vlad_numberlist_item::get()
{
  return m_number;
}

vlad_numberlist::vlad_numberlist() : vlad_list(false)
{
}

vlad_numberlist::~vlad_numberlist()
{
  vlad_list::purge(true);
}

/* add a number in the list */
int vlad_numberlist::add(unsigned int a_num)
{
  int retval;
  vlad_numberlist_item *nitem = NULL;

  if ((nitem = VLAD_MEM_NEW(vlad_numberlist_item())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = nitem->init(a_num)) != VLAD_OK)
    return retval;

  if ((retval = vlad_list::add(nitem)) != VLAD_OK) {
    delete nitem;
    return retval;
  }
  return VLAD_OK;
}

/* get the i'th number in the list */
int vlad_numberlist::get(unsigned int a_index, unsigned int *a_num)
{
  int retval;
  vlad_numberlist_item *nitem = NULL;

  if ((retval = vlad_list::get(a_index, (vlad_list_item **) &nitem)) != VLAD_OK)
    return retval;

  *a_num = nitem->get();

  return VLAD_OK;
}

/* empty the list */
void vlad_numberlist::purge()
{
  vlad_list::purge(true);
}

/* called by vlad_list::traverse() */
int vlad_numberlist::trav(vlad_list_item *a_item, vlad_list_trav *a_trav)
{
  vlad_numberlist_item *nitem;
  vlad_numberlist_trav *ntrav;

  if ((nitem = dynamic_cast<vlad_numberlist_item *>(a_item)) == NULL)
    return VLAD_NULLPTR;

  if ((ntrav = dynamic_cast<vlad_numberlist_trav *>(a_trav)) == NULL)
    return VLAD_NULLPTR;

  return ntrav->trav(nitem->get());
}
