/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004, 2005 University of Western Sydney
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
#include <vlad/varlist.h>

/* copy varlist */
int vlad_varlist::copy(vlad_varlist **a_vlist)
{
  int retval;
  unsigned int i;

  if (a_vlist == NULL)
    return VLAD_NULLPTR;

  if ((*a_vlist = VLAD_MEM_NEW(vlad_varlist())) == NULL)
    return VLAD_MALLOCFAILED;

  for (i = 0; i < vlad_list::length(); i++) {
    char *var;

    if ((retval = get(i, &var)) != VLAD_OK)
      return retval;

    if ((retval = (*a_vlist)->add(var)) != VLAD_OK) {
      VLAD_MEM_DELETE(*a_vlist);
      return retval;
    }
  }

  return VLAD_OK;
}

/* verify and copy */
int vlad_varlist::vcopy(vlad_symtab *a_symtab, vlad_varlist **a_vlist)
{
  int retval;
  unsigned int i;

  if (a_symtab == NULL || a_vlist == NULL)
    return VLAD_NULLPTR;

  if ((*a_vlist = VLAD_MEM_NEW(vlad_varlist())) == NULL)
    return VLAD_MALLOCFAILED;

  for (i = 0; i < vlad_list::length(); i++) {
    char *var;

    if ((retval = get(i, &var)) != VLAD_OK)
      return retval;

    /* before adding, check if the variable is already an identifier */

    if ((retval = a_symtab->find(var)) != VLAD_NOTFOUND) {
      VLAD_MEM_DELETE(*a_vlist);
      return (retval == VLAD_OK) ? VLAD_DUPLICATE : retval;
    }

    if ((retval = (*a_vlist)->add(var)) != VLAD_OK) {
      VLAD_MEM_DELETE(*a_vlist);
      return retval;
    }
  }
  return VLAD_OK;
}
