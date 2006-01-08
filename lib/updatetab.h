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

#ifndef __VLAD_UPDATETAB_H
#define __VLAD_UPDATETAB_H

#include <vlad/list.h>
#include <vlad/expression.h>
#include <vlad/stringlist.h>
#include <vlad/varlist.h>

class vlad_updatedef : public vlad_list_item
{
  public :

    vlad_updatedef();
    ~vlad_updatedef();

    /* compare 2 update defs */
    bool cmp(vlad_list_item *a_item);

    /* get attributes */
    int get(char **a_name,
            vlad_varlist **a_vlist,
            vlad_expression **a_prexp,
            vlad_expression **a_poexp);

    /* init attrbiutes */
    int init(const char *a_name,
             vlad_varlist *a_vlist,
             vlad_expression *a_prexp,
             vlad_expression *a_poexp);
  private :

    char *m_name;
    vlad_varlist *m_vlist;
    vlad_expression *m_prexp;
    vlad_expression *m_poexp;
    bool m_init;
} ;

class vlad_updatetab : public vlad_list
{
  public :

    vlad_updatetab();
    ~vlad_updatetab();

    /* add update def to table */
    int add(const char *a_name,
            vlad_varlist *a_vlist,
            vlad_expression *a_prexp,
            vlad_expression *a_poexp);

    /* get update by name */
    int get(const char *a_name,
            vlad_varlist **a_vlist,
            vlad_expression **a_prexp,
            vlad_expression **a_poexp);

    /* vlist1 contains the specified varlist, vlist2 contains all vars that
     * occur in the expressions but not in vlist1 */
    int get(const char *a_name,
            vlad_varlist **a_vlist1,
            vlad_varlist **a_vlist2,
            vlad_expression **a_prexp,
            vlad_expression **a_poexp);

    /* get update by index */
    int get(unsigned int a_index,
            char **a_name,
            vlad_varlist **a_vlist,
            vlad_expression **a_prexp,
            vlad_expression **a_poexp);
} ;

#endif
