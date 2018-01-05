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
#include <cstddef>
#include <cstring>

#include <vlad/vlad.h>
#include <vlad/mem.h>
#include <vlad/identifier.h>
#include <vlad/varlist.h>

/* add a variable into the list */
int vlad_varlist::add(const char *a_var)
{
  int retval;

  /* make sure what we're adding is really a variable */
  if ((retval = vlad_identifier::validate_var_ident(a_var)) != VLAD_OK)
    return retval;

  return vlad_stringlist::add(a_var);
}

/* add the contents of the given list to this list */
int vlad_varlist::add(vlad_varlist *a_vlist)
{
  return vlad_list::add(dynamic_cast<vlad_list *>(a_vlist));
}

/* delete all items that matches the items in the given list */
int vlad_varlist::del(vlad_varlist *a_vlist)
{
  return vlad_list::del(dynamic_cast<vlad_list *>(a_vlist), true);
}

/* copy varlist */
int vlad_varlist::copy(vlad_varlist **a_vlist)
{
  int retval;

  if (a_vlist == NULL)
    return VLAD_NULLPTR;

  if ((*a_vlist = VLAD_MEM_NEW(vlad_varlist())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = (*a_vlist)->vlad_list::add((vlad_list *) this)) != VLAD_OK) {
    VLAD_MEM_DELETE(*a_vlist);
    return retval;
  }

  return VLAD_OK;
}
