/*
 * symtab.cpp
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <new>

#include <vlad/vlad.h>
#include <vlad/symtab.h>

symtab::symtab()
{
  int i;

  for (i = VLAD_IDENT_FIRST; i <= VLAD_IDENT_LAST; i++)
    m_lists[i] = NULL;

  m_init = false;
}

symtab::~symtab()
{
  int i;

  for (i = VLAD_IDENT_FIRST; i <= VLAD_IDENT_LAST; i++) {
    if (m_lists[i] != NULL)
      delete m_lists[i];
  }
}

/* initialise synbol table */
int symtab::init()
{
  int i;

  for (i = VLAD_IDENT_FIRST; i <= VLAD_IDENT_LAST; i++) {
    /* first delete things if we need to */
    if (m_lists[i] != NULL)
      delete m_lists[i];
    /* now create them */
    if ((m_lists[i] = VLAD_NEW(stringlist())) == NULL)
      return VLAD_MALLOCFAILED;
  }

  m_init = true;

  return VLAD_OK;
}

/* add symbol in symbol table */
int symtab::add(const char *a_s, unsigned char a_t)
{
  int retval;

  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_t > VLAD_IDENT_LAST)
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
  return m_lists[a_t]->add(a_s);
}

/* get the index and type of the identifier based on name */
int symtab::get(const char *a_s, unsigned int *a_i, unsigned char *a_t)
{
  int retval;
  int i;

  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_s == NULL || a_i == NULL || a_t == NULL)
    return VLAD_NULLPTR;

  /* try to get s from all the lists sequentially */
  for (i = VLAD_IDENT_FIRST; i <= VLAD_IDENT_LAST; i++) {
    if ((retval = m_lists[i]->get(a_s, a_i)) != VLAD_NOTFOUND) {
      if (retval == VLAD_OK)
        *a_t = i;
      return retval;
    }
  }

  return VLAD_NOTFOUND;
}

/* get the ith identifier of type t */
int symtab::get(unsigned int a_i, unsigned char a_t, char **a_s)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_t > VLAD_IDENT_LAST)
    return VLAD_INVALIDINPUT;

  /* now get */
  return m_lists[a_t]->get(a_i, a_s);
}

/* get an array of identifiers that matches the given type */
int symtab::get(unsigned char a_t, char ***a_a, unsigned int *a_s)
{
  int retval;
  unsigned int i;

  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_a == NULL || a_s == NULL)
    return VLAD_NULLPTR;

  if (a_t > VLAD_IDENT_LAST)
    return VLAD_INVALIDINPUT;

  *a_s = symtab::length(a_t);

  if ((*a_a = VLAD_ADT_MALLOC(char *, *a_s)) == NULL)
    return VLAD_MALLOCFAILED;

  /* this is an array of references */
  for (i = 0; i < *a_s; i++) {
    if ((retval = m_lists[a_t]->get(i, &((*a_a)[i]))) != VLAD_OK) {
      free(*a_a);
      *a_a = NULL;
      return retval;
    }
  }

  return VLAD_OK;
}

/* return the number of identifiers that are of type t */
unsigned int symtab::length(unsigned char a_t)
{
  if (!m_init)
    return 0;

  if (a_t > VLAD_IDENT_LAST)
    return 0;

  return VLAD_LIST_LENGTH(m_lists[a_t]);
}

/* return 0 if symbol is in the table */
int symtab::find(const char *a_s)
{
  int retval;
  unsigned int i;

  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_s == NULL)
    return VLAD_NULLPTR;

  for (i = VLAD_IDENT_FIRST; i <= VLAD_IDENT_LAST; i++) {
    if ((retval = m_lists[i]->find(a_s)) != VLAD_NOTFOUND)
      return retval;
  }

  return VLAD_NOTFOUND;
}

/* return 0 if symbol of type t is in the table */
int symtab::find(const char *a_s, unsigned char a_t)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_s == NULL)
    return VLAD_NULLPTR;

  if (a_t > VLAD_IDENT_LAST)
    return VLAD_INVALIDINPUT;

  return m_lists[a_t]->find(a_s);
}

/* give the type of the given identifier */
int symtab::type(const char *a_s, unsigned char *a_t)
{
  int retval;
  unsigned int i;

  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_s == NULL || a_t == NULL)
    return VLAD_NULLPTR;

  for (i = VLAD_IDENT_FIRST; i <= VLAD_IDENT_LAST; i++) {
    if ((retval = m_lists[i]->find(a_s)) != VLAD_NOTFOUND) {
      if (retval == VLAD_OK)
        *a_t = i;
      return retval;
    }
  }

  return VLAD_NOTFOUND;
}
