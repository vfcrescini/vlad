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
#include <vlad/expression.h>

expression::expression() : list(true)
{
}

expression::~expression()
{
  purge(true);
}

/* add pre-malloc'ed atom */
int expression::add(atom *a)
{
  if (a == NULL)
    return VLAD_NULLPTR;

  return list::add(a);
}

/* return true if atom is in the expression */
int expression::find(atom *a)
{
  if (a == NULL)
    return VLAD_NULLPTR;

  return list::find(a);
}
/* give a reference to the i'th atom */
int expression::get(unsigned int i, atom **a)
{
  if (a == NULL)
    return VLAD_NULLPTR;

  return list::get(i, (list_item **) a);
}

/* replace vars in vlist to ident in ilist. create a new expression */
int expression::replace(stringlist *vlist, stringlist *ilist, expression **e)
{
  int retval;
  unsigned int i;
  atom *old_atom;
  atom *new_atom;

  if (e == NULL)
    return VLAD_NULLPTR;

  if ((*e = VLAD_NEW(expression())) == NULL)
    return VLAD_MALLOCFAILED;

  for (i = 0; i < list::length(); i++) {
    if ((retval = get(i, &old_atom)) != VLAD_OK)
      return retval;
    if ((retval = old_atom->replace(vlist, ilist, &new_atom)) != VLAD_OK)
      return retval;
    if ((retval = (*e)->add(new_atom)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

#ifdef VLAD_DEBUG
/* assumimg s has enough memory allocation */
void expression::print(char *s)
{
  unsigned int i;
  char tmps[VLAD_MAXLEN_STR];
  atom *tmpa;

  for (i = 0; i < list::length(); i++) {
    if (list::get(i, (list_item **) &tmpa) != VLAD_OK)
      break;

    memset(tmps, 0, VLAD_MAXLEN_STR);
    tmpa->print(tmps);
    sprintf(s, "%s %s", s, tmps);
  }
}
#endif
