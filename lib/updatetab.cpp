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
#include <vlad/updatetab.h>

vlad_updatedef::vlad_updatedef()
{
  m_name = NULL;
  m_vlist = NULL;
  m_prexp = NULL;
  m_poexp = NULL;
  m_init = false;
}

vlad_updatedef::~vlad_updatedef()
{
  if (m_name != NULL)
    VLAD_MEM_FREE(m_name);
  if (m_vlist != NULL)
    VLAD_MEM_DELETE(m_vlist);
  if (m_prexp != NULL)
    VLAD_MEM_DELETE(m_prexp);
  if (m_poexp != NULL)
    VLAD_MEM_DELETE(m_poexp);
}

/* compare 2 update defs */
bool vlad_updatedef::cmp(vlad_list_item *a_item)
{
  vlad_updatedef *udef = NULL;

  if (a_item == NULL)
    return false;

  if ((udef = dynamic_cast<vlad_updatedef *>(a_item)) == NULL)
    return false;

  /* if both are uninit return true. if only one -- false */
  if (!m_init)
    return !udef->m_init;

  if (!udef->m_init)
    return false;

  /*
   * here we are only concerned about the m_name. if the m_names match,
   * then we consider the update equivalent.
   */

  return !strcmp(m_name, udef->m_name);
}

int vlad_updatedef::init(const char *a_name,
                         vlad_varlist *a_vlist,
                         vlad_expression *a_prexp,
                         vlad_expression *a_poexp)
{
  if (m_init) {
    if (m_name != NULL)
      VLAD_MEM_FREE(m_name);
    if (m_vlist != NULL)
      VLAD_MEM_DELETE(m_vlist);
    if (m_prexp != NULL)
      VLAD_MEM_DELETE(m_prexp);
    if (m_poexp != NULL)
      VLAD_MEM_DELETE(m_poexp);
  }

  if (a_name == NULL)
    return VLAD_NULLPTR;

  m_name = (char *) a_name;
  m_vlist = a_vlist;
  m_prexp = a_prexp;
  m_poexp = a_poexp;

  m_init = true;

  return VLAD_OK;
}

/* get attributes */
int vlad_updatedef::get(char **a_name,
                        vlad_varlist **a_vlist,
                        vlad_expression **a_prexp,
                        vlad_expression **a_poexp)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_name == NULL || a_vlist == NULL || a_prexp == NULL || a_poexp == NULL)
    return VLAD_NULLPTR;

  *a_name = m_name;
  *a_vlist = m_vlist;
  *a_prexp = m_prexp;
  *a_poexp = m_poexp;

  return VLAD_OK;
}

vlad_updatetab::vlad_updatetab() : vlad_list(true)
{
}

vlad_updatetab::~vlad_updatetab()
{
  purge(true);
}

/* add update def to table */
int vlad_updatetab::add(const char *a_name,
                        vlad_varlist *a_vlist,
                        vlad_expression *a_prexp,
                        vlad_expression *a_poexp)
{
  int retval;
  vlad_updatedef *udef;

  if ((udef = VLAD_MEM_NEW(vlad_updatedef())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = udef->init(a_name, a_vlist, a_prexp, a_poexp)) != VLAD_OK) {
    VLAD_MEM_DELETE(udef);
    return retval;
  }

  return vlad_list::add((vlad_list_item *) udef);
}

/* get update by m_name */
int vlad_updatetab::get(const char *a_name,
                        vlad_varlist **a_vlist,
                        vlad_expression **a_prexp,
                        vlad_expression **a_poexp)
{
  int retval;
  char *name;
  vlad_updatedef *udef;
  vlad_updatedef **lst;
  vlad_expression *prexp;
  vlad_expression *poexp;
  unsigned int size;

  if (a_name == NULL || a_vlist == NULL)
    return VLAD_NULLPTR;

  /* create a dummy updatedef to search */
  if ((name = VLAD_MEM_STR_MALLOC(a_name)) == NULL)
    return VLAD_MALLOCFAILED;

  strcpy(name, a_name);

  if ((udef = VLAD_MEM_NEW(vlad_updatedef())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = udef->init(name, NULL, NULL, NULL)) != VLAD_OK)
    return retval;

  if ((retval = vlad_list::get((vlad_list_item *) udef, (vlad_list_item ***) &lst, &size)) != VLAD_OK)
    return retval;

  /* only one. m_name is already copied and will be deleted with udef */
  if ((retval = lst[0]->get(&name, a_vlist, &prexp, &poexp)) != VLAD_OK)
    return retval;

  VLAD_MEM_FREE(lst);
  VLAD_MEM_DELETE(udef);

  if (a_prexp != NULL)
    *a_prexp = prexp;
  if (a_poexp != NULL)
    *a_poexp = poexp;

  return VLAD_OK;
}

