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
#include <vlad/identifier.h>
#include <vlad/polbase.h>

/* some shorthand macros */
#define VLAD_LEN_SS (m_stable->length(VLAD_IDENT_ENT_SUB_SIN))
#define VLAD_LEN_SG (m_stable->length(VLAD_IDENT_ENT_SUB_GRP))
#define VLAD_LEN_AS (m_stable->length(VLAD_IDENT_ENT_ACC_SIN))
#define VLAD_LEN_AG (m_stable->length(VLAD_IDENT_ENT_ACC_GRP))
#define VLAD_LEN_OS (m_stable->length(VLAD_IDENT_ENT_OBJ_SIN))
#define VLAD_LEN_OG (m_stable->length(VLAD_IDENT_ENT_OBJ_GRP))

vlad_polbase::vlad_polbase()
{
  m_stage = 0;
  m_stable = NULL;
  m_itable = NULL;
  m_ctable = NULL;
  m_utable = NULL;
  m_setable = NULL;
  m_mapper = NULL;
#ifdef VLAD_SMODELS
  m_smobject = NULL;
#endif
}

vlad_polbase::~vlad_polbase()
{
  if (m_stable != NULL)
    VLAD_MEM_DELETE(m_stable);
  if (m_itable != NULL)
    VLAD_MEM_DELETE(m_itable);
  if (m_ctable != NULL)
    VLAD_MEM_DELETE(m_ctable);
  if (m_utable != NULL)
    VLAD_MEM_DELETE(m_utable);
  if (m_setable != NULL)
    VLAD_MEM_DELETE(m_setable);
  if (m_mapper != NULL)
    VLAD_MEM_DELETE(m_mapper);
#ifdef VLAD_SMODELS
  if (m_smobject != NULL)
    VLAD_MEM_DELETE(m_smobject);
#endif
}

/* (re)init the policy base */
int vlad_polbase::init()
{
  int retval;

  /* initialise symbol table */
  if (m_stable != NULL)
    VLAD_MEM_DELETE(m_stable);
  if ((m_stable = VLAD_MEM_NEW(vlad_symtab())) == NULL)
    return VLAD_MALLOCFAILED;
  if ((retval = m_stable->init()) != VLAD_OK)
    return retval;

  /* initialise initial expression table */
  if (m_itable != NULL)
    VLAD_MEM_DELETE(m_itable);
  if ((m_itable = VLAD_MEM_NEW(vlad_expression())) == NULL)
    return VLAD_MALLOCFAILED;

  /* initialise constraints table */
  if (m_ctable != NULL)
    VLAD_MEM_DELETE(m_ctable);
  if ((m_ctable = VLAD_MEM_NEW(vlad_consttab())) == NULL)
    return VLAD_MALLOCFAILED;

  /* initialise update declaration table */
  if (m_utable != NULL)
    VLAD_MEM_DELETE(m_utable);
  if ((m_utable = VLAD_MEM_NEW(vlad_updatetab())) == NULL)
    return VLAD_MALLOCFAILED;

  /* sequence table */
  if (m_setable != NULL)
    VLAD_MEM_DELETE(m_setable);
  if ((m_setable = VLAD_MEM_NEW(vlad_seqtab())) == NULL)
    return VLAD_MALLOCFAILED;

  /* the mapper */
  if (m_mapper != NULL)
    VLAD_MEM_DELETE(m_mapper);
  if ((m_mapper = VLAD_MEM_NEW(vlad_mapper())) == NULL)
    return VLAD_MALLOCFAILED;

#ifdef VLAD_SMODELS
  /* smodels smwrap */
  if (m_smobject != NULL)
    VLAD_MEM_DELETE(m_smobject);
  m_smobject = NULL;
#endif

  m_stage = 1;

  return VLAD_OK;
}

/* add an entity in the symbol table */
int vlad_polbase::add_symtab(const char *a_name, unsigned char a_type)
{
  if (m_stage != 1)
    return VLAD_INVALIDOP;

  return m_stable->add(a_name, a_type);
}

/* add a fact into the initial state table */
int vlad_polbase::add_inittab(vlad_fact *a_fact)
{
  int retval;
  vlad_fact *fact;

  if (m_stage != 2)
    return VLAD_INVALIDOP;

  if (a_fact == NULL)
    return VLAD_NULLPTR;

  /* verify and copy */
  retval = a_fact->vcopy(m_stable, NULL, &fact);

  /* add to inittab */
  if (retval == VLAD_OK)
    retval = m_itable->add(fact);

  /* cleanup */
  if (retval != VLAD_OK)
    VLAD_MEM_DELETE(fact);

  return retval;
}

/* add a constrant into the constraints table */
int vlad_polbase::add_consttab(vlad_expression *a_exp,
                               vlad_expression *a_cond,
                               vlad_expression *a_ncond)
{
  int retval = VLAD_OK;
  unsigned int i;
  vlad_varlist *vlist;

  if (m_stage != 2)
    return VLAD_INVALIDOP;

  /* only a_exp is required to be non-null */
  if (a_exp == NULL)
    return VLAD_NULLPTR;

  /* create the vlist */
  if ((vlist = VLAD_MEM_NEW(vlad_varlist())) == NULL)
    return VLAD_MALLOCFAILED;

  /* generate the vlist, also verify the expressions */
  if (retval == VLAD_OK)
    retval = a_exp->vvarlist(m_stable, vlist);
  if (retval == VLAD_OK && a_cond != NULL)
    retval = a_cond->vvarlist(m_stable, vlist);
  if (retval == VLAD_OK && a_ncond != NULL)
    retval = a_ncond->vvarlist(m_stable, vlist);

  if (retval != VLAD_OK) {
    VLAD_MEM_DELETE(vlist);
    return retval;
  }

  /* if there are no variables, forget trying to ground them */
  if (vlist->length() == 0) {
    vlad_expression *exp_e = NULL;
    vlad_expression *exp_c = NULL;
    vlad_expression *exp_n = NULL;

    /* make copies of the expresssions, no need to verify anymore */
    if (retval == VLAD_OK)
      retval = a_exp->copy(&exp_e);
    if (retval == VLAD_OK && a_cond != NULL)
      retval = a_cond->copy(&exp_c);
    if (retval == VLAD_OK && a_ncond != NULL)
      retval = a_ncond->copy(&exp_n);

    /* then, we add the expressions into the cosntraints table */
    if (retval == VLAD_OK)
      retval = m_ctable->add(exp_e, exp_c, exp_n);

    if (retval != VLAD_OK) {
      if (exp_e != NULL)
        VLAD_MEM_DELETE(exp_e);
      if (exp_c != NULL)
        VLAD_MEM_DELETE(exp_c);
      if (exp_n != NULL)
        VLAD_MEM_DELETE(exp_n);
    }
  }
  else {
    vlad_stringlistlist *tlist;

    /* get a tuple list */
    if (retval == VLAD_OK)
      retval = m_stable->tupleate(vlist, &tlist);

    /* ground and store */
    for (i = 0; retval == VLAD_OK && i < tlist->length(); i++) {
      vlad_stringlist *ilist;
      vlad_expression *exp_e = NULL;
      vlad_expression *exp_c = NULL;
      vlad_expression *exp_n = NULL;
    
      if (retval == VLAD_OK)
        retval = tlist->get(i, &ilist);
    
      /* now ground and verify each expression */
      if (retval == VLAD_OK)
        retval = a_exp->vreplace(m_stable, vlist, ilist, &exp_e);
      if (retval == VLAD_OK && a_cond != NULL)
        retval = a_cond->vreplace(m_stable, vlist, ilist, &exp_c);
      if (retval == VLAD_OK && a_ncond != NULL)
        retval = a_ncond->vreplace(m_stable, vlist, ilist, &exp_n);
     
      /* finally, we add this instantiation to the constraints table */
      if (retval == VLAD_OK)
        retval = m_ctable->add(exp_e, exp_c, exp_n);

      if (retval != VLAD_OK) {
        if (exp_e != NULL)
          VLAD_MEM_DELETE(exp_e);
        if (exp_c != NULL)
          VLAD_MEM_DELETE(exp_c);
        if (exp_n != NULL)
          VLAD_MEM_DELETE(exp_n);
      }
    }

    VLAD_MEM_DELETE(tlist);
  }

  VLAD_MEM_DELETE(vlist);

  return retval;
}

/* add an update declaration in the update table */
int vlad_polbase::add_updatetab(const char *a_name,
                                vlad_varlist *a_vlist,
                                vlad_expression *a_precond,
                                vlad_expression *a_postcond)
{
  int retval = VLAD_OK;
  char *name;
  vlad_varlist *vlist = NULL;
  vlad_expression *exp_pr = NULL;
  vlad_expression *exp_po = NULL;

  /* we only allow this function after symtab is closed */
  if (m_stage != 2)
    return VLAD_INVALIDOP;

  /* precondition and vlist are allowed to be NULL */
  if (a_name == NULL || a_postcond == NULL)
    return VLAD_NULLPTR;

  /* copy name */
  if ((name = VLAD_MEM_STR_MALLOC(a_name)) == NULL)
    return VLAD_MALLOCFAILED;

  strcpy(name, a_name);

  /* copy varlist */
  if (retval == VLAD_OK && a_vlist != NULL)
    retval = a_vlist->copy(&vlist);

  /* verify and copy precondition */
  if (retval == VLAD_OK && a_precond != NULL)
    retval = a_precond->vcopy(m_stable, NULL, &exp_pr);

  /* verify and copy the postcondition */
  if (retval == VLAD_OK)
    retval = a_postcond->vcopy(m_stable, NULL, &exp_po);

  /* if all went well, add to the udate table */
  if (retval == VLAD_OK)
    retval = m_utable->add(name, vlist, exp_pr, exp_po);

  /* cleanup */
  if (retval != VLAD_OK) {
    if (name != NULL)
      VLAD_MEM_FREE(name);
    if (vlist != NULL)
      VLAD_MEM_DELETE(vlist);
    if (exp_pr != NULL)
      VLAD_MEM_DELETE(exp_pr);
    if (exp_po != NULL)
      VLAD_MEM_DELETE(exp_po);
  }

  return retval;
}

/* add an update reference to the sequence table */
int vlad_polbase::add_seqtab(vlad_updateref *a_uref)
{
  int retval;

  /* we only allow this function after policy base is closed */
  if (m_stage < 3)
    return VLAD_INVALIDOP;

  if (a_uref == NULL)
    return VLAD_NULLPTR;

  /* we verify first before adding */
  if ((retval = a_uref->verify(m_stable, m_utable)) != VLAD_OK)
    return retval;

  /* if all is well, add */
  if ((retval = m_setable->add(a_uref)) != VLAD_OK)
    return retval;

  /* set back to m_stage 3 to prevent query before compute */
  m_stage = 3;

  return VLAD_OK;
}

