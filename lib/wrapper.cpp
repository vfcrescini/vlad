/*
 * wrapper.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <new>

#include <stdio.h>
#include <config.h>
#include <vlad/vlad.h>
#include <vlad/wrapper.h>
#include <vlad/kb.h>

/* create a kb */
VLAD_EXTERN int vlad_kb_create(void **a_kb)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = VLAD_NEW(kb())) == NULL)
    return VLAD_MALLOCFAILED;

  *a_kb = static_cast<void *>(tmp_kb);

  return VLAD_OK;
}

/* destroy a kb */
VLAD_EXTERN int vlad_kb_destroy(void *a_kb)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = static_cast<kb *>(a_kb)) == NULL)
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

  if ((tmp_kb = static_cast<kb *>(a_kb)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_kb->init();
}

/* close symbol table */
VLAD_EXTERN int vlad_kb_close_symtab(void *a_kb)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = static_cast<kb *>(a_kb)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_kb->close_symtab();
}

/* close kb table */
VLAD_EXTERN int vlad_kb_close_kb(void *a_kb)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = static_cast<kb *>(a_kb)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_kb->close_kb();
}

/* register an identifier in the kb */
VLAD_EXTERN int vlad_kb_add_symtab(void *a_kb,
                                   const char *a_n,
                                   unsigned char a_t)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = static_cast<kb *>(a_kb)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_kb->add_symtab(a_n, a_t);
}

/* add an atom into the initial state table */
VLAD_EXTERN int vlad_kb_add_inittab(void *a_kb, void *a_atm)
{
  kb *tmp_kb = NULL;
  atom *tmp_atm = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = static_cast<kb *>(a_kb)) == NULL)
    return VLAD_INVALIDINPUT;

  tmp_atm = static_cast<atom *>(a_atm);

  return tmp_kb->add_inittab(tmp_atm);
}

/* add an expression into the constraints table */
VLAD_EXTERN int vlad_kb_add_consttab(void *a_kb,
                                     void *a_e,
                                     void *a_c,
                                     void *a_n)
{
  kb *tmp_kb = NULL;
  expression *tmp_e = NULL;
  expression *tmp_c = NULL;
  expression *tmp_n = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = static_cast<kb *>(a_kb)) == NULL)
    return VLAD_INVALIDINPUT;

  tmp_e = static_cast<expression *>(a_e);
  tmp_c = static_cast<expression *>(a_c);
  tmp_n = static_cast<expression *>(a_n);

  return tmp_kb->add_consttab(tmp_e, tmp_c, tmp_n);
}

/* add a transformation declaration in the trans table */
VLAD_EXTERN int vlad_kb_add_transtab(void *a_kb,
                                     const char *a_n,
                                     void *a_v,
                                     void *a_pr,
                                     void *a_po)
{
  kb *tmp_kb = NULL;
  stringlist *tmp_v = NULL;
  expression *tmp_pr = NULL;
  expression *tmp_po = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = static_cast<kb *>(a_kb)) == NULL)
    return VLAD_INVALIDINPUT;

  tmp_v = static_cast<stringlist *>(a_v);
  tmp_pr = static_cast<expression *>(a_pr);
  tmp_po = static_cast<expression *>(a_po);

  return tmp_kb->add_transtab(a_n, tmp_v, tmp_pr, tmp_po);
}

/* add a transformation reference to the sequence table */
VLAD_EXTERN int vlad_kb_add_seqtab(void *a_kb, void *a_tr)
{
  kb *tmp_kb = NULL;
  transref *tmp_tr = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = static_cast<kb *>(a_kb)) == NULL)
    return VLAD_INVALIDINPUT;

  tmp_tr = static_cast<transref *>(a_tr);

  return tmp_kb->add_seqtab(tmp_tr);
}

/* delete a transformation reference from the sequence table */
VLAD_EXTERN int vlad_kb_del_seqtab(void *a_kb, unsigned int a_i)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = static_cast<kb *>(a_kb)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_kb->del_seqtab(a_i);
}

