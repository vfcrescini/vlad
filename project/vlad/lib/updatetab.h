/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004 University of Western Sydney
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

#ifndef __VLAD_UPDATETAB_H
#define __VLAD_UPDATETAB_H

#include <vlad/list.h>
#include <vlad/expression.h>
#include <vlad/stringlist.h>

class updatedef : public list_item
{
  public :
    updatedef();
    ~updatedef();
    bool cmp(list_item *a_item);
    int get(char **a_name,
            stringlist **a_vlist,
            expression **a_precond,
            expression **a_postcond);
    int init(const char *a_name,
             stringlist *a_vlist,
             expression *a_precond,
             expression *a_postcond);
  private :
    char *m_name;
    stringlist *m_vlist;
    expression *m_precond;
    expression *m_postcond;
    bool m_init;
} ;

class updatetab : public list
{
  public :
    updatetab();
    ~updatetab();
    int add(const char *a_name,
            stringlist *a_vlist,
            expression *a_precond,
            expression *a_postcond);
    /* get update by name */
    int get(const char *a_name,
             stringlist **a_vlist,
             expression **a_precond,
             expression **a_postcond);
    /* get update by index */
    int get(unsigned int a_index,
            char **a_name,
            stringlist **a_vlist,
            expression **a_precond,
            expression **a_postcond);
    /* replace variables with identifiers in ilist, then get pr and pp */
    int replace(const char *a_name,
                stringlist *a_ilist,
                expression **a_precond,
                expression **a_postcond);
} ;

#endif
