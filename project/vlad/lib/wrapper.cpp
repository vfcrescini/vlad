/*
 * wrapper.cpp
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <new>

#include <vlad/vlad.h>
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
    (((*(Y) = VLAD_WRAPPER_CAST(VLAD_NEW(X), void *)) == NULL) ? \
      VLAD_MALLOCFAILED :                                        \
      VLAD_OK))

/* create a polbase */
VLAD_EXTERN int vlad_polbase_create(void **a_polbase)
{
  return VLAD_WRAPPER_CREATE(polbase, a_polbase);
}

/* destroy a polbase */
VLAD_EXTERN int vlad_polbase_destroy(void *a_polbase)
{
  polbase *tmp_polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((tmp_polbase = VLAD_WRAPPER_CAST(a_polbase, polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  delete tmp_polbase;

  return VLAD_OK;
}

/* (re)init a polbase */
VLAD_EXTERN int vlad_polbase_init(void *a_polbase)
{
  polbase *tmp_polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((tmp_polbase = VLAD_WRAPPER_CAST(a_polbase, polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_polbase->init();
}

/* close symbol table */
VLAD_EXTERN int vlad_polbase_close_symtab(void *a_polbase)
{
  polbase *tmp_polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((tmp_polbase = VLAD_WRAPPER_CAST(a_polbase, polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_polbase->close_symtab();
}

/* close polbase table */
VLAD_EXTERN int vlad_polbase_close_polbase(void *a_polbase)
{
  polbase *tmp_polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((tmp_polbase = VLAD_WRAPPER_CAST(a_polbase, polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_polbase->close_polbase();
}

/* gives an array of identifiers of type t */
VLAD_EXTERN int vlad_polbase_get_array_symtab(void *a_polbase,
                                              unsigned char a_type,
                                              char ***a_array,
                                              unsigned int *a_size)
{
  polbase *tmp_polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((tmp_polbase = VLAD_WRAPPER_CAST(a_polbase, polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_polbase->get_symtab(a_type, a_array, a_size);
}

/* check if the identifier of the given type is in the symtab */
VLAD_EXTERN int vlad_polbase_check_symtab(void *a_polbase,
                                          const char *a_name,
                                          unsigned char a_type)
{
  polbase *tmp_polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((tmp_polbase = VLAD_WRAPPER_CAST(a_polbase, polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_polbase->check_symtab(a_name, a_type);
}

/* register an identifier in the polbase */
VLAD_EXTERN int vlad_polbase_add_symtab(void *a_polbase,
                                   const char *a_name,
                                   unsigned char a_type)
{
  polbase *tmp_polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((tmp_polbase = VLAD_WRAPPER_CAST(a_polbase, polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_polbase->add_symtab(a_name, a_type);
}

/* add a fact into the initial state table */
VLAD_EXTERN int vlad_polbase_add_inittab(void *a_polbase, void *a_fact)
{
  polbase *tmp_polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((tmp_polbase = VLAD_WRAPPER_CAST(a_polbase, polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_polbase->add_inittab(VLAD_WRAPPER_CAST(a_fact, fact *));
}

/* add an expression into the constraints table */
VLAD_EXTERN int vlad_polbase_add_consttab(void *a_polbase,
                                          void *a_exp,
                                          void *a_cond,
                                          void *a_ncond)
{
  polbase *tmp_polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((tmp_polbase = VLAD_WRAPPER_CAST(a_polbase, polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_polbase->add_consttab(VLAD_WRAPPER_CAST(a_exp, expression *),
                              VLAD_WRAPPER_CAST(a_cond, expression *),
                              VLAD_WRAPPER_CAST(a_ncond, expression *));
}

/* add an update declaration in the update table */
VLAD_EXTERN int vlad_polbase_add_updatetab(void *a_polbase,
                                           const char *a_name,
                                           void *a_vlist,
                                           void *a_precond,
                                           void *a_postcond)
{
  polbase *tmp_polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((tmp_polbase = VLAD_WRAPPER_CAST(a_polbase, polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_polbase->add_updatetab(a_name,
                                    VLAD_WRAPPER_CAST(a_vlist, stringlist *),
                                    VLAD_WRAPPER_CAST(a_precond, expression *),
                                    VLAD_WRAPPER_CAST(a_postcond, expression *));
}

/* returns the length of the update table */
VLAD_EXTERN unsigned int vlad_polbase_length_updatetab(void *a_polbase)
{
  polbase *tmp_polbase = NULL;

  if (a_polbase == NULL)
    return 0;

  if ((tmp_polbase = VLAD_WRAPPER_CAST(a_polbase, polbase *)) == NULL)
    return 0;

  return tmp_polbase->length_updatetab();
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
  polbase *tmp_polbase = NULL;
  stringlist *tmp_v;
  expression *tmp_pr;
  expression *tmp_po;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((tmp_polbase = VLAD_WRAPPER_CAST(a_polbase, polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  retval = tmp_polbase->get_updatetab(a_index,
                                      a_name,
                                      &tmp_v,
                                      &tmp_pr,
                                      &tmp_po);

  if (retval != VLAD_OK)
    return retval;

  *a_vlist  = VLAD_WRAPPER_CAST(tmp_v, void *);
  *a_precond = VLAD_WRAPPER_CAST(tmp_pr, void *);
  *a_postcond = VLAD_WRAPPER_CAST(tmp_po, void *);

  return VLAD_OK;
}

/* add a update reference to the sequence table */
VLAD_EXTERN int vlad_polbase_add_seqtab(void *a_polbase, void *a_uref)
{
  polbase *tmp_polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((tmp_polbase = VLAD_WRAPPER_CAST(a_polbase, polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_polbase->add_seqtab(VLAD_WRAPPER_CAST(a_uref, updateref *));
}

/* delete a update reference from the sequence table */
VLAD_EXTERN int vlad_polbase_del_seqtab(void *a_polbase, unsigned int a_index)
{
  polbase *tmp_polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((tmp_polbase = VLAD_WRAPPER_CAST(a_polbase, polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_polbase->del_seqtab(a_index);
}

/* returns the length of the sequence table */
VLAD_EXTERN unsigned int vlad_polbase_length_seqtab(void *a_polbase)
{
  polbase *tmp_polbase = NULL;

  if (a_polbase == NULL)
    return 0;

  if ((tmp_polbase = VLAD_WRAPPER_CAST(a_polbase, polbase *)) == NULL)
    return 0;

  return tmp_polbase->length_seqtab();
}

/* gives the index'th entry in the sequence table */
VLAD_EXTERN int vlad_polbase_get_seqtab(void *a_polbase,
                                        unsigned int a_index,
                                        char **a_name,
                                        void **a_ilist)
{
  int retval;
  polbase *tmp_polbase = NULL;
  stringlist *tmp_i;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((tmp_polbase = VLAD_WRAPPER_CAST(a_polbase, polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  retval = tmp_polbase->get_seqtab(a_index, a_name, &tmp_i);

  if (retval != VLAD_OK)
    return retval;

  *a_ilist  = VLAD_WRAPPER_CAST(tmp_i, void *);

  return VLAD_OK;
}

#ifdef VLAD_SMODELS
/* prepares the polbase for queries */
VLAD_EXTERN int vlad_polbase_compute_evaluate(void *a_polbase)
{
  polbase *tmp_polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((tmp_polbase = VLAD_WRAPPER_CAST(a_polbase, polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_polbase->compute_evaluate();
}

/* use smwrap class to evaluate a query */
VLAD_EXTERN int vlad_polbase_query_evaluate(void *a_polbase,
                                            void *a_exp,
                                            unsigned char *a_res)
{
  polbase *tmp_polbase = NULL;

  if (a_polbase == NULL)
    return VLAD_NULLPTR;

  if ((tmp_polbase = VLAD_WRAPPER_CAST(a_polbase, polbase *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_polbase->query_evaluate(VLAD_WRAPPER_CAST(a_exp, expression *), a_res);
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
VLAD_EXTERN int vlad_strlist_add(void *a_slist, const char *a_str)
{
  stringlist *tmp_slist = NULL;

  if (a_slist == NULL)
    return VLAD_NULLPTR;

  if ((tmp_slist = VLAD_WRAPPER_CAST(a_slist, stringlist *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_slist->add(a_str);
}

/* get the index'th string from list */
VLAD_EXTERN int vlad_strlist_get(void *a_slist,
                                 unsigned int a_index,
                                 char **a_str)
{
  stringlist *tmp_slist = NULL;

  if (a_slist == NULL)
    return VLAD_NULLPTR;

  if ((tmp_slist = VLAD_WRAPPER_CAST(a_slist, stringlist *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_slist->get(a_index, a_str);
}

/* create an fact */
VLAD_EXTERN int vlad_fact_create(void **a_fact)
{
  return VLAD_WRAPPER_CREATE(fact, a_fact);
}

/* destroy an fact */
VLAD_EXTERN int vlad_fact_destroy(void *a_fact)
{
  fact *tmp_fact = NULL;

  if (a_fact == NULL)
    return VLAD_NULLPTR;

  if ((tmp_fact = VLAD_WRAPPER_CAST(a_fact, fact *)) == NULL)
    return VLAD_INVALIDINPUT;

  delete tmp_fact;

  return VLAD_OK;
}

/* negate an fact */
VLAD_EXTERN int vlad_fact_negate(void *a_fact)
{
  fact *tmp_fact = NULL;

  if (a_fact == NULL)
    return VLAD_NULLPTR;

  if ((tmp_fact = VLAD_WRAPPER_CAST(a_fact, fact *)) == NULL)
    return VLAD_INVALIDINPUT;

  tmp_fact->negate();

  return VLAD_OK;
}

/* initialise facts */
VLAD_EXTERN int vlad_fact_init_holds(void *a_fact,
                                     const char *a_s,
                                     const char *a_a,
                                     const char *a_o,
                                     int a_t)
{
  fact *tmp_fact = NULL;

  if (a_fact == NULL)
    return VLAD_NULLPTR;

  if ((tmp_fact = VLAD_WRAPPER_CAST(a_fact, fact *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_fact->init_holds(a_s, a_a, a_o, (bool) a_t);
}

VLAD_EXTERN int vlad_fact_init_member(void *a_fact,
                                      const char *a_e,
                                      const char *a_g,
                                      int a_t)
{
  fact *tmp_fact = NULL;

  if (a_fact == NULL)
    return VLAD_NULLPTR;

  if ((tmp_fact = VLAD_WRAPPER_CAST(a_fact, fact *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_fact->init_member(a_e, a_g, (bool) a_t);
}

VLAD_EXTERN int vlad_fact_init_subset(void *a_fact,
                                      const char *a_g1,
                                      const char *a_g2,
                                      int a_t)
{
  fact *tmp_fact = NULL;

  if (a_fact == NULL)
    return VLAD_NULLPTR;

  if ((tmp_fact = VLAD_WRAPPER_CAST(a_fact, fact *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_fact->init_subset(a_g1, a_g2, (bool) a_t);
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

/* add an fact into expression */
VLAD_EXTERN int vlad_exp_add(void *a_exp, void *a_fact)
{
  expression *tmp_exp = NULL;

  if (a_exp == NULL)
    return VLAD_NULLPTR;

  if ((tmp_exp = VLAD_WRAPPER_CAST(a_exp, expression *)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_exp->add(VLAD_WRAPPER_CAST(a_fact, fact *));
}

/* gives the index'th fact */
VLAD_EXTERN int vlad_exp_get(void *a_exp,
                             unsigned int a_index,
                             void **a_fact)
{
  int retval;
  expression *tmp_exp = NULL;
  fact *tmp_fact = NULL;

  if (a_exp == NULL || a_fact == NULL)
    return VLAD_NULLPTR;

  if ((tmp_exp = VLAD_WRAPPER_CAST(a_exp, expression *)) == NULL)
    return VLAD_INVALIDINPUT;

  if ((retval = tmp_exp->get(a_index, &tmp_fact)) != VLAD_OK)
    return retval;

  *a_fact = (void *) tmp_fact;

  return VLAD_OK;
}

/* create an update ref */
VLAD_EXTERN int vlad_uref_create(void **a_uref)
{
  return VLAD_WRAPPER_CREATE(updateref, a_uref);
}

/* destroy an update ref */
VLAD_EXTERN int vlad_uref_destroy(void *a_uref)
{
  updateref *tmp_uref = NULL;

  if (a_uref == NULL)
    return VLAD_NULLPTR;

  if ((tmp_uref = VLAD_WRAPPER_CAST(a_uref, updateref *)) == NULL)
    return VLAD_INVALIDINPUT;

  delete tmp_uref;

  return VLAD_OK;
}

/* initialise a update ref */
VLAD_EXTERN int vlad_uref_init(void *a_uref,
                               const char *a_name,
                               void *a_vlist)
{
  updateref *tmp_uref = NULL;
  char *tmp_name = NULL;

  if (a_uref == NULL || a_name == NULL)
    return VLAD_NULLPTR;

  if ((tmp_uref = VLAD_WRAPPER_CAST(a_uref, updateref *)) == NULL)
    return VLAD_INVALIDINPUT;

  if ((tmp_name = VLAD_STRING_MALLOC(a_name)) == NULL)
    return VLAD_MALLOCFAILED;

  strcpy(tmp_name, a_name);

  return tmp_uref->init(tmp_name, VLAD_WRAPPER_CAST(a_vlist, stringlist *));
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
