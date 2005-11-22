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
#include <vlad/stringlist.h>
#include <vlad/expression.h>

/* consttab list is simply a list of expression/condition pairs */

class constraint : public list_item
{
  public :
    constraint();
    ~constraint();
    bool cmp(list_item *a_item);
    int init(expression *a_exp, expression *a_cond, expression *a_ncond);
    int get(expression **a_exp, expression **a_cond, expression **a_ncond);
    /* replaces occurences of var with ident. creates a new constraint */
    int replace(const char *a_var, const char *a_ident, constraint **a_constr);
    /* gives a list of vars occuring in the constr, creats a new constr */
    int varlist(stringlist **a_list);
  private :
    bool m_init;
    expression *m_exp;
    expression *m_cond;
    expression *m_ncond;
} ;

class consttab : public list
{
  public :
    consttab();
    ~consttab();
    int add(expression *a_exp, expression *a_cond, expression *a_ncond);
    int get(unsigned int a_index,
            expression **a_exp,
            expression **a_cond,
            expression **a_ncond);
} ;
#endif
