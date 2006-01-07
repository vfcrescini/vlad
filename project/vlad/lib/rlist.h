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

#ifndef __VLAD_RLIST_H
#define __VLAD_RLIST_H

#include <vlad/rel.h>
#include <vlad/list.h>
#include <vlad/stringlist.h>
#include <vlad/varlist.h>

class vlad_rlist : public vlad_list
{
  public :

    vlad_rlist();
    virtual ~vlad_rlist();

    /* adds a relation to the list */
    int add(const char *a_int1, const char *a_int2, unsigned int a_rel);

    /* gives a reference to the ith relation */
    int get(unsigned int a_i, vlad_rel **a_rel);

    /* replace vars in vlist to ident in ilist. create a new rlist */
    int replace(vlad_varlist *a_vlist,
                vlad_stringlist *a_ilist,
                vlad_rlist **a_rlist);

    /* returns VLAD_OK if the list contains no variables */
    int is_ground();
} ;

#endif
