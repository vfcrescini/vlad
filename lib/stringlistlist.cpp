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
#include <vlad/stringlistlist.h>

vlad_stringlistlist::vlad_stringlistlist() : vlad_list(false)
{
}

vlad_stringlistlist::~vlad_stringlistlist()
{
  purge(true);
}

/* add a stringlist in the list */
int vlad_stringlistlist::add(vlad_stringlist *a_slist)
{
  return vlad_list::add((vlad_list_item *) a_slist);
}

/* get the ith stringlist in the list */
int vlad_stringlistlist::get(unsigned int a_index, vlad_stringlist **a_slist)
{
  int retval;
  vlad_list_item *item;

  if (a_slist == NULL)
    return VLAD_NULLPTR;

  if ((retval = vlad_list::get(a_index, &item)) != VLAD_OK)
    return retval;

  if ((*a_slist = dynamic_cast<vlad_stringlist *>(item)) == NULL)
    return VLAD_FAILURE;

  return VLAD_OK;
}

/* called by vlad_list::traverse() */
int vlad_stringlistlist::trav(vlad_list_item *a_item, vlad_list_trav *a_trav)
{
  vlad_stringlist *sitem;
  vlad_stringlistlist_trav *strav;

  if ((sitem = dynamic_cast<vlad_stringlist *>(a_item)) == NULL)
    return VLAD_NULLPTR;

  if ((strav = dynamic_cast<vlad_stringlistlist_trav *>(a_trav)) == NULL)
    return VLAD_NULLPTR;

  return strav->trav(sitem);
}