/* close symbol table */
int vlad_polbase::close_symtab()
{
  int retval;

  if (m_stage != 1)
    return VLAD_INVALIDOP;

  /* initialise the mapper with the closed symbol table */
  if ((retval = m_mapper->init(m_stable)) != VLAD_OK)
    return retval;

  m_stage = 2;

  return VLAD_OK;
}

/* after this is called, no further calls to add_inittab(), add_consttab()
 * or add_updatetab() is allowed */
int vlad_polbase::close_polbase()
{
  if (m_stage != 2)
    return VLAD_INVALIDOP;

  m_stage = 3;

  return VLAD_OK;
}

/* delete an update reference from the sequence table */
int vlad_polbase::del_seqtab(unsigned int a_index)
{
  int retval;

  /* we only allow this function after policy base is closed */
  if (m_stage < 3)
    return VLAD_INVALIDOP;

  if ((retval = m_setable->del(a_index)) != VLAD_OK)
    return retval;

  /* set back to m_stage 3 to prevent query before compute */
  m_stage = 3;

  return retval;
}

/* gives an array of identifiers of the given type */
int vlad_polbase::get_symtab(unsigned char a_type,
                             char ***a_array,
                             unsigned int *a_size)
{
  if (m_stage < 2)
    return VLAD_INVALIDOP;

  return m_stable->get(a_type, a_array, a_size);
}

/* gives the index'th entry in the sequence table */
int vlad_polbase::get_seqtab(unsigned int a_index,
                             char **a_name,
                             vlad_stringlist **a_ilist)
{
  if (m_stage < 2)
    return VLAD_INVALIDOP;

  return m_setable->get(a_index, a_name, a_ilist);
}

/* gives the index'th entry in the update table */
int vlad_polbase::get_updatetab(unsigned int a_index,
                                char **a_name,
                                vlad_varlist **a_vlist,
                                vlad_expression **a_precond,
                                vlad_expression **a_postcond)
{
  if (m_stage < 2)
    return VLAD_INVALIDOP;

  return m_utable->get(a_index, a_name, a_vlist, a_precond, a_postcond);
}

/* returns the length of the sequence table */
unsigned int vlad_polbase::length_seqtab()
{
  if (m_stage < 2)
    return 0;

  return VLAD_LIST_LENGTH(m_setable);
}

/* returns the length of the update table */
unsigned int vlad_polbase::length_updatetab()
{
  if (m_stage < 2)
    return 0;

  return VLAD_LIST_LENGTH(m_utable);
}

/* checks whether name of the given type is in symtab */
int vlad_polbase::check_symtab(const char *a_name, unsigned char a_type)
{
  if (m_stage < 2)
    return VLAD_INVALIDOP;

  return m_stable->find(a_name, a_type);
}

/* enumerate the sequences in the sequence table, output to file */
int vlad_polbase::list_seqtab(FILE *a_fs)
{
  int retval;
  unsigned int i;
  unsigned int j;
  char *name;
  vlad_stringlist *ilist;

  /* we only allow this function after policy base is closed */
  if (m_stage < 3)
    return VLAD_INVALIDOP;

  if (a_fs == NULL)
    return VLAD_NULLPTR;

  for (i = 0; i < VLAD_LIST_LENGTH(m_setable); i++) {
    /* get the update ref */
    if ((retval = m_setable->get(i, &name, &ilist)) != VLAD_OK)
      return retval;
    /* now print */
    fprintf(a_fs, "%d %s(", i, name);
    /* also do the stringlist */
    for (j = 0; j < VLAD_LIST_LENGTH(ilist); j++) {
      char *arg;
      if ((retval = ilist->get(j, &arg)) != VLAD_OK) {
        fprintf(a_fs, ")\n");
        return retval;
      }
      fprintf(a_fs, "%s%s", ((j == 0) ? "" : ","), arg);
    }
    fprintf(a_fs, ")\n");
  }

  return VLAD_OK;
}

/* generate the rules necessary to evaluate queries */
int vlad_polbase::compute_generate(FILE *a_fs)
{
  int retval;

  /* we only allow this function after polbase is closed */
  if (m_stage < 3)
    return VLAD_INVALIDOP;

  /* make sure the filestream is not NULL */
  if (a_fs == NULL)
    return VLAD_NULLPTR;

  /* identity rules */
  fprintf(a_fs, "\n%s %s\n\n", VLAD_STR_IDENTITY, VLAD_STR_RULES);

  if ((retval = generate_identity(a_fs)) != VLAD_OK)
    return retval;

  /* inheritance rules */
  fprintf(a_fs, "%s %s\n\n", VLAD_STR_INHERITANCE, VLAD_STR_RULES);

  if ((retval = generate_inheritance(a_fs)) != VLAD_OK)
    return retval;

  /* transitivity */
  fprintf(a_fs, "%s %s\n\n", VLAD_STR_TRANSITIVITY, VLAD_STR_RULES);
  if ((retval = generate_transitivity(a_fs)) != VLAD_OK)
    return retval;

  /* negation rules */
  fprintf(a_fs, "%s %s\n\n", VLAD_STR_NEGATION, VLAD_STR_RULES);
  if ((retval = generate_negation(a_fs)) != VLAD_OK)
    return retval;

  /* inertial rules */
  fprintf(a_fs, "%s %s\n\n", VLAD_STR_INERTIAL, VLAD_STR_RULES);
  if ((retval = generate_inertial(a_fs)) != VLAD_OK)
    return retval;

  /* initial state */
  fprintf(a_fs, "%s %s\n\n", VLAD_STR_INITSTATE, VLAD_STR_RULES);
  if ((retval = generate_initial(a_fs)) != VLAD_OK)
    return retval;

  /* constraints */
  fprintf(a_fs, "%s %s\n\n", VLAD_STR_CONSTRAINT, VLAD_STR_RULES);
  if ((retval = generate_constraint(a_fs)) != VLAD_OK)
    return retval;

  /* update rules */
  fprintf(a_fs, "%s %s\n\n", VLAD_STR_UPDATE, VLAD_STR_RULES);
  if ((retval = generate_update(a_fs)) != VLAD_OK)
    return retval;

  return VLAD_OK;
}

/* generate the query */
int vlad_polbase::query_generate(vlad_expression *a_exp, FILE *a_fs)
{
  int retval;
  unsigned int i;

  /* only allowed after policy base has closed */
  if (m_stage < 3)
    return VLAD_INVALIDOP;

  /* make sure the filestream is not NULL */
  if (a_fs == NULL || a_exp == NULL)
    return VLAD_NULLPTR;

  /* verify expression */
  if ((retval = a_exp->verify(m_stable, NULL)) != VLAD_OK)
    return retval;

  for (i = 0; i < VLAD_LIST_LENGTH(a_exp); i++) {
    vlad_fact *fact;
    unsigned int id;

    if ((retval = a_exp->get(i, &fact)) != VLAD_OK)
      return retval;
    if ((retval = m_mapper->encode_fact(fact, VLAD_LIST_LENGTH(m_setable), &id)) != VLAD_OK)
      return retval;

    print_fact(id, a_fs);

    if (i + 1 == VLAD_LIST_LENGTH(a_exp))
      fprintf(a_fs, " %s\n", VLAD_STR_QUERY);
    else
      fprintf(a_fs, " %s ", VLAD_STR_AND);
  }

  return VLAD_OK;
}

#ifdef VLAD_SMODELS
/* prepares the policy base for queries */
int vlad_polbase::compute_evaluate()
{
  int retval;
  unsigned int i;

  /* we only allow this after policy base is closed */
  if (m_stage != 3 && m_stage != 4)
    return VLAD_INVALIDOP;

  /* create a new instance of the smodels smwrap and init it */
  if (m_smobject != NULL)
    VLAD_MEM_DELETE(m_smobject);
  if ((m_smobject = VLAD_MEM_NEW(vlad_smwrap())) == NULL)
    return VLAD_MALLOCFAILED;
  if ((retval = m_smobject->init()) != VLAD_OK)
    return retval;

  /* first we register all the possible atoms in smodels */
  for (i = 0; i < (m_mapper->get_totals(VLAD_ATOM_TOTAL) * 2 * (VLAD_LIST_LENGTH(m_setable) + 1)); i++) {
    if ((retval = m_smobject->add_atom(i)) != VLAD_OK)
      return retval;
  }

  m_smobject->close_atom();

  /* identity rules */
  if ((retval = evaluate_identity()) != VLAD_OK)
    return retval;

  /* inheritance rules */
  if ((retval = evaluate_inheritance()) != VLAD_OK)
    return retval;

  /* transitivity rules */
  if ((retval = evaluate_transitivity()) != VLAD_OK)
    return retval;

  /* negation rules */
  if ((retval = evaluate_negation()) != VLAD_OK)
    return retval;

  /* inertial rules */
  if ((retval = evaluate_inertial()) != VLAD_OK)
    return retval;

  /* initial state */
  if ((retval = evaluate_initial()) != VLAD_OK)
    return retval;

  /* constraints */
  if ((retval = evaluate_constraint()) != VLAD_OK)
    return retval;

  /* update sequence rules */
  if ((retval = evaluate_update()) != VLAD_OK)
    return retval;

  /* this might not succeed as there might not exist a model for this query */
  if ((retval = m_smobject->close_rule()) != VLAD_OK)
    return retval;

  m_stage = 4;

  return VLAD_OK;
}
#endif

#ifdef VLAD_SMODELS
/* use smwrap class to evaluate a query */
int vlad_polbase::query_evaluate(vlad_expression *a_exp, unsigned char *a_res)
{
  int retval;
  unsigned int i;

  *a_res = VLAD_RESULT_TRUE;

  /* we only allow this after a call to compute() */
  if (m_stage != 4)
    return VLAD_INVALIDOP;

  if (a_exp == NULL || a_res == NULL)
    return VLAD_NULLPTR;

  /* verify expression */
  if ((retval = a_exp->verify(m_stable, NULL)) != VLAD_OK)
    return retval;

  /* go through the facts to test */
  for (i = 0; i < VLAD_LIST_LENGTH(a_exp); i++) {
    vlad_fact *fact;
    unsigned int id;
    unsigned char res;

    if ((retval = a_exp->get(i, &fact)) != VLAD_OK)
      return retval;
    if ((retval = m_mapper->encode_fact(fact, VLAD_LIST_LENGTH(m_setable), &id)) != VLAD_OK)
      return retval;

    if ((retval = evaluate_fact(id, &res)) != VLAD_OK)
      return retval;

    switch (res) {
      case VLAD_RESULT_TRUE :
        continue;
      case VLAD_RESULT_FALSE :
        *a_res = VLAD_RESULT_FALSE;
        return VLAD_OK;
      default :
        *a_res = VLAD_RESULT_UNKNOWN;
    }
  }

  return VLAD_OK;
}
#endif

