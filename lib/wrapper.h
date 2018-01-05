/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004, 2005, 2006 University of Western Sydney
 * by Vino Fernando Crescini <jcrescin@scm.uws.edu.au>
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

#ifndef __VLAD_WRAPPER_H
#define __VLAD_WRAPPER_H

#include <vlad/vlad.h>

#ifdef __cplusplus
#include <cstdio>
#define VLAD_EXTERN extern "C"
#else
#include <stdio.h>
#define VLAD_EXTERN extern
#endif

/* create a polbase */
VLAD_EXTERN int vlad_polbase_create(void **a_polbase);

/* destroy a polbase */
VLAD_EXTERN int vlad_polbase_destroy(void *a_polbase);

/* (re)init a polbase */
VLAD_EXTERN int vlad_polbase_init(void *a_polbase);

/* close symbol table */
VLAD_EXTERN int vlad_polbase_close_symtab(void *a_polbase);

/* close polbase table */
VLAD_EXTERN int vlad_polbase_close_polbase(void *a_polbase);

/* gets the index'th identifier from symtab */
VLAD_EXTERN int vlad_polbase_get_symtab(void *a_polbase,
                                        unsigned int a_index,
                                        char **a_str);
/* gives an array of identifiers of type t */
VLAD_EXTERN int vlad_polbase_get_array_symtab(void *a_polbase,
                                              unsigned char a_type,
                                              char ***a_array,
                                              unsigned int *a_size);

/* check if the identifier of the given type is in the symtab */
VLAD_EXTERN int vlad_polbase_check_symtab(void *a_polbase,
                                         const char *a_name,
                                         unsigned char a_type);

/* register an identifier in the polbase */
VLAD_EXTERN int vlad_polbase_add_symtab(void *a_polbase,
                                        const char *a_name,
                                        unsigned char a_type);

/* add a fact into the initial state table */
VLAD_EXTERN int vlad_polbase_add_inittab(void *a_polbase, void *a_fact);

/* add an expression into the constraints table */
VLAD_EXTERN int vlad_polbase_add_consttab(void *a_polbase,
                                          void *a_exp,
                                          void *a_cond,
                                          void *a_ncond);

/* add an update declaration in the update table */
VLAD_EXTERN int vlad_polbase_add_updatetab(void *a_polbase,
                                           const char *a_name,
                                           void *a_vlist,
                                           void *a_precond,
                                           void *a_postcond);

/* returns the length of the update table */
VLAD_EXTERN unsigned int vlad_polbase_length_updatetab(void *a_polbase);

/* gives the index'th entry in the update table */
VLAD_EXTERN int vlad_polbase_get_updatetab(void *a_polbase,
                                           unsigned int a_index,
                                           char **a_name,
                                           void **a_vlist,
                                           void **a_precond,
                                           void **a_postcond);

/* add a update reference to the sequence table */
VLAD_EXTERN int vlad_polbase_add_seqtab(void *a_polbase, void *a_uref);

/* delete a update reference from the sequence table */
VLAD_EXTERN int vlad_polbase_del_seqtab(void *a_polbase, unsigned int a_index);

/* returns the length of the sequence table */
VLAD_EXTERN unsigned int vlad_polbase_length_seqtab(void *a_polbase);

/* gives the i'th entry in the sequence table */
VLAD_EXTERN int vlad_polbase_get_seqtab(void *a_polbase,
                                        unsigned int a_index,
                                        char **a_name,
                                        void **a_ilist);
#ifdef VLAD_SMODELS
/* prepares the polbase for queries */
VLAD_EXTERN int vlad_polbase_compute_evaluate(void *a_polbase);

/* use smwrap class to evaluate a query */
VLAD_EXTERN int vlad_polbase_query_evaluate(void *a_polbase,
                                            void *a_exp,
                                            unsigned char *a_res);
#endif

/* create a stringlist */
VLAD_EXTERN int vlad_strlist_create(void **a_slist);

/* destroy a stringlist */
VLAD_EXTERN int vlad_strlist_destroy(void *a_slist);

/* add a string into stringlist */
VLAD_EXTERN int vlad_strlist_add(void *a_slist, const char *a_str);

/* get the index'th string from list */
VLAD_EXTERN int vlad_strlist_get(void *a_slist,
                                 unsigned int a_index,
                                 char **a_str);

/* create an fact */
VLAD_EXTERN int vlad_fact_create(void **a_fact);

/* destroy an fact */
VLAD_EXTERN int vlad_fact_destroy(void *a_fact);

/* negate an fact */
VLAD_EXTERN int vlad_fact_negate(void *a_fact);

/* initialise facts */
VLAD_EXTERN int vlad_fact_init_holds(void *a_fact,
                                     const char *a_s,
                                     const char *a_a,
                                     const char *a_o,
                                     int a_t);

VLAD_EXTERN int vlad_fact_init_member(void *a_fact,
                                      const char *a_e,
                                      const char *a_g,
                                      int a_t);

VLAD_EXTERN int vlad_fact_init_subset(void *a_fact,
                                      const char *a_g1,
                                      const char *a_g2,
                                      int a_t);

/* create an expression */
VLAD_EXTERN int vlad_exp_create(void **a_exp);

/* destroy an expression */
VLAD_EXTERN int vlad_exp_destroy(void *a_exp);

/* add an fact into expression */
VLAD_EXTERN int vlad_exp_add(void *a_exp, void *a_fact);

/* gives the index'th fact */
VLAD_EXTERN int vlad_exp_get(void *a_exp,
                             unsigned int a_i,
                             void **a_fact);

/* create an update ref */
VLAD_EXTERN int vlad_uref_create(void **a_uref);

/* destroy an update aref */
VLAD_EXTERN int vlad_uref_destroy(void *a_uref);

/* initialise a update ref */
VLAD_EXTERN int vlad_uref_init(void *a_uref,
                               const char *a_name,
                               void *a_vlist);

/* get the length of the list */
VLAD_EXTERN int vlad_list_length(void *a_list);
#endif
