/*
 * symtab.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <new.h>
#include <vlad.h>
#include "symtab.h"

symtab::symtab()
{
  list::list(false);
}

symtab::~symtab()
{
  purge(true);
}

/* add symbol in symbol table */
int symtab::add(const char *n, ident_type t, bool g)
{
  int retval;
  identifier *ident;
 
  if ((ident = new(nothrow) identifier()) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = ident->init(n, t, g)) != VLAD_OK)
    return retval;

  return list::add(ident);
}

/* delete the symbol n from the table */
int symtab::del(const char *n)
{
  int retval;
  identifier ident;
 
  if ((retval = ident.init(n, subject, false)) != VLAD_OK)
    return retval;

  if ((retval = list::del_d(&ident, true)) != VLAD_OK)
    return retval;

  return VLAD_OK;
}

/* get the identifier object associated with the given name */
int symtab::get(const char *n, identifier **i)
{
  int retval;
  unsigned int size;
  identifier ident;
  identifier **array;
 
  if ((retval = ident.init(n, subject, false)) != VLAD_OK)
    return retval;
  
  if ((retval = list::get_d(&ident, (list_item ***) &array, &size)) != VLAD_OK)
    return retval;

  /* there should be exactly one in the array */
  *i = array[0];

  free(array);

  return VLAD_OK;
}

/* return true if symbol is in the table */
int symtab::find(const char *n)
{
  int retval;
  identifier ident;
 
  if ((retval = ident.init(n, subject, false)) != VLAD_OK)
    return retval;
  
  if ((retval = list::find(&ident)) != VLAD_OK)
    return retval;

  return VLAD_OK;
}
