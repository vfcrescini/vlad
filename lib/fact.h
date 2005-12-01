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

#ifndef __VLAD_FACT_H
#define __VLAD_FACT_H

#include <vlad/list.h>
#include <vlad/atom.h>
#include <vlad/stringlist.h>
#include <vlad/varlist.h>
#include <vlad/symtab.h>

/* structure to hold facts. no checking done here, values are just stored */

typedef struct
{
  char *subject;
  char *access;
  char *object;
} vlad_holds_atom;

typedef struct
{
  char *element;
  char *group;
} vlad_member_atom;

typedef struct
{
  char *group1;
  char *group2;
} vlad_subset_atom;

class vlad_fact : public vlad_list_item
{
  public :
    vlad_fact();
    ~vlad_fact();
    bool cmp(vlad_list_item *a_item);
    /* get attributes from fact */
    int get(char **a_ent1,
            char **a_ent2,
            char **a_ent3,
            unsigned char *a_type,
            bool *a_truth);
    int get_holds(char **a_sub, char **a_acc, char **a_obj);
    int get_member(char **a_elt, char **a_grp);
    int get_subset(char **a_grp1, char **a_grp2);
    /*initialise facts */
    int init(const char *a_ent1,
             const char *a_ent2,
             const char *a_ent3,
             unsigned char a_type,
             bool a_truth);
    int init_holds(const char *a_sub,
                   const char *a_acc,
                   const char *a_obj,
                   bool a_truth);
    int init_member(const char *a_elt,
                    const char *a_grp,
                    bool a_truth);
    int init_subset(const char *a_grp1,
                    const char *a_grp2,
                    bool a_truth);
    /* create a new instance of this fact */
    int copy(vlad_fact **a_fact);
    /* verify and copy */
    int vcopy(vlad_symtab *a_stab,
              vlad_varlist *a_vlist,
              bool a_gndflag,
              vlad_fact **a_fact);
    /*
     * verify if fact is valid, if vlist is non-null, check if variables
     * occur within this list. if gnd_flag is true, ensure that the fact
     * is ground.
     */
    int verify(vlad_symtab *a_stab, vlad_varlist *a_vlist, bool a_gndflag);
    /* replaces instances of var with ident, gives a new fact */
    int replace(const char *a_var, const char *a_ident, vlad_fact **a_fact);
    /* replaces vars in vlist with entities in ilist. gives a new fact */
    int replace(vlad_varlist *a_vlist,
                vlad_stringlist *a_ilist,
                vlad_fact **a_fact);
    /* reverses the truth value */
    void negate();
    /* gives the type of the fact */
    int type(unsigned char *a_type);
    /* gives the truth value of the fact */
    int truth(bool *a_truth);
    /* gives a list of vars occuring in the fact. assumes list is init'ed */
    int varlist(vlad_varlist **a_list);
#ifdef VLAD_DEBUG
    /* assuming a_str has enough memory allocation */
    void print(char *a_str);
#endif
  private :
    int reset();
    unsigned char m_type;
    bool m_truth;
    union {
      vlad_holds_atom m_holds;
      vlad_member_atom m_member;
      vlad_subset_atom m_subset;
    } ;
    bool m_init;
} ;

#endif
