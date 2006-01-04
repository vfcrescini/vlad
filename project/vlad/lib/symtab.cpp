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

/* class for tuple generation traversing */
class vlad_symtab_tuple_trav : public vlad_stringlist_trav
{
  public :

    vlad_symtab_tuple_trav();
    ~vlad_symtab_tuple_trav();

    /* (re)initialise */
    void init(vlad_varlist *a_vlist,
              vlad_stringlistlist *m_tlist,
              vlad_symtab *m_stab,
              vlad_stringlist *a_tuple,
              unsigned int a_iteration);

    /* the function called at every node */
    int trav(const char *a_str);

  private :

    vlad_varlist *m_vlist;
    vlad_stringlistlist *m_tlist;
    vlad_symtab *m_stab;
    vlad_stringlist *m_tuple;
    unsigned int m_iteration;
} ;

vlad_symtab_tuple_trav::vlad_symtab_tuple_trav()
{
  m_vlist = NULL;
  m_tlist = NULL;
  m_stab = NULL;
  m_tuple = NULL;
  m_iteration = 0;
}

vlad_symtab_tuple_trav::~vlad_symtab_tuple_trav()
{
}

/* (re)initialise */
void vlad_symtab_tuple_trav::init(vlad_varlist *a_vlist,
                                  vlad_stringlistlist *a_tlist,
                                  vlad_symtab *a_stab,
                                  vlad_stringlist *a_tuple,
                                  unsigned int a_iteration)
{
  m_vlist = a_vlist;
  m_tlist = a_tlist;
  m_stab = a_stab;
  m_tuple = a_tuple;
  m_iteration = a_iteration;
}

/* the function called at every node */
int vlad_symtab_tuple_trav::trav(const char *a_str)
{
  int retval;
  vlad_stringlist *tuple;

  if (m_iteration == 0) {
    /* first item in the tuple */
    if ((tuple = VLAD_MEM_NEW(vlad_stringlist(false))) == NULL)
      return VLAD_NULLPTR;
  }
  else if (m_tuple != NULL) {
    /* not the first tuple so we copy our tuple and load this string */
    if ((retval = m_tuple->copy(&tuple)) != VLAD_OK)
      return retval;
  }
  else {
    /* something's wrong here */
    return VLAD_FAILURE;
  }

  /* add the current variable into the current tuple */
  if ((retval = tuple->add(a_str)) != VLAD_OK) {
    VLAD_MEM_DELETE(tuple);
    return retval;
  }

  /* if the tuple is full, add to our tuple list */
  if (m_iteration + 1 >= m_vlist->length())
    return m_tlist->add(tuple);

  /* not full yet so we re-iterate */
  if ((retval = m_stab->tupleateify(m_vlist, m_tlist, tuple, m_iteration + 1)) != VLAD_OK) {
    /* something's very wrong here */
    VLAD_MEM_DELETE(tuple);
    return retval;
  }

  /* always cleanup */
  VLAD_MEM_DELETE(tuple);

  return VLAD_OK;
}

vlad_symtab::vlad_symtab()
{
  int i;

  for (i = 0; i < VLAD_SYMTAB_TABLES; i++)
    m_lists[i] = NULL;

  m_init = false;
}

vlad_symtab::~vlad_symtab()
{
  int i;

  for (i = 0; i < VLAD_SYMTAB_TABLES; i++) {
    if (m_lists[i] != NULL)
      VLAD_MEM_DELETE(m_lists[i]);
  }
}

