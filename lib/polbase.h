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

#ifndef __VLAD_POLBASE_H
#define __VLAD_POLBASE_H

#include <cstdio>

#include <vlad/vlad.h>
#include <vlad/atom.h>
#include <vlad/symtab.h>
#include <vlad/expression.h>
#include <vlad/consttab.h>
#include <vlad/updatetab.h>
#include <vlad/seqtab.h>
#include <vlad/mapper.h>
#ifdef VLAD_SMODELS
  #include <vlad/smwrap.h>
#endif

class vlad_polbase {
  public :
    vlad_polbase();
    ~vlad_polbase();
    /* (re)init the policy base */
    int init();
    /* add an entity in the symbol table */
    int add_symtab(const char *a_name, unsigned char a_type);
    /* add a fact into the initial state table */
    int add_inittab(vlad_fact *a_fact);
    /* add a constrant into the constraints table */
    int add_consttab(vlad_expression *a_exp,
                     vlad_expression *a_cond,
                     vlad_expression *a_ncond);
    /* add an update declaration in the update table */
    int add_updatetab(const char *a_name,
                      vlad_varlist *a_vlist,
                      vlad_expression *a_precond,
                      vlad_expression *a_postcond);
    /* add an update reference to the sequence table */
    int add_seqtab(vlad_updateref *a_uref);
    /* close symbol table */
    int close_symtab();
    /* after this is called, no further calls to add_inittab(), add_consttab()
     * or add_updatetab() is allowed */
    int close_polbase();
    /* delete an update reference from the sequence table */
    int del_seqtab(unsigned int a_index);
    /* gives an array of identifiers of the given type */
    int get_symtab(unsigned char a_type,
                   char ***a_array,
                   unsigned int *a_size);
    /* gives the index'th entry in the sequence table */
    int get_seqtab(unsigned int a_index,
                   char **a_name,
                   vlad_stringlist **a_ilist);
    /* gives the index'th entry in the update table */
    int get_updatetab(unsigned int a_index,
                      char **a_name,
                      vlad_varlist **a_vlist,
                      vlad_expression **a_precond,
                      vlad_expression **a_postcond);
    /* returns the length of the sequence table */
    unsigned int length_seqtab();
    /* returns the length of the update table */
    unsigned int length_updatetab();
    /* checks whether name of the given type is in symtab */
    int check_symtab(const char *a_name, unsigned char a_type);
    /* enumerate the sequences in the sequence table, output to file */
    int list_seqtab(FILE *a_file);
    /* generate the rules necessary to evaluate queries */
    int compute_generate(FILE *a_file);
    /* generate the query */
    int query_generate(vlad_expression *a_exp, FILE *a_file);
#ifdef VLAD_SMODELS
    /* prepares the kb for queries */
    int compute_evaluate();
    /* use smwrap class to evaluate a query */
    int query_evaluate(vlad_expression *a_exp, unsigned char *a_res);
#endif
  private :
    /*
     * m_stage: 0 = uninit, 1 = init, 2 = symtab closed, 3 = polbase closed,
     *          4 = after compute()
     */
    unsigned char m_stage;
    /* the symbol table */
    vlad_symtab *m_stable;
    /* the initial expression table */
    vlad_expression *m_itable;
    /* the constraints table */
    vlad_consttab *m_ctable;
    /* the update table */
    vlad_updatetab *m_utable;
    /* the sequence table */
    vlad_seqtab *m_setable;
    /* the mapper */
    vlad_mapper *m_mapper;
#ifdef VLAD_SMODELS
    vlad_smwrap *m_smobject;
#endif
#ifdef VLAD_SMODELS
    /* checks whether the given fact is true, false or unknown */
    int evaluate_fact(unsigned int a_fact, unsigned char *a_res);
#endif
} ;
#endif