#ifdef VLAD_SMODELS
/* checks whether the given fact is true, false or unknown */
int vlad_polbase::evaluate_fact(unsigned int a_fact, unsigned char *a_res)
{
  int retval;
  bool res;

  if (m_stage != 4)
    return VLAD_INVALIDOP;
  if (a_res == NULL)
    return VLAD_NULLPTR;

  if ((retval = m_smobject->ask(a_fact, &res)) != VLAD_OK)
    return retval;

  if (res)
    *a_res = VLAD_RESULT_TRUE;
  else {
    /* false, so we must try the negation */
    if ((retval = m_smobject->ask(m_mapper->negate_fact(a_fact), &res)) != VLAD_OK)
      return retval;

    *a_res = (res ? VLAD_RESULT_FALSE : VLAD_RESULT_UNKNOWN);
  }
  return VLAD_OK;
}
#endif

/* dumps a fact with the given id onto the given (open) stream */
int vlad_polbase::print_fact(unsigned int a_id, FILE *a_fs)
{
  int retval;
  vlad_fact *fact;
  unsigned int state;

  /* we only allow this after polbase is closed */
  if (m_stage < 3)
    return VLAD_INVALIDOP;

  if (a_fs == NULL)
    return VLAD_NULLPTR;

  if ((retval = m_mapper->decode_fact(a_id, &state, &fact)) != VLAD_OK)
    return retval;

  if ((retval = print_fact(fact, state, a_fs)) != VLAD_OK)
    return retval;

  VLAD_MEM_DELETE(fact);

  return VLAD_OK;
}

/* dumps a fact with the given state onto the given (open) stream */
int vlad_polbase::print_fact(vlad_fact *a_fact,
                             unsigned int a_state,
                             FILE *a_fs)
{
  int retval;
  unsigned char type;
  bool truth;
  char *parm[3];

  if (a_fact == NULL | a_fs == NULL)
    return VLAD_NULLPTR;

  retval = a_fact->get(&(parm[0]), &(parm[1]), &(parm[2]), &type, &truth);

  if (retval != VLAD_OK)
    return retval;

  switch(type) {
    case VLAD_ATOM_HOLDS :
      fprintf(a_fs,
              "%s(%s, %s, %s, S%d, %s)",
              VLAD_STR_HOLDS,
              parm[0],
              parm[1],
              parm[2],
              a_state,
              truth ? VLAD_STR_TRUE : VLAD_STR_FALSE);
      break;
    case VLAD_ATOM_MEMBER :
        fprintf(a_fs,
                "%s(%s, %s, S%d, %s)",
                VLAD_STR_MEMBER,
                parm[0],
                parm[1],
                a_state,
                truth ? VLAD_STR_TRUE : VLAD_STR_FALSE);
      break;
    case VLAD_ATOM_SUBSET :
        fprintf(a_fs,
                "%s(%s, %s, S%d, %s)",
                VLAD_STR_SUBSET,
                parm[0],
                parm[1],
                a_state,
                truth ? VLAD_STR_TRUE : VLAD_STR_FALSE);
      break;
  }

  return VLAD_OK;
}

/* generates identity rules and prints them to a_fs */
int vlad_polbase::generate_identity(FILE *a_fs)
{
  unsigned int i_sta;
  unsigned int i_grp;

  if (a_fs == NULL)
    return VLAD_NULLPTR;

  if (m_stage < 3)
    return VLAD_INVALIDOP;

  /* state loop */
  for (i_sta = 0; i_sta <= VLAD_LIST_LENGTH(m_setable); i_sta++) {
    /* subject groups */
    for (i_grp = 0; i_grp < VLAD_LEN_SG; i_grp++) {
      fprintf(a_fs, "  ");
      print_fact(m_mapper->compute_subst(VLAD_IDENT_SUB, i_grp, i_grp, i_sta, true), a_fs);
      fprintf(a_fs, " %s\n    %s%s\n", VLAD_STR_ARROW, VLAD_STR_TRUE, VLAD_STR_TERMINATOR);
    }
    /* access groups */
    for (i_grp = 0; i_grp < VLAD_LEN_AG; i_grp++) {
      fprintf(a_fs, "  ");
      print_fact(m_mapper->compute_subst(VLAD_IDENT_ACC, i_grp, i_grp, i_sta, true), a_fs);
      fprintf(a_fs, " %s\n    %s%s\n", VLAD_STR_ARROW, VLAD_STR_TRUE, VLAD_STR_TERMINATOR);
    }

    /* object groups */
    for (i_grp = 0; i_grp < VLAD_LEN_OG; i_grp++) {
      fprintf(a_fs, "  ");
      print_fact(m_mapper->compute_subst(VLAD_IDENT_OBJ, i_grp, i_grp, i_sta, true), a_fs);
      fprintf(a_fs, " %s\n    %s%s\n", VLAD_STR_ARROW, VLAD_STR_TRUE, VLAD_STR_TERMINATOR);
    }
  }

  fprintf(a_fs, "\n"); 

  return VLAD_OK;
}

