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

#ifndef __VLAD_NUMBERLIST_H
#define __VLAD_NUMBERLIST_H

#include <vlad/list.h>

/* a number */
class vlad_number : public vlad_list_item
{
  public :
    vlad_number();
    ~vlad_number();
    /* compare item with this number */
    bool cmp(vlad_list_item *a_item);
    int init(unsigned int a_num);
    unsigned int get();
  private :
    unsigned int m_number;
} ;

/* a list of numbers */
class vlad_numberlist : public vlad_list
{
  public :
    vlad_numberlist();
    ~vlad_numberlist();
    /* add a number in the list */
    int add(unsigned int a_num);
    /* get the i'th number in the list */
    int get(unsigned int a_index, unsigned int *a_num);
    /* empty the list */
    void purge();
} ;

#endif