/* vlist1 contains vars in the varlist that are actually used, vlist2
 * contains all other vars that occur in the expressions */
int vlad_updatetab::get(const char *a_name,
                        vlad_varlist **a_vlist1,
                        vlad_varlist **a_vlist2,
                        vlad_expression **a_prexp,
                        vlad_expression **a_poexp)
{
  int retval;
  vlad_varlist *vlist_spec;
  vlad_varlist *vlist_used;
  vlad_expression *prexp;
  vlad_expression *poexp;

  if (a_vlist1 == NULL || a_vlist2 == NULL)
    return VLAD_NULLPTR;

  if ((retval = get(a_name, &vlist_spec, &prexp, &poexp)) != VLAD_OK)
    return retval;

  /* create a list of variables that are actually used in the exps */
  if ((vlist_used = VLAD_MEM_NEW(vlad_varlist())) == NULL)
    return retval;

  /* generate variable list */
  if (retval == VLAD_OK)
    retval = prexp->varlist(vlist_used);
  if (retval == VLAD_OK)
    retval = prexp->varlist(vlist_used);

  /* make copies of the lists */
  if (retval == VLAD_OK)
    retval = vlist_used->copy(a_vlist1);
  if (retval == VLAD_OK)
    retval = vlist_used->copy(a_vlist2);

  /* trim the lists */
  if (retval == VLAD_OK)
    retval = (*a_vlist2)->del(vlist_spec);
  if (retval == VLAD_OK)
    retval = (*a_vlist1)->del(*a_vlist2);

  /* cleanup */
  if (vlist_used != NULL)
    VLAD_MEM_DELETE(vlist_used);
  if (retval != VLAD_OK) {
    if (*a_vlist1 != NULL)
      VLAD_MEM_DELETE(*a_vlist1);
    if (*a_vlist2 != NULL)
      VLAD_MEM_DELETE(*a_vlist2);
  }

  if (a_prexp != NULL)
    *a_prexp = prexp;
  if (a_poexp != NULL)
    *a_poexp = poexp;
  
  return retval;
}

/* get update by index */
int vlad_updatetab::get(unsigned int a_index,
                        char **a_name,
                        vlad_varlist **a_vlist,
                        vlad_expression **a_prexp,
                        vlad_expression **a_poexp)
{
  int retval;
  vlad_expression *prexp;
  vlad_expression *poexp;
  vlad_updatedef *udef;

  if (a_name == NULL || a_vlist == NULL)
    return VLAD_NULLPTR;

  if ((retval = vlad_list::get(a_index, (vlad_list_item **) &udef)) != VLAD_OK)
    return retval;

  if ((retval = udef->get(a_name, a_vlist, &prexp, &poexp)) != VLAD_OK)
    return retval;

  if (a_prexp)
    *a_prexp = prexp;
  if (a_poexp)
    *a_poexp = poexp;

  return VLAD_OK;
}

/* replace variables with identifiers in v, then get pr and pp */
int vlad_updatetab::replace(const char *a_name,
                            vlad_stringlist *a_ilist,
                            vlad_expression **a_prexp,
                            vlad_expression **a_poexp)
{
  int retval;
  vlad_expression *prexp;
  vlad_expression *poexp;
  vlad_varlist *vlist;

  if (a_name == NULL || a_prexp == NULL || a_poexp == NULL)
    return VLAD_NULLPTR;

  if ((retval = get(a_name, &vlist, &prexp, &poexp)) != VLAD_OK)
    return retval;

  if (prexp != NULL) {
    if ((retval = prexp->replace(vlist, a_ilist, a_prexp)) != VLAD_OK)
      return retval;
  }
  else
    *a_prexp = NULL;

  if (poexp != NULL) {
    if ((retval = poexp->replace(vlist, a_ilist, a_poexp)) != VLAD_OK)
      return retval;
  }
  else
    *a_poexp = NULL;

  return VLAD_OK;
}
