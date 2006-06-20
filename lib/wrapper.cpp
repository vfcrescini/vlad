/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004, 2005 University of Western Sydney
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

#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <new>

#include <stdio.h>

#include <vlad/vlad.h>
#include <vlad/wrapper.h>
#include <vlad/kb.h>

/* static cast X into tyoe Y */
#define VLAD_WRAPPER_CAST(X,Y)                                   \
  (((X) == NULL) ? NULL : static_cast<Y>(X))

/* super macro that creates an instance of class X and assign to Y (void **)
 * evaluate to VLAD_OK, VLAD_MALLOCFAILED or VLAD_NULLPTR */
#define VLAD_WRAPPER_CREATE(X,Y)                                 \
  (((Y) == NULL) ?                                               \
    VLAD_NULLPTR :                                               \
    (((*(Y) = VLAD_WRAPPER_CAST(VLAD_NEW(X), void *)) == NULL) ? \
      VLAD_MALLOCFAILED :                                        \
      VLAD_OK))

/* create a kb */
VLAD_EXTERN int vlad_kb_create(void **a_kb)
{
  return VLAD_WRAPPER_CREATE(kb, a_kb);
}

/* destroy a kb */
VLAD_EXTERN int vlad_kb_destroy(void *a_kb)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = VLAD_WRAPPER_CAST(a_kb, kb *)) == NULL)
    return VLAD_INVALIDINPUT;

  delete tmp_kb;

  return VLAD_OK;
}

/* (re)init a kb */
VLAD_EXTERN int vlad_kb_init(void *a_kb)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = VLAD_WRAPPER_CAST(a_kb, kb *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_kb->init();
}

/* close symbol table */
VLAD_EXTERN int vlad_kb_close_symtab(void *a_kb)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = VLAD_WRAPPER_CAST(a_kb, kb *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_kb->close_symtab();
}

/* close kb table */
VLAD_EXTERN int vlad_kb_close_kb(void *a_kb)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = VLAD_WRAPPER_CAST(a_kb, kb *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_kb->close_kb();
}

/* returns the length of the symtab */
VLAD_EXTERN unsigned int vlad_kb_length_symtab(void *a_kb)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return 0;

  if ((tmp_kb = VLAD_WRAPPER_CAST(a_kb, kb *)) == NULL)
    return 0;

  return tmp_kb->length_symtab();
}

/* gets the i'th identifier from symtab */
VLAD_EXTERN int vlad_kb_get_symtab(void *a_kb,
                                   unsigned int a_index,
                                   char **a_str)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = VLAD_WRAPPER_CAST(a_kb, kb *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_kb->get_symtab(a_index, a_str);
}

/* gives an array of identifiers of type t */
VLAD_EXTERN int vlad_kb_get_array_symtab(void *a_kb,
                                         unsigned char a_type,
                                         char ***a_array,
                                         unsigned int *a_size)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = VLAD_WRAPPER_CAST(a_kb, kb *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_kb->get_symtab(a_type, a_array, a_size);
}

/* check if the identifier of the given type is in the symtab */
VLAD_EXTERN int vlad_kb_check_symtab(void *a_kb,
                                     const char *a_n,
                                     unsigned char a_t)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = VLAD_WRAPPER_CAST(a_kb, kb *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_kb->check_symtab(a_n, a_t);
}

/* register an identifier in the kb */
VLAD_EXTERN int vlad_kb_add_symtab(void *a_kb,
                                   const char *a_n,
                                   unsigned char a_t)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = VLAD_WRAPPER_CAST(a_kb, kb *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_kb->add_symtab(a_n, a_t);
}

/* add an atom into the initial state table */
VLAD_EXTERN int vlad_kb_add_inittab(void *a_kb, void *a_atm)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = VLAD_WRAPPER_CAST(a_kb, kb *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_kb->add_inittab(VLAD_WRAPPER_CAST(a_atm, atom *));
}

