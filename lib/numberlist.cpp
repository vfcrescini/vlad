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
#include <new>

#include <vlad/vlad.h>
#include <vlad/numberlist.h>

number::number()
{
  num = 0;
}

number::~number()
{
}

/* compare item with this number */
bool number::cmp(list_item *item)
{
  number *tmp = NULL;

  /* a NULL will not match anything */
  if (item == NULL)
    return false;

  if ((tmp = dynamic_cast<number *> (item)) == NULL)
    return false;

  return (tmp->num == num);
}

int number::init(unsigned int n)
{
  num = n;
  return VLAD_OK;
}

unsigned int number::get()
{
  return num;
}

numberlist::numberlist() : list(false)
{
}

numberlist::~numberlist()
{
  list::purge(true);
}

/* add a number in the list */
int numberlist::add(unsigned int n)
{
  int retval;
  number *tmp = NULL;

  if ((tmp = VLAD_NEW(number())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = tmp->init(n)) != VLAD_OK)
    return retval;

  if ((retval = list::add(tmp)) != VLAD_OK) {
    delete tmp;
    return retval;
  }
  return VLAD_OK;
}

/* get the i'th number in the list */
int numberlist::get(unsigned int i, unsigned int *n)
{
  int retval;
  number *tmp = NULL;

  if ((retval = list::get(i, (list_item **) &tmp)) != VLAD_OK)
    return retval;

  *n = tmp->get();

  return VLAD_OK;
}

/* empty the list */
void numberlist::purge()
{
  list::purge(true);
}
