/*
 * polbase.cpp
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <new>

#include <vlad/vlad.h>
#include <vlad/polbase.h>

/* some shorthand macros */
#define sslen (m_stable->length(VLAD_IDENT_SUB_SIN))
#define aslen (m_stable->length(VLAD_IDENT_ACC_SIN))
#define oslen (m_stable->length(VLAD_IDENT_OBJ_SIN))
#define sglen (m_stable->length(VLAD_IDENT_SUB_GRP))
#define aglen (m_stable->length(VLAD_IDENT_ACC_GRP))
#define oglen (m_stable->length(VLAD_IDENT_OBJ_GRP))

polbase::polbase()
{
  m_stage = 0;
  m_stable = NULL;
  m_itable = NULL;
  m_ctable = NULL;
  m_utable = NULL;
  m_setable = NULL;
#ifdef VLAD_SMODELS
  m_smobject = NULL;
#endif
}

polbase::~polbase()
{
  if (m_stable != NULL)
    delete m_stable;
  if (m_itable != NULL)
    delete m_itable;
  if (m_ctable != NULL)
    delete m_ctable;
  if (m_utable != NULL)
    delete m_utable;
  if (m_setable != NULL)
    delete m_setable;
#ifdef VLAD_SMODELS
  if (m_smobject != NULL)
    delete m_smobject;
#endif
}

/* (re)init the policy base */
int polbase::init()
{
  int i;
  int retval;

  /* reset all the totals */
  m_tot_atom = 0;
  for (i = VLAD_ATOM_FIRST; i <= VLAD_ATOM_LAST; i++)
    m_tot_atoms[i] = 0;

  /* initialise symbol table */
  if (m_stable != NULL)
    delete m_stable;
  if ((m_stable = VLAD_NEW(symtab())) == NULL)
    return VLAD_MALLOCFAILED;
  if ((retval = m_stable->init()) != VLAD_OK)
    return retval;

  /* initialise initial expression table */
  if (m_itable != NULL)
    delete m_itable;
  if ((m_itable = VLAD_NEW(expression())) == NULL)
    return VLAD_MALLOCFAILED;

  /* initialise constraints table */
  if (m_ctable != NULL)
    delete m_ctable;
  if ((m_ctable = VLAD_NEW(consttab())) == NULL)
    return VLAD_MALLOCFAILED;

  /* initialise update declaration table */
  if (m_utable != NULL)
    delete m_utable;
  if ((m_utable = VLAD_NEW(updatetab())) == NULL)
    return VLAD_MALLOCFAILED;

  /* sequence table */
  if (m_setable != NULL)
    delete m_setable;
  if ((m_setable = VLAD_NEW(seqtab())) == NULL)
    return VLAD_MALLOCFAILED;

#ifdef VLAD_SMODELS
  /* smodels smwrap */
  if (m_smobject != NULL)
    delete m_smobject;
  m_smobject = NULL;
#endif

  m_stage = 1;

  return VLAD_OK;
}

/* add an entity in the symbol table */
int polbase::add_symtab(const char *a_name, unsigned char a_type)
{
  if (m_stage != 1)
    return VLAD_INVALIDOP;

  return m_stable->add(a_name, a_type);
}

/* add a fact into the initial state table */
int polbase::add_inittab(fact *a_fact)
{
  int retval;
  fact *tmp;

  if (m_stage != 2)
    return VLAD_INVALIDOP;

  if (a_fact == NULL)
    return VLAD_NULLPTR;

  /* first check if the fact is valid */
  if ((retval = verify_fact(a_fact, NULL)) != VLAD_OK)
    return retval;

  /* now copy and add to inittab */
  if ((retval = a_fact->copy(&tmp)) != VLAD_OK)
    return retval;

  return m_itable->add(tmp);
}

/* add a constrant into the constraints table */
int polbase::add_consttab(expression *a_exp,
                          expression *a_cond,
                          expression *a_ncond)
{
  int retval;
  unsigned int i;
  expression *exp = NULL;
  expression *cond = NULL;
  expression *ncond = NULL;
  fact *tmp1;
  fact *tmp2;

  if (m_stage != 2)
    return VLAD_INVALIDOP;

  /* only a_exp is required to be non-null */
  if (a_exp == NULL)
    return VLAD_NULLPTR;

  /*
   * now, we must go through every fact of every exression to ensure
   * their validity. while we are going through them, we might as well
   * make a copy.
   */

  /* exression */
  if ((exp = VLAD_NEW(expression())) == NULL)
    return VLAD_MALLOCFAILED;

  for (i = 0; i < VLAD_LIST_LENGTH(a_exp); i++) {
    if ((retval = a_exp->get(i, &tmp1)) != VLAD_OK)
      return retval;
    if ((retval = verify_fact(tmp1, NULL)) != VLAD_OK)
      return retval;
    if ((retval = tmp1->copy(&tmp2)) != VLAD_OK)
      return retval;
    if ((retval = exp->add(tmp2)) != VLAD_OK)
      return retval;
  }

  /* condition */
  if (a_cond != NULL) {
    if ((cond = VLAD_NEW(expression())) == NULL)
      return VLAD_MALLOCFAILED;

    for (i = 0; i < VLAD_LIST_LENGTH(a_cond); i++) {
      if ((retval = a_cond->get(i, &tmp1)) != VLAD_OK)
        return retval;
      if ((retval = verify_fact(tmp1, NULL)) != VLAD_OK)
        return retval;
      if ((retval = tmp1->copy(&tmp2)) != VLAD_OK)
        return retval;
      if ((retval = cond->add(tmp2)) != VLAD_OK)
        return retval;
    }
  }

  /* negative condition */
  if (a_ncond != NULL) {
    if ((ncond = VLAD_NEW(expression())) == NULL)
      return VLAD_MALLOCFAILED;

    for (i = 0; i < VLAD_LIST_LENGTH(a_ncond); i++) {
      if ((retval = a_ncond->get(i, &tmp1)) != VLAD_OK)
        return retval;
      if ((retval = verify_fact(tmp1, NULL)) != VLAD_OK)
        return retval;
      if ((retval = tmp1->copy(&tmp2)) != VLAD_OK)
        return retval;
      if ((retval = ncond->add(tmp2)) != VLAD_OK)
        return retval;
    }
  }

  /* finally, we add the expressions into the cosntraints table */
  return m_ctable->add(exp, cond, ncond);
}

