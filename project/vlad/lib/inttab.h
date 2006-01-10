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

#ifndef __VLAD_INTTAB_H
#define __VLAD_INTTAB_H

#include <vlad/list.h>

class vlad_inttab_item : public vlad_list_item
{
  public :

    vlad_inttab_item();
    ~vlad_inttab_item();

    /* compare item with this */
    bool cmp(vlad_list_item *a_item);

    /* return an exact duplicate of this item */
    vlad_list_item *copy();

    /* initialise */
    int init(const char *a_name, unsigned int a_ep1, unsigned int a_ep2);

    /* get values */
    int get(char **a_name, unsigned int *a_ep1, unsigned int *a_ep2);

  private :

    char *m_name;
    unsigned int m_ep1;
    unsigned int m_ep2;
    bool m_init;
} ;

class vlad_inttab : public vlad_list
{
  public :

    vlad_inttab();
    ~vlad_inttab();

   /* add an interval */
   int add(const char *a_name, unsigned int a_ep1, unsigned int a_ep2);

   /* gives the i'th item in the table */
   int get(unsigned int a_index,
           char **a_name,
           unsigned int *a_ep1,
           unsigned int *a_ep2);

   /* gives the endpoints of the interval of the given name */
   int get(char *a_name, unsigned int *a_ep1, unsigned int *a_ep2);
} ;

#endif