/* add an expression into the constraints table */
VLAD_EXTERN int vlad_kb_add_consttab(void *a_kb,
                                     void *a_e,
                                     void *a_c,
                                     void *a_n)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = VLAD_WRAPPER_CAST(a_kb, kb *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_kb->add_consttab(VLAD_WRAPPER_CAST(a_e, expression *),
                              VLAD_WRAPPER_CAST(a_c, expression *),
                              VLAD_WRAPPER_CAST(a_n, expression *));
}

/* add a transformation declaration in the trans table */
VLAD_EXTERN int vlad_kb_add_transtab(void *a_kb,
                                     const char *a_n,
                                     void *a_v,
                                     void *a_pr,
                                     void *a_po)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = VLAD_WRAPPER_CAST(a_kb, kb *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_kb->add_transtab(a_n,
                              VLAD_WRAPPER_CAST(a_v, stringlist *),
                              VLAD_WRAPPER_CAST(a_pr, expression *),
                              VLAD_WRAPPER_CAST(a_po, expression *));
}

/* returns the length of the transformation table */
VLAD_EXTERN unsigned int vlad_kb_length_transtab(void *a_kb)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return 0;

  if ((tmp_kb = VLAD_WRAPPER_CAST(a_kb, kb *)) == NULL)
    return 0;

  return tmp_kb->length_transtab();
}

/* gives the i'th entry in the transformation table */
VLAD_EXTERN int vlad_kb_get_transtab(void *a_kb,
                                     unsigned int a_index,
                                     char **a_n,
                                     void **a_v,
                                     void **a_pr,
                                     void **a_po)
{
  int retval;
  kb *tmp_kb = NULL;
  stringlist *tmp_s;
  expression *tmp_pr;
  expression *tmp_po;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = VLAD_WRAPPER_CAST(a_kb, kb *)) == NULL)
    return VLAD_INVALIDINPUT;

  retval = tmp_kb->get_transtab(a_index,
                                a_n,
                                &tmp_s,
                                &tmp_pr,
                                &tmp_po);

  if (retval != VLAD_OK)
    return retval;

  *a_v  = VLAD_WRAPPER_CAST(tmp_s, void *);
  *a_pr = VLAD_WRAPPER_CAST(tmp_pr, void *);
  *a_po = VLAD_WRAPPER_CAST(tmp_po, void *);

  return VLAD_OK;
}

/* add a transformation reference to the sequence table */
VLAD_EXTERN int vlad_kb_add_seqtab(void *a_kb, void *a_tr)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = VLAD_WRAPPER_CAST(a_kb, kb *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_kb->add_seqtab(VLAD_WRAPPER_CAST(a_tr, transref *));
}

/* delete a transformation reference from the sequence table */
VLAD_EXTERN int vlad_kb_del_seqtab(void *a_kb, unsigned int a_i)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = VLAD_WRAPPER_CAST(a_kb, kb *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_kb->del_seqtab(a_i);
}

/* enumerate the sequences in the sequence table, output to f */
VLAD_EXTERN int vlad_kb_list_seqtab(void *a_kb, FILE *a_fs)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = VLAD_WRAPPER_CAST(a_kb, kb *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_kb->list_seqtab(a_fs);
}

/* returns the length of the sequence table */
VLAD_EXTERN unsigned int vlad_kb_length_seqtab(void *a_kb)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return 0;

  if ((tmp_kb = VLAD_WRAPPER_CAST(a_kb, kb *)) == NULL)
    return 0;

  return tmp_kb->length_seqtab();
}

/* gives the i'th entry in the sequence table */
VLAD_EXTERN int vlad_kb_get_seqtab(void *a_kb,
                                   unsigned int a_index,
                                   char **a_n,
                                   void **a_il)
{
  int retval;
  kb *tmp_kb = NULL;
  stringlist *tmp_s;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = VLAD_WRAPPER_CAST(a_kb, kb *)) == NULL)
    return VLAD_INVALIDINPUT;

  retval = tmp_kb->get_seqtab(a_index, a_n, &tmp_s);

  if (retval != VLAD_OK)
    return retval;

  *a_il  = VLAD_WRAPPER_CAST(tmp_s, void *);

  return VLAD_OK;
}

/* generate the rules necessary to evaluate queries */
VLAD_EXTERN int vlad_kb_compute_generate(void *a_kb, FILE *a_fs)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = VLAD_WRAPPER_CAST(a_kb, kb *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_kb->compute_generate(a_fs);
}

