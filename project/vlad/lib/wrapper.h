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

/* create a kb */
VLAD_EXTERN int vlad_kb_create(void **a_kb);
/* destroy a kb */
VLAD_EXTERN int vlad_kb_destroy(void *a_kb);
/* (re)init a kb */
VLAD_EXTERN int vlad_kb_init(void *a_kb);
/* close symbol table */
VLAD_EXTERN int vlad_kb_close_symtab(void *a_kb);
/* close kb table */
VLAD_EXTERN int vlad_kb_close_kb(void *a_kb);
/* returns the length of the symtab */
VLAD_EXTERN unsigned int vlad_kb_length_symtab(void *a_kb);
/* gets the i'th identifier from symtab */
VLAD_EXTERN int vlad_kb_get_symtab(void *a_kb,
                                   unsigned int a_index,
                                   char **a_str);
/* gives an array of identifiers of type t */
VLAD_EXTERN int vlad_kb_get_array_symtab(void *a_kb,
                                         unsigned char a_type,
                                         char ***a_array,
                                         unsigned int *a_size);
/* check if the identifier of the given type is in the symtab */
VLAD_EXTERN int vlad_kb_check_symtab(void *a_kb,
                                     const char *a_n,
                                     unsigned char a_t);
/* register an identifier in the kb */
VLAD_EXTERN int vlad_kb_add_symtab(void *a_kb,
                                   const char *a_n,
                                   unsigned char a_t);
/* add an atom into the initial state table */
VLAD_EXTERN int vlad_kb_add_inittab(void *a_kb, void *a_atm);
/* add an expression into the constraints table */
VLAD_EXTERN int vlad_kb_add_consttab(void *a_kb,
                                     void *a_e,
                                     void *a_c,
                                     void *a_n);
/* add a transformation declaration in the trans table */
VLAD_EXTERN int vlad_kb_add_transtab(void *a_kb,
                                     const char *a_n,
                                     void *a_v,
                                     void *a_pr,
                                     void *a_po);
/* returns the length of the transformation table */
VLAD_EXTERN unsigned int vlad_kb_length_transtab(void *a_kb);
/* gives the i'th entry in the transformation table */
VLAD_EXTERN int vlad_kb_get_transtab(void *a_kb,
                                     unsigned int a_index,
                                     char **a_n,
                                     void **a_v,
                                     void **a_pr,
                                     void **a_po);
/* add a transformation reference to the sequence table */
VLAD_EXTERN int vlad_kb_add_seqtab(void *a_kb, void *a_tr);
/* delete a transformation reference from the sequence table */
VLAD_EXTERN int vlad_kb_del_seqtab(void *a_kb, unsigned int a_i);
/* enumerate the sequences in the sequence table, output to f */
VLAD_EXTERN int vlad_kb_list_seqtab(void *a_kb, FILE *a_fs);
/* returns the length of the sequence table */
VLAD_EXTERN unsigned int vlad_kb_length_seqtab(void *a_kb);
/* gives the i'th entry in the sequence table */
VLAD_EXTERN int vlad_kb_get_seqtab(void *a_kb,
                                   unsigned int a_index,
                                   char **a_n,
                                   void **a_il);
/* generate the rules necessary to evaluate queries */
VLAD_EXTERN int vlad_kb_compute_generate(void *a_kb, FILE *a_fs);
/* generate the query */
VLAD_EXTERN int vlad_kb_query_generate(void *a_kb, void *a_exp, FILE *a_fs);
#ifdef VLAD_SMODELS
/* prepares the kb for queries */
VLAD_EXTERN int vlad_kb_compute_evaluate(void *a_kb);
/* use smwrap class to evaluate a query */
VLAD_EXTERN int vlad_kb_query_evaluate(void *a_kb,
                                       void *a_exp,
                                       unsigned char *a_res);
#endif

/* create a stringlist */
VLAD_EXTERN int vlad_strlist_create(void **a_slist);
/* destroy a stringlist */
VLAD_EXTERN int vlad_strlist_destroy(void *a_slist);
/* add a string into stringlist */
VLAD_EXTERN int vlad_strlist_add(void *a_slist, const char *a_s);
/* get the i'th string from list */
VLAD_EXTERN int vlad_strlist_get(void *a_slist, unsigned int a_i, char **a_s);

/* create an atom */
VLAD_EXTERN int vlad_atom_create(void **a_atm);
/* destroy an atom */
VLAD_EXTERN int vlad_atom_destroy(void *a_atm);
/* negate an atom */
VLAD_EXTERN int vlad_atom_negate(void *a_atm);
/* initialise atoms */
VLAD_EXTERN int vlad_atom_init_holds(void *a_atm,
                                     const char *a_s,
                                     const char *a_a,
                                     const char *a_o,
                                     int a_t);
VLAD_EXTERN int vlad_atom_init_member(void *a_atm,
                                      const char *a_e,
                                      const char *a_g,
                                      int a_t);
VLAD_EXTERN int vlad_atom_init_subset(void *a_atm,
                                      const char *a_g1,
                                      const char *a_g2,
                                      int a_t);

/* create an expression */
VLAD_EXTERN int vlad_exp_create(void **a_exp);
/* destroy an expression */
VLAD_EXTERN int vlad_exp_destroy(void *a_exp);
/* add an atom into expression */
VLAD_EXTERN int vlad_exp_add(void *a_exp, void *a_atm);
/* gives the i'th atom */
VLAD_EXTERN int vlad_exp_get(void *a_exp, unsigned int a_i, void **a_atm);

/* create a trans ref */
VLAD_EXTERN int vlad_tref_create(void **a_tref);
/* destroy a tran sref */
VLAD_EXTERN int vlad_tref_destroy(void *a_tref);
/* initialise a trans ref */
VLAD_EXTERN int vlad_tref_init(void *a_tref, const char *a_n, void *a_slist);

/* get the length of the list */
VLAD_EXTERN int vlad_list_length(void *a_list);

#endif
