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

#ifndef __VLAD_RLIST_H
#define __VLAD_RLIST_H

#include <vlad/rel.h>
#include <vlad/list.h>
#include <vlad/stringlist.h>
#include <vlad/varlist.h>
#include <vlad/symtab.h>

class vlad_rlist : public vlad_list
{
  public :

    vlad_rlist();
    virtual ~vlad_rlist();

    /* adds a relation to the list */
    int add(vlad_rel *a_rel);

    /* gives a reference to the ith relation */
    int get(unsigned int a_i, vlad_rel **a_rel);

    /* make a copy */
    int copy(vlad_rlist **a_rlist);

    /* verify then copy */
    int vcopy(vlad_symtab *a_stab,
              vlad_varlist *a_vlist,
              vlad_rlist **a_rlist);

    /* replace vars in vlist to ident in ilist. create a new rlist */
    int replace(vlad_varlist *a_vlist,
                vlad_stringlist *a_ilist,
                vlad_rlist **a_rlist);

    /* replace then verify */
    int vreplace(vlad_symtab *a_stab,
                 vlad_varlist *a_vlist,
                 vlad_stringlist *a_ilist,
                 vlad_rlist **a_rlist);

    /* gives a list of vars occurring in the rlist. assumes list is init'ed */
    int varlist(vlad_varlist *a_vlist);

    /* as above, but verify first */
    int vvarlist(vlad_symtab *a_stab, vlad_varlist *a_vlist);

    /* ensure that each interval is in symtab and each var is in varlist */
    int verify(vlad_symtab *a_stab, vlad_varlist *a_vlist);

    /* returns VLAD_OK if the list contains no variables */
    int is_ground();

    /* returns VLAD_OK if all the rels contain at least one variable */
    int is_allnonground();
} ;

#endif