/* add an update declaration in the update table */
int polbase::add_updatetab(const char *a_name,
                           stringlist *a_vlist,
                           expression *a_precond,
                           expression *a_postcond)
{
  int retval;
  unsigned int i;
  char *name;
  stringlist *vlist = NULL;
  expression *precond = NULL;
  expression *postcond = NULL;

  /* we only allow this function after symtab is closed */
  if (m_stage != 2)
    return VLAD_INVALIDOP;

  /* precondition and vlist are allowed to be NULL */
  if (a_name == NULL || a_postcond == NULL)
    return VLAD_NULLPTR;

  /* copy name */
  if ((name = VLAD_STRING_MALLOC(a_name)) == NULL)
    return VLAD_MALLOCFAILED;

  strcpy(name, a_name);

  /* verify and copy varlist */
  if (a_vlist != NULL) {
    if ((vlist = VLAD_NEW(stringlist())) == NULL)
      return VLAD_MALLOCFAILED;

    for (i = 0; i < VLAD_LIST_LENGTH(a_vlist); i++) {
      char *tmp;
      if ((retval = a_vlist->get(i, &tmp)) != VLAD_OK)
        return retval;
      /* check if the variable is already used as an identifier */
      if ((retval = m_stable->find(tmp)) != VLAD_NOTFOUND)
        return (retval == VLAD_OK) ? VLAD_DUPLICATE : retval;
      if ((retval = vlist->add(tmp)) != VLAD_OK)
        return retval;
    }
  }

  /* verify and copy precondition */
  if (a_precond != NULL) {
    if ((precond = VLAD_NEW(expression())) == NULL)
      return VLAD_MALLOCFAILED;

    for (i = 0; i < VLAD_LIST_LENGTH(a_precond); i++) {
      fact *tmp1;
      fact *tmp2;
      if ((retval = a_precond->get(i, &tmp1)) != VLAD_OK)
        return retval;
      if ((retval = verify_fact(tmp1, a_vlist)) != VLAD_OK)
        return retval;
      if ((retval = tmp1->copy(&tmp2)) != VLAD_OK)
        return retval;
      if ((retval = precond->add(tmp2)) != VLAD_OK)
        return retval;
    }
  }

  /* verify and copy the postcondition */
  if ((postcond = VLAD_NEW(expression())) == NULL)
    return VLAD_MALLOCFAILED;

  for (i = 0; i < VLAD_LIST_LENGTH(a_postcond); i++) {
    fact *tmp1;
    fact *tmp2;
    if ((retval = a_postcond->get(i, &tmp1)) != VLAD_OK)
      return retval;
    if ((retval = verify_fact(tmp1, a_vlist)) != VLAD_OK)
      return retval;
    if ((retval = tmp1->copy(&tmp2)) != VLAD_OK)
      return retval;
    if ((retval = postcond->add(tmp2)) != VLAD_OK)
      return retval;
  }

  /* if all went well, add to the udate table */
  return m_utable->add(name, vlist, precond, postcond);
}

/* add an update reference to the sequence table */
int polbase::add_seqtab(updateref *a_uref)
{
  int retval;
  char *name;
  stringlist *ilist;

  /* we only allow this function after kb is closed */
  if (m_stage < 3)
    return VLAD_INVALIDOP;

  if (a_uref == NULL)
    return VLAD_NULLPTR;

  /* get components */
  if ((retval = a_uref->get(&name, &ilist)) != VLAD_OK)
    return retval;

  /* now verify the update */
  if ((retval = verify_updateref(name, ilist)) != VLAD_OK)
    return retval;

  /* if all is well, add */
  if ((retval = m_setable->add(a_uref)) != VLAD_OK)
    return retval;

  /* set back to m_stage 3 to prevent query before compute */
  m_stage = 3;

  return VLAD_OK;
}

/* close symbol table */
int polbase::close_symtab()
{
  int i;

  if (m_stage != 1)
    return VLAD_INVALIDOP;

  /* compute totals */
  m_tot_atoms[VLAD_ATOM_HOLDS] = (sslen + sglen) *
                                 (aslen + aglen) *
                                 (oslen + oglen);
  m_tot_atoms[VLAD_ATOM_MEMBER] = (sslen * sglen) +
                                  (aslen * aglen) +
                                  (oslen * oglen);
  m_tot_atoms[VLAD_ATOM_SUBSET]= (sglen * sglen) +
                                 (aglen * aglen) +
                                 (oglen * oglen);

  for (i = VLAD_ATOM_FIRST, m_tot_atom = 0; i <= VLAD_ATOM_LAST; i++)
    m_tot_atom = m_tot_atom + m_tot_atoms[i];

  m_stage = 2;

  return VLAD_OK;
}

/* after this is called, no further calls to add_inittab(), add_consttab()
 * or add_updatetab() is allowed */
int polbase::close_polbase()
{
  if (m_stage != 2)
    return VLAD_INVALIDOP;

  m_stage = 3;

  return VLAD_OK;
}

/* delete an update reference from the sequence table */
int polbase::del_seqtab(unsigned int a_index)
{
  int retval;

  /* we only allow this function after kb is closed */
  if (m_stage < 3)
    return VLAD_INVALIDOP;

  if ((retval = m_setable->del(a_index)) != VLAD_OK)
    return retval;

  /* set back to m_stage 3 to prevent query before compute */
  m_stage = 3;

  return retval;
}

/* gives an array of identifiers of the given type */
int polbase::get_symtab(unsigned char a_type,
                        char ***a_array,
                        unsigned int *a_size)
{
  if (m_stage < 2)
    return VLAD_INVALIDOP;

  return m_stable->get(a_type, a_array, a_size);
}

/* gives the index'th entry in the sequence table */
int polbase::get_seqtab(unsigned int a_index,
                        char **a_name,
                        stringlist **a_ilist)
{
  if (m_stage < 2)
    return VLAD_INVALIDOP;

  return m_setable->get(a_index, a_name, a_ilist);
}

/* gives the index'th entry in the update table */
int polbase::get_updatetab(unsigned int a_index,
                           char **a_name,
                           stringlist **a_vlist,
                           expression **a_precond,
                           expression **a_postcond)
{
  if (m_stage < 2)
    return VLAD_INVALIDOP;

  return m_utable->get(a_index, a_name, a_vlist, a_precond, a_postcond);
}

/* returns the length of the sequence table */
unsigned int polbase::length_seqtab()
{
  if (m_stage < 2)
    return 0;

  return m_setable->length();
}

/* returns the length of the update table */
unsigned int polbase::length_updatetab()
{
  if (m_stage < 2)
    return 0;

  return m_utable->length();
}

/* checks whether name of the given type is in symtab */
int polbase::check_symtab(const char *a_name, unsigned char a_type)
{
  if (m_stage < 2)
    return VLAD_INVALIDOP;

  return m_stable->find(a_name, a_type);
}

/* enumerate the sequences in the sequence table, output to file */
int polbase::list_seqtab(FILE *a_file)
{
  int retval;
  unsigned int i;
  unsigned int j;
  char *tmp_name;
  stringlist *tmp_ilist;
                                                                                  /* we only allow this function after kb is closed */
  if (m_stage < 3)
    return VLAD_INVALIDOP;
                                                                               
  if (a_file == NULL)
    return VLAD_NULLPTR;
                                                                               
  for (i = 0; i < VLAD_LIST_LENGTH(m_setable); i++) {
    /* get the update ref */
    if ((retval = m_setable->get(i, &tmp_name, &tmp_ilist)) != VLAD_OK)
      return retval;
    /* now print */
    fprintf(a_file, "%d %s(", i, tmp_name);
    /* also do the stringlist */
    for (j = 0; j < VLAD_LIST_LENGTH(tmp_ilist); j++) {
      char *tmp_arg;
      if ((retval = tmp_ilist->get(j, &tmp_arg)) != VLAD_OK) {
        fprintf(a_file, ")\n");
        return retval;
      }
      fprintf(a_file, "%s%s", ((j == 0) ? "" : ","), tmp_arg);
    }
    fprintf(a_file, ")\n");
  }
                                                                               
  return VLAD_OK;
}

/* generate the rules necessary to evaluate queries */
int polbase::compute_generate(FILE *a_file)
{
  return VLAD_OK;
}

/* generate the query */
int polbase::query_generate(expression *a_exp, FILE *a_file)
{
  return VLAD_OK;
}