/* generate the query */
VLAD_EXTERN int vlad_kb_query_generate(void *a_kb, void *a_exp, FILE *a_fs)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = VLAD_WRAPPER_CAST(a_kb, kb *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_kb->query_generate(VLAD_WRAPPER_CAST(a_exp, expression *), a_fs);
}

#ifdef VLAD_SMODELS
/* prepares the kb for queries */
VLAD_EXTERN int vlad_kb_compute_evaluate(void *a_kb)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = VLAD_WRAPPER_CAST(a_kb, kb *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_kb->compute_evaluate();
}

/* use smwrap class to evaluate a query */
VLAD_EXTERN int vlad_kb_query_evaluate(void *a_kb,
                                       void *a_exp,
                                       unsigned char *a_res)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = VLAD_WRAPPER_CAST(a_kb, kb *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_kb->query_evaluate(VLAD_WRAPPER_CAST(a_exp, expression *), a_res);
}
#endif

/* create a stringlist */
VLAD_EXTERN int vlad_strlist_create(void **a_slist)
{
  return VLAD_WRAPPER_CREATE(stringlist, a_slist);
}

/* destroy a stringlist */
VLAD_EXTERN int vlad_strlist_destroy(void *a_slist)
{
  stringlist *tmp_slist = NULL;

  if (a_slist == NULL)
    return VLAD_NULLPTR;

  if ((tmp_slist = VLAD_WRAPPER_CAST(a_slist, stringlist *)) == NULL)
    return VLAD_INVALIDINPUT;

  delete tmp_slist;

  return VLAD_OK;
}

