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

#ifndef __VLAD_REL_H
#define __VLAD_REL_H

#include <tribe/rel.h>

#include <vlad/list.h>
#include <vlad/stringlist.h>
#include <vlad/varlist.h>
#include <vlad/symtab.h>

class vlad_rel : public vlad_list_item
{
  public :

    vlad_rel();
    virtual ~vlad_rel();

    /* returns true if rels are equal */
    bool cmp(vlad_list_item *a_item);

    /* (re)initialise the relation */
    int init(const char *a_int1, const char *a_int2, unsigned int a_rs);

    /* gives the values */
    int get(char **a_int1, char **a_int2, unsigned int *a_rs);

    /* adds the given relation to the relset */
    int add(unsigned int a_rel);

    /* creates a new instance of this relation */
    int copy(vlad_rel **a_rel);

    /* ensure that each interval is in symtab and each var is in varlist */
    int verify(vlad_symtab *a_stab, vlad_varlist *a_vlist);

    /* replaces vars in a_vlist with idents in a_ilist. gives a new rel */
    int replace(vlad_varlist *a_vlist,
                vlad_stringlist *a_ilist,
                vlad_rel **a_rel);

    /* gives a list of vars occurring in the rel. assumes list is init'ed */
    int varlist(vlad_varlist *a_vlist);

    /* returns VLAD_OK if none of the interval identifiers are variables */
    int is_ground();

  private :

    char *m_int1;
    char *m_int2;
    unsigned char m_rs;
    bool m_init;
} ;

#endif