/* enumerate the sequences in the sequence table, output to f */
VLAD_EXTERN int vlad_kb_list_seqtab(void *a_kb, FILE *a_fs)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = static_cast<kb *>(a_kb)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_kb->list_seqtab(a_fs);
}

/* generate the rules necessary to evaluate queries */
VLAD_EXTERN int vlad_kb_compute_generate(void *a_kb, FILE *a_fs)
{
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = static_cast<kb *>(a_kb)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_kb->compute_generate(a_fs);
}

/* generate the query */
VLAD_EXTERN int vlad_kb_query_generate(void *a_kb, void *a_exp, FILE *a_fs)
{
  kb *tmp_kb = NULL;
  expression *tmp_exp = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = static_cast<kb *>(a_kb)) == NULL)
    return VLAD_INVALIDINPUT;

  tmp_exp = static_cast<expression *>(a_exp);

  return tmp_kb->query_generate(tmp_exp, a_fs);
}

/* prepares the kb for queries */
VLAD_EXTERN int vlad_kb_compute_evaluate(void *a_kb)
{
#ifdef SMODELS
  kb *tmp_kb = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = static_cast<kb *>(a_kb)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_kb->compute_evaluate();
#else
  return VLAD_FAILURE;
#endif
}

/* use smwrap class to evaluate a query */
VLAD_EXTERN int vlad_kb_query_evaluate(void *a_kb,
                                       void *a_exp,
                                       unsigned char *a_res)
{
#ifdef SMODELS
  kb *tmp_kb = NULL;
  expression *tmp_exp = NULL;

  if (a_kb == NULL)
    return VLAD_NULLPTR;

  if ((tmp_kb = static_cast<kb *>(a_kb)) == NULL)
    return VLAD_INVALIDINPUT;

  tmp_exp = static_cast<expression *>(a_exp);

  return tmp_kb->query_evaluate(tmp_exp, a_res);
#else
  return VLAD_FAILURE;
#endif
}

/* create a stringlist */
VLAD_EXTERN int vlad_strlist_create(void **a_slist)
{
  stringlist *tmp_slist = NULL;

  if (a_slist == NULL)
    return VLAD_NULLPTR;

  if ((tmp_slist = VLAD_NEW(stringlist())) == NULL)
    return VLAD_MALLOCFAILED;

  *a_slist = static_cast<void *>(tmp_slist);

  return VLAD_OK;
}