/* generates inheritance rules and prints them to a_fs */
int vlad_polbase::generate_inheritance(FILE *a_fs)
{
  unsigned int i_sta;
  unsigned int i_grp1;
  unsigned int i_grp2;
  unsigned int i_sub;
  unsigned int i_acc;
  unsigned int i_obj;

  if (a_fs == NULL)
    return VLAD_NULLPTR;

  if (m_stage < 3)
    return VLAD_INVALIDOP;

  /* subset inheritance */

  for (i_sta = 0; i_sta <= VLAD_LIST_LENGTH(m_setable); i_sta++) {
    /* subject groups */
    for (i_grp1 = 0; i_grp1 < VLAD_LEN_SG; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < VLAD_LEN_SG; i_grp2++) {
        if (i_grp1 == i_grp2)
          continue;
        for (i_acc = 0; i_acc < VLAD_LEN_AS + VLAD_LEN_AG; i_acc++) {
          for (i_obj = 0; i_obj < VLAD_LEN_OS + VLAD_LEN_OG; i_obj++) {
            fprintf(a_fs, "  ");
            print_fact(m_mapper->compute_holds(i_grp1 + VLAD_LEN_SS, i_acc, i_obj, i_sta, true), a_fs);
            fprintf(a_fs, " %s\n    ", VLAD_STR_ARROW);
            print_fact(m_mapper->compute_holds(i_grp2 + VLAD_LEN_SS, i_acc, i_obj, i_sta, true), a_fs);
            fprintf(a_fs, " %s\n    ", VLAD_STR_AND);
            print_fact(m_mapper->compute_subst(VLAD_IDENT_SUB, i_grp1, i_grp2, i_sta, true), a_fs);
            fprintf(a_fs, " %s\n    %s ", VLAD_STR_AND, VLAD_STR_NOT);
            print_fact(m_mapper->compute_holds(i_grp1 + VLAD_LEN_SS, i_acc, i_obj, i_sta, false), a_fs);
            fprintf(a_fs, "%s\n", VLAD_STR_TERMINATOR);
    
            fprintf(a_fs, "  ");
            print_fact(m_mapper->compute_holds(i_grp1 + VLAD_LEN_SS, i_acc, i_obj, i_sta, false), a_fs);
            fprintf(a_fs, " %s\n    ", VLAD_STR_ARROW);
            print_fact(m_mapper->compute_holds(i_grp2 + VLAD_LEN_SS, i_acc, i_obj, i_sta, false), a_fs);
            fprintf(a_fs, " %s\n    ", VLAD_STR_AND);
            print_fact(m_mapper->compute_subst(VLAD_IDENT_SUB, i_grp1, i_grp2, i_sta, true), a_fs);
            fprintf(a_fs, "%s\n", VLAD_STR_TERMINATOR);
          }
        }
      }
    }
    /* access groups */
    for (i_grp1 = 0; i_grp1 < VLAD_LEN_AG; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < VLAD_LEN_AG; i_grp2++) {
        if (i_grp1 == i_grp2)
          continue;
        for (i_sub = 0; i_sub < VLAD_LEN_SS + VLAD_LEN_SG; i_sub++) {
          for (i_obj = 0; i_obj < VLAD_LEN_OS + VLAD_LEN_OG; i_obj++) {
            fprintf(a_fs, "  ");
            print_fact(m_mapper->compute_holds(i_sub, i_grp1 + VLAD_LEN_AS, i_obj, i_sta, true), a_fs);
            fprintf(a_fs, " %s\n    ", VLAD_STR_ARROW);
            print_fact(m_mapper->compute_holds(i_sub, i_grp2 + VLAD_LEN_AS, i_obj, i_sta, true), a_fs);
            fprintf(a_fs, " %s\n    ", VLAD_STR_AND);
            print_fact(m_mapper->compute_subst(VLAD_IDENT_ACC, i_grp1, i_grp2, i_sta, true), a_fs);
            fprintf(a_fs, " %s\n    %s ", VLAD_STR_AND, VLAD_STR_NOT);
            print_fact(m_mapper->compute_holds(i_sub, i_grp1 + VLAD_LEN_AS, i_obj, i_sta, false), a_fs);
            fprintf(a_fs, "%s\n", VLAD_STR_TERMINATOR);
    
            fprintf(a_fs, "  ");
            print_fact(m_mapper->compute_holds(i_sub, i_grp1 + VLAD_LEN_AS, i_obj, i_sta, false), a_fs);
            fprintf(a_fs, " %s\n    ", VLAD_STR_ARROW);
            print_fact(m_mapper->compute_holds(i_sub, i_grp2 + VLAD_LEN_AS, i_obj, i_sta, false), a_fs);
            fprintf(a_fs, " %s\n    ", VLAD_STR_AND);
            print_fact(m_mapper->compute_subst(VLAD_IDENT_ACC, i_grp1, i_grp2, i_sta, true), a_fs);
            fprintf(a_fs, "%s\n", VLAD_STR_TERMINATOR);
          }
        }
      }
    }
    /* object groups */
    for (i_grp1 = 0; i_grp1 < VLAD_LEN_OG; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < VLAD_LEN_OG; i_grp2++) {
        if (i_grp1 == i_grp2)
          continue;
        for (i_sub = 0; i_sub < VLAD_LEN_SS + VLAD_LEN_SG; i_sub++) {
          for (i_acc = 0; i_acc < VLAD_LEN_AS + VLAD_LEN_AG; i_acc++) {
            fprintf(a_fs, "  ");
            print_fact(m_mapper->compute_holds(i_sub, i_acc, i_grp1 + VLAD_LEN_OS, i_sta, true), a_fs);
            fprintf(a_fs, " %s\n    ", VLAD_STR_ARROW);
            print_fact(m_mapper->compute_holds(i_sub, i_acc, i_grp2 + VLAD_LEN_OS, i_sta, true), a_fs);
            fprintf(a_fs, " %s\n    ", VLAD_STR_AND);
            print_fact(m_mapper->compute_subst(VLAD_IDENT_OBJ, i_grp1, i_grp2, i_sta, true), a_fs);
            fprintf(a_fs, " %s\n    %s ", VLAD_STR_AND, VLAD_STR_NOT);
            print_fact(m_mapper->compute_holds(i_sub, i_acc, i_grp1 + VLAD_LEN_OS, i_sta, false), a_fs);
            fprintf(a_fs, "%s\n", VLAD_STR_TERMINATOR);
    
            fprintf(a_fs, "  ");
            print_fact(m_mapper->compute_holds(i_sub, i_acc, i_grp1 + VLAD_LEN_OS, i_sta, false), a_fs);
            fprintf(a_fs, " %s\n    ", VLAD_STR_ARROW);
            print_fact(m_mapper->compute_holds(i_sub, i_acc, i_grp2 + VLAD_LEN_OS, i_sta, false), a_fs);
            fprintf(a_fs, " %s\n    ", VLAD_STR_AND);
            print_fact(m_mapper->compute_subst(VLAD_IDENT_OBJ, i_grp1, i_grp2, i_sta, true), a_fs);
            fprintf(a_fs, "%s\n", VLAD_STR_TERMINATOR);
          }
        }
      }
    }
  }

  /* membership inheritance */

  for (i_sta = 0; i_sta <= VLAD_LIST_LENGTH(m_setable); i_sta++) {
    /* subject groups */
    for (i_grp1 = 0; i_grp1 < VLAD_LEN_SG; i_grp1++) {
      for (i_sub = 0; i_sub < VLAD_LEN_SS; i_sub++) {
        for (i_acc = 0; i_acc < VLAD_LEN_AS + VLAD_LEN_AG; i_acc++) {
          for (i_obj = 0; i_obj < VLAD_LEN_OS + VLAD_LEN_OG; i_obj++) {
            fprintf(a_fs, "  ");
            print_fact(m_mapper->compute_holds(i_sub, i_acc, i_obj, i_sta, true), a_fs);
            fprintf(a_fs, " %s\n    ", VLAD_STR_ARROW);
            print_fact(m_mapper->compute_holds(i_grp1 + VLAD_LEN_SS, i_acc, i_obj, i_sta, true), a_fs);
            fprintf(a_fs, " %s\n    ", VLAD_STR_AND);
            print_fact(m_mapper->compute_memb(VLAD_IDENT_SUB, i_sub, i_grp1, i_sta, true), a_fs);
            fprintf(a_fs, "%s\n    %s ", VLAD_STR_AND, VLAD_STR_NOT);
            print_fact(m_mapper->compute_holds(i_sub, i_acc, i_obj, i_sta, false), a_fs);
            fprintf(a_fs, "%s\n", VLAD_STR_TERMINATOR);

            fprintf(a_fs, "  ");
            print_fact(m_mapper->compute_holds(i_sub, i_acc, i_obj, i_sta, false), a_fs);
            fprintf(a_fs, " %s\n    ", VLAD_STR_ARROW);
            print_fact(m_mapper->compute_holds(i_grp1 + VLAD_LEN_SS, i_acc, i_obj, i_sta, false), a_fs);
            fprintf(a_fs, " %s\n    ", VLAD_STR_AND);
            print_fact(m_mapper->compute_memb(VLAD_IDENT_SUB, i_sub, i_grp1, i_sta, true), a_fs);
            fprintf(a_fs, "%s\n", VLAD_STR_TERMINATOR);
          }
        }
      }
    }
    /* access groups */
    for (i_grp1 = 0; i_grp1 < VLAD_LEN_AG; i_grp1++) {
      for (i_sub = 0; i_sub < VLAD_LEN_SS + VLAD_LEN_SG; i_sub++) {
        for (i_acc = 0; i_acc < VLAD_LEN_AS; i_acc++) {
          for (i_obj = 0; i_obj < VLAD_LEN_OS + VLAD_LEN_OG; i_obj++) {
            fprintf(a_fs, "  ");
            print_fact(m_mapper->compute_holds(i_sub, i_acc, i_obj, i_sta, true), a_fs);
            fprintf(a_fs, " %s\n    ", VLAD_STR_ARROW);
            print_fact(m_mapper->compute_holds(i_sub, i_grp1 + VLAD_LEN_AS, i_obj, i_sta, true), a_fs);
            fprintf(a_fs, " %s\n    ", VLAD_STR_AND);
            print_fact(m_mapper->compute_memb(VLAD_IDENT_ACC, i_acc, i_grp1, i_sta, true), a_fs);
            fprintf(a_fs, "%s\n    %s ", VLAD_STR_AND, VLAD_STR_NOT);
            print_fact(m_mapper->compute_holds(i_sub, i_acc, i_obj, i_sta, false), a_fs);
            fprintf(a_fs, "%s\n", VLAD_STR_TERMINATOR);

            fprintf(a_fs, "  ");
            print_fact(m_mapper->compute_holds(i_sub, i_acc, i_obj, i_sta, false), a_fs);
            fprintf(a_fs, " %s\n    ", VLAD_STR_ARROW);
            print_fact(m_mapper->compute_holds(i_sub, i_grp1 + VLAD_LEN_AS, i_obj, i_sta, false), a_fs);
            fprintf(a_fs, " %s\n    ", VLAD_STR_AND);
            print_fact(m_mapper->compute_memb(VLAD_IDENT_ACC, i_acc, i_grp1, i_sta, true), a_fs);
            fprintf(a_fs, "%s\n", VLAD_STR_TERMINATOR);
          }
        }
      }
    }
    /* object groups */
    for (i_grp1 = 0; i_grp1 < VLAD_LEN_OG; i_grp1++) {
      for (i_sub = 0; i_sub < VLAD_LEN_SS + VLAD_LEN_SG; i_sub++) {
        for (i_acc = 0; i_acc < VLAD_LEN_AS + VLAD_LEN_AG; i_acc++) {
          for (i_obj = 0; i_obj < VLAD_LEN_OS; i_obj++) {
            fprintf(a_fs, "  ");
            print_fact(m_mapper->compute_holds(i_sub, i_acc, i_obj, i_sta, true), a_fs);
            fprintf(a_fs, " %s\n    ", VLAD_STR_ARROW);
            print_fact(m_mapper->compute_holds(i_sub, i_acc, i_grp1 + VLAD_LEN_OS, i_sta, true), a_fs);
            fprintf(a_fs, " %s\n    ", VLAD_STR_AND);
            print_fact(m_mapper->compute_memb(VLAD_IDENT_OBJ, i_obj, i_grp1, i_sta, true), a_fs);
            fprintf(a_fs, "%s\n    %s ", VLAD_STR_AND, VLAD_STR_NOT);
            print_fact(m_mapper->compute_holds(i_sub, i_acc, i_obj, i_sta, false), a_fs);
            fprintf(a_fs, "%s\n", VLAD_STR_TERMINATOR);

            fprintf(a_fs, "  ");
            print_fact(m_mapper->compute_holds(i_sub, i_acc, i_obj, i_sta, false), a_fs);
            fprintf(a_fs, " %s\n    ", VLAD_STR_ARROW);
            print_fact(m_mapper->compute_holds(i_sub, i_acc, i_grp1 + VLAD_LEN_OS, i_sta, false), a_fs);
            fprintf(a_fs, " %s\n    ", VLAD_STR_AND);
            print_fact(m_mapper->compute_memb(VLAD_IDENT_OBJ, i_obj, i_grp1, i_sta, true), a_fs);
            fprintf(a_fs, "%s\n", VLAD_STR_TERMINATOR);
          }
        }
      }
    }
  }

  fprintf(a_fs, "\n");

  return VLAD_OK;
}

/* generates transitivity rules and prints them to a_fs */
int vlad_polbase::generate_transitivity(FILE *a_fs)
{
  unsigned int i_sta;
  unsigned int i_grp1;
  unsigned int i_grp2;
  unsigned int i_grp3;
  
  if (a_fs == NULL)
    return VLAD_NULLPTR;

  if (m_stage < 3)
    return VLAD_INVALIDOP;

  /* state loop */
  for (i_sta = 0; i_sta <= VLAD_LIST_LENGTH(m_setable); i_sta++) {
    /* subject groups */
    for (i_grp1 = 0; i_grp1 < VLAD_LEN_SG; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < VLAD_LEN_SG; i_grp2++) {
        for (i_grp3 = 0; i_grp3 < VLAD_LEN_SG; i_grp3++) {
          /* ignore if any 2 are the same */
          if (i_grp1 == i_grp2 || i_grp1 == i_grp3 || i_grp2 == i_grp3)
            continue;

          fprintf(a_fs, "  ");
          print_fact(m_mapper->compute_subst(VLAD_IDENT_SUB, i_grp1, i_grp3, i_sta, true), a_fs);
          fprintf(a_fs, " %s\n    ", VLAD_STR_ARROW);
          print_fact(m_mapper->compute_subst(VLAD_IDENT_SUB, i_grp1, i_grp2, i_sta, true), a_fs);
          fprintf(a_fs, " %s\n    ", VLAD_STR_AND);
          print_fact(m_mapper->compute_subst(VLAD_IDENT_SUB, i_grp2, i_grp3, i_sta, true), a_fs);
          fprintf(a_fs, "%s\n", VLAD_STR_TERMINATOR);
        }
      }
    }
    /* access groups */
    for (i_grp1 = 0; i_grp1 < VLAD_LEN_AG; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < VLAD_LEN_AG; i_grp2++) {
        for (i_grp3 = 0; i_grp3 < VLAD_LEN_AG; i_grp3++) {
          /* ignore if any 2 are the same */
          if (i_grp1 == i_grp2 || i_grp1 == i_grp3 || i_grp2 == i_grp3)
            continue;

          fprintf(a_fs, "  ");
          print_fact(m_mapper->compute_subst(VLAD_IDENT_ACC, i_grp1, i_grp3, i_sta, true), a_fs);
          fprintf(a_fs, " %s\n    ", VLAD_STR_ARROW);
          print_fact(m_mapper->compute_subst(VLAD_IDENT_ACC, i_grp1, i_grp2, i_sta, true), a_fs);
          fprintf(a_fs, " %s\n    ", VLAD_STR_AND);
          print_fact(m_mapper->compute_subst(VLAD_IDENT_ACC, i_grp2, i_grp3, i_sta, true), a_fs);
          fprintf(a_fs, "%s\n", VLAD_STR_TERMINATOR);
        }
      }
    }
    /* object groups */
    for (i_grp1 = 0; i_grp1 < VLAD_LEN_OG; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < VLAD_LEN_OG; i_grp2++) {
        for (i_grp3 = 0; i_grp3 < VLAD_LEN_OG; i_grp3++) {
          /* ignore if any 2 are the same */
          if (i_grp1 == i_grp2 || i_grp1 == i_grp3 || i_grp2 == i_grp3)
            continue;

          fprintf(a_fs, "  ");
          print_fact(m_mapper->compute_subst(VLAD_IDENT_OBJ, i_grp1, i_grp3, i_sta, true), a_fs);
          fprintf(a_fs, " %s\n    ", VLAD_STR_ARROW);
          print_fact(m_mapper->compute_subst(VLAD_IDENT_OBJ, i_grp1, i_grp2, i_sta, true), a_fs);
          fprintf(a_fs, " %s\n    ", VLAD_STR_AND);
          print_fact(m_mapper->compute_subst(VLAD_IDENT_OBJ, i_grp2, i_grp3, i_sta, true), a_fs);
          fprintf(a_fs, "%s\n", VLAD_STR_TERMINATOR);
        }
      }
    }
  }

  fprintf(a_fs, "\n");

  return VLAD_OK;
}