/* initialise synbol table */
int vlad_symtab::init()
{
  int i;

  for (i = 0; i < VLAD_SYMTAB_TABLES; i++) {
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

  /* make sure it's not a variable */
  if ((retval = vlad_identifier::validate_nvar_ident(a_s)) != VLAD_OK)
    return retval;

  /* make sure the type is either an entity or an interval */
  if (vlad_identifier::validate_ent_type(a_t) != VLAD_OK)
    if (vlad_identifier::validate_int_type(a_t) != VLAD_OK)
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

  /* check if it's valid */
  if ((retval = vlad_identifier::validate_nvar_ident(a_s)) != VLAD_OK)
    return retval;

  /* try to get s from all the lists sequentially */
  for (i = 0; i < VLAD_SYMTAB_TABLES; i++) {
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

  /* type must be either an entity or an interval */
  if (vlad_identifier::validate_ent_type(a_t) != VLAD_OK)
    if (vlad_identifier::validate_int_type(a_t) != VLAD_OK)
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

  /* make sure type is either an entity or an interval */
  if (vlad_identifier::validate_ent_type(a_t) != VLAD_OK)
    if (vlad_identifier::validate_int_type(a_t) != VLAD_OK)
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

  /* make sure type is either an entity or an interval */
  if (vlad_identifier::validate_ent_type(a_t) != VLAD_OK)
    if (vlad_identifier::validate_int_type(a_t) != VLAD_OK)
      return 0;

  return VLAD_LIST_LENGTH(m_lists[map(a_t)]);
}

/* return VLAD_OK if symbol is in the table */
int vlad_symtab::find(const char *a_s)
{
  int retval;
  unsigned int i;

  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_s == NULL)
    return VLAD_NULLPTR;

  /* make sure identifier is valid */
  if ((vlad_identifier::validate_nvar_ident(a_s)) != VLAD_OK)
    return VLAD_INVALIDINPUT;

  for (i = 0; i < VLAD_SYMTAB_TABLES; i++) {
    if ((retval = m_lists[i]->find(a_s)) != VLAD_NOTFOUND)
      return retval;
  }

  return VLAD_NOTFOUND;
}

/* return VLAD_OK if symbol of type t is in the table */
int vlad_symtab::find(const char *a_s, unsigned char a_t)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  /* make sure identifier is valid */
  if ((vlad_identifier::validate_nvar_ident(a_s)) != VLAD_OK)
    return VLAD_INVALIDINPUT;

  /* make sure type is either an entity or an interval */
  if (vlad_identifier::validate_ent_type(a_t) != VLAD_OK)
    if (vlad_identifier::validate_int_type(a_t) != VLAD_OK)
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

  if (a_t == NULL)
    return VLAD_NULLPTR;

  /* make sure identifier is valid */
  if ((vlad_identifier::validate_nvar_ident(a_s)) != VLAD_OK)
    return VLAD_INVALIDINPUT;

  for (i = 0; i < VLAD_SYMTAB_TABLES; i++) {
    if ((retval = m_lists[i]->find(a_s)) != VLAD_NOTFOUND) {
      if (retval == VLAD_OK)
        *a_t = unmap(i);
      return retval;
    }
  }

  return VLAD_NOTFOUND;
}

/* given a list of variables, generate a list of tuples for grounding */
int vlad_symtab::tupleate(vlad_varlist *a_vlist, vlad_stringlistlist **a_tlist)
{
  if (a_tlist == NULL)
    return VLAD_NULLPTR;

  /* we create a new tuple list */
  if ((*a_tlist = VLAD_MEM_NEW(vlad_stringlistlist())) == NULL)
    return VLAD_MALLOCFAILED;

  /* nothing to do */
  if (a_vlist == NULL || a_vlist->length() == 0)
    return VLAD_OK;

  return tupleateify(a_vlist, *a_tlist, NULL, 0);
}

/* map the identifier types into sequential numbers */
unsigned int vlad_symtab::map(unsigned int a_t)
{
  switch(a_t) {
    case VLAD_IDENT_ENT_SUB_SIN :
      return 0;
    case VLAD_IDENT_ENT_ACC_SIN :
      return 1;
    case VLAD_IDENT_ENT_OBJ_SIN :
      return 2;
    case VLAD_IDENT_ENT_SUB_GRP :
      return 3;
    case VLAD_IDENT_ENT_ACC_GRP :
      return 4;
    case VLAD_IDENT_ENT_OBJ_GRP :
      return 5;
    case VLAD_IDENT_INT :
      return 6;
  }

  return 0;
}