/* destroy a stringlist */
VLAD_EXTERN int vlad_strlist_destroy(void *a_slist)
{
  stringlist *tmp_slist = NULL;

  if (a_slist == NULL)
    return VLAD_NULLPTR;

  if ((tmp_slist = static_cast<stringlist *>(a_slist)) == NULL)
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

  if ((tmp_slist = static_cast<stringlist *>(a_slist)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_slist->add(a_s);
}

/* create an atom */
VLAD_EXTERN int vlad_atom_create(void **a_atm)
{
  atom *tmp_atm = NULL;

  if (a_atm == NULL)
    return VLAD_NULLPTR;

  if ((tmp_atm = VLAD_NEW(atom())) == NULL)
    return VLAD_MALLOCFAILED;

  *a_atm = static_cast<void *>(tmp_atm);

  return VLAD_OK;
}

/* destroy an atom */
VLAD_EXTERN int vlad_atom_destroy(void *a_atm)
{
  atom *tmp_atm = NULL;

  if (a_atm == NULL)
    return VLAD_NULLPTR;

  if ((tmp_atm = static_cast<atom *>(a_atm)) == NULL)
    return VLAD_INVALIDINPUT;

  delete tmp_atm;

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

  if ((tmp_atm = static_cast<atom *>(a_atm)) == NULL)
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

  if ((tmp_atm = static_cast<atom *>(a_atm)) == NULL)
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

  if ((tmp_atm = static_cast<atom *>(a_atm)) == NULL)
    return VLAD_INVALIDINPUT;

  return tmp_atm->init_subset(a_g1, a_g2, (bool) a_t);
}

/* create an expression */
VLAD_EXTERN int vlad_exp_create(void **a_exp)
{
  expression *tmp_exp = NULL;

  if (a_exp == NULL)
    return VLAD_NULLPTR;

  if ((tmp_exp = VLAD_NEW(expression())) == NULL)
    return VLAD_MALLOCFAILED;

  *a_exp = static_cast<void *>(tmp_exp);

  return VLAD_OK;
}

/* destroy an expression */
VLAD_EXTERN int vlad_exp_destroy(void *a_exp)
{
  expression *tmp_exp = NULL;

  if (a_exp == NULL)
    return VLAD_NULLPTR;

  if ((tmp_exp = static_cast<expression *>(a_exp)) == NULL)
    return VLAD_INVALIDINPUT;

  delete tmp_exp;

  return VLAD_OK;
}

/* add an atom into expression */
VLAD_EXTERN int vlad_exp_add(void *a_exp, void *a_atm)
{
  expression *tmp_exp = NULL;
  atom *tmp_atm = NULL;

  if (a_exp == NULL)
    return VLAD_NULLPTR;

  if ((tmp_exp = static_cast<expression *>(a_exp)) == NULL)
    return VLAD_INVALIDINPUT;

  tmp_atm = static_cast<atom *>(a_atm);

  return tmp_exp->add(tmp_atm);
}

/* gives the i'th atom */
VLAD_EXTERN int vlad_exp_get(void *a_exp, unsigned int a_i, void **a_atm)
{
  int retval;
  expression *tmp_exp = NULL;
  atom *tmp_atom;

  if (a_exp == NULL || a_atm == NULL)
    return VLAD_NULLPTR;

  if ((tmp_exp = static_cast<expression *>(a_exp)) == NULL)
    return VLAD_INVALIDINPUT;

  if ((retval = tmp_exp->get(a_i, &tmp_atom)) != VLAD_OK)
    return retval;

  *a_atm = (void *) tmp_atom;

  return VLAD_OK;
}

/* create a trans ref */
VLAD_EXTERN int vlad_tref_create(void **a_tref)
{
  transref *tmp_tref = NULL;

  if (a_tref == NULL)
    return VLAD_NULLPTR;

  if ((tmp_tref = VLAD_NEW(transref())) == NULL)
    return VLAD_MALLOCFAILED;

  *a_tref = static_cast<void *>(tmp_tref);

  return VLAD_OK;
}

/* destroy a tran sref */
VLAD_EXTERN int vlad_tref_destroy(void *a_tref)
{
  transref *tmp_tref = NULL;

  if (a_tref == NULL)
    return VLAD_NULLPTR;

  if ((tmp_tref = static_cast<transref *>(a_tref)) == NULL)
    return VLAD_INVALIDINPUT;

  delete tmp_tref;

  return VLAD_OK;
}

/* initialise a trans ref */
VLAD_EXTERN int vlad_tref_init(void *a_tref, const char *a_n, void *a_slist)
{
  transref *tmp_tref = NULL;
  stringlist *tmp_slist = NULL;

  if (a_tref == NULL)
    return VLAD_NULLPTR;

  if ((tmp_tref = static_cast<transref *>(a_tref)) == NULL)
    return VLAD_INVALIDINPUT;

  tmp_slist = static_cast<stringlist *>(a_slist);

  return tmp_tref->init(a_n, tmp_slist);
}

/* get the length of the list */
VLAD_EXTERN int vlad_list_length(void *a_list)
{
  list *tmp_list;

  if (a_list == NULL)
    return 0;

  if ((tmp_list = static_cast<list *>(a_list)) == NULL)
    return 0;

  return tmp_list->length();
}