/* generates negation rules and prints them to a_fs */
int vlad_polbase::generate_negation(FILE *a_fs)
{
  unsigned int i_sta;
  unsigned int i_atm;

  if (a_fs == NULL)
    return VLAD_NULLPTR;

  if (m_stage < 3)
    return VLAD_INVALIDOP;

  for (i_sta = 0; i_sta <= VLAD_LIST_LENGTH(m_setable); i_sta++) {
    for (i_atm = 0; i_atm < m_mapper->get_totals(VLAD_ATOM_TOTAL); i_atm++) {
      fprintf(a_fs, "  %s %s\n    ", VLAD_STR_FALSE, VLAD_STR_ARROW);
      print_fact(m_mapper->compute_fact(i_atm, i_sta, true), a_fs);
      fprintf(a_fs, " %s\n    ", VLAD_STR_AND);
      print_fact(m_mapper->compute_fact(i_atm, i_sta, false), a_fs);
      fprintf(a_fs, "%s\n", VLAD_STR_TERMINATOR);
    }
  }

  fprintf(a_fs, "\n");

  return VLAD_OK;
}

/* generates inertial rules and prints them to a_fs */
int vlad_polbase::generate_inertial(FILE *a_fs)
{
  unsigned int i_sta;
  unsigned int i_atm;

  if (a_fs == NULL)
    return VLAD_NULLPTR;

  if (m_stage < 3)
    return VLAD_INVALIDOP;

  /* state loop */
  for (i_sta = 0; i_sta < VLAD_LIST_LENGTH(m_setable); i_sta++) {
    for (i_atm = 0; i_atm < m_mapper->get_totals(VLAD_ATOM_TOTAL); i_atm++) {
      fprintf(a_fs, "  ");
      print_fact(m_mapper->compute_fact(i_atm, i_sta + 1, true), a_fs);
      fprintf(a_fs, " %s\n    ", VLAD_STR_ARROW);
      print_fact(m_mapper->compute_fact(i_atm, i_sta, true), a_fs);
      fprintf(a_fs, " %s\n    %s ", VLAD_STR_AND, VLAD_STR_NOT);
      print_fact(m_mapper->compute_fact(i_atm, i_sta + 1, false), a_fs);
      fprintf(a_fs, "%s\n", VLAD_STR_TERMINATOR);

      fprintf(a_fs, "  ");
      print_fact(m_mapper->compute_fact(i_atm, i_sta + 1, false), a_fs);
      fprintf(a_fs, " %s\n    ", VLAD_STR_ARROW);
      print_fact(m_mapper->compute_fact(i_atm, i_sta, false), a_fs);
      fprintf(a_fs, " %s\n    %s ", VLAD_STR_AND, VLAD_STR_NOT);
      print_fact(m_mapper->compute_fact(i_atm, i_sta + 1, true), a_fs);
      fprintf(a_fs, "%s\n", VLAD_STR_TERMINATOR);
    }
  }

  fprintf(a_fs, "\n");

  return VLAD_OK;
}

/* generates initial state rules and prints them to a_fs */
int vlad_polbase::generate_initial(FILE *a_fs)
{
  int retval;
  unsigned int i;

  if (a_fs == NULL)
    return VLAD_NULLPTR;

  if (m_stage < 3)
    return VLAD_INVALIDOP;

  for (i = 0; i < VLAD_LIST_LENGTH(m_itable); i++) {
    vlad_fact *fact = NULL;

    if ((retval = m_itable->get(i, &fact)) != VLAD_OK)
      return retval;

    fprintf(a_fs, "  ");
    print_fact(fact, 0, a_fs);
    fprintf(a_fs,
            " %s\n    %s%s\n",
            VLAD_STR_ARROW,
            VLAD_STR_TRUE,
            VLAD_STR_TERMINATOR);
  }

  fprintf(a_fs, "\n");

  return VLAD_OK;
}

/* generates constraint rules and prints them to a_fs */
int vlad_polbase::generate_constraint(FILE *a_fs)
{
  int retval;
  unsigned int i_sta;
  unsigned int i_con;
  unsigned int i_exp1;
  unsigned int i_exp2;

  if (a_fs == NULL)
    return VLAD_NULLPTR;

  if (m_stage < 3)
    return VLAD_INVALIDOP;

  for (i_sta = 0; i_sta <= VLAD_LIST_LENGTH(m_setable); i_sta++) {
    /* constraint loop */
    for (i_con = 0; i_con < VLAD_LIST_LENGTH(m_ctable); i_con++) {
      vlad_expression *exp_e = NULL;
      vlad_expression *exp_c = NULL;
      vlad_expression *exp_n = NULL;
      vlad_fact *fact = NULL;

      if ((retval = m_ctable->get(i_con, &exp_e, &exp_c, &exp_n)) != VLAD_OK)
        return retval;

      /* constaint expression */
      for (i_exp1 = 0; i_exp1 < VLAD_LIST_LENGTH(exp_e); i_exp1++) {
        if ((retval = exp_e->get(i_exp1, &fact)) != VLAD_OK)
          return retval;

        fprintf(a_fs, "  ");

        print_fact(fact, i_sta, a_fs);

        fprintf(a_fs, " %s\n    ", VLAD_STR_ARROW);
        
        /* constraint condition */
        for (i_exp2 = 0; i_exp2 < VLAD_LIST_LENGTH(exp_c); i_exp2++) {
          if ((retval = exp_c->get(i_exp2, &fact)) != VLAD_OK)
            return retval;
        
          print_fact(fact, i_sta, a_fs);
        
          if (i_exp2 + 1 < VLAD_LIST_LENGTH(exp_c) || exp_n != NULL)
            fprintf(a_fs, " %s\n    ", VLAD_STR_AND);
        }
        
        /* constraint negative condition */
        for (i_exp2 = 0; i_exp2 < VLAD_LIST_LENGTH(exp_n); i_exp2++) {
          if ((retval = exp_n->get(i_exp2, &fact)) != VLAD_OK)
            return retval;
        
          fprintf(a_fs, "%s ", VLAD_STR_NOT);
        
          print_fact(fact, i_sta, a_fs);
        
          if (i_exp2 + 1 < VLAD_LIST_LENGTH(exp_n))
            fprintf(a_fs, "%s\n    ", VLAD_STR_AND);
        }
        
        if (exp_c == NULL && exp_n == NULL)
          fprintf(a_fs, "%s%s\n", VLAD_STR_TRUE, VLAD_STR_TERMINATOR);
        else
          fprintf(a_fs, "%s\n", VLAD_STR_TERMINATOR);
      }
    }
  }

  fprintf(a_fs, "\n");

  return VLAD_OK;
}

