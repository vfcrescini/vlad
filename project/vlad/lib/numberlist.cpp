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

vlad_number::vlad_number()
{
  m_number = 0;
}

vlad_number::~vlad_number()
{
}

/* compare item with this number */
bool vlad_number::cmp(vlad_list_item *a_item)
{
  vlad_number *tmp = NULL;

  /* a NULL will not match anything */
  if (a_item == NULL)
    return false;

  if ((tmp = dynamic_cast<vlad_number *>(a_item)) == NULL)
    return false;

  return (tmp->m_number == m_number);
}

/* initialise with num */
int vlad_number::init(unsigned int a_num)
{
  m_number = a_num;
  return VLAD_OK;
}

/* get value */
unsigned int vlad_number::get()
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
  vlad_number *tmp = NULL;

  if ((tmp = VLAD_MEM_NEW(vlad_number())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = tmp->init(a_num)) != VLAD_OK)
    return retval;

  if ((retval = vlad_list::add(tmp)) != VLAD_OK) {
    delete tmp;
    return retval;
  }
  return VLAD_OK;
}

/* get the i'th number in the list */
int vlad_numberlist::get(unsigned int a_index, unsigned int *a_num)
{
  int retval;
  vlad_number *tmp = NULL;

  if ((retval = vlad_list::get(a_index, (vlad_list_item **) &tmp)) != VLAD_OK)
    return retval;

  *a_num = tmp->get();

  return VLAD_OK;
}

/* empty the list */
void vlad_numberlist::purge()
{
  vlad_list::purge(true);
}
