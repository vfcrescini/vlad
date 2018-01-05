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
#include <vlad/symtab.h>

vlad_symtab::vlad_symtab()
{
  int i;

  for (i = 0; i < VLAD_IDENT_TOTAL; i++)
    m_lists[i] = NULL;

  m_init = false;
}

vlad_symtab::~vlad_symtab()
{
  int i;

  for (i = 0; i < VLAD_IDENT_TOTAL; i++) {
    if (m_lists[i] != NULL)
      VLAD_MEM_DELETE(m_lists[i]);
  }
}

/* initialise synbol table */
int vlad_symtab::init()
{
  int i;

  for (i = 0; i < VLAD_IDENT_TOTAL; i++) {
    /* first delete things if we need to */
    if (m_lists[i] != NULL)
      VLAD_MEM_DELETE(m_lists[i]);
    /* now create them */
    if ((m_lists[i] = VLAD_MEM_NEW(vlad_stringlist())) == NULL)
      return VLAD_MALLOCFAILED;
  }

  m_init = true;

  return VLAD_OK;
}

/* add symbol in symbol table */
int vlad_symtab::add(const char *a_s, unsigned char a_t)
{
  int retval;

  if (!m_init)
    return VLAD_UNINITIALISED;

  if (!VLAD_IDENT_IS_VALID(a_t))
    return VLAD_INVALIDINPUT;

  /* ensure that the identifier is not already used */
  switch(retval = find(a_s)) {
    case VLAD_NOTFOUND :
      break;
    case VLAD_OK :
      return VLAD_DUPLICATE;
    default :
      return retval;
  }

  /* now add */
  return m_lists[map(a_t)]->add(a_s);
}

/* get the index and type of the identifier based on name */
int vlad_symtab::get(const char *a_s, unsigned int *a_i, unsigned char *a_t)
{
  int retval;
  int i;

  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_s == NULL || a_i == NULL || a_t == NULL)
    return VLAD_NULLPTR;

  /* try to get s from all the lists sequentially */
  for (i = 0; i < VLAD_IDENT_TOTAL; i++) {
    if ((retval = m_lists[i]->get(a_s, a_i)) != VLAD_NOTFOUND) {
      if (retval == VLAD_OK)
        *a_t = unmap(i);
      return retval;
    }
  }

  return VLAD_NOTFOUND;
}

/* get the ith identifier of type t */
int vlad_symtab::get(unsigned int a_i, unsigned char a_t, char **a_s)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  if (!VLAD_IDENT_IS_VALID(a_t))
    return VLAD_INVALIDINPUT;

  /* now get */
  return m_lists[map(a_t)]->get(a_i, a_s);
}

/* get an array of identifiers that matches the given type */
int vlad_symtab::get(unsigned char a_t, char ***a_a, unsigned int *a_s)
{
  int retval;
  unsigned int i;

  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_a == NULL || a_s == NULL)
    return VLAD_NULLPTR;

  if (!VLAD_IDENT_IS_VALID(a_t))
    return VLAD_INVALIDINPUT;

  *a_s = vlad_symtab::length(map(a_t));

  if ((*a_a = VLAD_MEM_ADT_MALLOC(char *, *a_s)) == NULL)
    return VLAD_MALLOCFAILED;

  /* this is an array of references */
  for (i = 0; i < *a_s; i++) {
    if ((retval = m_lists[map(a_t)]->get(i, &((*a_a)[i]))) != VLAD_OK) {
      VLAD_MEM_FREE(*a_a);
      *a_a = NULL;
      return retval;
    }
  }

  return VLAD_OK;
}

/* return the number of identifiers that are of type t */
unsigned int vlad_symtab::length(unsigned char a_t)
{
  if (!m_init)
    return 0;

  if (!VLAD_IDENT_IS_VALID(a_t))
    return 0;

  return VLAD_LIST_LENGTH(m_lists[map(a_t)]);
}

/* return 0 if symbol is in the table */
int vlad_symtab::find(const char *a_s)
{
  int retval;
  unsigned int i;

  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_s == NULL)
    return VLAD_NULLPTR;

  for (i = 0; i < VLAD_IDENT_TOTAL; i++) {
    if ((retval = m_lists[i]->find(a_s)) != VLAD_NOTFOUND)
      return retval;
  }

  return VLAD_NOTFOUND;
}

/* return 0 if symbol of type t is in the table */
int vlad_symtab::find(const char *a_s, unsigned char a_t)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_s == NULL)
    return VLAD_NULLPTR;

  if (!VLAD_IDENT_IS_VALID(a_t))
    return VLAD_INVALIDINPUT;

  return m_lists[map(a_t)]->find(a_s);
}

/* give the type of the given identifier */
int vlad_symtab::type(const char *a_s, unsigned char *a_t)
{
  int retval;
  unsigned int i;

  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_s == NULL || a_t == NULL)
    return VLAD_NULLPTR;

  for (i = 0; i < VLAD_IDENT_TOTAL; i++) {
    if ((retval = m_lists[i]->find(a_s)) != VLAD_NOTFOUND) {
      if (retval == VLAD_OK)
        *a_t = unmap(i);
      return retval;
    }
  }

  return VLAD_NOTFOUND;
}
/* map the identifier types into sequential numbers */
unsigned int vlad_symtab::map(unsigned int a_t)
{
  switch(a_t) {
    case VLAD_IDENT_SUB_SIN :
      return 0;
    case VLAD_IDENT_ACC_SIN :
      return 1;
    case VLAD_IDENT_OBJ_SIN :
      return 2;
    case VLAD_IDENT_SUB_GRP :
      return 3;
    case VLAD_IDENT_ACC_GRP :
      return 4;
    case VLAD_IDENT_OBJ_GRP :
      return 5;
  }

  return 0;
}

/* unmap the sequential numbers into identifier types */
unsigned int vlad_symtab::unmap(unsigned int a_n)
{
  switch(a_n) {
    case 0 :
      return VLAD_IDENT_SUB_SIN;
    case 1 :
      return VLAD_IDENT_ACC_SIN;
    case 2 :
      return VLAD_IDENT_OBJ_SIN;
    case 3 :
      return VLAD_IDENT_SUB_GRP;
    case 4 :
      return VLAD_IDENT_ACC_GRP;
    case 5 :
      return VLAD_IDENT_OBJ_GRP;
  }

  return 0; 
}