/* generates policy update rules and prints them to a_fs */
int vlad_polbase::generate_update(FILE *a_fs)
{
  int retval = VLAD_OK;
  unsigned int i_sta;
  unsigned int i_tup;
  unsigned int i_exp1;
  unsigned int i_exp2;

  if (a_fs == NULL)
    return VLAD_NULLPTR;

  if (m_stage < 3)
    return VLAD_INVALIDOP;

  /* state loop */
  for (i_sta = 0; retval == VLAD_OK && i_sta < VLAD_LIST_LENGTH(m_setable); i_sta++) {
    char *name;
    vlad_fact *fact;
    vlad_expression *exp_pr = NULL;
    vlad_expression *exp_po = NULL;
    vlad_varlist *vlist1 = NULL;
    vlad_varlist *vlist2 = NULL;
    vlad_stringlist *ilist;

    /* get the details of this update sequence */
    if ((retval = m_setable->get(i_sta, &name, &ilist)) != VLAD_OK)
      return retval;

    /* get the details of this update */
    if ((retval = m_utable->get(name, &vlist1, &vlist2, &exp_pr, &exp_po)) != VLAD_OK)
      return retval;

    /* check if there are no unbound variables */
    if (vlist2 == NULL || vlist2->length() == 0) {
      vlad_expression *exp_tpr = NULL;
      vlad_expression *exp_tpo = NULL;

      /* the only variables that exist (if any) are already grounded */
      if (retval == VLAD_OK && exp_pr != NULL)
        retval = exp_pr->vreplace(m_stable, vlist1, ilist, &exp_tpr);
      if (retval == VLAD_OK && exp_po != NULL)
        retval = exp_po->vreplace(m_stable, vlist1, ilist, &exp_tpo);

      /* postcondition loop */
      for (i_exp1 = 0; retval == VLAD_OK && i_exp1 < VLAD_LIST_LENGTH(exp_tpo); i_exp1++) {
        if (retval == VLAD_OK)
          retval = exp_tpo->get(i_exp1, &fact);
        if (retval == VLAD_OK) {
          fprintf(a_fs, "  ");
          print_fact(fact, i_sta + 1, a_fs);
          fprintf(a_fs, " %s\n    ", VLAD_STR_ARROW);
        }
  
        /* precondition loop */
        for (i_exp2 = 0; retval == VLAD_OK && i_exp2 < VLAD_LIST_LENGTH(exp_tpr); i_exp2++) {
          if (retval == VLAD_OK)
            retval = exp_tpr->get(i_exp2, &fact);
          if (retval == VLAD_OK)
            print_fact(fact, i_sta, a_fs);
          if (retval == VLAD_OK && i_exp2 + 1 < VLAD_LIST_LENGTH(exp_pr))
            fprintf(a_fs, " %s\n    ", VLAD_STR_AND);
        }
  
        if (retval == VLAD_OK) {
          if (exp_tpr == NULL || exp_tpr->length() == 0)
            fprintf(a_fs, "%s%s\n", VLAD_STR_TRUE, VLAD_STR_TERMINATOR);
          else
            fprintf(a_fs, "%s\n", VLAD_STR_TERMINATOR);
        }
      }

      /* cleanup */
      if (exp_tpr != NULL)
        VLAD_MEM_DELETE(exp_tpr);
      if (exp_tpo != NULL)
        VLAD_MEM_DELETE(exp_tpo);

      if (retval != VLAD_OK)
        return retval;
    }
    else {
      vlad_stringlistlist *tlist;

      /* generate tuples */
      if (retval == VLAD_OK)
        retval = m_stable->tupleate(vlist2, &tlist);

      /* after generating the tuples, add vlist1 back to vlist2 */
      if (retval == VLAD_OK)
        retval = vlist2->add(vlist1);
      
      /* go through each tuple */
      for (i_tup = 0; retval == VLAD_OK && i_tup < VLAD_LIST_LENGTH(tlist); i_tup++) {
        vlad_expression *exp_tpr = NULL;
        vlad_expression *exp_tpo = NULL;
        vlad_stringlist *tuple;
      
        if (retval == VLAD_OK)
          retval = tlist->get(i_tup, &tuple);
      
        /* now we append each tuple with the ilist from the sequence */
        if (retval == VLAD_OK)
          retval = tuple->add(ilist);
      
        /* replace */
        if (retval == VLAD_OK && exp_pr != NULL)
          retval = exp_pr->vreplace(m_stable, vlist2, tuple, &exp_tpr);
        if (retval == VLAD_OK && exp_po != NULL)
          retval = exp_po->vreplace(m_stable, vlist2, tuple, &exp_tpo);

        /* postcondition loop */
        for (i_exp1 = 0; retval == VLAD_OK && i_exp1 < VLAD_LIST_LENGTH(exp_tpo); i_exp1++) {
          if (retval == VLAD_OK)
            retval = exp_tpo->get(i_exp1, &fact);
          if (retval == VLAD_OK) {
            fprintf(a_fs, "  ");
            print_fact(fact, i_sta + 1, a_fs);
            fprintf(a_fs, " %s\n    ", VLAD_STR_ARROW);
          }
          
          /* precondition loop */
          for (i_exp2 = 0; retval == VLAD_OK && i_exp2 < VLAD_LIST_LENGTH(exp_tpr); i_exp2++) {
            if (retval == VLAD_OK)
              retval = exp_tpr->get(i_exp2, &fact);
            if (retval == VLAD_OK)
              print_fact(fact, i_sta, a_fs);
            if (retval == VLAD_OK && i_exp2 + 1 < VLAD_LIST_LENGTH(exp_pr))
              fprintf(a_fs, " %s\n    ", VLAD_STR_AND);
          }
          
          if (retval == VLAD_OK) {
            if (exp_tpr == NULL || exp_tpr->length() == 0)
              fprintf(a_fs, "%s%s\n", VLAD_STR_TRUE, VLAD_STR_TERMINATOR);
            else
              fprintf(a_fs, "%s\n", VLAD_STR_TERMINATOR);
          }
        }

        /* cleanup */
        if (exp_tpr != NULL)
          VLAD_MEM_DELETE(exp_tpr);
        if (exp_tpo != NULL)
          VLAD_MEM_DELETE(exp_tpo);
      }

      /* more cleanup */
      if (tlist != NULL)
        VLAD_MEM_DELETE(tlist);
    }

    /* yet even more more cleanup */
    if (vlist1 != NULL)
      VLAD_MEM_DELETE(vlist1);
    if (vlist2 != NULL)
      VLAD_MEM_DELETE(vlist2);

    if (retval != VLAD_OK)
      return retval;
  }

  fprintf(a_fs, "\n");

  return VLAD_OK;
}

#ifdef VLAD_SMODELS

/* registers identity rules in smodels object for evaluation */
int vlad_polbase::evaluate_identity()
{
  int retval;
  unsigned int i_sta;
  unsigned int i_grp;
  unsigned int id;

  /* we only allow this after policy base is closed */
  if (m_stage != 3 && m_stage != 4)
    return VLAD_INVALIDOP;

  /* state loop */
  for (i_sta = 0; i_sta <= VLAD_LIST_LENGTH(m_setable); i_sta++) {
    /* subject groups */
    for (i_grp = 0; i_grp < VLAD_LEN_SG; i_grp++) {
      id = m_mapper->compute_subst(VLAD_IDENT_SUB, i_grp, i_grp, i_sta, true);

      if ((retval = m_smobject->add_axiom(true, 1, id)) != VLAD_OK)
        return retval;
    }

    /* access groups */
    for (i_grp = 0; i_grp < VLAD_LEN_AG; i_grp++) {
      id = m_mapper->compute_subst(VLAD_IDENT_ACC, i_grp, i_grp, i_sta, true);

      if ((retval = m_smobject->add_axiom(true, 1, id)) != VLAD_OK)
        return retval;
    }

    /* object groups */
    for (i_grp = 0; i_grp < VLAD_LEN_OG; i_grp++) {
      id = m_mapper->compute_subst(VLAD_IDENT_OBJ, i_grp, i_grp, i_sta, true);

      if ((retval = m_smobject->add_axiom(true, 1, id)) != VLAD_OK)
        return retval;
    }
  }

  return VLAD_OK;
}

