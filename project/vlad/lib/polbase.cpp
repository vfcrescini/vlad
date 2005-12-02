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
#define sslen (m_stable->length(VLAD_IDENT_SUB_SIN))
#define aslen (m_stable->length(VLAD_IDENT_ACC_SIN))
#define oslen (m_stable->length(VLAD_IDENT_OBJ_SIN))
#define sglen (m_stable->length(VLAD_IDENT_SUB_GRP))
#define aglen (m_stable->length(VLAD_IDENT_ACC_GRP))
#define oglen (m_stable->length(VLAD_IDENT_OBJ_GRP))

vlad_polbase::vlad_polbase()
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
#ifdef VLAD_SMODELS
  if (m_smobject != NULL)
    VLAD_MEM_DELETE(m_smobject);
#endif
}

/* (re)init the policy base */
int vlad_polbase::init()
{
  int i;
  int retval;

  /* reset all the totals */
  m_tot_atom = 0;
  for (i = VLAD_ATOM_FIRST; i <= VLAD_ATOM_LAST; i++)
    m_tot_atoms[i] = 0;

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
  if ((retval = a_fact->vcopy(m_stable, NULL, &fact)) != VLAD_OK)
    return retval;

  /* add to inittab */
  return m_itable->add(fact);
}

/* add a constrant into the constraints table */
int vlad_polbase::add_consttab(vlad_expression *a_exp,
                               vlad_expression *a_cond,
                               vlad_expression *a_ncond)
{
  int retval;
  vlad_expression *exp = NULL;
  vlad_expression *cond = NULL;
  vlad_expression *ncond = NULL;

  if (m_stage != 2)
    return VLAD_INVALIDOP;

  /* only a_exp is required to be non-null */
  if (a_exp == NULL)
    return VLAD_NULLPTR;

  /* verify and copy exp */
  if ((retval = a_exp->vcopy(m_stable, NULL, &exp)) != VLAD_OK)
    return retval;

  /* verify and copy condition, if any */
  if (a_cond != NULL) {
    if ((retval = a_cond->vcopy(m_stable, NULL, &cond)) != VLAD_OK)
      return retval;
  }

  /* verify and copy negative condition, if any */
  if (a_ncond != NULL) {
    if ((retval = a_ncond->vcopy(m_stable, NULL, &ncond)) != VLAD_OK)
      return retval;
  }

  /* finally, we add the expressions into the cosntraints table */
  return m_ctable->add(exp, cond, ncond);
}

/* add an update declaration in the update table */
int vlad_polbase::add_updatetab(const char *a_name,
                                vlad_varlist *a_vlist,
                                vlad_expression *a_precond,
                                vlad_expression *a_postcond)
{
  int retval;
  char *name;
  vlad_varlist *vlist = NULL;
  vlad_expression *precond = NULL;
  vlad_expression *postcond = NULL;

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

  /* verify and copy varlist */
  if (a_vlist != NULL) {
    if ((retval = a_vlist->vcopy(m_stable, &vlist)) != VLAD_OK)
      return retval;
  }

  /* verify and copy precondition */
  if (a_precond != NULL) {
    retval = a_precond->vcopy(m_stable, a_vlist, &precond);

    if (retval != VLAD_OK)
      return retval;
  }

  /* verify and copy the postcondition */
  retval = a_postcond->vcopy(m_stable, a_vlist, &postcond);

  if (retval != VLAD_OK)
    return retval;

  /* if all went well, add to the udate table */
  return m_utable->add(name, vlist, precond, postcond);
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
  if ((retval = a_uref->verify(m_stable)) != VLAD_OK)
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

  return m_setable->length();
}

/* returns the length of the update table */
unsigned int vlad_polbase::length_updatetab()
{
  if (m_stage < 2)
    return 0;

  return m_utable->length();
}

