/*
 * symtab.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <new.h>
#include <vlad.h>
#include "symtab.h"

symtab::symtab() : list::list(true)
{
}

symtab::~symtab()
{
  purge(true);
}

/* add symbol in symbol table */
int symtab::add(const char *n, unsigned char t)
{
  int retval;
  identifier *ident;

  if (n == NULL)
    return VLAD_NULLPTR;

  if (t == 0)
    return VLAD_INVALIDINPUT;
 
  if ((ident = new(nothrow) identifier()) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = ident->init(n, t)) != VLAD_OK)
    return retval;

  if ((retval = list::add(ident)) != VLAD_OK) {
    delete ident;
    return retval;
  }

  return VLAD_OK; 
}

/* delete the symbol n from the table */
int symtab::del(const char *n)
{
  int retval;
  identifier ident;

  /* do not accept a wildcard here. del(NULL) == purge() */

  if (n == NULL)
    return VLAD_NULLPTR;
 
  if ((retval = ident.init(n, 0)) != VLAD_OK)
    return retval;

  return list::del_d(&ident, true);

  return VLAD_OK;
}

/* get the identifier object associated with the given name */
int symtab::get(const char *n, identifier **i)
{
  int retval;
  unsigned int size;
  identifier ident;
  identifier **array;

  if (n == NULL)
    return VLAD_NULLPTR;
 
  if ((retval = ident.init(n, 0)) != VLAD_OK)
    return retval;
  
  if ((retval = list::get_d(&ident, (list_item ***) &array, &size)) != VLAD_OK)
    return retval;

  /* there should be exactly one in the array */
  *i = array[0];

  free(array);

  return VLAD_OK;
}

/* get an array of identifiers of a particular type */
int symtab::get(unsigned char t, identifier ***a, unsigned int *s)
{
  int retval;
  identifier ident;

  if (a == NULL || s == NULL)
    return VLAD_NULLPTR;
 
  if ((retval = ident.init(NULL, t)) != VLAD_OK)
    return retval;
  
  return list::get_d(&ident, (list_item ***) a, s);
}

/* return true if symbol is in the table */
int symtab::find(const char *n)
{
  int retval;
  identifier ident;
 
  if ((retval = ident.init(n, 0)) != VLAD_OK)
    return retval;
  
  return list::find(&ident);
}
