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

#include <cstdlib>
#include <cstddef>
#include <cstring>

#include <vlad/vlad.h>
#include <vlad/mem.h>
#include <vlad/wrapper.h>
#include <vlad/polbase.h>

/* static cast X into tyoe Y */
#define VLAD_WRAPPER_CAST(X,Y)                                   \
  (((X) == NULL) ? NULL : static_cast<Y>(X))

/* super macro that creates an instance of class X and assign to Y (void **)
 * evaluate to VLAD_OK, VLAD_MALLOCFAILED or VLAD_NULLPTR */
#define VLAD_WRAPPER_CREATE(X,Y)                                 \
  (((Y) == NULL) ?                                               \
    VLAD_NULLPTR :                                               \
    (((*(Y) = VLAD_WRAPPER_CAST(VLAD_MEM_NEW(X), void *)) == NULL) ? \
      VLAD_MALLOCFAILED :                                        \
      VLAD_OK))

/* create a polbase */
VLAD_EXTERN int vlad_polbase_create(void **a_polbase)
{
  return VLAD_WRAPPER_CREATE(vlad_polbase, a_polbase);
}

/* destroy a polbase */
VLAD_EXTERN int vlad_polbase_destroy(void *a_polbase)
{
  vlad_polbase *polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((polbase = VLAD_WRAPPER_CAST(a_polbase, vlad_polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  VLAD_MEM_DELETE(polbase);

  return VLAD_OK;
}

/* (re)init a polbase */
VLAD_EXTERN int vlad_polbase_init(void *a_polbase)
{
  vlad_polbase *polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((polbase = VLAD_WRAPPER_CAST(a_polbase, vlad_polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return polbase->init();
}

/* close symbol table */
VLAD_EXTERN int vlad_polbase_close_symtab(void *a_polbase)
{
  vlad_polbase *polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((polbase = VLAD_WRAPPER_CAST(a_polbase, vlad_polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return polbase->close_symtab();
}

/* close polbase table */
VLAD_EXTERN int vlad_polbase_close_polbase(void *a_polbase)
{
  vlad_polbase *polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((polbase = VLAD_WRAPPER_CAST(a_polbase, vlad_polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return polbase->close_polbase();
}

/* gives an array of identifiers of type t */
VLAD_EXTERN int vlad_polbase_get_array_symtab(void *a_polbase,
                                              unsigned char a_type,
                                              char ***a_array,
                                              unsigned int *a_size)
{
  vlad_polbase *polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((polbase = VLAD_WRAPPER_CAST(a_polbase, vlad_polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return polbase->get_symtab(a_type, a_array, a_size);
}

/* check if the identifier of the given type is in the symtab */
VLAD_EXTERN int vlad_polbase_check_symtab(void *a_polbase,
                                          const char *a_name,
                                          unsigned char a_type)
{
  vlad_polbase *polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((polbase = VLAD_WRAPPER_CAST(a_polbase, vlad_polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return polbase->check_symtab(a_name, a_type);
}

/* register an identifier in the polbase */
VLAD_EXTERN int vlad_polbase_add_symtab(void *a_polbase,
                                        const char *a_name,
                                        unsigned char a_type)
{
  vlad_polbase *polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((polbase = VLAD_WRAPPER_CAST(a_polbase, vlad_polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return polbase->add_symtab(a_name, a_type);
}

/* add a fact into the initial state table */
VLAD_EXTERN int vlad_polbase_add_inittab(void *a_polbase, void *a_fact)
{
  vlad_polbase *polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((polbase = VLAD_WRAPPER_CAST(a_polbase, vlad_polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return polbase->add_inittab(VLAD_WRAPPER_CAST(a_fact, vlad_fact *));
}

/* add an expression into the constraints table */
VLAD_EXTERN int vlad_polbase_add_consttab(void *a_polbase,
                                          void *a_exp,
                                          void *a_cond,
                                          void *a_ncond)
{
  vlad_polbase *polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((polbase = VLAD_WRAPPER_CAST(a_polbase, vlad_polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return
    polbase->add_consttab(VLAD_WRAPPER_CAST(a_exp, vlad_expression *),
                              VLAD_WRAPPER_CAST(a_cond, vlad_expression *),
                              VLAD_WRAPPER_CAST(a_ncond, vlad_expression *));
}

/* add an update declaration in the update table */
VLAD_EXTERN int vlad_polbase_add_updatetab(void *a_polbase,
                                           const char *a_name,
                                           void *a_vlist,
                                           void *a_precond,
                                           void *a_postcond)
{
  vlad_polbase *polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((polbase = VLAD_WRAPPER_CAST(a_polbase, vlad_polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return
    polbase->add_updatetab(a_name,
                               VLAD_WRAPPER_CAST(a_vlist, vlad_varlist *),
                               VLAD_WRAPPER_CAST(a_precond, vlad_expression *),
                               VLAD_WRAPPER_CAST(a_postcond, vlad_expression *));
}

/* returns the length of the update table */
VLAD_EXTERN unsigned int vlad_polbase_length_updatetab(void *a_polbase)
{
  vlad_polbase *polbase = NULL;

  if (a_polbase == NULL)
    return 0;

  if ((polbase = VLAD_WRAPPER_CAST(a_polbase, vlad_polbase *)) == NULL)
    return 0;

  return polbase->length_updatetab();
}

/* gives the i'th entry in the update table */
VLAD_EXTERN int vlad_polbase_get_updatetab(void *a_polbase,
                                           unsigned int a_index,
                                           char **a_name,
                                           void **a_vlist,
                                           void **a_precond,
                                           void **a_postcond)
{
  int retval;
  vlad_polbase *polbase = NULL;
  vlad_varlist *v;
  vlad_expression *pr;
  vlad_expression *po;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((polbase = VLAD_WRAPPER_CAST(a_polbase, vlad_polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  retval = polbase->get_updatetab(a_index,
                                      a_name,
                                      &v,
                                      &pr,
                                      &po);

  if (retval != VLAD_OK)
    return retval;

  *a_vlist  = VLAD_WRAPPER_CAST(v, void *);
  *a_precond = VLAD_WRAPPER_CAST(pr, void *);
  *a_postcond = VLAD_WRAPPER_CAST(po, void *);

  return VLAD_OK;
}

/* add a update reference to the sequence table */
VLAD_EXTERN int vlad_polbase_add_seqtab(void *a_polbase, void *a_uref)
{
  vlad_polbase *polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((polbase = VLAD_WRAPPER_CAST(a_polbase, vlad_polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return polbase->add_seqtab(VLAD_WRAPPER_CAST(a_uref, vlad_updateref *));
}

/* delete a update reference from the sequence table */
VLAD_EXTERN int vlad_polbase_del_seqtab(void *a_polbase, unsigned int a_index)
{
  vlad_polbase *polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((polbase = VLAD_WRAPPER_CAST(a_polbase, vlad_polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return polbase->del_seqtab(a_index);
}

/* returns the length of the sequence table */
VLAD_EXTERN unsigned int vlad_polbase_length_seqtab(void *a_polbase)
{
  vlad_polbase *polbase = NULL;

  if (a_polbase == NULL)
    return 0;

  if ((polbase = VLAD_WRAPPER_CAST(a_polbase, vlad_polbase *)) == NULL)
    return 0;

  return polbase->length_seqtab();
}

/* gives the index'th entry in the sequence table */
VLAD_EXTERN int vlad_polbase_get_seqtab(void *a_polbase,
                                        unsigned int a_index,
                                        char **a_name,
                                        void **a_ilist)
{
  int retval;
  vlad_polbase *polbase = NULL;
  vlad_stringlist *i;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((polbase = VLAD_WRAPPER_CAST(a_polbase, vlad_polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  retval = polbase->get_seqtab(a_index, a_name, &i);

  if (retval != VLAD_OK)
    return retval;

  *a_ilist  = VLAD_WRAPPER_CAST(i, void *);

  return VLAD_OK;
}

#ifdef VLAD_SMODELS
/* prepares the polbase for queries */
VLAD_EXTERN int vlad_polbase_compute_evaluate(void *a_polbase)
{
  vlad_polbase *polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((polbase = VLAD_WRAPPER_CAST(a_polbase, vlad_polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return polbase->compute_evaluate();
}

/* use smwrap class to evaluate a query */
VLAD_EXTERN int vlad_polbase_query_evaluate(void *a_polbase,
                                            void *a_exp,
                                            unsigned char *a_res)
{
  vlad_polbase *polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((polbase = VLAD_WRAPPER_CAST(a_polbase, vlad_polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return
    polbase->query_evaluate(VLAD_WRAPPER_CAST(a_exp, vlad_expression *),
                                a_res);
}
#endif

/* create a stringlist */
VLAD_EXTERN int vlad_strlist_create(void **a_slist)
{
  return VLAD_WRAPPER_CREATE(vlad_stringlist, a_slist);
}

/* destroy a stringlist */
VLAD_EXTERN int vlad_strlist_destroy(void *a_slist)
{
  vlad_stringlist *slist = NULL;

  if (a_slist == NULL)
    return VLAD_NULLPTR;

  if ((slist = VLAD_WRAPPER_CAST(a_slist, vlad_stringlist *)) == NULL)
    return VLAD_INVALIDINPUT;

  VLAD_MEM_DELETE(slist);

  return VLAD_OK;
}

/* add a string into stringlist */
VLAD_EXTERN int vlad_strlist_add(void *a_slist, const char *a_str)
{
  vlad_stringlist *slist = NULL;

  if (a_slist == NULL)
    return VLAD_NULLPTR;

  if ((slist = VLAD_WRAPPER_CAST(a_slist, vlad_stringlist *)) == NULL)
    return VLAD_INVALIDINPUT;

  return slist->add(a_str);
}

/* get the index'th string from list */
VLAD_EXTERN int vlad_strlist_get(void *a_slist,
                                 unsigned int a_index,
                                 char **a_str)
{
  vlad_stringlist *slist = NULL;

  if (a_slist == NULL)
    return VLAD_NULLPTR;

  if ((slist = VLAD_WRAPPER_CAST(a_slist, vlad_stringlist *)) == NULL)
    return VLAD_INVALIDINPUT;

  return slist->get(a_index, a_str);
}

/* create an fact */
VLAD_EXTERN int vlad_fact_create(void **a_fact)
{
  return VLAD_WRAPPER_CREATE(vlad_fact, a_fact);
}

/* destroy an fact */
VLAD_EXTERN int vlad_fact_destroy(void *a_fact)
{
  vlad_fact *fact = NULL;

  if (a_fact == NULL)
    return VLAD_NULLPTR;

  if ((fact = VLAD_WRAPPER_CAST(a_fact, vlad_fact *)) == NULL)
    return VLAD_INVALIDINPUT;

  VLAD_MEM_DELETE(fact);

  return VLAD_OK;
}

/* negate an fact */
VLAD_EXTERN int vlad_fact_negate(void *a_fact)
{
  vlad_fact *fact = NULL;

  if (a_fact == NULL)
    return VLAD_NULLPTR;

  if ((fact = VLAD_WRAPPER_CAST(a_fact, vlad_fact *)) == NULL)
    return VLAD_INVALIDINPUT;

  fact->negate();

  return VLAD_OK;
}

/* initialise facts */
VLAD_EXTERN int vlad_fact_init_holds(void *a_fact,
                                     const char *a_s,
                                     const char *a_a,
                                     const char *a_o,
                                     int a_t)
{
  vlad_fact *fact = NULL;

  if (a_fact == NULL)
    return VLAD_NULLPTR;

  if ((fact = VLAD_WRAPPER_CAST(a_fact, vlad_fact *)) == NULL)
    return VLAD_INVALIDINPUT;

  return fact->init_holds(a_s, a_a, a_o, (bool) a_t);
}

VLAD_EXTERN int vlad_fact_init_member(void *a_fact,
                                      const char *a_e,
                                      const char *a_g,
                                      int a_t)
{
  vlad_fact *fact = NULL;

  if (a_fact == NULL)
    return VLAD_NULLPTR;

  if ((fact = VLAD_WRAPPER_CAST(a_fact, vlad_fact *)) == NULL)
    return VLAD_INVALIDINPUT;

  return fact->init_member(a_e, a_g, (bool) a_t);
}

VLAD_EXTERN int vlad_fact_init_subset(void *a_fact,
                                      const char *a_g1,
                                      const char *a_g2,
                                      int a_t)
{
  vlad_fact *fact = NULL;

  if (a_fact == NULL)
    return VLAD_NULLPTR;

  if ((fact = VLAD_WRAPPER_CAST(a_fact, vlad_fact *)) == NULL)
    return VLAD_INVALIDINPUT;

  return fact->init_subset(a_g1, a_g2, (bool) a_t);
}

/* create an expression */
VLAD_EXTERN int vlad_exp_create(void **a_exp)
{
  return VLAD_WRAPPER_CREATE(vlad_expression, a_exp);
}

/* destroy an expression */
VLAD_EXTERN int vlad_exp_destroy(void *a_exp)
{
  vlad_expression *exp = NULL;

  if (a_exp == NULL)
    return VLAD_NULLPTR;

  if ((exp = VLAD_WRAPPER_CAST(a_exp, vlad_expression *)) == NULL)
    return VLAD_INVALIDINPUT;

  VLAD_MEM_DELETE(exp);

  return VLAD_OK;
}

/* add an fact into expression */
VLAD_EXTERN int vlad_exp_add(void *a_exp, void *a_fact)
{
  vlad_expression *exp = NULL;

  if (a_exp == NULL)
    return VLAD_NULLPTR;

  if ((exp = VLAD_WRAPPER_CAST(a_exp, vlad_expression *)) == NULL)
    return VLAD_INVALIDINPUT;

  return exp->add(VLAD_WRAPPER_CAST(a_fact, vlad_fact *));
}

/* gives the index'th fact */
VLAD_EXTERN int vlad_exp_get(void *a_exp,
                             unsigned int a_index,
                             void **a_fact)
{
  int retval;
  vlad_expression *exp = NULL;
  vlad_fact *fact = NULL;

  if (a_exp == NULL || a_fact == NULL)
    return VLAD_NULLPTR;

  if ((exp = VLAD_WRAPPER_CAST(a_exp, vlad_expression *)) == NULL)
    return VLAD_INVALIDINPUT;

  if ((retval = exp->get(a_index, &fact)) != VLAD_OK)
    return retval;

  *a_fact = (void *) fact;

  return VLAD_OK;
}

/* create an update ref */
VLAD_EXTERN int vlad_uref_create(void **a_uref)
{
  return VLAD_WRAPPER_CREATE(vlad_updateref, a_uref);
}

/* destroy an update ref */
VLAD_EXTERN int vlad_uref_destroy(void *a_uref)
{
  vlad_updateref *uref = NULL;

  if (a_uref == NULL)
    return VLAD_NULLPTR;

  if ((uref = VLAD_WRAPPER_CAST(a_uref, vlad_updateref *)) == NULL)
    return VLAD_INVALIDINPUT;

  VLAD_MEM_DELETE(uref);

  return VLAD_OK;
}

/* initialise a update ref */
VLAD_EXTERN int vlad_uref_init(void *a_uref,
                               const char *a_name,
                               void *a_vlist)
{
  vlad_updateref *uref = NULL;
  char *name = NULL;

  if (a_uref == NULL || a_name == NULL)
    return VLAD_NULLPTR;

  if ((uref = VLAD_WRAPPER_CAST(a_uref, vlad_updateref *)) == NULL)
    return VLAD_INVALIDINPUT;

  if ((name = VLAD_MEM_STR_MALLOC(a_name)) == NULL)
    return VLAD_MALLOCFAILED;

  strcpy(name, a_name);

  return
    uref->init(name, VLAD_WRAPPER_CAST(a_vlist, vlad_varlist *));
}

/* get the length of the list */
VLAD_EXTERN int vlad_list_length(void *a_list)
{
  vlad_list *list;

  if (a_list == NULL)
    return 0;

  if ((list = VLAD_WRAPPER_CAST(a_list, vlad_list *)) == NULL)
    return 0;

  return list->length();
}
