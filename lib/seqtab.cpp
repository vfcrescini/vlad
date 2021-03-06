/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004, 2005, 2006 University of Western Sydney
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

#include <vlad/vlad.h>
#include <vlad/mem.h>
#include <vlad/seqtab.h>

vlad_updateref::vlad_updateref()
{
  m_name = NULL;
  m_list = NULL;
  m_init = false;	
}

vlad_updateref::~vlad_updateref()
{
  if (m_name != NULL)
    VLAD_MEM_FREE(m_name);
  if (m_list != NULL)
    VLAD_MEM_DELETE(m_list);
}

/* compares two update references */
bool vlad_updateref::cmp(vlad_list_item *a_item)
{
  vlad_updateref *uref = NULL;

  if (a_item == NULL)
    return false;

  if ((uref = dynamic_cast<vlad_updateref *>(a_item)) == NULL)
    return false;

  /* if both are uninit return true. if only one -- false */
  if (!m_init)
    return !uref->m_init;

  if (!uref->m_init)
    return false;

  /* m_name cannot be NULL */
  if (strcmp(m_name, uref->m_name))
    return false;

  return VLAD_LIST_ITEMCMP(m_list, uref->m_list);
}

/* initialise with this update ref */
int vlad_updateref::init(const char *a_name, vlad_stringlist *a_list)
{
  if (a_name == NULL)
    return VLAD_NULLPTR;

  m_name = (char *) a_name;
  m_list = a_list;
  m_init = true;

  return VLAD_OK;
}

/* get a reference of this update ref */
int vlad_updateref::get(char **a_name, vlad_stringlist **a_list)
{
  if (a_name == NULL || a_list == NULL)
    return VLAD_NULLPTR;

  if (!m_init)
    return VLAD_UNINITIALISED;

  *a_name = m_name;
  *a_list = m_list;

  return VLAD_OK;
}

/* verify that identifiers are in symtab and update is in updatetab */
int vlad_updateref::verify(vlad_symtab *a_stab, vlad_updatetab *a_utab)
{
  int retval;
  unsigned int i;
  vlad_varlist *vlist;

  if (a_stab == NULL || a_utab == NULL)
    return VLAD_NULLPTR;

  /* get the varlist of the update definition */
  if ((retval = a_utab->get(m_name, &vlist, NULL, NULL, NULL)) != VLAD_OK)
    return retval;

  /* if the lists are empty, there's nothing to do */
  if (m_list == NULL && vlist == NULL)
    return VLAD_OK;

  /* check if the two lists are of the same length */
  if (m_list == NULL || vlist == NULL || vlist->length() != m_list->length())
    return VLAD_FAILURE;

  /* check if the identifiers are in symtab */
  for (i = 0; i < VLAD_LIST_LENGTH(m_list); i++) {
    char *ident;
    char *var;
    unsigned char type;

    if ((retval = m_list->get(i, &ident)) != VLAD_OK)
      return retval;

    if ((retval = vlist->get(i, &var)) != VLAD_OK)
      return retval;

    if ((retval = a_stab->type(ident, &type)) != VLAD_OK)
      return retval;

    /* now check if the two are of compatible types */
    if ((retval = vlad_identifier::check_compat(var, type)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

vlad_seqtab::vlad_seqtab() : vlad_list(false)
{
}

vlad_seqtab::~vlad_seqtab()
{
  purge(true);
}

/* add pre-malloc'ed updateref */
int vlad_seqtab::add(vlad_updateref *a_uref)
{
  if (a_uref == NULL)
    return VLAD_NULLPTR;

  return vlad_list::add((vlad_list_item *) a_uref);
}

/* add pre-malloc'ed m_name and ilist */
int vlad_seqtab::add(const char *a_name, vlad_stringlist *a_list)
{
  int retval;
  vlad_updateref *uref;

  if ((uref = VLAD_MEM_NEW(vlad_updateref())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = uref->init(a_name, a_list)) != VLAD_OK)
    return retval;

  return vlad_list::add((vlad_list_item *) uref);
}

/* delete i'th item */
int vlad_seqtab::del(unsigned int a_index)
{
  return vlad_list::del(a_index, true);
}

/* get i'th m_name and ilist */
int vlad_seqtab::get(unsigned int a_index,
                     char **a_name,
                     vlad_stringlist **a_list)
{
  int retval;
  vlad_updateref *uref;

  if (a_name == NULL || a_list == NULL)
    return VLAD_NULLPTR;

  retval = vlad_list::get(a_index, (vlad_list_item **) &uref);
  if (retval != VLAD_OK)
    return retval;

  return uref->get(a_name, a_list);
}