/* registers inheritance rules in smodels object for evaluation */
int vlad_polbase::evaluate_inheritance()
{
  int retval;
  unsigned int i_sta;
  unsigned int i_grp1;
  unsigned int i_grp2;
  unsigned int i_sub;
  unsigned int i_acc;
  unsigned int i_obj;
  unsigned int id[4];

  /* we only allow this after policy base is closed */
  if (m_stage != 3 && m_stage != 4)
    return VLAD_INVALIDOP;

  /* state loop */
  for (i_sta = 0; i_sta <= VLAD_LIST_LENGTH(m_setable); i_sta++) {
    /* subset inheritance */

    /* subject groups */
    for (i_grp1 = 0; i_grp1 < VLAD_LEN_SG; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < VLAD_LEN_SG; i_grp2++) {
        if (i_grp1 == i_grp2)
          continue;
        for (i_acc = 0; i_acc < VLAD_LEN_AS + VLAD_LEN_AG; i_acc++) {
          for (i_obj = 0; i_obj < VLAD_LEN_OS + VLAD_LEN_OG; i_obj++) {
            /* positive */
            id[0] = m_mapper->compute_holds(i_grp1 + VLAD_LEN_SS, i_acc, i_obj, i_sta, true);
            id[1] = m_mapper->compute_holds(i_grp2 + VLAD_LEN_SS, i_acc, i_obj, i_sta, true);
            id[2] = m_mapper->compute_subst(VLAD_IDENT_SUB, i_grp1, i_grp2, i_sta, true);
            id[3] = m_mapper->compute_holds(i_grp1 + VLAD_LEN_SS, i_acc, i_obj, i_sta, false);

            retval = m_smobject->add_rule(2, 1, id[0], id[1], id[2], id[3]);

            if (retval != VLAD_OK)
              return retval;

            /* negative */
            id[0] = m_mapper->compute_holds(i_grp1 + VLAD_LEN_SS, i_acc, i_obj, i_sta, false);
            id[1] = m_mapper->compute_holds(i_grp2 + VLAD_LEN_SS, i_acc, i_obj, i_sta, false);
            id[2] = m_mapper->compute_subst(VLAD_IDENT_SUB, i_grp1, i_grp2, i_sta, true);

            retval = m_smobject->add_rule(2, 0, id[0], id[1], id[2]);

            if (retval != VLAD_OK)
              return retval;
          }
        }
      }
    }
    /* access groups */
    for (i_grp1 = 0; i_grp1 < VLAD_LEN_AG; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < VLAD_LEN_AG; i_grp2++) {
        if (i_grp1 == i_grp2)
          continue;
        for (i_sub = 0; i_sub < VLAD_LEN_SS + VLAD_LEN_SG; i_sub++) {
          for (i_obj = 0; i_obj < VLAD_LEN_OS + VLAD_LEN_OG; i_obj++) {
            /* positive */

            id[0] = m_mapper->compute_holds(i_sub, i_grp1 + VLAD_LEN_AS, i_obj, i_sta, true);
            id[1] = m_mapper->compute_holds(i_sub, i_grp2 + VLAD_LEN_AS, i_obj, i_sta, true);
            id[2] = m_mapper->compute_subst(VLAD_IDENT_ACC, i_grp1, i_grp2, i_sta, true);
            id[3] = m_mapper->compute_holds(i_sub, i_grp1 + VLAD_LEN_AS, i_obj, i_sta, false);

            retval = m_smobject->add_rule(2, 1, id[0], id[1], id[2], id[3]);

            if (retval != VLAD_OK)
              return retval;

            /* negative */
            id[0] = m_mapper->compute_holds(i_sub, i_grp1 + VLAD_LEN_AS, i_obj, i_sta, false);
            id[1] = m_mapper->compute_holds(i_sub, i_grp2 + VLAD_LEN_AS, i_obj, i_sta, false);
            id[2] = m_mapper->compute_subst(VLAD_IDENT_ACC, i_grp1, i_grp2, i_sta, true);

            retval = m_smobject->add_rule(2, 0, id[0], id[1], id[2]);

            if (retval != VLAD_OK)
              return retval;
          }
        }
      }
    }
    /* object groups */
    for (i_grp1 = 0; i_grp1 < VLAD_LEN_OG; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < VLAD_LEN_OG; i_grp2++) {
        if (i_grp1 == i_grp2)
          continue;
        for (i_sub = 0; i_sub < VLAD_LEN_SS + VLAD_LEN_SG; i_sub++) {
          for (i_acc = 0; i_acc < VLAD_LEN_AS + VLAD_LEN_AG; i_acc++) {
            /* positive */
            id[0] = m_mapper->compute_holds(i_sub, i_acc, i_grp1 + VLAD_LEN_OS, i_sta, true);
            id[1] = m_mapper->compute_holds(i_sub, i_acc, i_grp2 + VLAD_LEN_OS, i_sta, true);
            id[2] = m_mapper->compute_subst(VLAD_IDENT_OBJ, i_grp1, i_grp2, i_sta, true);
            id[3] = m_mapper->compute_holds(i_sub, i_acc, i_grp1 + VLAD_LEN_OS, i_sta, false);

            retval = m_smobject->add_rule(2, 1, id[0], id[1], id[2], id[3]);

            if (retval != VLAD_OK)
              return retval;

            /* negative */
            id[0] = m_mapper->compute_holds(i_sub, i_acc, i_grp1 + VLAD_LEN_OS, i_sta, false);
            id[1] = m_mapper->compute_holds(i_sub, i_acc, i_grp2 + VLAD_LEN_OS, i_sta, false);
            id[2] = m_mapper->compute_subst(VLAD_IDENT_OBJ, i_grp1, i_grp2, i_sta, true);

            retval = m_smobject->add_rule(2, 0, id[0], id[1], id[2]);

            if (retval != VLAD_OK)
              return retval;
          }
        }
      }
    }

    /* member inheritance */

    /* subject groups */
    for (i_grp1 = 0; i_grp1 < VLAD_LEN_SG; i_grp1++) {
      for (i_sub = 0; i_sub < VLAD_LEN_SS; i_sub++) {
        for (i_acc = 0; i_acc < VLAD_LEN_AS + VLAD_LEN_AG; i_acc++) {
          for (i_obj = 0; i_obj < VLAD_LEN_OS + VLAD_LEN_OG; i_obj++) {
            /* positive */
            id[0] = m_mapper->compute_holds(i_sub, i_acc, i_obj, i_sta, true);
            id[1] = m_mapper->compute_holds(i_grp1 + VLAD_LEN_SS, i_acc, i_obj, i_sta, true);
            id[2] = m_mapper->compute_memb(VLAD_IDENT_SUB, i_sub, i_grp1, i_sta, true);
            id[3] = m_mapper->compute_holds(i_sub, i_acc, i_obj, i_sta, false);

            retval = m_smobject->add_rule(2, 1, id[0], id[1], id[2], id[3]);

            if (retval != VLAD_OK)
              return retval;

            /* negative */
            id[0] = m_mapper->compute_holds(i_sub, i_acc, i_obj, i_sta, false);
            id[1] = m_mapper->compute_holds(i_grp1 + VLAD_LEN_SS, i_acc, i_obj, i_sta, false);
            id[2] = m_mapper->compute_memb(VLAD_IDENT_SUB, i_sub, i_grp1, i_sta, true);

            retval = m_smobject->add_rule(2, 0, id[0], id[1], id[2]);

            if (retval != VLAD_OK)
              return retval;
          }
        }
      }
    }
    /* access groups */
    for (i_grp1 = 0; i_grp1 < VLAD_LEN_AG; i_grp1++) {
      for (i_sub = 0; i_sub < VLAD_LEN_SS + VLAD_LEN_SG; i_sub++) {
        for (i_acc = 0; i_acc < VLAD_LEN_AS; i_acc++) {
          for (i_obj = 0; i_obj < VLAD_LEN_OS + VLAD_LEN_OG; i_obj++) {
            /* positive */
            id[0] = m_mapper->compute_holds(i_sub, i_acc, i_obj, i_sta, true);
            id[1] = m_mapper->compute_holds(i_sub, i_grp1 + VLAD_LEN_AS, i_obj, i_sta, true);
            id[2] = m_mapper->compute_memb(VLAD_IDENT_ACC, i_acc, i_grp1, i_sta, true);
            id[3] = m_mapper->compute_holds(i_sub, i_acc, i_obj, i_sta, false);

            retval = m_smobject->add_rule(2, 1, id[0], id[1], id[2], id[3]);

            if (retval != VLAD_OK)
              return retval;

            /* negative */
            id[0] = m_mapper->compute_holds(i_sub, i_acc, i_obj, i_sta, false);
            id[1] = m_mapper->compute_holds(i_sub, i_grp1 + VLAD_LEN_AS, i_obj, i_sta, false);
            id[2] = m_mapper->compute_memb(VLAD_IDENT_ACC, i_acc, i_grp1, i_sta, true);

            retval = m_smobject->add_rule(2, 0, id[0], id[1], id[2]);

            if (retval != VLAD_OK)
              return retval;
          }
        }
      }
    }
    /* object groups */
    for (i_grp1 = 0; i_grp1 < VLAD_LEN_OG; i_grp1++) {
      for (i_sub = 0; i_sub < VLAD_LEN_SS + VLAD_LEN_SG; i_sub++) {
        for (i_acc = 0; i_acc < VLAD_LEN_AS + VLAD_LEN_AG; i_acc++) {
          for (i_obj = 0; i_obj < VLAD_LEN_OS; i_obj++) {
            /* positive */
            id[0] = m_mapper->compute_holds(i_sub, i_acc, i_obj, i_sta, true);
            id[1] = m_mapper->compute_holds(i_sub, i_acc, i_grp1 + VLAD_LEN_OS, i_sta, true);
            id[2] = m_mapper->compute_memb(VLAD_IDENT_OBJ, i_obj, i_grp1, i_sta, true);
            id[3] = m_mapper->compute_holds(i_sub, i_acc, i_obj, i_sta, false);

            retval = m_smobject->add_rule(2, 1, id[0], id[1], id[2], id[3]);

            if (retval != VLAD_OK)
              return retval;

            /* negative */
            id[0] = m_mapper->compute_holds(i_sub, i_acc, i_obj, i_sta, false);
            id[1] = m_mapper->compute_holds(i_sub, i_acc, i_grp1 + VLAD_LEN_OS, i_sta, false);
            id[2] = m_mapper->compute_memb(VLAD_IDENT_OBJ, i_obj, i_grp1, i_sta, true);

            retval = m_smobject->add_rule(2, 0, id[0], id[1], id[2]);

            if (retval != VLAD_OK)
              return retval;                                                   
          }
        }
      }
    }
  }
  return VLAD_OK;
}

/* registers transitivity rules in smodels object for evaluation */
int vlad_polbase::evaluate_transitivity()
{
  int retval;
  unsigned int i_sta;
  unsigned int i_grp1;
  unsigned int i_grp2;
  unsigned int i_grp3;
  unsigned int id[3];

  /* we only allow this after policy base is closed */
  if (m_stage != 3 && m_stage != 4)
    return VLAD_INVALIDOP;

  /* state loop */
  for (i_sta = 0; i_sta <= VLAD_LIST_LENGTH(m_setable); i_sta++) {

    /* subject groups */
    for (i_grp1 = 0; i_grp1 < VLAD_LEN_SG; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < VLAD_LEN_SG; i_grp2++) {
        /* ignore if the 2 are the same */
        if (i_grp1 == i_grp2)
          continue;
        for (i_grp3 = 0; i_grp3 < VLAD_LEN_SG; i_grp3++) {
          /* ignore if any 2 are the same */
          if (i_grp1 == i_grp3 || i_grp2 == i_grp3)
            continue;

          id[0] = m_mapper->compute_subst(VLAD_IDENT_SUB, i_grp1, i_grp3, i_sta, true);
          id[1] = m_mapper->compute_subst(VLAD_IDENT_SUB, i_grp1, i_grp2, i_sta, true);
          id[2] = m_mapper->compute_subst(VLAD_IDENT_SUB, i_grp2, i_grp3, i_sta, true);

          retval = m_smobject->add_rule(2, 0, id[0], id[1], id[2]);

          if (retval != VLAD_OK)
            return retval;
        }
      }
    }
    /* access groups */
    for (i_grp1 = 0; i_grp1 < VLAD_LEN_AG; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < VLAD_LEN_AG; i_grp2++) {
        /* ignore if the 2 are the same */
        if (i_grp1 == i_grp2)
          continue;
        for (i_grp3 = 0; i_grp3 < VLAD_LEN_AG; i_grp3++) {
          /* ignore if any 2 are the same */
          if (i_grp1 == i_grp3 || i_grp2 == i_grp3)
            continue;

          id[0] = m_mapper->compute_subst(VLAD_IDENT_ACC, i_grp1, i_grp3, i_sta, true);
          id[1] = m_mapper->compute_subst(VLAD_IDENT_ACC, i_grp1, i_grp2, i_sta, true);
          id[2] = m_mapper->compute_subst(VLAD_IDENT_ACC, i_grp2, i_grp3, i_sta, true);

          retval = m_smobject->add_rule(2, 0, id[0], id[1], id[2]);

          if (retval != VLAD_OK)
            return retval;
        }
      }
    }
    /* object groups */
    for (i_grp1 = 0; i_grp1 < VLAD_LEN_OG; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < VLAD_LEN_OG; i_grp2++) {
        /* ignore if the 2 are the same */
        if (i_grp1 == i_grp2)
          continue;
        for (i_grp3 = 0; i_grp3 < VLAD_LEN_OG; i_grp3++) {
          /* ignore if any 2 are the same */
          if (i_grp1 == i_grp3 || i_grp2 == i_grp3)
            continue;

          id[0] = m_mapper->compute_subst(VLAD_IDENT_OBJ, i_grp1, i_grp3, i_sta, true);
          id[1] = m_mapper->compute_subst(VLAD_IDENT_OBJ, i_grp1, i_grp2, i_sta, true);
          id[2] = m_mapper->compute_subst(VLAD_IDENT_OBJ, i_grp2, i_grp3, i_sta, true);

          retval = m_smobject->add_rule(2, 0, id[0], id[1], id[2]);

          if (retval != VLAD_OK)
            return retval;
        }
      }
    }
  }

  return VLAD_OK;
}

/* registers negation rules in smodels object for evaluation */
int vlad_polbase::evaluate_negation()
{
  int retval;
  unsigned int i_sta;
  unsigned int i_fact;
  unsigned int id[2];

  /* we only allow this after policy base is closed */
  if (m_stage != 3 && m_stage != 4)
    return VLAD_INVALIDOP;

  /* state loop */
  for (i_sta = 0; i_sta <= VLAD_LIST_LENGTH(m_setable); i_sta++) {
    for (i_fact = 0; i_fact < m_mapper->get_totals(VLAD_ATOM_TOTAL); i_fact++) {
      id[0] = m_mapper->compute_fact(i_fact, i_sta, true);
      id[1] = m_mapper->compute_fact(i_fact, i_sta, false);

      if ((retval = m_smobject->add_axiom(false, 2, id[0], id[1])) != VLAD_OK)
        return retval;
    }
  }

  return VLAD_OK;
}