/* checks whether name of the given type is in symtab */
int vlad_polbase::check_symtab(const char *a_name, unsigned char a_type)
{
  if (m_stage < 2)
    return VLAD_INVALIDOP;

  return m_stable->find(a_name, a_type);
}

/* enumerate the sequences in the sequence table, output to file */
int vlad_polbase::list_seqtab(FILE *a_file)
{
  int retval;
  unsigned int i;
  unsigned int j;
  char *tmp_name;
  vlad_stringlist *tmp_ilist;

  /* we only allow this function after policy base is closed */
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
int vlad_polbase::compute_generate(FILE *a_file)
{
  int retval;
  unsigned int i;

  /* we only allow this function after kb is closed */
  if (m_stage < 3)
    return VLAD_INVALIDOP;

  /* make sure the filestream is not NULL */
  if (a_file == NULL)
    return VLAD_NULLPTR;

  /* first we print out all the possible facts in the kb */
  fprintf(a_file, "%s\n", VLAD_STR_FACTS);

  for (i = 0; i < (m_tot_atom * 2 * (VLAD_LIST_LENGTH(m_setable) + 1)); i++) {
    vlad_fact *tmp_fact;
    unsigned char tmp_ty;
    unsigned int tmp_s;
    bool tmp_tr;
    char *tmp_param1;
    char *tmp_param2;
    char *tmp_param3;

    if ((retval = decode_fact(&tmp_fact, &tmp_s, i)) != VLAD_OK)
      return retval;

    if ((retval = tmp_fact->get(&tmp_param1, &tmp_param2, &tmp_param3, &tmp_ty, &tmp_tr)) != VLAD_OK)
      return retval;

    switch(tmp_ty) {
      case VLAD_ATOM_HOLDS :
        fprintf(a_file,
                "  %d = %s(S%d, %s, %s, %s, %s, %s)\n",
                i,
                VLAD_STR_HOLDS,
                tmp_s,
                tmp_tr ? VLAD_STR_TRUE : VLAD_STR_FALSE,
                VLAD_STR_HOLDS,
                tmp_param1,
                tmp_param2,
                tmp_param3);
        break;
      case VLAD_ATOM_MEMBER :
          fprintf(a_file,
                  "  %d = %s(S%d, %s, %s, %s, %s)\n",
                  i, VLAD_STR_HOLDS,
                  tmp_s,
                  tmp_tr ? VLAD_STR_TRUE : VLAD_STR_FALSE,
                  VLAD_STR_MEMBER,
                  tmp_param1,
                  tmp_param2);
        break;
      case VLAD_ATOM_SUBSET :
          fprintf(a_file,
                  "  %d = %s(S%d, %s, %s, %s, %s)\n",
                  i,
                  VLAD_STR_HOLDS,
                  tmp_s,
                  tmp_tr ? VLAD_STR_TRUE : VLAD_STR_FALSE,
                  VLAD_STR_SUBSET,
                  tmp_param1,
                  tmp_param2);
        break;
    }

    VLAD_MEM_DELETE(tmp_fact);
  }

  /* identity rules */
  fprintf(a_file, "%s %s\n", VLAD_STR_IDENTITY, VLAD_STR_RULES);

  /* state loop */
  for (i = 0; i <= VLAD_LIST_LENGTH(m_setable); i++) {
    unsigned int i_grp;
    /* subject groups */
    for (i_grp = 0; i_grp < sglen; i_grp++)
      fprintf(a_file,
              "  %d %s %s\n",
              compute_subst(i, true, VLAD_IDENT_SUB_SIN, i_grp, i_grp),
              VLAD_STR_ARROW,
              VLAD_STR_TRUE);
    /* access groups */
    for (i_grp = 0; i_grp < aglen; i_grp++)
      fprintf(a_file,
              "  %d %s %s\n",
              compute_subst(i, true, VLAD_IDENT_ACC_SIN, i_grp, i_grp),
              VLAD_STR_ARROW,
              VLAD_STR_TRUE);
    /* object groups */
    for (i_grp = 0; i_grp < oglen; i_grp++)
      fprintf(a_file,
              "  %d %s %s\n",
              compute_subst(i, true, VLAD_IDENT_OBJ_SIN, i_grp, i_grp),
              VLAD_STR_ARROW,
              VLAD_STR_TRUE);
  }

  /* inheritance rules */
  fprintf(a_file, "%s %s\n", VLAD_STR_INHERITANCE, VLAD_STR_RULES);

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
            fprintf(a_file,
                    "  %d %s %d %s %d %s %s %d\n",
                    compute_holds(i, true, i_grp1 + sslen, i_acc, i_obj),
                    VLAD_STR_ARROW,
                    compute_holds(i, true, i_grp2 + sslen, i_acc, i_obj),
                    VLAD_STR_AND,
                    compute_subst(i, true, VLAD_IDENT_SUB_SIN, i_grp1, i_grp2),
                    VLAD_STR_AND,
                    VLAD_STR_NOT,
                    compute_holds(i, false, i_grp1 + sslen, i_acc, i_obj));
            fprintf(a_file,
                    "  %d %s %d %s %d\n",
                    compute_holds(i, false, i_grp1 + sslen, i_acc, i_obj),
                    VLAD_STR_ARROW,
                    compute_holds(i, false, i_grp2 + sslen, i_acc, i_obj),
                    VLAD_STR_AND,
                    compute_subst(i, true, VLAD_IDENT_SUB_SIN, i_grp1, i_grp2));
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
            fprintf(a_file,
                    "  %d %s %d %s %d %s %s %d\n",
                    compute_holds(i, true, i_sub, i_grp1 + aslen, i_obj),
                    VLAD_STR_ARROW,
                    compute_holds(i, true, i_sub, i_grp2 + aslen, i_obj),
                    VLAD_STR_AND,
                    compute_subst(i, true, VLAD_IDENT_ACC_SIN, i_grp1, i_grp2),
                    VLAD_STR_AND,
                    VLAD_STR_NOT,
                    compute_holds(i, false, i_sub, i_grp1 + aslen, i_obj));
            fprintf(a_file,
                    "  %d %s %d %s %d\n",
                    compute_holds(i, false, i_sub, i_grp1 + aslen, i_obj),
                    VLAD_STR_ARROW,
                    compute_holds(i, false, i_sub, i_grp2 + aslen, i_obj),
                    VLAD_STR_AND,
                    compute_subst(i, true, VLAD_IDENT_ACC_SIN, i_grp1, i_grp2));
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
            fprintf(a_file,
                    "  %d %s %d %s %d %s %s %d\n",
                    compute_holds(i, true, i_sub, i_acc, i_grp1 + oslen),
                    VLAD_STR_ARROW,
                    compute_holds(i, true, i_sub, i_acc, i_grp2 + oslen),
                    VLAD_STR_AND,
                    compute_subst(i, true, VLAD_IDENT_OBJ_SIN, i_grp1, i_grp2),
                    VLAD_STR_AND,
                    VLAD_STR_NOT,
                    compute_holds(i, false, i_sub, i_acc, i_grp1 + oslen));
            fprintf(a_file,
                    "  %d %s %d %s %d\n",
                    compute_holds(i, false, i_sub, i_acc, i_grp1 + oslen),
                    VLAD_STR_ARROW,
                    compute_holds(i, false, i_sub, i_acc, i_grp2 + oslen),
                    VLAD_STR_AND,
                    compute_subst(i, true, VLAD_IDENT_OBJ_SIN, i_grp1, i_grp2));
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
            fprintf(a_file,
                    "  %d %s %d %s %d %s %s %d\n",
                    compute_holds(i, true, i_sub, i_acc, i_obj),
                    VLAD_STR_ARROW,
                    compute_holds(i, true, i_grp1 + sslen, i_acc, i_obj),
                    VLAD_STR_AND,
                    compute_memb(i, true, VLAD_IDENT_SUB_SIN, i_sub, i_grp1),
                    VLAD_STR_AND,
                    VLAD_STR_NOT,
                    compute_holds(i, false, i_sub, i_acc, i_obj));
            fprintf(a_file,
                    "  %d %s %d %s %d\n",
                    compute_holds(i, false, i_sub, i_acc, i_obj),
                    VLAD_STR_ARROW,
                    compute_holds(i, false, i_grp1 + sslen, i_acc, i_obj),
                    VLAD_STR_AND,
                    compute_memb(i, true, VLAD_IDENT_SUB_SIN, i_sub, i_grp1));
          }
        }
      }
    }
    /* access groups */
    for (i_grp1 = 0; i_grp1 < aglen; i_grp1++) {
      for (i_sub = 0; i_sub < sslen + sglen; i_sub++) {
        for (i_acc = 0; i_acc < aslen; i_acc++) {
          for (i_obj = 0; i_obj < oslen + oglen; i_obj++) {
            fprintf(a_file,
                    "  %d %s %d %s %d %s %s %d\n",
                    compute_holds(i, true, i_sub, i_acc, i_obj),
                    VLAD_STR_ARROW,
                    compute_holds(i, true, i_sub, i_grp1 + aslen, i_obj),
                    VLAD_STR_AND,
                    compute_memb(i, true, VLAD_IDENT_ACC_SIN, i_acc, i_grp1),
                    VLAD_STR_AND,
                    VLAD_STR_NOT,
                    compute_holds(i, false, i_sub, i_acc, i_obj));
            fprintf(a_file,
                    "  %d %s %d %s %d\n",
                    compute_holds(i, false, i_sub, i_acc, i_obj),
                    VLAD_STR_ARROW,
                    compute_holds(i, false, i_sub, i_grp1 + aslen, i_obj),
                    VLAD_STR_AND,
                    compute_memb(i, true, VLAD_IDENT_ACC_SIN, i_acc, i_grp1));
          }
        }
      }
    }
    /* object groups */
    for (i_grp1 = 0; i_grp1 < oglen; i_grp1++) {
      for (i_sub = 0; i_sub < sslen + sglen; i_sub++) {
        for (i_acc = 0; i_acc < aslen + aglen; i_acc++) {
          for (i_obj = 0; i_obj < oslen; i_obj++) {
            fprintf(a_file,
                    "  %d %s %d %s %d %s %s %d\n",
                    compute_holds(i, true, i_sub, i_acc, i_obj),
                    VLAD_STR_ARROW,
                    compute_holds(i, true, i_sub, i_acc, i_grp1 + oslen),
                    VLAD_STR_AND,
                    compute_memb(i, true, VLAD_IDENT_OBJ_SIN, i_obj, i_grp1),
                    VLAD_STR_AND,
                    VLAD_STR_NOT,
                    compute_holds(i, false, i_sub, i_acc, i_obj));
            fprintf(a_file,
                    "  %d %s %d %s %d\n",
                    compute_holds(i, false, i_sub, i_acc, i_obj),
                    VLAD_STR_ARROW,
                    compute_holds(i, false, i_sub, i_acc, i_grp1 + oslen),
                    VLAD_STR_AND,
                    compute_memb(i, true, VLAD_IDENT_OBJ_SIN, i_obj, i_grp1));
          }
        }
      }
    }
  }

  /* transitivity */
  fprintf(a_file, "%s %s\n", VLAD_STR_TRANSITIVITY, VLAD_STR_RULES);

  /* state loop */
  for (i = 0; i <= VLAD_LIST_LENGTH(m_setable); i++) {
    unsigned int i_grp1;
    unsigned int i_grp2;
    unsigned int i_grp3;
    /* subject groups */
    for (i_grp1 = 0; i_grp1 < sglen; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < sglen; i_grp2++) {
        for (i_grp3 = 0; i_grp3 < sglen; i_grp3++) {
          /* ignore if any 2 are the same */
          if (i_grp1 == i_grp2 || i_grp1 == i_grp3 || i_grp2 == i_grp3)
            continue;

          fprintf(a_file,
                  "  %d %s %d %s %d\n",
                  compute_subst(i, true, VLAD_IDENT_SUB_SIN, i_grp1, i_grp3),
                  VLAD_STR_ARROW,
                  compute_subst(i, true, VLAD_IDENT_SUB_SIN, i_grp1, i_grp2),
                  VLAD_STR_AND,
                  compute_subst(i, true, VLAD_IDENT_SUB_SIN, i_grp2, i_grp3));
        }
      }
    }
    /* access groups */
    for (i_grp1 = 0; i_grp1 < aglen; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < aglen; i_grp2++) {
        for (i_grp3 = 0; i_grp3 < aglen; i_grp3++) {
          /* ignore if any 2 are the same */
          if (i_grp1 == i_grp2 || i_grp1 == i_grp3 || i_grp2 == i_grp3)
            continue;
          fprintf(a_file,
                  "  %d %s %d %s %d\n",
                  compute_subst(i, true, VLAD_IDENT_ACC_SIN, i_grp1, i_grp3),
                  VLAD_STR_ARROW,
                  compute_subst(i, true, VLAD_IDENT_ACC_SIN, i_grp1, i_grp2),
                  VLAD_STR_AND,
                  compute_subst(i, true, VLAD_IDENT_ACC_SIN, i_grp2, i_grp3));
        }
      }
    }
    /* object groups */
    for (i_grp1 = 0; i_grp1 < oglen; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < oglen; i_grp2++) {
        for (i_grp3 = 0; i_grp3 < oglen; i_grp3++) {
          /* ignore if any 2 are the same */
          if (i_grp1 == i_grp2 || i_grp1 == i_grp3 || i_grp2 == i_grp3)
            continue;
          fprintf(a_file,
                  "  %d %s %d %s %d\n",
                  compute_subst(i, true, VLAD_IDENT_OBJ_SIN, i_grp1, i_grp3),
                  VLAD_STR_ARROW,
                  compute_subst(i, true, VLAD_IDENT_OBJ_SIN, i_grp1, i_grp2),
                  VLAD_STR_AND,
                  compute_subst(i, true, VLAD_IDENT_OBJ_SIN, i_grp2, i_grp3));
        }
      }
    }
  }

  /* negation rules */
  fprintf(a_file, "%s %s\n", VLAD_STR_NEGATION, VLAD_STR_RULES);

  /* state loop */
  for (i = 0; i <= VLAD_LIST_LENGTH(m_setable); i++) {
    unsigned int i_atom;
    for (i_atom = 0; i_atom < m_tot_atom; i_atom++) {
      fprintf(a_file,
              "  %s %s %d %s %d\n",
              VLAD_STR_FALSE,
              VLAD_STR_ARROW,
              compute_fact(i, true, i_atom),
              VLAD_STR_AND,
              compute_fact(i, false, i_atom));
    }
  }

  /* inertial rules */
  fprintf(a_file, "%s %s\n", VLAD_STR_INERTIAL, VLAD_STR_RULES);

  /* state loop */
  for (i = 0; i < VLAD_LIST_LENGTH(m_setable); i++) {
    unsigned int i_atom;
    for (i_atom = 0; i_atom < m_tot_atom; i_atom++) {
      fprintf(a_file,
              "  %d %s %d %s %s %d\n",
              compute_fact(i + 1, true, i_atom),
              VLAD_STR_ARROW,
              compute_fact(i, true, i_atom),
              VLAD_STR_AND,
              VLAD_STR_NOT,
              compute_fact(i + 1, false, i_atom));
      fprintf(a_file,
              "  %d %s %d %s %s %d\n",
              compute_fact(i + 1, false, i_atom),
              VLAD_STR_ARROW,
              compute_fact(i, false, i_atom),
              VLAD_STR_AND,
              VLAD_STR_NOT,
              compute_fact(i + 1, true, i_atom));
    }
  }

  /* initial state */
  fprintf(a_file, "%s %s\n", VLAD_STR_INITSTATE, VLAD_STR_RULES);

  for (i = 0; i < VLAD_LIST_LENGTH(m_itable); i++) {
    vlad_fact *tmp_fact;
    unsigned int tmp_num;

    if ((retval = m_itable->get(i, &tmp_fact)) != VLAD_OK)
      return retval;
    if ((retval = encode_fact(tmp_fact, 0, &tmp_num)) != VLAD_OK)
      return retval;

    fprintf(a_file, "  %d %s %s\n", tmp_num, VLAD_STR_ARROW, VLAD_STR_TRUE);
  }

  /* constraints */
  fprintf(a_file, "%s %s\n", VLAD_STR_CONSTRAINT, VLAD_STR_RULES);

  for (i = 0; i <= VLAD_LIST_LENGTH(m_setable); i++) {
    unsigned int  i_const;
    unsigned int i_exp;

    /* constraint loop */
    for (i_const = 0; i_const < VLAD_LIST_LENGTH(m_ctable); i_const++) {
      vlad_expression *tmp_e;
      vlad_expression *tmp_c;
      vlad_expression *tmp_n;
      vlad_fact *tmp_fact;
      unsigned int tmp_num;

      if ((retval = m_ctable->get(i_const, &tmp_e, &tmp_c, &tmp_n)) != VLAD_OK)
        return retval;

      fprintf(a_file, " ");

      /* constaint expression */
      for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_e); i_exp++) {
        if ((retval = tmp_e->get(i_exp, &tmp_fact)) != VLAD_OK)
          return retval;
        if ((retval = encode_fact(tmp_fact, i, &tmp_num)) != VLAD_OK)
          return retval;
        fprintf(a_file, " %d", tmp_num);
      }

      fprintf(a_file, " %s", VLAD_STR_ARROW);

      /* constraint condition */
      for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_c); i_exp++) {
        if ((retval = tmp_c->get(i_exp, &tmp_fact)) != VLAD_OK)
          return retval;
        if ((retval = encode_fact(tmp_fact, i, &tmp_num)) != VLAD_OK)
          return retval;
        fprintf(a_file, " %d", tmp_num);
      }

      /* constraint negative condition */
      for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_n); i_exp++) {
        if ((retval = tmp_n->get(i_exp, &tmp_fact)) != VLAD_OK)
          return retval;
        if ((retval = encode_fact(tmp_fact, i, &tmp_num)) != VLAD_OK)
          return retval;
        fprintf(a_file, " %s %d", VLAD_STR_NOT, tmp_num);
      }

      if (tmp_c == NULL && tmp_n == NULL)
        fprintf(a_file, " %s\n", VLAD_STR_TRUE);
      else
        fprintf(a_file, "\n");
    }
  }

  /* update rules */
  fprintf(a_file, "%s %s\n", VLAD_STR_UPDATE, VLAD_STR_RULES);

  /* state loop */
  for (i = 0; i < VLAD_LIST_LENGTH(m_setable); i++) {
    unsigned int i_exp;
    char *tmp_name;
    vlad_fact *tmp_fact;
    unsigned int tmp_num;
    vlad_stringlist *tmp_ilist = NULL;
    vlad_expression *tmp_pre = NULL;
    vlad_expression *tmp_post = NULL;

    if ((retval = m_setable->get(i, &tmp_name, &tmp_ilist)) != VLAD_OK)
      return retval;

    if ((retval = m_utable->replace(tmp_name, tmp_ilist, &tmp_pre, &tmp_post)) != VLAD_OK)
      return retval;

    fprintf(a_file, " ");

    /* postcondition loop */
    for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_post); i_exp++) {
      if ((retval = tmp_post->get(i_exp, &tmp_fact)) != VLAD_OK)
        return retval;
      if ((retval = encode_fact(tmp_fact, i + 1, &tmp_num)) != VLAD_OK)
        return retval;
      fprintf(a_file, " %d", tmp_num);
    }

    fprintf(a_file, " %s", VLAD_STR_ARROW);

    /* precondition loop */
    for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_pre); i_exp++) {
      if ((retval = tmp_pre->get(i_exp, &tmp_fact)) != VLAD_OK)
        return retval;
      if ((retval = encode_fact(tmp_fact, i, &tmp_num)) != VLAD_OK)
        return retval;
      fprintf(a_file, " %d", tmp_num);
    }

    if (tmp_pre == NULL)
      fprintf(a_file, " %s\n", VLAD_STR_TRUE);
    else
      fprintf(a_file, "\n");
  }

  return VLAD_OK;
}

