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

#ifndef __VLAD_EXPRESSION_H
#define __VLAD_EXPRESSION_H

#include <vlad/list.h>
#include <vlad/stringlist.h>
#include <vlad/symtab.h>
#include <vlad/fact.h>

/* a list of facts. no checking. */

class expression : public list
{
  public :
    expression();
    ~expression();
    /* add pre-malloc'ed factg */
    int add(fact *a_fact);
    /* return true if fact is in the expression */
    int find(fact *a_fact);
    /* give i'th fact */
    int get(unsigned int a_index, fact **a_fact);
    /* replace occurences of var with ident, creates a new expression */
    int replace(const char *a_var, const char *a_ident, expression **a_exp);
    /* replace vars in vlist to entity in ilist. create a new expression */
    int replace(stringlist *a_vlist, stringlist *a_ilist, expression **a_exp);
    /* gives a list of vars occuring in the expr. assumes list is init'ed */
    int varlist(stringlist **a_list);
    /*
     * verify if fact is valid, if vlist is non-null, check if variables
     * occur within this list. if gnd_flag is true, ensure that the fact
     * is ground.
     */
    int verify(symtab *a_stab, stringlist *a_vlist, bool a_gndflag);
#ifdef VLAD_DEBUG
    /* assumimg s has enough memory allocation */
    void print(char *a_str);
#endif
} ;

#endif