#ifdef VLAD_SMODELS
/* prepares the kb for queries */
int polbase::compute_evaluate()
{
  int retval;
  unsigned int i;

  /* we only allow this after kb is closed */
  if (m_stage != 3 && m_stage != 4)
    return VLAD_INVALIDOP;

  /* create a new instance of the smodels smwrap and init it */
  if (m_smobject != NULL)
    delete m_smobject;
  if ((m_smobject = VLAD_NEW(smwrap())) == NULL)
    return VLAD_MALLOCFAILED;
  if ((retval = m_smobject->init()) != VLAD_OK)
    return retval;

  /* first we register all the possible atoms in smodels */
  for (i = 0; i < (m_tot_atom * 2 * (VLAD_LIST_LENGTH(m_setable) + 1)); i++) {
    if ((retval = m_smobject->add_atom(i)) != VLAD_OK)
      return retval;
  }
  m_smobject->close_atom();

  /* now *sigh* we register all the built-in rules */

  /* identity rules */

  /* state loop */
  for (i = 0; i <= VLAD_LIST_LENGTH(m_setable); i++) {
    unsigned int i_grp;
    /* subject groups */
    for (i_grp = 0; i_grp < sglen; i_grp++) {
      unsigned int tmp_num = compute_subst(i,
                                           true,
                                           VLAD_IDENT_SUB_SIN,
                                           i_grp,
                                           i_grp);
      if ((retval = m_smobject->add_axiom(true, 1, tmp_num)) != VLAD_OK)
        return retval;
    }
    /* access groups */
    for (i_grp = 0; i_grp < aglen; i_grp++) {
      unsigned int tmp_num = compute_subst(i,
                                           true,
                                           VLAD_IDENT_ACC_SIN,
                                           i_grp,
                                           i_grp);
      if ((retval = m_smobject->add_axiom(true, 1, tmp_num)) != VLAD_OK)
        return retval;
    }
    /* object groups */
    for (i_grp = 0; i_grp < oglen; i_grp++) {
      unsigned int tmp_num = compute_subst(i,
                                           true,
                                           VLAD_IDENT_OBJ_SIN,
                                           i_grp,
                                           i_grp);
      if ((retval = m_smobject->add_axiom(true, 1, tmp_num)) != VLAD_OK)
        return retval;
    }
  }

  /* inheritance rules */

  /* state loop */
  for (i = 0; i <= VLAD_LIST_LENGTH(m_setable); i++) {
    unsigned int i_grp1;
    unsigned int i_grp2;
    unsigned int i_sub;
    unsigned int i_acc;
    unsigned int i_obj;

    /* subset inheritance */

    /* subject groups */
    for (i_grp1 = 0; i_grp1 < sglen; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < sglen; i_grp2++) {
        if (i_grp1 == i_grp2)
          continue;
        for (i_acc = 0; i_acc < aslen + aglen; i_acc++) {
          for (i_obj = 0; i_obj < oslen + oglen; i_obj++) {
            /* positive */
            if ((retval = m_smobject->add_rule(2,
                                               1,
                                               compute_holds(i, true, i_grp1 + sslen, i_acc, i_obj),
                                               compute_holds(i, true, i_grp2 + sslen, i_acc, i_obj),
                                               compute_subst(i, true, VLAD_IDENT_SUB_SIN, i_grp1, i_grp2),
                                               compute_holds(i, false, i_grp1 + sslen, i_acc, i_obj))) != VLAD_OK)
              return retval;
            /* negative */
            if ((retval = m_smobject->add_rule(2,
                                               0,
                                               compute_holds(i, false, i_grp1 + sslen, i_acc, i_obj),
                                               compute_holds(i, false, i_grp2 + sslen, i_acc, i_obj),
                                               compute_subst(i, true, VLAD_IDENT_SUB_SIN, i_grp1, i_grp2))) != VLAD_OK)
              return retval;
          }
        }
      }
    }
    /* access groups */
    for (i_grp1 = 0; i_grp1 < aglen; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < aglen; i_grp2++) {
        if (i_grp1 == i_grp2)
          continue;
        for (i_sub = 0; i_sub < sslen + sglen; i_sub++) {
          for (i_obj = 0; i_obj < oslen + oglen; i_obj++) {
            /* positive */
            if ((retval = m_smobject->add_rule(2,
                                               1,
                                               compute_holds(i, true, i_sub, i_grp1 + aslen, i_obj),
                                               compute_holds(i, true, i_sub, i_grp2 + aslen, i_obj),
                                               compute_subst(i, true, VLAD_IDENT_ACC_SIN, i_grp1, i_grp2),
                                               compute_holds(i, false, i_sub, i_grp1 + aslen, i_obj))) != VLAD_OK)
              return retval;
            /* negative */
            if ((retval = m_smobject->add_rule(2,
                                               0,
                                               compute_holds(i, false, i_sub, i_grp1 + aslen, i_obj),
                                               compute_holds(i, false, i_sub, i_grp2 + aslen, i_obj),
                                               compute_subst(i, true, VLAD_IDENT_ACC_SIN, i_grp1, i_grp2))) != VLAD_OK)
              return retval;
          }
        }
      }
    }
    /* object groups */
    for (i_grp1 = 0; i_grp1 < oglen; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < oglen; i_grp2++) {
        if (i_grp1 == i_grp2)
          continue;
        for (i_sub = 0; i_sub < sslen + sglen; i_sub++) {
          for (i_acc = 0; i_acc < aslen + aglen; i_acc++) {
            /* positive */
            if ((retval = m_smobject->add_rule(2,
                                               1,
                                               compute_holds(i, true, i_sub, i_acc, i_grp1 + oslen),
                                               compute_holds(i, true, i_sub, i_acc, i_grp2 + oslen),
                                               compute_subst(i, true, VLAD_IDENT_OBJ_SIN, i_grp1, i_grp2),
                                               compute_holds(i, false, i_sub, i_acc, i_grp1 + oslen))) != VLAD_OK)
              return retval;
            /* negative */
            if ((retval = m_smobject->add_rule(2,
                                               0,
                                               compute_holds(i, false, i_sub, i_acc, i_grp1 + oslen),
                                               compute_holds(i, false, i_sub, i_acc, i_grp2 + oslen),
                                               compute_subst(i, true, VLAD_IDENT_OBJ_SIN, i_grp1, i_grp2))) != VLAD_OK)
              return retval;
          }
        }
      }
    }
    /* member inheritance */

    /* subject groups */
    for (i_grp1 = 0; i_grp1 < sglen; i_grp1++) {
      for (i_sub = 0; i_sub < sslen; i_sub++) {
        for (i_acc = 0; i_acc < aslen + aglen; i_acc++) {
          for (i_obj = 0; i_obj < oslen + oglen; i_obj++) {
            /* positive */
            if ((retval = m_smobject->add_rule(2,
                                               1,
                                               compute_holds(i, true, i_sub, i_acc, i_obj),
                                               compute_holds(i, true, i_grp1 + sslen, i_acc, i_obj),
                                               compute_memb(i, true, VLAD_IDENT_SUB_SIN, i_sub, i_grp1),
                                               compute_holds(i, false, i_sub, i_acc, i_obj))) != VLAD_OK)
              return retval;
            /* negative */
            if ((retval = m_smobject->add_rule(2,
                                               0,
                                               compute_holds(i, false, i_sub, i_acc, i_obj),
                                               compute_holds(i, false, i_grp1 + sslen, i_acc, i_obj),
                                               compute_memb(i, true, VLAD_IDENT_SUB_SIN, i_sub, i_grp1))) != VLAD_OK)
              return retval;
          }
        }
      }
    }
    /* access groups */
    for (i_grp1 = 0; i_grp1 < aglen; i_grp1++) {
      for (i_sub = 0; i_sub < sslen + sglen; i_sub++) {
        for (i_acc = 0; i_acc < aslen; i_acc++) {
          for (i_obj = 0; i_obj < oslen + oglen; i_obj++) {
            /* positive */
            if ((retval = m_smobject->add_rule(2,
                                               1,
                                               compute_holds(i, true, i_sub, i_acc, i_obj),
                                               compute_holds(i, true, i_sub, i_grp1 + aslen, i_obj),
                                               compute_memb(i, true, VLAD_IDENT_ACC_SIN, i_acc, i_grp1),
                                               compute_holds(i, false, i_sub, i_acc, i_obj))) != VLAD_OK)
              return retval;
            /* negative */
            if ((retval = m_smobject->add_rule(2,
                                               0,
                                               compute_holds(i, false, i_sub, i_acc, i_obj),
                                               compute_holds(i, false, i_sub, i_grp1 + aslen, i_obj),
                                               compute_memb(i, true, VLAD_IDENT_ACC_SIN, i_acc, i_grp1))) != VLAD_OK)
              return retval;
          }
        }
      }
    }
    /* object groups */
    for (i_grp1 = 0; i_grp1 < oglen; i_grp1++) {
      for (i_sub = 0; i_sub < sslen + sglen; i_sub++) {
        for (i_acc = 0; i_acc < aslen + aglen; i_acc++) {
          for (i_obj = 0; i_obj < oslen; i_obj++) {
            /* positive */
            if ((retval = m_smobject->add_rule(2,
                                               1,
                                               compute_holds(i, true, i_sub, i_acc, i_obj),
                                               compute_holds(i, true, i_sub, i_acc, i_grp1 + oslen),
                                               compute_memb(i, true, VLAD_IDENT_OBJ_SIN, i_obj, i_grp1),
                                               compute_holds(i, false, i_sub, i_acc, i_obj))) != VLAD_OK)
              return retval;
            /* negative */
            if ((retval = m_smobject->add_rule(2,
                                               0,
                                               compute_holds(i, false, i_sub, i_acc, i_obj),
                                               compute_holds(i, false, i_sub, i_acc, i_grp1 + oslen),
                                               compute_memb(i, true, VLAD_IDENT_OBJ_SIN, i_obj, i_grp1))) != VLAD_OK)
              return retval;
          }
        }
      }
    }
  }

  /* transitivity rules */

  /* state loop */
  for (i = 0; i <= VLAD_LIST_LENGTH(m_setable); i++) {
    unsigned int i_grp1;
    unsigned int i_grp2;
    unsigned int i_grp3;
    /* subject groups */
    for (i_grp1 = 0; i_grp1 < sglen; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < sglen; i_grp2++) {
        /* ignore if the 2 are the same */
        if (i_grp1 == i_grp2)
          continue;
        for (i_grp3 = 0; i_grp3 < sglen; i_grp3++) {
          unsigned int tmp_num1;
          unsigned int tmp_num2;
          unsigned int tmp_num3;

          /* ignore if any 2 are the same */
          if (i_grp1 == i_grp3 || i_grp2 == i_grp3)
            continue;

          tmp_num1 = compute_subst(i, true, VLAD_IDENT_SUB_SIN, i_grp1, i_grp3);
          tmp_num2 = compute_subst(i, true, VLAD_IDENT_SUB_SIN, i_grp1, i_grp2);
          tmp_num3 = compute_subst(i, true, VLAD_IDENT_SUB_SIN, i_grp2, i_grp3);

          if ((retval = m_smobject->add_rule(2, 0, tmp_num1, tmp_num2, tmp_num3)) != VLAD_OK)
            return retval;
        }
      }
    }
    /* access groups */
    for (i_grp1 = 0; i_grp1 < aglen; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < aglen; i_grp2++) {
        /* ignore if the 2 are the same */
        if (i_grp1 == i_grp2)
          continue;
        for (i_grp3 = 0; i_grp3 < aglen; i_grp3++) {
          unsigned int tmp_num1;
          unsigned int tmp_num2;
          unsigned int tmp_num3;

          /* ignore if any 2 are the same */
          if (i_grp1 == i_grp3 || i_grp2 == i_grp3)
            continue;

          tmp_num1 = compute_subst(i, true, VLAD_IDENT_ACC_SIN, i_grp1, i_grp3);
          tmp_num2 = compute_subst(i, true, VLAD_IDENT_ACC_SIN, i_grp1, i_grp2);
          tmp_num3 = compute_subst(i, true, VLAD_IDENT_ACC_SIN, i_grp2, i_grp3);

          if ((retval = m_smobject->add_rule(2, 0, tmp_num1, tmp_num2, tmp_num3)) != VLAD_OK)
            return retval;
        }
      }
    }
    /* object groups */
    for (i_grp1 = 0; i_grp1 < oglen; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < oglen; i_grp2++) {
        /* ignore if the 2 are the same */
        if (i_grp1 == i_grp2)
          continue;
        for (i_grp3 = 0; i_grp3 < oglen; i_grp3++) {
          unsigned int tmp_num1;
          unsigned int tmp_num2;
          unsigned int tmp_num3;

          /* ignore if any 2 are the same */
          if (i_grp1 == i_grp3 || i_grp2 == i_grp3)
            continue;

          tmp_num1 = compute_subst(i, true, VLAD_IDENT_OBJ_SIN, i_grp1, i_grp3);
          tmp_num2 = compute_subst(i, true, VLAD_IDENT_OBJ_SIN, i_grp1, i_grp2);
          tmp_num3 = compute_subst(i, true, VLAD_IDENT_OBJ_SIN, i_grp2, i_grp3);

          if ((retval = m_smobject->add_rule(2, 0, tmp_num1, tmp_num2, tmp_num3)) != VLAD_OK)
            return retval;
        }
      }
    }
  }

  /* negation rules */

  /* state loop */
  for (i = 0; i <= VLAD_LIST_LENGTH(m_setable); i++) {
    unsigned int i_fact;
    for (i_fact = 0; i_fact < m_tot_atom; i_fact++) {
      unsigned int tmp_num1 = compute_fact(i, true, i_fact);
      unsigned int tmp_num2 = compute_fact(i, false, i_fact);
      if ((retval = m_smobject->add_axiom(false, 2, tmp_num1, tmp_num2)) != VLAD_OK)
        return retval;
    }
  }

  /* inertial rules */

  /* state loop */
  for (i = 0; i < VLAD_LIST_LENGTH(m_setable); i++) {
    unsigned int i_fact;
    for (i_fact = 0; i_fact < m_tot_atom; i_fact++) {
      unsigned int tmp_num1;
      unsigned int tmp_num2;
      unsigned int tmp_num3;

      tmp_num1 = compute_fact(i + 1, true, i_fact);
      tmp_num2 = compute_fact(i, true, i_fact);
      tmp_num3 = compute_fact(i + 1, false, i_fact);

      if ((retval = m_smobject->add_rule(1, 1, tmp_num1, tmp_num2, tmp_num3)) != VLAD_OK)
        return retval;

      tmp_num1 = compute_fact(i + 1, false, i_fact);
      tmp_num2 = compute_fact(i, false, i_fact);
      tmp_num3 = compute_fact(i + 1, true, i_fact);

      if ((retval = m_smobject->add_rule(1, 1, tmp_num1, tmp_num2, tmp_num3)) != VLAD_OK)
        return retval;
    }
  }

  /* initial state */

  for (i = 0; i < VLAD_LIST_LENGTH(m_itable); i++) {
    fact *tmp_fact;
    unsigned int tmp_num;

    if ((retval = m_itable->get(i, &tmp_fact)) != VLAD_OK)
      return retval;
    if ((retval = encode_fact(tmp_fact, 0, &tmp_num)) != VLAD_OK)
      return retval;
    if ((retval = m_smobject->add_axiom(true, 1, tmp_num)) != VLAD_OK)
      return retval;
  }

  /* constraints */

  for (i = 0; i <= VLAD_LIST_LENGTH(m_setable); i++) {
    unsigned int  i_const;
    unsigned int i_exp;

    /* constraint loop */
    for (i_const = 0; i_const < VLAD_LIST_LENGTH(m_ctable); i_const++) {
      expression *tmp_e;
      expression *tmp_c;
      expression *tmp_n;
      fact *tmp_fact;
      unsigned int tmp_num;
      numberlist *tmp_list1;
      numberlist *tmp_list2;

      if ((tmp_list1 = VLAD_NEW(numberlist())) == NULL)
        return VLAD_MALLOCFAILED;
      if ((tmp_list2 = VLAD_NEW(numberlist())) == NULL)
        return VLAD_MALLOCFAILED;

      if ((retval = m_ctable->get(i_const, &tmp_e, &tmp_c, &tmp_n)) != VLAD_OK)
        return retval;

      /* constraint condition */
      for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_c); i_exp++) {
        if ((retval = tmp_c->get(i_exp, &tmp_fact)) != VLAD_OK)
          return retval;
        if ((retval = encode_fact(tmp_fact, i, &tmp_num)) != VLAD_OK)
          return retval;
        if ((retval = tmp_list1->add(tmp_num)) != VLAD_OK)
          return retval;
      }

      /* constraint negative condition */
      for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_n); i_exp++) {
        if ((retval = tmp_n->get(i_exp, &tmp_fact)) != VLAD_OK)
          return retval;
        if ((retval = encode_fact(tmp_fact, i, &tmp_num)) != VLAD_OK)
          return retval;
        if ((retval = tmp_list2->add(tmp_num)) != VLAD_OK)
          return retval;
      }

      /* constaint expression */
      for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_e); i_exp++) {
        if ((retval = tmp_e->get(i_exp, &tmp_fact)) != VLAD_OK)
          return retval;
        if ((retval = encode_fact(tmp_fact, i, &tmp_num)) != VLAD_OK)
          return retval;
	/* for every fact in the exression, we add a separate rule */
        if ((retval = m_smobject->add_rule(tmp_num, tmp_list1, tmp_list2)) != VLAD_OK)
          return retval;
      }

      delete tmp_list1;
      delete tmp_list2;
    }
  }

  /* update sequence rules */

  /* state loop */
  for (i = 0; i < VLAD_LIST_LENGTH(m_setable); i++) {
    unsigned int i_exp;
    char *tmp_name;
    fact *tmp_fact;
    unsigned int tmp_num;
    stringlist *tmp_ilist = NULL;
    expression *tmp_pre = NULL;
    expression *tmp_post = NULL;
    numberlist *tmp_list;

   if ((tmp_list = VLAD_NEW(numberlist())) == NULL)
     return VLAD_MALLOCFAILED;

    if ((retval = m_setable->get(i, &tmp_name, &tmp_ilist)) != VLAD_OK)
      return retval;

    if ((retval = m_utable->replace(tmp_name, tmp_ilist, &tmp_pre, &tmp_post)) != VLAD_OK)
      return retval;

    /* precondition loop */
    for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_pre); i_exp++) {
      if ((retval = tmp_pre->get(i_exp, &tmp_fact)) != VLAD_OK)
        return retval;
      if ((retval = encode_fact(tmp_fact, i, &tmp_num)) != VLAD_OK)
        return retval;
      if ((retval = tmp_list->add(tmp_num)) != VLAD_OK)
        return retval;
    }

    /* postcondition loop */
    for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_post); i_exp++) {
      if ((retval = tmp_post->get(i_exp, &tmp_fact)) != VLAD_OK)
        return retval;
      if ((retval = encode_fact(tmp_fact, i + 1, &tmp_num)) != VLAD_OK)
        return retval;
      /* for every fact in the postcondition we add a rule */
      if ((retval = m_smobject->add_rule(tmp_num, tmp_list, NULL)) != VLAD_OK)
        return retval;
    }

    delete tmp_list;
  }

  /* this might not succeed as there might not exist a model for this query */
  if ((retval = m_smobject->close_rule()) != VLAD_OK)
    return retval;

  m_stage = 4;

  return VLAD_OK;
}
#endif