/* add a string into stringlist */
VLAD_EXTERN int vlad_strlist_add(void *a_slist, const char *a_s)
{
  stringlist *tmp_slist = NULL;

  if (a_slist == NULL)
    return VLAD_NULLPTR;

  if ((tmp_slist = VLAD_WRAPPER_CAST(a_slist, stringlist *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_slist->add(a_s);
}

/* get the i'th string from list */
VLAD_EXTERN int vlad_strlist_get(void *a_slist, unsigned int a_i, char **a_s)
{
  stringlist *tmp_slist = NULL;

  if (a_slist == NULL)
    return VLAD_NULLPTR;

  if ((tmp_slist = VLAD_WRAPPER_CAST(a_slist, stringlist *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_slist->get(a_i, a_s);
}

/* create an atom */
VLAD_EXTERN int vlad_atom_create(void **a_atm)
{
  return VLAD_WRAPPER_CREATE(atom, a_atm);
}

/* destroy an atom */
VLAD_EXTERN int vlad_atom_destroy(void *a_atm)
{
  atom *tmp_atm = NULL;

  if (a_atm == NULL)
    return VLAD_NULLPTR;

  if ((tmp_atm = VLAD_WRAPPER_CAST(a_atm, atom *)) == NULL)
    return VLAD_INVALIDINPUT;

  delete tmp_atm;

  return VLAD_OK;
}

/* negate an atom */
VLAD_EXTERN int vlad_atom_negate(void *a_atm)
{
  atom *tmp_atm = NULL;

  if (a_atm == NULL)
    return VLAD_NULLPTR;

  if ((tmp_atm = VLAD_WRAPPER_CAST(a_atm, atom *)) == NULL)
    return VLAD_INVALIDINPUT;

  tmp_atm->negate();

  return VLAD_OK;
}

/* initialise atoms */
VLAD_EXTERN int vlad_atom_init_holds(void *a_atm,
                                     const char *a_s,
                                     const char *a_a,
                                     const char *a_o,
                                     int a_t)
{
  atom *tmp_atm = NULL;

  if (a_atm == NULL)
    return VLAD_NULLPTR;

  if ((tmp_atm = VLAD_WRAPPER_CAST(a_atm, atom *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_atm->init_holds(a_s, a_a, a_o, (bool) a_t);
}

VLAD_EXTERN int vlad_atom_init_member(void *a_atm,
                                      const char *a_e,
                                      const char *a_g,
                                      int a_t)
{
  atom *tmp_atm = NULL;

  if (a_atm == NULL)
    return VLAD_NULLPTR;

  if ((tmp_atm = VLAD_WRAPPER_CAST(a_atm, atom *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_atm->init_member(a_e, a_g, (bool) a_t);
}

VLAD_EXTERN int vlad_atom_init_subset(void *a_atm,
                                      const char *a_g1,
                                      const char *a_g2,
                                      int a_t)
{
  atom *tmp_atm = NULL;

  if (a_atm == NULL)
    return VLAD_NULLPTR;

  if ((tmp_atm = VLAD_WRAPPER_CAST(a_atm, atom *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_atm->init_subset(a_g1, a_g2, (bool) a_t);
}

/* create an expression */
VLAD_EXTERN int vlad_exp_create(void **a_exp)
{
  return VLAD_WRAPPER_CREATE(expression, a_exp);
}

/* destroy an expression */
VLAD_EXTERN int vlad_exp_destroy(void *a_exp)
{
  expression *tmp_exp = NULL;

  if (a_exp == NULL)
    return VLAD_NULLPTR;

  if ((tmp_exp = VLAD_WRAPPER_CAST(a_exp, expression *)) == NULL)
    return VLAD_INVALIDINPUT;

  delete tmp_exp;

  return VLAD_OK;
}

/* add an atom into expression */
VLAD_EXTERN int vlad_exp_add(void *a_exp, void *a_atm)
{
  expression *tmp_exp = NULL;

  if (a_exp == NULL)
    return VLAD_NULLPTR;

  if ((tmp_exp = VLAD_WRAPPER_CAST(a_exp, expression *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_exp->add(VLAD_WRAPPER_CAST(a_atm, atom *));
}

/* gives the i'th atom */
VLAD_EXTERN int vlad_exp_get(void *a_exp, unsigned int a_i, void **a_atm)
{
  int retval;
  expression *tmp_exp = NULL;
  atom *tmp_atom = NULL;

  if (a_exp == NULL || a_atm == NULL)
    return VLAD_NULLPTR;

  if ((tmp_exp = VLAD_WRAPPER_CAST(a_exp, expression *)) == NULL)
    return VLAD_INVALIDINPUT;

  if ((retval = tmp_exp->get(a_i, &tmp_atom)) != VLAD_OK)
    return retval;

  *a_atm = (void *) tmp_atom;

  return VLAD_OK;
}

/* create a trans ref */
VLAD_EXTERN int vlad_tref_create(void **a_tref)
{
  return VLAD_WRAPPER_CREATE(transref, a_tref);
}

/* destroy a tran sref */
VLAD_EXTERN int vlad_tref_destroy(void *a_tref)
{
  transref *tmp_tref = NULL;

  if (a_tref == NULL)
    return VLAD_NULLPTR;

  if ((tmp_tref = VLAD_WRAPPER_CAST(a_tref, transref *)) == NULL)
    return VLAD_INVALIDINPUT;

  delete tmp_tref;

  return VLAD_OK;
}

/* initialise a trans ref */
VLAD_EXTERN int vlad_tref_init(void *a_tref, const char *a_n, void *a_slist)
{
  transref *tmp_tref = NULL;
  char *tmp_name = NULL;

  if (a_tref == NULL || a_n == NULL)
    return VLAD_NULLPTR;

  if ((tmp_tref = VLAD_WRAPPER_CAST(a_tref, transref *)) == NULL)
    return VLAD_INVALIDINPUT;

  if ((tmp_name = VLAD_STRING_MALLOC(a_n)) == NULL)
    return VLAD_MALLOCFAILED;

  strcpy(tmp_name, a_n);

  return tmp_tref->init(tmp_name, VLAD_WRAPPER_CAST(a_slist, stringlist *));
}

/* get the length of the list */
VLAD_EXTERN int vlad_list_length(void *a_list)
{
  list *tmp_list;

  if (a_list == NULL)
    return 0;

  if ((tmp_list = VLAD_WRAPPER_CAST(a_list, list *)) == NULL)
    return 0;

  return tmp_list->length();
}
