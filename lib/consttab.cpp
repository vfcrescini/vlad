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
#include <vlad/consttab.h>

constraint::constraint()
{
  exp = NULL;
  cond = NULL;
  ncond = NULL;
  initialised = false;
}

constraint::~constraint()
{
  if (exp != NULL)
    delete exp;
  if (cond != NULL)
    delete cond;
  if (ncond != NULL)
    delete ncond;
}

bool constraint::cmp(list_item *item)
{
  constraint *tmp = NULL;

  if (item == NULL)
    return false;

  if ((tmp = dynamic_cast<constraint *> (item)) == NULL)
    return false;

  /* ensure both are initialised. return true if both are uninitialised */

  if (!initialised)
    return !tmp->initialised;

  if (!tmp->initialised)
    return false;

  /* exp cannot be NULL */
  if (!exp->cmp(tmp->exp))
    return false;

  if (!VLAD_LIST_ITEMCMP(cond, tmp->cond))
    return false;

  if (!VLAD_LIST_ITEMCMP(ncond, tmp->ncond))
    return false;

  return true;
}

int constraint::init(expression *e, expression *c, expression *n)
{
  if (e == NULL)
    return VLAD_NULLPTR;

  if (exp != NULL)
    delete exp;
  if (cond != NULL)
    delete cond;
  if (ncond != NULL)
    delete ncond;

  exp = e;
  cond = c;
  ncond = n;
  initialised = true;

  return VLAD_OK;
}

int constraint::get(expression **e, expression **c, expression **n)
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  if (e == NULL || c == NULL || n == NULL)
    return VLAD_NULLPTR;

  *e = exp;
  *c = cond;
  *n = ncond;

  return VLAD_OK;
}

consttab::consttab() : list(true)
{
}

consttab::~consttab()
{
  purge(true);
}

int consttab::add(expression *e, expression *c, expression *n)
{
  int retval;
  constraint *tmp;

  if ((tmp = VLAD_NEW(constraint())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = tmp->init(e, c, n)) != VLAD_OK) {
    delete tmp;
    return retval;
  }

  return list::add((list_item *) tmp);
}

int consttab::get(unsigned int i, expression **e, expression **c, expression **n)
{
  int retval;
  constraint *tmp;

  if ((retval = list::get(i, (list_item **) &tmp)) != VLAD_OK)
    return retval;

  return tmp->get(e, c, n);
}