/* unmap the sequential numbers into identifier types */
unsigned int vlad_symtab::unmap(unsigned int a_n)
{
  switch(a_n) {
    case 0 :
      return VLAD_IDENT_ENT_SUB_SIN;
    case 1 :
      return VLAD_IDENT_ENT_ACC_SIN;
    case 2 :
      return VLAD_IDENT_ENT_OBJ_SIN;
    case 3 :
      return VLAD_IDENT_ENT_SUB_GRP;
    case 4 :
      return VLAD_IDENT_ENT_ACC_GRP;
    case 5 :
      return VLAD_IDENT_ENT_OBJ_GRP;
    case 6 :
      return VLAD_IDENT_INT;
  }

  return 0;
}

/* tupleate() helper */
int vlad_symtab::tupleateify(vlad_varlist *a_vlist,
                             vlad_stringlistlist *a_tlist,
                             vlad_stringlist *a_tuple,
                             unsigned int a_iteration)
{
  int retval;
  char *var;
  unsigned int vtype;
  vlad_symtab_tuple_trav *trav;

  if (a_vlist == NULL || a_tlist == NULL)
    return VLAD_NULLPTR;

  /* make sure we don't iterate too much */
  if (a_iteration >= a_vlist->length())
    return VLAD_INVALIDINPUT;

  /* get a variable to work with */
  if ((retval = a_vlist->get(a_iteration, &var)) != VLAD_OK)
    return retval;

  /* once we have the variable, determine its type */
  if ((vtype = vlad_identifier::get_var_type(var)) == VLAD_IDENT_NUL)
    return VLAD_INVALIDINPUT;

  /* build and initialise a traverse object */
  if ((trav = VLAD_MEM_NEW(vlad_symtab_tuple_trav())) == NULL)
    return VLAD_MALLOCFAILED;

  trav->init(a_vlist, a_tlist, this, a_tuple, a_iteration);

  retval = VLAD_OK;

  /* now we figure out which list(s) to traverse */
  switch(vtype & VLAD_IDENT_MASK_TYPE) {
    case VLAD_IDENT_MASK_ENT :
      /* it's an entity */
      switch(vtype & VLAD_IDENT_MASK_ENT_BASE) {
        case VLAD_IDENT_ENT_SUB :
          if (retval == VLAD_OK && VLAD_IDENT_TYPE_IS_SIN(vtype))
            retval = m_lists[map(VLAD_IDENT_ENT_SUB_SIN)]->traverse(trav);
          if (retval == VLAD_OK && VLAD_IDENT_TYPE_IS_GRP(vtype))
            retval = m_lists[map(VLAD_IDENT_ENT_SUB_GRP)]->traverse(trav);
          break;
        case VLAD_IDENT_ENT_ACC :
          if (retval == VLAD_OK && VLAD_IDENT_TYPE_IS_SIN(vtype))
            retval = m_lists[map(VLAD_IDENT_ENT_ACC_SIN)]->traverse(trav);
          if (retval == VLAD_OK && VLAD_IDENT_TYPE_IS_GRP(vtype))
            retval = m_lists[map(VLAD_IDENT_ENT_ACC_GRP)]->traverse(trav);
          break;
        case VLAD_IDENT_ENT_OBJ :
          if (retval == VLAD_OK && VLAD_IDENT_TYPE_IS_SIN(vtype))
            retval = m_lists[map(VLAD_IDENT_ENT_OBJ_SIN)]->traverse(trav);
          if (retval == VLAD_OK && VLAD_IDENT_TYPE_IS_GRP(vtype))
            retval = m_lists[map(VLAD_IDENT_ENT_OBJ_GRP)]->traverse(trav);
          break;
        default :
          retval = VLAD_FAILURE;
      }
      break;
    case VLAD_IDENT_MASK_INT :
      /* it's an interval */
      retval = m_lists[map(VLAD_IDENT_INT)]->traverse(trav);
      break;
    default :
      retval = VLAD_FAILURE;
  }

  /* cleanup */
  VLAD_MEM_DELETE(trav);
  return retval;
}
