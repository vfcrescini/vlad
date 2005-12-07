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

#ifndef __VLAD_EXPRESSION_H
#define __VLAD_EXPRESSION_H

#include <vlad/list.h>
#include <vlad/stringlist.h>
#include <vlad/varlist.h>
#include <vlad/symtab.h>
#include <vlad/fact.h>

/* class for traversing */
class vlad_expression_trav : public vlad_list_trav
{
  public :

    virtual int trav(vlad_fact *a_fact) = 0;
} ;

/* a list of facts. no checking. */
class vlad_expression : public vlad_list
{
  public :

    vlad_expression();
    ~vlad_expression();

    /* add pre-malloc'ed factg */
    int add(vlad_fact *a_fact);

    /* return true if fact is in the expression */
    int find(vlad_fact *a_fact);

    /* give i'th fact */
    int get(unsigned int a_index, vlad_fact **a_fact);

    /* replace occurences of var with ident, creates a new expression */
    int replace(const char *a_var,
                const char *a_ident,
                vlad_expression **a_exp);

    /* replace vars in vlist to entity in ilist. create a new expression */
    int replace(vlad_varlist *a_vlist,
                vlad_stringlist *a_ilist,
                vlad_expression **a_exp);

    /* gives a list of vars occuring in the expr. assumes list is init'ed */
    int varlist(vlad_varlist **a_list);

    /* check if exp is valid, any variables that occur must be in a_vlist */
    int verify(vlad_symtab *a_stab, vlad_varlist *a_vlist);

    /* make a copy */
    int copy(vlad_expression **a_exp);

    /* verify and copy */
    int vcopy(vlad_symtab *a_stab,
              vlad_varlist *a_vlist,
              vlad_expression **a_exp);

#ifdef VLAD_DEBUG
    /* assumimg s has enough memory allocation */
    void print(char *a_str);
#endif

  private :

    /* called by vlad_list::traverse() */
    int trav(vlad_list_item *a_item, vlad_list_trav *a_trav);
} ;

#endif
