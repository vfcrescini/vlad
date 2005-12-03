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
#define VLAD_LEN_SS (m_stable->length(VLAD_IDENT_SUB_SIN))
#define VLAD_LEN_SG (m_stable->length(VLAD_IDENT_SUB_GRP))
#define VLAD_LEN_AS (m_stable->length(VLAD_IDENT_ACC_SIN))
#define VLAD_LEN_AG (m_stable->length(VLAD_IDENT_ACC_GRP))
#define VLAD_LEN_OS (m_stable->length(VLAD_IDENT_OBJ_SIN))
#define VLAD_LEN_OG (m_stable->length(VLAD_IDENT_OBJ_GRP))

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

  /* we only allow this function after polbase is closed */
  if (m_stage < 3)
    return VLAD_INVALIDOP;

  /* make sure the filestream is not NULL */
  if (a_file == NULL)
    return VLAD_NULLPTR;

  /* first we print out all the possible facts in the polbase */
  fprintf(a_file, "%s\n", VLAD_STR_FACTS);

  for (i = 0; i < (m_mapper->get_totals(VLAD_ATOM_TOTAL) * 2 * (VLAD_LIST_LENGTH(m_setable) + 1)); i++) {
    vlad_fact *tmp_fact;
    unsigned char tmp_ty;
    unsigned int tmp_s;
    bool tmp_tr;
    char *tmp_param1;
    char *tmp_param2;
    char *tmp_param3;

    if ((retval = m_mapper->decode_fact(i, &tmp_s, &tmp_fact)) != VLAD_OK)
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
    for (i_grp = 0; i_grp < VLAD_LEN_SG; i_grp++)
      fprintf(a_file,
              "  %d %s %s\n",
              m_mapper->compute_subst(VLAD_IDENT_SUB_SIN, i_grp, i_grp, i, true),
              VLAD_STR_ARROW,
              VLAD_STR_TRUE);
    /* access groups */
    for (i_grp = 0; i_grp < VLAD_LEN_AG; i_grp++)
      fprintf(a_file,
              "  %d %s %s\n",
              m_mapper->compute_subst(VLAD_IDENT_ACC_SIN, i_grp, i_grp, i, true),
              VLAD_STR_ARROW,
              VLAD_STR_TRUE);
    /* object groups */
    for (i_grp = 0; i_grp < VLAD_LEN_OG; i_grp++)
      fprintf(a_file,
              "  %d %s %s\n",
              m_mapper->compute_subst(VLAD_IDENT_OBJ_SIN, i_grp, i_grp, i, true),
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
    for (i_grp1 = 0; i_grp1 < VLAD_LEN_SG; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < VLAD_LEN_SG; i_grp2++) {
        if (i_grp1 == i_grp2)
          continue;
        for (i_acc = 0; i_acc < VLAD_LEN_AS + VLAD_LEN_AG; i_acc++) {
          for (i_obj = 0; i_obj < VLAD_LEN_OS + VLAD_LEN_OG; i_obj++) {
            fprintf(a_file,
                    "  %d %s %d %s %d %s %s %d\n",
                    m_mapper->compute_holds(i_grp1 + VLAD_LEN_SS, i_acc, i_obj, i, true),
                    VLAD_STR_ARROW,
                    m_mapper->compute_holds(i_grp2 + VLAD_LEN_SS, i_acc, i_obj, i, true),
                    VLAD_STR_AND,
                    m_mapper->compute_subst(VLAD_IDENT_SUB_SIN, i_grp1, i_grp2, i, true),
                    VLAD_STR_AND,
                    VLAD_STR_NOT,
                    m_mapper->compute_holds(i_grp1 + VLAD_LEN_SS, i_acc, i_obj, i, false));
            fprintf(a_file,
                    "  %d %s %d %s %d\n",
                    m_mapper->compute_holds(i_grp1 + VLAD_LEN_SS, i_acc, i_obj, i, false),
                    VLAD_STR_ARROW,
                    m_mapper->compute_holds(i_grp2 + VLAD_LEN_SS, i_acc, i_obj, i, false),
                    VLAD_STR_AND,
                    m_mapper->compute_subst(VLAD_IDENT_SUB_SIN, i_grp1, i_grp2, i, true));
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
            fprintf(a_file,
                    "  %d %s %d %s %d %s %s %d\n",
                    m_mapper->compute_holds(i_sub, i_grp1 + VLAD_LEN_AS, i_obj, i, true),
                    VLAD_STR_ARROW,
                    m_mapper->compute_holds(i_sub, i_grp2 + VLAD_LEN_AS, i_obj, i, true),
                    VLAD_STR_AND,
                    m_mapper->compute_subst(VLAD_IDENT_ACC_SIN, i_grp1, i_grp2, i, true),
                    VLAD_STR_AND,
                    VLAD_STR_NOT,
                    m_mapper->compute_holds(i_sub, i_grp1 + VLAD_LEN_AS, i_obj, i, false));
            fprintf(a_file,
                    "  %d %s %d %s %d\n",
                    m_mapper->compute_holds(i_sub, i_grp1 + VLAD_LEN_AS, i_obj, i, false),
                    VLAD_STR_ARROW,
                    m_mapper->compute_holds(i_sub, i_grp2 + VLAD_LEN_AS, i_obj, i, false),
                    VLAD_STR_AND,
                    m_mapper->compute_subst(VLAD_IDENT_ACC_SIN, i_grp1, i_grp2, i, true));
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
            fprintf(a_file,
                    "  %d %s %d %s %d %s %s %d\n",
                    m_mapper->compute_holds(i_sub, i_acc, i_grp1 + VLAD_LEN_OS, i, true),
                    VLAD_STR_ARROW,
                    m_mapper->compute_holds(i_sub, i_acc, i_grp2 + VLAD_LEN_OS, i, true),
                    VLAD_STR_AND,
                    m_mapper->compute_subst(VLAD_IDENT_OBJ_SIN, i_grp1, i_grp2, i, true),
                    VLAD_STR_AND,
                    VLAD_STR_NOT,
                    m_mapper->compute_holds(i_sub, i_acc, i_grp1 + VLAD_LEN_OS, i, false));
            fprintf(a_file,
                    "  %d %s %d %s %d\n",
                    m_mapper->compute_holds(i_sub, i_acc, i_grp1 + VLAD_LEN_OS, i, false),
                    VLAD_STR_ARROW,
                    m_mapper->compute_holds(i_sub, i_acc, i_grp2 + VLAD_LEN_OS, i, false),
                    VLAD_STR_AND,
                    m_mapper->compute_subst(VLAD_IDENT_OBJ_SIN, i_grp1, i_grp2, i, true));
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
            fprintf(a_file,
                    "  %d %s %d %s %d %s %s %d\n",
                    m_mapper->compute_holds(i_sub, i_acc, i_obj, i, true),
                    VLAD_STR_ARROW,
                    m_mapper->compute_holds(i_grp1 + VLAD_LEN_SS, i_acc, i_obj, i, true),
                    VLAD_STR_AND,
                    m_mapper->compute_memb(VLAD_IDENT_SUB_SIN, i_sub, i_grp1, i, true),
                    VLAD_STR_AND,
                    VLAD_STR_NOT,
                    m_mapper->compute_holds(i_sub, i_acc, i_obj, i, false));
            fprintf(a_file,
                    "  %d %s %d %s %d\n",
                    m_mapper->compute_holds(i_sub, i_acc, i_obj, i, false),
                    VLAD_STR_ARROW,
                    m_mapper->compute_holds(i_grp1 + VLAD_LEN_SS, i_acc, i_obj, i, false),
                    VLAD_STR_AND,
                    m_mapper->compute_memb(VLAD_IDENT_SUB_SIN, i_sub, i_grp1, i, true));
          }
        }
      }
    }
    /* access groups */
    for (i_grp1 = 0; i_grp1 < VLAD_LEN_AG; i_grp1++) {
      for (i_sub = 0; i_sub < VLAD_LEN_SS + VLAD_LEN_SG; i_sub++) {
        for (i_acc = 0; i_acc < VLAD_LEN_AS; i_acc++) {
          for (i_obj = 0; i_obj < VLAD_LEN_OS + VLAD_LEN_OG; i_obj++) {
            fprintf(a_file,
                    "  %d %s %d %s %d %s %s %d\n",
                    m_mapper->compute_holds(i_sub, i_acc, i_obj, i, true),
                    VLAD_STR_ARROW,
                    m_mapper->compute_holds(i_sub, i_grp1 + VLAD_LEN_AS, i_obj, i, true),
                    VLAD_STR_AND,
                    m_mapper->compute_memb(VLAD_IDENT_ACC_SIN, i_acc, i_grp1, i, true),
                    VLAD_STR_AND,
                    VLAD_STR_NOT,
                    m_mapper->compute_holds(i_sub, i_acc, i_obj, i, false));
            fprintf(a_file,
                    "  %d %s %d %s %d\n",
                    m_mapper->compute_holds(i_sub, i_acc, i_obj, i, false),
                    VLAD_STR_ARROW,
                    m_mapper->compute_holds(i_sub, i_grp1 + VLAD_LEN_AS, i_obj, i, false),
                    VLAD_STR_AND,
                    m_mapper->compute_memb(VLAD_IDENT_ACC_SIN, i_acc, i_grp1, i, true));
          }
        }
      }
    }
    /* object groups */
    for (i_grp1 = 0; i_grp1 < VLAD_LEN_OG; i_grp1++) {
      for (i_sub = 0; i_sub < VLAD_LEN_SS + VLAD_LEN_SG; i_sub++) {
        for (i_acc = 0; i_acc < VLAD_LEN_AS + VLAD_LEN_AG; i_acc++) {
          for (i_obj = 0; i_obj < VLAD_LEN_OS; i_obj++) {
            fprintf(a_file,
                    "  %d %s %d %s %d %s %s %d\n",
                    m_mapper->compute_holds(i_sub, i_acc, i_obj, i, true),
                    VLAD_STR_ARROW,
                    m_mapper->compute_holds(i_sub, i_acc, i_grp1 + VLAD_LEN_OS, i, true),
                    VLAD_STR_AND,
                    m_mapper->compute_memb(VLAD_IDENT_OBJ_SIN, i_obj, i_grp1, i, true),
                    VLAD_STR_AND,
                    VLAD_STR_NOT,
                    m_mapper->compute_holds(i_sub, i_acc, i_obj, i, false));
            fprintf(a_file,
                    "  %d %s %d %s %d\n",
                    m_mapper->compute_holds(i_sub, i_acc, i_obj, i, false),
                    VLAD_STR_ARROW,
                    m_mapper->compute_holds(i_sub, i_acc, i_grp1 + VLAD_LEN_OS, i, false),
                    VLAD_STR_AND,
                    m_mapper->compute_memb(VLAD_IDENT_OBJ_SIN, i_obj, i_grp1, i, true));
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
    for (i_grp1 = 0; i_grp1 < VLAD_LEN_SG; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < VLAD_LEN_SG; i_grp2++) {
        for (i_grp3 = 0; i_grp3 < VLAD_LEN_SG; i_grp3++) {
          /* ignore if any 2 are the same */
          if (i_grp1 == i_grp2 || i_grp1 == i_grp3 || i_grp2 == i_grp3)
            continue;

          fprintf(a_file,
                  "  %d %s %d %s %d\n",
                  m_mapper->compute_subst(VLAD_IDENT_SUB_SIN, i_grp1, i_grp3, i, true),
                  VLAD_STR_ARROW,
                  m_mapper->compute_subst(VLAD_IDENT_SUB_SIN, i_grp1, i_grp2, i, true),
                  VLAD_STR_AND,
                  m_mapper->compute_subst(VLAD_IDENT_SUB_SIN, i_grp2, i_grp3, i, true));
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
          fprintf(a_file,
                  "  %d %s %d %s %d\n",
                  m_mapper->compute_subst(VLAD_IDENT_ACC_SIN, i_grp1, i_grp3, i, true),
                  VLAD_STR_ARROW,
                  m_mapper->compute_subst(VLAD_IDENT_ACC_SIN, i_grp1, i_grp2, i, true),
                  VLAD_STR_AND,
                  m_mapper->compute_subst(VLAD_IDENT_ACC_SIN, i_grp2, i_grp3, i, true));
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
          fprintf(a_file,
                  "  %d %s %d %s %d\n",
                  m_mapper->compute_subst(VLAD_IDENT_OBJ_SIN, i_grp1, i_grp3, i, true),
                  VLAD_STR_ARROW,
                  m_mapper->compute_subst(VLAD_IDENT_OBJ_SIN, i_grp1, i_grp2, i, true),
                  VLAD_STR_AND,
                  m_mapper->compute_subst(VLAD_IDENT_OBJ_SIN, i_grp2, i_grp3, i, true));
        }
      }
    }
  }

  /* negation rules */
  fprintf(a_file, "%s %s\n", VLAD_STR_NEGATION, VLAD_STR_RULES);

  /* state loop */
  for (i = 0; i <= VLAD_LIST_LENGTH(m_setable); i++) {
    unsigned int i_atom;
    for (i_atom = 0; i_atom < m_mapper->get_totals(VLAD_ATOM_TOTAL); i_atom++) {
      fprintf(a_file,
              "  %s %s %d %s %d\n",
              VLAD_STR_FALSE,
              VLAD_STR_ARROW,
              m_mapper->compute_fact(i_atom, i, true),
              VLAD_STR_AND,
              m_mapper->compute_fact(i_atom, i, false));
    }
  }

  /* inertial rules */
  fprintf(a_file, "%s %s\n", VLAD_STR_INERTIAL, VLAD_STR_RULES);

  /* state loop */
  for (i = 0; i < VLAD_LIST_LENGTH(m_setable); i++) {
    unsigned int i_atom;
    for (i_atom = 0; i_atom < m_mapper->get_totals(VLAD_ATOM_TOTAL); i_atom++) {
      fprintf(a_file,
              "  %d %s %d %s %s %d\n",
              m_mapper->compute_fact(i_atom, i + 1, true),
              VLAD_STR_ARROW,
              m_mapper->compute_fact(i_atom, i, true),
              VLAD_STR_AND,
              VLAD_STR_NOT,
              m_mapper->compute_fact(i_atom, i + 1, false));
      fprintf(a_file,
              "  %d %s %d %s %s %d\n",
              m_mapper->compute_fact(i_atom, i + 1, false),
              VLAD_STR_ARROW,
              m_mapper->compute_fact(i_atom, i, false),
              VLAD_STR_AND,
              VLAD_STR_NOT,
              m_mapper->compute_fact(i_atom, i + 1, true));
    }
  }

  /* initial state */
  fprintf(a_file, "%s %s\n", VLAD_STR_INITSTATE, VLAD_STR_RULES);

  for (i = 0; i < VLAD_LIST_LENGTH(m_itable); i++) {
    vlad_fact *tmp_fact;
    unsigned int tmp_num;

    if ((retval = m_itable->get(i, &tmp_fact)) != VLAD_OK)
      return retval;

    if ((retval = m_mapper->encode_fact(tmp_fact, 0, &tmp_num)) != VLAD_OK)
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
        if ((retval = m_mapper->encode_fact(tmp_fact, i, &tmp_num)) != VLAD_OK)
          return retval;
        fprintf(a_file, " %d", tmp_num);
      }

      fprintf(a_file, " %s", VLAD_STR_ARROW);

      /* constraint condition */
      for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_c); i_exp++) {
        if ((retval = tmp_c->get(i_exp, &tmp_fact)) != VLAD_OK)
          return retval;
        if ((retval = m_mapper->encode_fact(tmp_fact, i, &tmp_num)) != VLAD_OK)
          return retval;
        fprintf(a_file, " %d", tmp_num);
      }

      /* constraint negative condition */
      for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_n); i_exp++) {
        if ((retval = tmp_n->get(i_exp, &tmp_fact)) != VLAD_OK)
          return retval;
        if ((retval = m_mapper->encode_fact(tmp_fact, i, &tmp_num)) != VLAD_OK)
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
      if ((retval = m_mapper->encode_fact(tmp_fact, i + 1, &tmp_num)) != VLAD_OK)
        return retval;
      fprintf(a_file, " %d", tmp_num);
    }

    fprintf(a_file, " %s", VLAD_STR_ARROW);

    /* precondition loop */
    for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_pre); i_exp++) {
      if ((retval = tmp_pre->get(i_exp, &tmp_fact)) != VLAD_OK)
        return retval;
      if ((retval = m_mapper->encode_fact(tmp_fact, i, &tmp_num)) != VLAD_OK)
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
    if ((retval = m_mapper->encode_fact(tmp_fact, VLAD_LIST_LENGTH(m_setable), &tmp_num)) != VLAD_OK)
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
  for (i = 0; i < (m_mapper->get_totals(VLAD_ATOM_TOTAL) * 2 * (VLAD_LIST_LENGTH(m_setable) + 1)); i++) {
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
    for (i_grp = 0; i_grp < VLAD_LEN_SG; i_grp++) {
      unsigned int tmp_num = m_mapper->compute_subst(VLAD_IDENT_SUB_SIN,
                                           i_grp,
                                           i_grp,
                                           i,
                                           true);
      if ((retval = m_smobject->add_axiom(true, 1, tmp_num)) != VLAD_OK)
        return retval;
    }
    /* access groups */
    for (i_grp = 0; i_grp < VLAD_LEN_AG; i_grp++) {
      unsigned int tmp_num = m_mapper->compute_subst(
                                           VLAD_IDENT_ACC_SIN,
                                           i_grp,
                                           i_grp,
                                           i,
                                           true);
      if ((retval = m_smobject->add_axiom(true, 1, tmp_num)) != VLAD_OK)
        return retval;
    }
    /* object groups */
    for (i_grp = 0; i_grp < VLAD_LEN_OG; i_grp++) {
      unsigned int tmp_num = m_mapper->compute_subst(
                                           VLAD_IDENT_OBJ_SIN,
                                           i_grp,
                                           i_grp,
                                           i,
                                           true);
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
    for (i_grp1 = 0; i_grp1 < VLAD_LEN_SG; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < VLAD_LEN_SG; i_grp2++) {
        if (i_grp1 == i_grp2)
          continue;
        for (i_acc = 0; i_acc < VLAD_LEN_AS + VLAD_LEN_AG; i_acc++) {
          for (i_obj = 0; i_obj < VLAD_LEN_OS + VLAD_LEN_OG; i_obj++) {
            /* positive */
            if ((retval = m_smobject->add_rule(2,
                                               1,
                                               m_mapper->compute_holds(i_grp1 + VLAD_LEN_SS, i_acc, i_obj, i, true),
                                               m_mapper->compute_holds(i_grp2 + VLAD_LEN_SS, i_acc, i_obj, i, true),
                                               m_mapper->compute_subst(VLAD_IDENT_SUB_SIN, i_grp1, i_grp2, i, true),
                                               m_mapper->compute_holds(i_grp1 + VLAD_LEN_SS, i_acc, i_obj, i, false))) != VLAD_OK)
              return retval;
            /* negative */
            if ((retval = m_smobject->add_rule(2,
                                               0,
                                               m_mapper->compute_holds(i_grp1 + VLAD_LEN_SS, i_acc, i_obj, i, false),
                                               m_mapper->compute_holds(i_grp2 + VLAD_LEN_SS, i_acc, i_obj, i, false),
                                               m_mapper->compute_subst(VLAD_IDENT_SUB_SIN, i_grp1, i_grp2, i, true))) != VLAD_OK)
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
            if ((retval = m_smobject->add_rule(2,
                                               1,
                                               m_mapper->compute_holds(i_sub, i_grp1 + VLAD_LEN_AS, i_obj, i, true),
                                               m_mapper->compute_holds(i_sub, i_grp2 + VLAD_LEN_AS, i_obj, i, true),
                                               m_mapper->compute_subst(VLAD_IDENT_ACC_SIN, i_grp1, i_grp2, i, true),
                                               m_mapper->compute_holds(i_sub, i_grp1 + VLAD_LEN_AS, i_obj, i, false))) != VLAD_OK)
              return retval;
            /* negative */
            if ((retval = m_smobject->add_rule(2,
                                               0,
                                               m_mapper->compute_holds(i_sub, i_grp1 + VLAD_LEN_AS, i_obj, i, false),
                                               m_mapper->compute_holds(i_sub, i_grp2 + VLAD_LEN_AS, i_obj, i, false),
                                               m_mapper->compute_subst(VLAD_IDENT_ACC_SIN, i_grp1, i_grp2, i, true))) != VLAD_OK)
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
            if ((retval = m_smobject->add_rule(2,
                                               1,
                                               m_mapper->compute_holds(i_sub, i_acc, i_grp1 + VLAD_LEN_OS, i, true),
                                               m_mapper->compute_holds(i_sub, i_acc, i_grp2 + VLAD_LEN_OS, i, true),
                                               m_mapper->compute_subst(VLAD_IDENT_OBJ_SIN, i_grp1, i_grp2, i, true),
                                               m_mapper->compute_holds(i_sub, i_acc, i_grp1 + VLAD_LEN_OS, i, false))) != VLAD_OK)
              return retval;
            /* negative */
            if ((retval = m_smobject->add_rule(2,
                                               0,
                                               m_mapper->compute_holds(i_sub, i_acc, i_grp1 + VLAD_LEN_OS, i, false),
                                               m_mapper->compute_holds(i_sub, i_acc, i_grp2 + VLAD_LEN_OS, i, false),
                                               m_mapper->compute_subst(VLAD_IDENT_OBJ_SIN, i_grp1, i_grp2, i, true))) != VLAD_OK)
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
            if ((retval = m_smobject->add_rule(2,
                                               1,
                                               m_mapper->compute_holds(i_sub, i_acc, i_obj, i, true),
                                               m_mapper->compute_holds(i_grp1 + VLAD_LEN_SS, i_acc, i_obj, i, true),
                                               m_mapper->compute_memb(VLAD_IDENT_SUB_SIN, i_sub, i_grp1, i, true),
                                               m_mapper->compute_holds(i_sub, i_acc, i_obj, i, false))) != VLAD_OK)
              return retval;
            /* negative */
            if ((retval = m_smobject->add_rule(2,
                                               0,
                                               m_mapper->compute_holds(i_sub, i_acc, i_obj, i, false),
                                               m_mapper->compute_holds(i_grp1 + VLAD_LEN_SS, i_acc, i_obj, i, false),
                                               m_mapper->compute_memb(VLAD_IDENT_SUB_SIN, i_sub, i_grp1, i, true))) != VLAD_OK)
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
            if ((retval = m_smobject->add_rule(2,
                                               1,
                                               m_mapper->compute_holds(i_sub, i_acc, i_obj, i, true),
                                               m_mapper->compute_holds(i_sub, i_grp1 + VLAD_LEN_AS, i_obj, i, true),
                                               m_mapper->compute_memb(VLAD_IDENT_ACC_SIN, i_acc, i_grp1, i, true),
                                               m_mapper->compute_holds(i_sub, i_acc, i_obj, i, false))) != VLAD_OK)
              return retval;
            /* negative */
            if ((retval = m_smobject->add_rule(2,
                                               0,
                                               m_mapper->compute_holds(i_sub, i_acc, i_obj, i, false),
                                               m_mapper->compute_holds(i_sub, i_grp1 + VLAD_LEN_AS, i_obj, i, false),
                                               m_mapper->compute_memb(VLAD_IDENT_ACC_SIN, i_acc, i_grp1, i, true))) != VLAD_OK)
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
            if ((retval = m_smobject->add_rule(2,
                                               1,
                                               m_mapper->compute_holds(i_sub, i_acc, i_obj, i, true),
                                               m_mapper->compute_holds(i_sub, i_acc, i_grp1 + VLAD_LEN_OS, i, true),
                                               m_mapper->compute_memb(VLAD_IDENT_OBJ_SIN, i_obj, i_grp1, i, true),
                                               m_mapper->compute_holds(i_sub, i_acc, i_obj, i, false))) != VLAD_OK)
              return retval;
            /* negative */
            if ((retval = m_smobject->add_rule(2,
                                               0,
                                               m_mapper->compute_holds(i_sub, i_acc, i_obj, i, false),
                                               m_mapper->compute_holds(i_sub, i_acc, i_grp1 + VLAD_LEN_OS, i, false),
                                               m_mapper->compute_memb(VLAD_IDENT_OBJ_SIN, i_obj, i_grp1, i, true))) != VLAD_OK)
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
    for (i_grp1 = 0; i_grp1 < VLAD_LEN_SG; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < VLAD_LEN_SG; i_grp2++) {
        /* ignore if the 2 are the same */
        if (i_grp1 == i_grp2)
          continue;
        for (i_grp3 = 0; i_grp3 < VLAD_LEN_SG; i_grp3++) {
          unsigned int tmp_num1;
          unsigned int tmp_num2;
          unsigned int tmp_num3;

          /* ignore if any 2 are the same */
          if (i_grp1 == i_grp3 || i_grp2 == i_grp3)
            continue;

          tmp_num1 = m_mapper->compute_subst(VLAD_IDENT_SUB_SIN, i_grp1, i_grp3, i, true);
          tmp_num2 = m_mapper->compute_subst(VLAD_IDENT_SUB_SIN, i_grp1, i_grp2, i, true);
          tmp_num3 = m_mapper->compute_subst(VLAD_IDENT_SUB_SIN, i_grp2, i_grp3, i, true);

          if ((retval = m_smobject->add_rule(2, 0, tmp_num1, tmp_num2, tmp_num3)) != VLAD_OK)
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
          unsigned int tmp_num1;
          unsigned int tmp_num2;
          unsigned int tmp_num3;

          /* ignore if any 2 are the same */
          if (i_grp1 == i_grp3 || i_grp2 == i_grp3)
            continue;

          tmp_num1 = m_mapper->compute_subst(VLAD_IDENT_ACC_SIN, i_grp1, i_grp3, i, true);
          tmp_num2 = m_mapper->compute_subst(VLAD_IDENT_ACC_SIN, i_grp1, i_grp2, i, true);
          tmp_num3 = m_mapper->compute_subst(VLAD_IDENT_ACC_SIN, i_grp2, i_grp3, i, true);

          if ((retval = m_smobject->add_rule(2, 0, tmp_num1, tmp_num2, tmp_num3)) != VLAD_OK)
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
          unsigned int tmp_num1;
          unsigned int tmp_num2;
          unsigned int tmp_num3;

          /* ignore if any 2 are the same */
          if (i_grp1 == i_grp3 || i_grp2 == i_grp3)
            continue;

          tmp_num1 = m_mapper->compute_subst(VLAD_IDENT_OBJ_SIN, i_grp1, i_grp3, i, true);
          tmp_num2 = m_mapper->compute_subst(VLAD_IDENT_OBJ_SIN, i_grp1, i_grp2, i, true);
          tmp_num3 = m_mapper->compute_subst(VLAD_IDENT_OBJ_SIN, i_grp2, i_grp3, i, true);

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
    for (i_fact = 0; i_fact < m_mapper->get_totals(VLAD_ATOM_TOTAL); i_fact++) {
      unsigned int tmp_num1 = m_mapper->compute_fact(i_fact, i, true);
      unsigned int tmp_num2 = m_mapper->compute_fact(i_fact, i, false);
      if ((retval = m_smobject->add_axiom(false, 2, tmp_num1, tmp_num2)) != VLAD_OK)
        return retval;
    }
  }

  /* inertial rules */

  /* state loop */
  for (i = 0; i < VLAD_LIST_LENGTH(m_setable); i++) {
    unsigned int i_fact;
    for (i_fact = 0; i_fact < m_mapper->get_totals(VLAD_ATOM_TOTAL); i_fact++) {
      unsigned int tmp_num1;
      unsigned int tmp_num2;
      unsigned int tmp_num3;

      tmp_num1 = m_mapper->compute_fact(i_fact, i + 1, true);
      tmp_num2 = m_mapper->compute_fact(i_fact, i, true);
      tmp_num3 = m_mapper->compute_fact(i_fact, i + 1, false);

      if ((retval = m_smobject->add_rule(1, 1, tmp_num1, tmp_num2, tmp_num3)) != VLAD_OK)
        return retval;

      tmp_num1 = m_mapper->compute_fact(i_fact, i + 1, false);
      tmp_num2 = m_mapper->compute_fact(i_fact, i, false);
      tmp_num3 = m_mapper->compute_fact(i_fact, i + 1, true);

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
    if ((retval = m_mapper->encode_fact(tmp_fact, 0, &tmp_num)) != VLAD_OK)
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
        if ((retval = m_mapper->encode_fact(tmp_fact, i, &tmp_num)) != VLAD_OK)
          return retval;
        if ((retval = tmp_list1->add(tmp_num)) != VLAD_OK)
          return retval;
      }

      /* constraint negative condition */
      for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_n); i_exp++) {
        if ((retval = tmp_n->get(i_exp, &tmp_fact)) != VLAD_OK)
          return retval;
        if ((retval = m_mapper->encode_fact(tmp_fact, i, &tmp_num)) != VLAD_OK)
          return retval;
        if ((retval = tmp_list2->add(tmp_num)) != VLAD_OK)
          return retval;
      }

      /* constaint expression */
      for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_e); i_exp++) {
        if ((retval = tmp_e->get(i_exp, &tmp_fact)) != VLAD_OK)
          return retval;
        if ((retval = m_mapper->encode_fact(tmp_fact, i, &tmp_num)) != VLAD_OK)
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
      if ((retval = m_mapper->encode_fact(tmp_fact, i, &tmp_num)) != VLAD_OK)
        return retval;
      if ((retval = tmp_list->add(tmp_num)) != VLAD_OK)
        return retval;
    }

    /* postcondition loop */
    for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_post); i_exp++) {
      if ((retval = tmp_post->get(i_exp, &tmp_fact)) != VLAD_OK)
        return retval;
      if ((retval = m_mapper->encode_fact(tmp_fact, i + 1, &tmp_num)) != VLAD_OK)
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
    if ((retval = m_mapper->encode_fact(tmp_fact, VLAD_LIST_LENGTH(m_setable), &tmp_num)) != VLAD_OK)
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
    if ((retval = m_smobject->ask(m_mapper->negate_fact(a_fact), &tmp_res)) != VLAD_OK)
      return retval;

    *a_res = (tmp_res ? VLAD_RESULT_FALSE : VLAD_RESULT_UNKNOWN);
  }
  return VLAD_OK;
}
#endif