/* registers inertial rules in smodels object for evaluation */
int vlad_polbase::evaluate_inertial()
{
  int retval;
  unsigned int i_sta;
  unsigned int i_fact;
  unsigned int id[3];

  /* we only allow this after policy base is closed */
  if (m_stage != 3 && m_stage != 4)
    return VLAD_INVALIDOP;

  /* state loop */
  for (i_sta = 0; i_sta < VLAD_LIST_LENGTH(m_setable); i_sta++) {
    for (i_fact = 0; i_fact < m_mapper->get_totals(VLAD_ATOM_TOTAL); i_fact++) {
      /* positive */
      id[0] = m_mapper->compute_fact(i_fact, i_sta + 1, true);
      id[1] = m_mapper->compute_fact(i_fact, i_sta, true);
      id[2] = m_mapper->compute_fact(i_fact, i_sta + 1, false);

      if ((retval = m_smobject->add_rule(1, 1, id[0], id[1], id[2])) != VLAD_OK)
        return retval;

      /* negative */
      id[0] = m_mapper->compute_fact(i_fact, i_sta + 1, false);
      id[1] = m_mapper->compute_fact(i_fact, i_sta, false);
      id[2] = m_mapper->compute_fact(i_fact, i_sta + 1, true);

      if ((retval = m_smobject->add_rule(1, 1, id[0], id[1], id[2])) != VLAD_OK)
        return retval;
    }
  }

  return VLAD_OK;
}

/* registers initial state rules in smodels object for evaluation */
int vlad_polbase::evaluate_initial()
{
  int retval;
  unsigned int i_ini;
  unsigned int id;

  /* we only allow this after policy base is closed */
  if (m_stage != 3 && m_stage != 4)
    return VLAD_INVALIDOP;

  for (i_ini = 0; i_ini < VLAD_LIST_LENGTH(m_itable); i_ini++) {
    vlad_fact *fact;

    if ((retval = m_itable->get(i_ini, &fact)) != VLAD_OK)
      return retval;
    if ((retval = m_mapper->encode_fact(fact, 0, &id)) != VLAD_OK)
      return retval;
    if ((retval = m_smobject->add_axiom(true, 1, id)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

/* registers constraint rules in smodels object for evaluation */
int vlad_polbase::evaluate_constraint()
{
  int retval = VLAD_OK;
  unsigned int id;
  unsigned int i_sta;
  unsigned int i_con;
  unsigned int i_exp1;
  unsigned int i_exp2;
  unsigned int i_exp3;

  /* we only allow this after policy base is closed */
  if (m_stage != 3 && m_stage != 4)
    return VLAD_INVALIDOP;

  /* state loop */
  for (i_sta = 0; retval == VLAD_OK && i_sta <= VLAD_LIST_LENGTH(m_setable); i_sta++) {
    /* constraint loop */
    for (i_con = 0; retval == VLAD_OK && i_con < VLAD_LIST_LENGTH(m_ctable); i_con++) {
      vlad_expression *exp_e;
      vlad_expression *exp_c;
      vlad_expression *exp_n;
      vlad_fact *fact;

      if ((retval = m_ctable->get(i_con, &exp_e, &exp_c, &exp_n)) != VLAD_OK)
        return retval;

      /* constaint expression */
      for (i_exp1 = 0; retval == VLAD_OK && i_exp1 < VLAD_LIST_LENGTH(exp_e); i_exp1++) {
        if (retval == VLAD_OK)
          retval = exp_e->get(i_exp1, &fact);
        if (retval == VLAD_OK)
          retval = m_mapper->encode_fact(fact, i_sta, &id);
        if (retval == VLAD_OK)
          retval = m_smobject->construct_rule_begin(id);

        /* constraint positive condition */
        for (i_exp2 = 0; retval == VLAD_OK && i_exp2 < VLAD_LIST_LENGTH(exp_c); i_exp2++) {
          if (retval == VLAD_OK)
            retval = exp_c->get(i_exp2, &fact);
          if (retval == VLAD_OK)
            retval = m_mapper->encode_fact(fact, i_sta, &id);
          if (retval == VLAD_OK)
            retval = m_smobject->construct_rule_body(id, true);
        }
  
        /* constraint negative condition */
        for (i_exp3 = 0; retval == VLAD_OK && i_exp3 < VLAD_LIST_LENGTH(exp_n); i_exp3++) {
          if (retval == VLAD_OK)
            retval = exp_n->get(i_exp3, &fact);
          if (retval == VLAD_OK)
            retval = m_mapper->encode_fact(fact, i_sta, &id);
          if (retval == VLAD_OK)
            retval = m_smobject->construct_rule_body(id, false);
        }

        if (retval == VLAD_OK)
          retval = m_smobject->construct_rule_end();
      }
    }
  }

  return retval;
}

/* registers policy update rules in smodels object for evaluation */
int vlad_polbase::evaluate_update()
{
  int retval = VLAD_OK;
  unsigned int i_sta;
  unsigned int i_tup;
  unsigned int i_exp1;
  unsigned int i_exp2;
  unsigned int id;

  /* we only allow this after policy base is closed */
  if (m_stage != 3 && m_stage != 4)
    return VLAD_INVALIDOP;

  /* state loop */
  for (i_sta = 0; retval == VLAD_OK && i_sta < VLAD_LIST_LENGTH(m_setable); i_sta++) {
    char *name;
    vlad_fact *fact;
    vlad_expression *exp_pr = NULL;
    vlad_expression *exp_po = NULL;
    vlad_varlist *vlist1 = NULL;
    vlad_varlist *vlist2 = NULL;
    vlad_stringlist *ilist;

    /* get the details of this update sequence */
    if ((retval = m_setable->get(i_sta, &name, &ilist)) != VLAD_OK)
      return retval;

    /* get the details of this update */
    if ((retval = m_utable->get(name, &vlist1, &vlist2, &exp_pr, &exp_po)) != VLAD_OK)
      return retval;

    /* check if there are no unbound variables */
    if (vlist2 == NULL || vlist2->length() == 0) {
      vlad_expression *exp_tpr = NULL;
      vlad_expression *exp_tpo = NULL;

      /* the only varibables that exist (if any) are already grounded */
      if (retval == VLAD_OK && exp_pr != NULL)
        retval = exp_pr->vreplace(m_stable, vlist1, ilist, &exp_tpr);
      if (retval == VLAD_OK && exp_po != NULL)
        retval = exp_po->vreplace(m_stable, vlist1, ilist, &exp_tpo);

      /* postcondition loop */
      for (i_exp1 = 0; retval == VLAD_OK && i_exp1 < VLAD_LIST_LENGTH(exp_tpo); i_exp1++) {
        if (retval == VLAD_OK)
          retval = exp_tpo->get(i_exp1, &fact);
        if (retval == VLAD_OK)
          retval = m_mapper->encode_fact(fact, i_sta + 1, &id);
        if (retval == VLAD_OK) {
          retval = m_smobject->construct_rule_begin(id);
        }

        /* precondition loop */
        for (i_exp2 = 0; retval == VLAD_OK && i_exp2 < VLAD_LIST_LENGTH(exp_tpr); i_exp2++) {
          if (retval == VLAD_OK)
            retval = exp_tpr->get(i_exp2, &fact);
          if (retval == VLAD_OK)
            retval = m_mapper->encode_fact(fact, i_sta, &id);
          if (retval == VLAD_OK)
            retval = m_smobject->construct_rule_body(id, true);
        }

        if (retval == VLAD_OK)
          retval = m_smobject->construct_rule_end(); 
      }

      /* cleanup */
      if (exp_tpr != NULL)
        VLAD_MEM_DELETE(exp_tpr);
      if (exp_tpo != NULL)
        VLAD_MEM_DELETE(exp_tpo);

      if (retval != VLAD_OK)
        return retval;
    }
    else {
      vlad_stringlistlist *tlist;

      /* generate tuples */
      if (retval == VLAD_OK)
        retval = m_stable->tupleate(vlist2, &tlist);

      /* after generating the tuples, add vlist1 back to vlist2 */
      if (retval == VLAD_OK)
        retval = vlist2->add(vlist1);

      /* go through each tuple */
      for (i_tup = 0; retval == VLAD_OK && i_tup < VLAD_LIST_LENGTH(tlist); i_tup++) {
        vlad_expression *exp_tpr = NULL;
        vlad_expression *exp_tpo = NULL;
        vlad_stringlist *tuple;

        /* get this tuple */
        if (retval == VLAD_OK)
          retval = tlist->get(i_tup, &tuple);

        /* now we append each tuple with the ilist from the sequence */
        if (retval == VLAD_OK)
          retval = tuple->add(ilist);
      
        /* replace */
        if (retval == VLAD_OK && exp_pr != NULL)
          retval = exp_pr->vreplace(m_stable, vlist2, tuple, &exp_tpr);
        if (retval == VLAD_OK && exp_po != NULL)
          retval = exp_po->vreplace(m_stable, vlist2, tuple, &exp_tpo);

        /* postcondition loop */
        for (i_exp1 = 0; retval == VLAD_OK && i_exp1 < VLAD_LIST_LENGTH(exp_tpo); i_exp1++) {
          if (retval == VLAD_OK)
            retval = exp_tpo->get(i_exp1, &fact);
          if (retval == VLAD_OK)
            retval = m_mapper->encode_fact(fact, i_sta + 1, &id);
          if (retval == VLAD_OK)
            retval = m_smobject->construct_rule_begin(id);

          /* precondition loop */
          for (i_exp2 = 0; retval == VLAD_OK && i_exp2 < VLAD_LIST_LENGTH(exp_tpr); i_exp2++) {
            if (retval == VLAD_OK)
              retval = exp_tpr->get(i_exp2, &fact);
            if (retval == VLAD_OK)
              retval = m_mapper->encode_fact(fact, i_sta, &id);
            if (retval == VLAD_OK)
              retval = m_smobject->construct_rule_body(id, true);
          }

          if (retval == VLAD_OK)
            retval = m_smobject->construct_rule_end();
        }

        /* cleanup */
        if (exp_tpr != NULL)
          VLAD_MEM_DELETE(exp_tpr);
        if (exp_tpo != NULL)
          VLAD_MEM_DELETE(exp_tpo);
      }

      /* more cleanup */
      if (tlist != NULL)
        VLAD_MEM_DELETE(tlist);
    }

    /* yet even more more cleanup */
    if (vlist1 != NULL)
      VLAD_MEM_DELETE(vlist1);
    if (vlist2 != NULL)
      VLAD_MEM_DELETE(vlist2);

    if (retval != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

#endif
