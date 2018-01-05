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

#ifndef __VLAD_CONSTTAB_H
#define __VLAD_CONSTTAB_H

#include <vlad/list.h>
#include <vlad/varlist.h>
#include <vlad/expression.h>

/* consttab list is simply a list of expression/condition pairs */

class vlad_constraint : public vlad_list_item
{
  public :

    vlad_constraint();
    ~vlad_constraint();

    /* return true if the 2 are equal */
    bool cmp(vlad_list_item *a_item);

    /* initialise with the given values */
    int init(vlad_expression *a_exp,
             vlad_expression *a_cond,
             vlad_expression *a_ncond);

    /* gives a reference to the values */
    int get(vlad_expression **a_exp,
            vlad_expression **a_cond,
            vlad_expression **a_ncond);

  private :

    bool m_init;
    vlad_expression *m_exp;
    vlad_expression *m_cond;
    vlad_expression *m_ncond;
} ;

class vlad_consttab : public vlad_list
{
  public :

    vlad_consttab();
    ~vlad_consttab();

    /* store in list */
    int add(vlad_expression *a_exp,
            vlad_expression *a_cond,
            vlad_expression *a_ncond);

    /* get reference of the constraint matching the index */
    int get(unsigned int a_index,
            vlad_expression **a_exp,
            vlad_expression **a_cond,
            vlad_expression **a_ncond);
} ;
#endif
