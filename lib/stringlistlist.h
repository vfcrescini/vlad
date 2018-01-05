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

#include <vlad/list.h>
#include <vlad/stringlist.h>

#ifndef __VLAD_STRINGLISTLIST_H
#define __VLAD_STRINGLISTLIST_H

/* class for traversing */
class vlad_stringlistlist_trav : public vlad_list_trav
{
  public :

    virtual int trav(vlad_stringlist *a_slist) = 0;
} ;

class vlad_stringlistlist : public vlad_list
{
  public :

    vlad_stringlistlist();
    ~vlad_stringlistlist();

    /* add a stringlist in the list */
    int add(vlad_stringlist *a_slist);

    /* get the ith stringlist in the list */
    int get(unsigned int a_index, vlad_stringlist **a_slist);

  private :

    /* called by vlad_list::traverse() */
    int trav(vlad_list_item *a_item, vlad_list_trav *a_trav);
} ;

#endif