/* generate the query */
int vlad_polbase::query_generate(vlad_expression *a_exp, FILE *a_file)
{
  int retval;
  unsigned int i;

  /* only allowed after policy base has closed */
  if (m_stage < 3)
    return VLAD_INVALIDOP;

  /* make sure the filestream is not NULL */
  if (a_file == NULL || a_exp == NULL)
    return VLAD_NULLPTR;

  /* verify expression */
  if ((retval = a_exp->verify(m_stable, NULL)) != VLAD_OK)
    return retval;

  /* and now for the queries */
  fprintf(a_file, "Queries\n");

  for (i = 0; i < VLAD_LIST_LENGTH(a_exp); i++) {
    vlad_fact *tmp_fact;
    unsigned int tmp_num;

    if ((retval = a_exp->get(i, &tmp_fact)) != VLAD_OK)
      return retval;
    if ((retval = encode_fact(tmp_fact, VLAD_LIST_LENGTH(m_setable), &tmp_num)) != VLAD_OK)
      return retval;

    if (i == 0)
      fprintf(a_file, "  ");

    if (i + 1 == VLAD_LIST_LENGTH(a_exp))
      fprintf(a_file, "%d %s\n", tmp_num, VLAD_STR_QUERY);
    else
      fprintf(a_file, "%d %s ", tmp_num, VLAD_STR_AND);
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
    vlad_fact *tmp_fact;
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
      vlad_expression *tmp_e;
      vlad_expression *tmp_c;
      vlad_expression *tmp_n;
      vlad_fact *tmp_fact;
      unsigned int tmp_num;
      vlad_numberlist *tmp_list1;
      vlad_numberlist *tmp_list2;

      if ((tmp_list1 = VLAD_MEM_NEW(vlad_numberlist())) == NULL)
        return VLAD_MALLOCFAILED;
      if ((tmp_list2 = VLAD_MEM_NEW(vlad_numberlist())) == NULL)
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

      VLAD_MEM_DELETE(tmp_list1);
      VLAD_MEM_DELETE(tmp_list2);
    }
  }

  /* update sequence rules */

  /* state loop */
  for (i = 0; i < VLAD_LIST_LENGTH(m_setable); i++) {
    unsigned int i_exp;
    char *tmp_name;
    vlad_fact *tmp_fact;
    unsigned int tmp_num;
    vlad_stringlist *tmp_ilist = NULL;
    vlad_expression *tmp_pre = NULL;
    vlad_expression *tmp_post = NULL;
    vlad_numberlist *tmp_list;

   if ((tmp_list = VLAD_MEM_NEW(vlad_numberlist())) == NULL)
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

    VLAD_MEM_DELETE(tmp_list);
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
    vlad_fact *tmp_fact;
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

unsigned int vlad_polbase::compute_holds(unsigned int a_state,
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

unsigned int vlad_polbase::compute_memb(unsigned int a_state,
                                        bool a_truth,
                                        unsigned char a_type,
                                        unsigned int a_elt,
                                        unsigned int a_grp)
{
  switch(VLAD_IDENT_BASETYPE(a_type)) {
    case VLAD_IDENT_SUB_SIN :
      return compute_fact(a_state,
                          a_truth,
                          m_tot_atoms[VLAD_ATOM_HOLDS] +
                          (a_elt * sglen) +
                          a_grp);
    case VLAD_IDENT_ACC_SIN :
      return compute_fact(a_state,
                          a_truth,
                          m_tot_atoms[VLAD_ATOM_HOLDS] +
                          (sslen * sglen) +
                          (a_elt * aglen) +
                          a_grp);
    case VLAD_IDENT_OBJ_SIN :
      return compute_fact(a_state,
                          a_truth,
                          m_tot_atoms[VLAD_ATOM_HOLDS] +
                          (sslen * sglen) +
                          (aslen * aglen) +
                          (a_elt * oglen) +
                          a_grp);
  }

  return 0;
}

unsigned int vlad_polbase::compute_subst(unsigned a_state,
                                         bool a_truth,
                                         unsigned char a_type,
                                         unsigned char a_grp1,
                                         unsigned char a_grp2)
{
  switch(VLAD_IDENT_BASETYPE(a_type)) {
    case VLAD_IDENT_SUB_SIN :
      return compute_fact(a_state,
                          a_truth,
                          m_tot_atoms[VLAD_ATOM_HOLDS] +
                          m_tot_atoms[VLAD_ATOM_MEMBER] +
                          (a_grp1 * sglen) +
                          a_grp2);
    case VLAD_IDENT_ACC_SIN :
      return compute_fact(a_state,
                          a_truth,
                          m_tot_atoms[VLAD_ATOM_HOLDS] +
                          m_tot_atoms[VLAD_ATOM_MEMBER] +
                          (sglen * sglen) +
                          (a_grp1 * aglen) +
                          a_grp2);
    case VLAD_IDENT_OBJ_SIN :
      return compute_fact(a_state,
                          a_truth,
                          m_tot_atoms[VLAD_ATOM_HOLDS] +
                          m_tot_atoms[VLAD_ATOM_MEMBER] +
                          (sglen * sglen) +
                          (aglen * aglen) +
                          (a_grp1 * oglen) +
                          a_grp2);
  }

  return 0;
}

unsigned int vlad_polbase::compute_fact(unsigned int a_state,
                                        bool a_truth,
                                        unsigned int a_fact)
{
  return (a_state * m_tot_atom * 2) + (a_truth ? m_tot_atom : 0) + a_fact;
}

/* gives a unique id based on the holds entities given */
int vlad_polbase::encode_holds(const char *a_sub,
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
int vlad_polbase::encode_memb(const char *a_elt,
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
int vlad_polbase::encode_subst(const char *a_grp1,
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
int vlad_polbase::encode_fact(vlad_fact *a_fact,
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
int vlad_polbase::decode_holds(unsigned int a_id,
                               char **a_sub,
                               char **a_acc,
                               char **a_obj)
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
int vlad_polbase::decode_memb(unsigned int a_id, char **a_elt, char **a_grp)
{
  int retval;

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
int vlad_polbase::decode_subst(unsigned int a_id, char **a_grp1, char **a_grp2)
{
  int retval;

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
int vlad_polbase::decode_fact(vlad_fact **a_fact,
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
  if ((*a_fact = VLAD_MEM_NEW(vlad_fact())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = (*a_fact)->init(tmp1, tmp2, tmp3, type, truth)) != VLAD_OK) {
    VLAD_MEM_DELETE(*a_fact);
    return retval;
  }

  return VLAD_OK;
}

/* returns the id of the negation of the given fact id */
unsigned int vlad_polbase::negate_fact(unsigned int a_fact)
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
int vlad_polbase::evaluate_fact(unsigned int a_fact, unsigned char *a_res)
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