#ifdef VLAD_SMODELS
/* use smwrap class to evaluate a query */
int polbase::query_evaluate(expression *a_exp, unsigned char *a_res)
{
  int retval;
  unsigned int i;

  *a_res = VLAD_RESULT_TRUE;

  /* we only allow this after a call to compute() */
  if (m_stage != 4)
    return VLAD_INVALIDOP;

  /* verify expression */
  if ((retval = verify_expression(a_exp)) != VLAD_OK)
    return retval;

  /* go through the facts to test */
  for (i = 0; i < VLAD_LIST_LENGTH(a_exp); i++) {
    fact *tmp_fact;
    unsigned int tmp_num;
    unsigned char tmp_res;

    if ((retval = a_exp->get(i, &tmp_fact)) != VLAD_OK)
      return retval;
    if ((retval = encode_fact(tmp_fact, VLAD_LIST_LENGTH(m_setable), &tmp_num)) != VLAD_OK)
      return retval;

    if ((retval = evaluate_fact(tmp_num, &tmp_res)) != VLAD_OK)
      return retval;

    switch (tmp_res) {
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

unsigned int polbase::compute_holds(unsigned int a_state,
                                    bool a_truth,
                                    unsigned int a_sub,
                                    unsigned int a_acc,
                                    unsigned int a_obj)
{
  return compute_fact(a_state,
                      a_truth,
                      (a_sub * (aslen + aglen) * (oslen + oglen)) +
                      (a_acc * (oslen + oglen)) + a_obj);
}

unsigned int polbase::compute_memb(unsigned int a_state,
                                   bool a_truth,
                                   unsigned char a_type,
                                   unsigned int a_elt,
                                   unsigned int a_grp)
{
  switch(VLAD_IDENT_BASETYPE(a_type)) {
    case VLAD_IDENT_SUB_SIN :
      return compute_fact(a_state,
                          a_truth,
                          m_tot_atoms[VLAD_ATOM_HOLDS] + (a_elt * sglen) + a_grp);
    case VLAD_IDENT_ACC_SIN :
      return compute_fact(a_state,
                          a_truth,
                          m_tot_atoms[VLAD_ATOM_HOLDS] + (sslen * sglen) + (a_elt * aglen) + a_grp);
    case VLAD_IDENT_OBJ_SIN :
      return compute_fact(a_state,
                          a_truth,
                          m_tot_atoms[VLAD_ATOM_HOLDS] + (sslen * sglen) + (aslen * aglen) + (a_elt * oglen) + a_grp);
  }

  return 0;
}

unsigned int polbase::compute_subst(unsigned a_state,
                                    bool a_truth,
                                    unsigned char a_type,
                                    unsigned char a_grp1,
                                    unsigned char a_grp2)
{
  switch(VLAD_IDENT_BASETYPE(a_type)) {
    case VLAD_IDENT_SUB_SIN :
      return compute_fact(a_state,
                          a_truth,
                          m_tot_atoms[VLAD_ATOM_HOLDS] + m_tot_atoms[VLAD_ATOM_MEMBER] + (a_grp1 * sglen) + a_grp2);
    case VLAD_IDENT_ACC_SIN :
      return compute_fact(a_state,
                          a_truth,
                          m_tot_atoms[VLAD_ATOM_HOLDS] + m_tot_atoms[VLAD_ATOM_MEMBER] + (sglen * sglen) + (a_grp1 * aglen) + a_grp2);
    case VLAD_IDENT_OBJ_SIN :
      return compute_fact(a_state,
                          a_truth,
                          m_tot_atoms[VLAD_ATOM_HOLDS] + m_tot_atoms[VLAD_ATOM_MEMBER] + (sglen * sglen) + (aglen * aglen) + (a_grp1 * oglen) + a_grp2);
  }

  return 0;
}

unsigned int polbase::compute_fact(unsigned int a_state,
                                   bool a_truth,
                                   unsigned int a_fact)
{
  return (a_state * m_tot_atom * 2) + (a_truth ? m_tot_atom : 0) + a_fact;
}

/* gives a unique id based on the holds entities given */
int polbase::encode_holds(const char *a_sub,
                          const char *a_acc,
                          const char *a_obj,
                          bool a_truth,
                          unsigned int a_state,
                          unsigned int *a_id)
{
  int retval;
  unsigned int index[3];
  unsigned char type[3];

  /* get indices and types */
  if ((retval = m_stable->get(a_sub, &(index[0]), &(type[0]))) != VLAD_OK)
    return retval;
  if ((retval = m_stable->get(a_acc, &(index[1]), &(type[1]))) != VLAD_OK)
    return retval;
  if ((retval = m_stable->get(a_obj, &(index[2]), &(type[2]))) != VLAD_OK)
    return retval;

  /* verify types */
  if (!VLAD_IDENT_IS_SUBJECT(type[0]) ||
      !VLAD_IDENT_IS_ACCESS(type[1]) ||
      !VLAD_IDENT_IS_OBJECT(type[2]))
    return VLAD_INVALIDINPUT;

  /* adjust for groups */
  index[0] += (VLAD_IDENT_IS_GROUP(type[0]) ? sslen : 0);
  index[1] += (VLAD_IDENT_IS_GROUP(type[1]) ? aslen : 0);
  index[2] += (VLAD_IDENT_IS_GROUP(type[2]) ? oslen : 0);

  *a_id = compute_holds(a_state, a_truth, index[0], index[1], index[2]);

  return VLAD_OK;
}

/* gives a unique id based on the member entities given */
int polbase::encode_memb(const char *a_elt,
                         const char *a_grp,
                         bool a_truth,
                         unsigned int a_state,
                         unsigned int *a_id)
{
  int retval;
  unsigned int index[2];
  unsigned char type[2];

  /* get indices and types */
  if ((retval = m_stable->get(a_elt, &(index[0]), &(type[0]))) != VLAD_OK)
    return retval;
  if ((retval = m_stable->get(a_grp, &(index[1]), &(type[1]))) != VLAD_OK)
    return retval;

  /* verify */
  if (VLAD_IDENT_IS_GROUP(type[0]) || !VLAD_IDENT_IS_GROUP(type[1]))
    return VLAD_INVALIDINPUT;
  if (type[0] != VLAD_IDENT_BASETYPE(type[1]))
    return VLAD_INVALIDINPUT;

  *a_id = compute_memb(a_state, a_truth, type[0], index[0], index[1]);

  return VLAD_OK;
}

/* gives a unique id based on the subset entities given */
int polbase::encode_subst(const char *a_grp1,
                          const char *a_grp2,
                          bool a_truth,
                          unsigned int a_state,
                          unsigned int *a_id)
{
  int retval;
  unsigned int index[2];
  unsigned char type[2];

  /* get indices and types */
  if ((retval = m_stable->get(a_grp1, &(index[0]), &(type[0]))) != VLAD_OK)
    return retval;
  if ((retval = m_stable->get(a_grp2, &(index[1]), &(type[1]))) != VLAD_OK)
    return retval;

  /* verify */
  if (!VLAD_IDENT_IS_GROUP(type[0]) || !VLAD_IDENT_IS_GROUP(type[1]))
    return VLAD_INVALIDINPUT;
  if (VLAD_IDENT_BASETYPE(type[0]) != VLAD_IDENT_BASETYPE(type[1]))
    return VLAD_INVALIDINPUT;

  *a_id = compute_subst(a_state, a_truth, type[0], index[0], index[1]);

  return VLAD_OK;
}

/* gives a unique id based on the fact given */
int polbase::encode_fact(fact *a_fact,
                         unsigned int a_state,
                         unsigned int *a_id)
{
  int retval;
  unsigned char type;
  bool truth;
  char *tmp[3];

  if (m_stage < 2)
    return VLAD_INVALIDOP;

  if (a_fact == NULL || a_id == NULL)
    return VLAD_NULLPTR;

  if ((retval = a_fact->get(&tmp[0], &tmp[1], &tmp[2], &type, &truth)) != VLAD_OK)
    return retval;

  switch(type) {
    case VLAD_ATOM_HOLDS :
      return encode_holds(tmp[0], tmp[1], tmp[2], truth, a_state, a_id);
    case VLAD_ATOM_MEMBER :
      return encode_memb(tmp[0], tmp[1], truth, a_state, a_id);
    case VLAD_ATOM_SUBSET :
      return encode_subst(tmp[0], tmp[1], truth, a_state, a_id);
  }

  return VLAD_FAILURE;
}

/* gives the entities based on the holds id */
int polbase::decode_holds(unsigned int a_id, char **a_sub, char **a_acc, char **a_obj)
{
  int retval;
  unsigned int index[3];
  unsigned int rem;

  rem = a_id % ((aslen + aglen) * (oslen + oglen));
  index[0] = a_id / ((aslen + aglen) * (oslen + oglen));
  index[1] = rem / (oslen + oglen);
  index[2] = rem % (oslen + oglen);

  if ((retval = m_stable->get(index[0] - ((index[0] < sslen) ? 0 : sslen), ((index[0] < sslen) ? VLAD_IDENT_SUB_SIN : VLAD_IDENT_SUB_GRP), a_sub)) != VLAD_OK)
    return retval;
  if ((retval = m_stable->get(index[1] - ((index[1] < aslen) ? 0 : aslen), ((index[1] < aslen) ? VLAD_IDENT_ACC_SIN : VLAD_IDENT_ACC_GRP), a_acc)) != VLAD_OK)
    return retval;
  if ((retval = m_stable->get(index[2] - ((index[2] < oslen) ? 0 : oslen), ((index[2] < oslen) ? VLAD_IDENT_OBJ_SIN : VLAD_IDENT_OBJ_GRP), a_obj)) != VLAD_OK)
    return retval;

  return VLAD_OK;
}

/* gives the entities based on the member id */
int polbase::decode_memb(unsigned int a_id, char **a_elt, char **a_grp)
{
  int retval;

  a_id = a_id - m_tot_atoms[VLAD_ATOM_HOLDS];

  if (a_id < (sslen * sglen)) {
    /* subject member fact */
    if ((retval = m_stable->get(a_id / sglen, VLAD_IDENT_SUB_SIN, a_elt)) != VLAD_OK)
      return retval;
    if ((retval = m_stable->get(a_id % sglen, VLAD_IDENT_SUB_GRP, a_grp)) != VLAD_OK)
      return retval;
  }
  else if (a_id < ((sslen * sglen) + (aslen *aglen))) {
    /* access member fact */
    a_id = a_id - (sslen * sglen);
    if ((retval = m_stable->get(a_id / aglen, VLAD_IDENT_ACC_SIN, a_elt)) != VLAD_OK)
      return retval;
    if ((retval = m_stable->get(a_id % aglen, VLAD_IDENT_ACC_GRP, a_grp)) != VLAD_OK)
      return retval;
  }
  else {
    /* object member fact */
    a_id = a_id - ((sslen * sglen) + (aslen * aglen));
    if ((retval = m_stable->get(a_id / oglen, VLAD_IDENT_OBJ_SIN, a_elt)) != VLAD_OK)
      return retval;
    if ((retval = m_stable->get(a_id % oglen, VLAD_IDENT_OBJ_GRP, a_grp)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

/* gives the entities based on the subst id */
int polbase::decode_subst(unsigned int a_id, char **a_grp1, char **a_grp2)
{
  int retval;

  a_id = a_id - (m_tot_atoms[VLAD_ATOM_HOLDS] + m_tot_atoms[VLAD_ATOM_MEMBER]);

  if (a_id < (sglen * sglen)) {
    /* subject subset fact */
    if ((retval = m_stable->get(a_id / sglen, VLAD_IDENT_SUB_GRP, a_grp1)) != VLAD_OK)
      return retval;
    if ((retval = m_stable->get(a_id & sglen, VLAD_IDENT_SUB_GRP, a_grp2)) != VLAD_OK)
      return retval;
  }
  else if (a_id < (sglen * sglen) + (aglen * aglen)) {
    /* access subset fact */
    a_id = a_id - (sglen * sglen);
    if ((retval = m_stable->get(a_id / aglen, VLAD_IDENT_ACC_GRP, a_grp1)) != VLAD_OK)
      return retval;
    if ((retval = m_stable->get(a_id % aglen, VLAD_IDENT_ACC_GRP, a_grp2)) != VLAD_OK)
      return retval;
  }
  else {
    /* object subset fact */
    a_id = a_id - ((sglen * sglen) + (aglen * aglen));
    if ((retval = m_stable->get(a_id / oglen, VLAD_IDENT_OBJ_GRP, a_grp1)) != VLAD_OK)
      return retval;
    if ((retval = m_stable->get(a_id % oglen, VLAD_IDENT_OBJ_GRP, a_grp2)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

/* gives the fact based on the id given */
int polbase::decode_fact(fact **a_fact,
                         unsigned int *a_state,
                         unsigned int a_id)
{
  int retval;
  char *tmp1;
  char *tmp2;
  char *tmp3;
  unsigned char type;
  bool truth;

  if (a_fact == NULL || a_state == NULL)
    return VLAD_NULLPTR;

  /* extract the state */
  *a_state = a_id / (m_tot_atom * 2);
  a_id = a_id % (m_tot_atom * 2);

  /* now the truth value */
  truth = (a_id >= m_tot_atom);
  a_id = a_id % m_tot_atom;

  /* then the fact type */
  if (a_id < m_tot_atoms[VLAD_ATOM_HOLDS]) {
    type = VLAD_ATOM_HOLDS;

    if ((retval = decode_holds(a_id, &tmp1, &tmp2, &tmp3)) != VLAD_OK)
      return retval;
  }
  else if (a_id < m_tot_atoms[VLAD_ATOM_HOLDS] + m_tot_atoms[VLAD_ATOM_MEMBER]) {
    type = VLAD_ATOM_MEMBER;

    if ((retval = decode_memb(a_id - m_tot_atoms[VLAD_ATOM_HOLDS], &tmp1, &tmp2)) != VLAD_OK)
      return retval;
  }
  else {
    type = VLAD_ATOM_SUBSET;

    if ((retval = decode_subst(a_id - (m_tot_atoms[VLAD_ATOM_HOLDS] + m_tot_atoms[VLAD_ATOM_MEMBER]), &tmp1, &tmp2)) != VLAD_OK)
      return retval;
  }

  /*  now create a new fact */
  if ((*a_fact = VLAD_NEW(fact())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = (*a_fact)->init(tmp1, tmp2, tmp3, type, truth)) != VLAD_OK) {
    delete (*a_fact);
    return retval;
  }

  return VLAD_OK;
}

/*
 * verifies that s, a and o are in the symtab and that they are of the
 * right type, or listed in vlist if vlist is non-null
 */
int polbase::verify_fact_holds(const char *a_sub,
                               const char *a_acc,
                               const char *a_obj,
                               stringlist *a_vlist)
{
  int retval;
  unsigned char type;

  if (a_sub == NULL || a_acc == NULL || a_obj == NULL)
    return VLAD_NULLPTR;

  /* check subject */
  switch((retval = m_stable->type(a_sub, &type))) {
    case VLAD_OK :
      /* in symtab so check if it is a subject */
      if (!VLAD_IDENT_IS_SUBJECT(type))
        return VLAD_INVALIDINPUT;
      break;
    case VLAD_NOTFOUND :
      /* not in symtab */
      if (a_vlist == NULL)
        return VLAD_INVALIDINPUT;
      /* if this is a non-ground fact, check in the var list */
      if ((retval = a_vlist->find(a_sub)) != VLAD_OK)
        return (retval == VLAD_NOTFOUND) ? VLAD_INVALIDINPUT : retval;
      break;
    default :
      return retval;
  }

  /* check access */
  switch((retval = m_stable->type(a_acc, &type))) {
    case VLAD_OK :
      /* in symtab so check if it is an access */
      if (!VLAD_IDENT_IS_ACCESS(type))
        return VLAD_INVALIDINPUT;
      break;
    case VLAD_NOTFOUND :
      /* not in symtab */
      if (a_vlist == NULL)
        return VLAD_INVALIDINPUT;
      /* if this is a non-ground fact, check in the var list */
      if ((retval = a_vlist->find(a_acc)) != VLAD_OK)
        return (retval == VLAD_NOTFOUND) ? VLAD_INVALIDINPUT : retval;
      break;
    default :
      return retval;
  }

  /* check object */
  switch((retval = m_stable->type(a_obj, &type))) {
    case VLAD_OK :
      /* in symtab so check if it is an object */
      if (!VLAD_IDENT_IS_OBJECT(type))
        return VLAD_INVALIDINPUT;
      break;
    case VLAD_NOTFOUND :
      /* not in symtab */
      if (a_vlist == NULL)
        return VLAD_INVALIDINPUT;
      /* if this is a non-ground fact, check in the var list */
      if ((retval = a_vlist->find(a_obj)) != VLAD_OK)
        return (retval == VLAD_NOTFOUND) ? VLAD_INVALIDINPUT : retval;
      break;
    default :
      return retval;
  }

  return VLAD_OK;
}

/*
 * verifies that e and g are in the symtab and that they are of the right
 * type, or listed in vlist if vlist is non-null
 */
int polbase::verify_fact_memb(const char *a_elt,
                              const char *a_grp,
                              stringlist *a_vlist)
{
 int retval;
  unsigned char type_elt;
  unsigned char type_grp;
  bool var = false;

  if (a_elt == NULL || a_grp == NULL)
    return VLAD_NULLPTR;

  /* check element */
  switch((retval = m_stable->type(a_elt, &type_elt))) {
    case VLAD_OK :
      /* in symtab so check if it is a non-group */
      if (VLAD_IDENT_IS_GROUP(type_elt))
        return VLAD_INVALIDINPUT;
      break;
    case VLAD_NOTFOUND :
      /* not in symtab */
      if (a_vlist == NULL)
        return VLAD_INVALIDINPUT;
      /* if this is a non-ground fact, check in the var list */
      if ((retval = a_vlist->find(a_elt)) != VLAD_OK)
        return (retval == VLAD_NOTFOUND) ? VLAD_INVALIDINPUT : retval;
      var = true;
      break;
    default :
      return retval;
  }

  /* check group */
  switch((retval = m_stable->type(a_grp, &type_grp))) {
    case VLAD_OK :
      /* in symtab so check if it is a group */
      if (!VLAD_IDENT_IS_GROUP(type_grp))
        return VLAD_INVALIDINPUT;
      /* also check if it has the same base type as e if e is not a var */
      if (!var && VLAD_IDENT_BASETYPE(type_elt) != VLAD_IDENT_BASETYPE(type_grp))
        return VLAD_INVALIDINPUT;
      break;
    case VLAD_NOTFOUND :
      /* not in symtab */
      if (a_vlist == NULL)
        return VLAD_INVALIDINPUT;
      /* if this is a non-ground fact, check in the var list */
      if ((retval = a_vlist->find(a_grp)) != VLAD_OK)
        return (retval == VLAD_NOTFOUND) ? VLAD_INVALIDINPUT : retval;
      break;
    default :
      return retval;
  }

  return VLAD_OK;
}

/*
 * verifies that g1 and g2 are in the symtab and that they are of the right
 * type, or listed in v if v is non-null
 */
int polbase::verify_fact_subst(const char *a_grp1,
                               const char *a_grp2,
                               stringlist *a_vlist)
{
  int retval;
  unsigned char type_grp1;
  unsigned char type_grp2;
  bool var = false;

  if (a_grp1 == NULL || a_grp2 == NULL)
    return VLAD_NULLPTR;

  /* check group1 */
  switch((retval = m_stable->type(a_grp1, &type_grp1))) {
    case VLAD_OK :
      /* in symtab so check if it is a group */
      if (!VLAD_IDENT_IS_GROUP(type_grp1))
        return VLAD_INVALIDINPUT;
      break;
    case VLAD_NOTFOUND :
      /* not in symtab */
      if (a_vlist == NULL)
        return VLAD_INVALIDINPUT;
      /* if this is a non-ground fact, check in the var list */
      if ((retval = a_vlist->find(a_grp1)) != VLAD_OK)
        return (retval == VLAD_NOTFOUND) ? VLAD_INVALIDINPUT : retval;
      var = true;
      break;
    default :
      return retval;
  }

  /* check group2 */
  switch((retval = m_stable->type(a_grp2, &type_grp2))) {
    case VLAD_OK :
      /* in symtab so check if it is a group */
      if (!VLAD_IDENT_IS_GROUP(type_grp2))
        return VLAD_INVALIDINPUT;
      /* also check if it has the same base type as g1 if g1 is not a var */
      if (!var && VLAD_IDENT_BASETYPE(type_grp1) != VLAD_IDENT_BASETYPE(type_grp2))
        return VLAD_INVALIDINPUT;
      break;
    case VLAD_NOTFOUND :
      /* not in symtab */
      if (a_vlist == NULL)
        return VLAD_INVALIDINPUT;
      /* if this is a non-ground fact, check in the var list */
      if ((retval = a_vlist->find(a_grp2)) != VLAD_OK)
        return (retval == VLAD_NOTFOUND) ? VLAD_INVALIDINPUT : retval;
      break;
    default :
      return retval;
  }

  return VLAD_OK;
}

/* make sure fact is valid */
int polbase::verify_fact(fact *a_fact, stringlist *a_vlist)
{
  int retval;
  char *tmp1;
  char *tmp2;
  char *tmp3;
  unsigned char type;
  bool truth;

  /* this function is only valid after the symtab is closed */
  if (m_stage < 2)
    return VLAD_INVALIDOP;

  if ((retval = a_fact->get(&tmp1, &tmp2, &tmp3, &type, &truth)) != VLAD_OK)
    return retval;

  switch(type) {
    case VLAD_ATOM_HOLDS :
      return verify_fact_holds(tmp1, tmp2, tmp3, a_vlist);
    case VLAD_ATOM_MEMBER :
      return verify_fact_memb(tmp1, tmp2, a_vlist);
    case VLAD_ATOM_SUBSET :
      return verify_fact_subst(tmp1, tmp2, a_vlist);
  }

  return VLAD_INVALIDINPUT;
}

/* make sure expression e is valid */
int polbase::verify_expression(expression *a_exp)
{
  int retval;
  unsigned int i;
  fact *tmp_fact;

  if (a_exp == NULL)
    return VLAD_NULLPTR;

  for (i = 0; i < VLAD_LIST_LENGTH(a_exp); i++) {
    if ((retval = a_exp->get(i, &tmp_fact)) != VLAD_OK)
      return retval;
    if ((retval = verify_fact(tmp_fact, NULL)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

/* make sure updateref is valid */
int polbase::verify_updateref(char *a_name, stringlist *a_ilist)
{
  int retval;
  expression *tmp_pr;
  expression *tmp_po;

  if (a_name == NULL)
    return VLAD_NULLPTR;

  /* replace the variables in transformation n with the identifiers in il */
  if ((retval = m_utable->replace(a_name, a_ilist, &tmp_pr, &tmp_po)) != VLAD_OK)
    return retval;

  /* now verify the pre and post condition expressions */
  if (tmp_pr != NULL) {
    if ((retval = verify_expression(tmp_pr)) != VLAD_OK)
      return retval;
  }

  if (tmp_po != NULL) {
    if ((retval = verify_expression(tmp_po)) != VLAD_OK)
      return retval;
  }
  return VLAD_OK;
}

/* returns the id of the negation of the given fact id */
unsigned int polbase::negate_fact(unsigned int a_fact)
{
  unsigned int state;
  bool truth;

  /* first extract the state */
  state = a_fact / (m_tot_atom * 2);
  a_fact = a_fact % (m_tot_atom * 2);

  /* then get truth value */
  truth = ! (a_fact >= m_tot_atom);
  a_fact = a_fact % m_tot_atom;

  return compute_fact(state, truth, a_fact);
}

#ifdef VLAD_SMODELS
/* checks whether the given fact is true, false or unknown */
int polbase::evaluate_fact(unsigned int a_fact, unsigned char *a_res)
{
  int retval;
  bool tmp_res;

  if (m_stage != 4)
    return VLAD_INVALIDOP;
  if (a_res == NULL)
    return VLAD_NULLPTR;

  if ((retval = m_smobject->ask(a_fact, &tmp_res)) != VLAD_OK)
    return retval;

  if (tmp_res)
    *a_res = VLAD_RESULT_TRUE;
  else {
    /* false, so we must try the negation */
    if ((retval = m_smobject->ask(negate_fact(a_fact), &tmp_res)) != VLAD_OK)
      return retval;

    *a_res = (tmp_res ? VLAD_RESULT_FALSE : VLAD_RESULT_UNKNOWN);
  }
  return VLAD_OK;
}
#endif
