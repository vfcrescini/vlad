/*
 * symtab.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <new>

#include <config.h>
#include <vlad.h>
#include <symtab.h>

identlist::identlist(unsigned char t) : list(true)
{
  type = t; 
}

identlist::~identlist()
{
  purge(true);
}

/* add an identifier into the list */
int identlist::add(const char *n)
{
  int retval;
  identifier *ident;

  if (n == NULL)
    return VLAD_NULLPTR;

  if ((ident = VLAD_NEW(identifier())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = ident->init(n, type)) != VLAD_OK)
    return retval;

  if ((retval = list::add(ident)) != VLAD_OK) {
    delete ident;
    return retval;
  }

  return VLAD_OK; 
}

/* get the identifier object associated with the given name */
int identlist::get(const char *n, identifier **i)
{
  int retval;
  unsigned int size;
  identifier ident;
  identifier **array;

  if (n == NULL)
    return VLAD_NULLPTR;
 
  if ((retval = ident.init(n, type)) != VLAD_OK)
    return retval;
  
  if ((retval = list::get_d(&ident, (list_item ***) &array, &size)) != VLAD_OK)
    return retval;

  /* there should be exactly one in the array */
  *i = array[0];

  free(array);

  return VLAD_OK;
}

/* get the ith identifier of the list */
int identlist::get(unsigned int i, identifier **id)
{
  return list::get_i(i, (list_item **) id);
}

/* return true if symbol is in the list */
int identlist::find(const char *n)
{
  int retval;
  identifier ident;
 
  if ((retval = ident.init(n, type)) != VLAD_OK)
    return retval;
  
  return list::find(&ident);
}

symtab::symtab()
{
  sub_list = NULL;
  acc_list = NULL;
  obj_list = NULL;
  sub_grp_list = NULL;
  acc_grp_list = NULL;
  obj_grp_list = NULL;
  initialised = false;
}

symtab::~symtab()
{
  if (sub_list != NULL)
    delete sub_list;
  if (acc_list != NULL)
    delete acc_list;
  if (obj_list != NULL)
    delete obj_list;
  if (sub_grp_list != NULL)
    delete sub_grp_list;
  if (acc_grp_list != NULL)
    delete acc_grp_list;
  if (obj_grp_list != NULL)
    delete obj_grp_list;
}

int symtab::init()
{
  /* first delete things if we need to */
  if (sub_list != NULL)
    delete sub_list;
  if (acc_list != NULL)
    delete acc_list;
  if (obj_list != NULL)
    delete obj_list;
  if (sub_grp_list != NULL)
    delete sub_grp_list;
  if (acc_grp_list != NULL)
    delete acc_grp_list;
  if (obj_grp_list != NULL)
    delete obj_grp_list;

  /* now create them */
  if ((sub_list = VLAD_NEW(identlist(VLAD_IDENT_SUBJECT))) == NULL)
    return VLAD_MALLOCFAILED;
  if ((acc_list = VLAD_NEW(identlist(VLAD_IDENT_ACCESS))) == NULL)
    return VLAD_MALLOCFAILED;
  if ((obj_list = VLAD_NEW(identlist(VLAD_IDENT_OBJECT))) == NULL)
    return VLAD_MALLOCFAILED;
  if ((sub_grp_list = VLAD_NEW(identlist(VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP))) == NULL)
    return VLAD_MALLOCFAILED;
  if ((acc_grp_list = VLAD_NEW(identlist(VLAD_IDENT_ACCESS | VLAD_IDENT_GROUP))) == NULL)
    return VLAD_MALLOCFAILED;
  if ((obj_grp_list = VLAD_NEW(identlist(VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP))) == NULL)
    return VLAD_MALLOCFAILED;

  initialised = true;

  return VLAD_OK;
}

/* add symbol in symbol table */
int symtab::add(const char *n, unsigned char t)
{
  int retval;

  if (!initialised)
    return VLAD_UNINITIALISED;

  if ((retval = find(n)) != VLAD_NOTFOUND)
    return retval;

  switch(t) { 
    case VLAD_IDENT_SUBJECT :
      return sub_list->add(n);
    case VLAD_IDENT_ACCESS :
      return acc_list->add(n);
    case VLAD_IDENT_OBJECT :
      return obj_list->add(n);
    case VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP :
      return sub_grp_list->add(n);
    case VLAD_IDENT_ACCESS | VLAD_IDENT_GROUP :
      return acc_grp_list->add(n);
    case VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP :
      return obj_grp_list->add(n);
  }
  return VLAD_INVALIDINPUT;
}

/* get the identifier object associated with the given name */
int symtab::get(const char *n, identifier **i)
{
  int retval;

  if (!initialised)
    return VLAD_UNINITIALISED;

  if (n == NULL)
    return VLAD_NULLPTR;

  /* try to get n from all the lists sequentially */
  if ((retval = sub_list->get(n, i)) != VLAD_NOTFOUND)
    return retval;
  if ((retval = acc_list->get(n, i)) != VLAD_NOTFOUND)
    return retval;
  if ((retval = obj_list->get(n, i)) != VLAD_NOTFOUND)
    return retval;
  if ((retval = sub_grp_list->get(n, i)) != VLAD_NOTFOUND)
    return retval;
  if ((retval = acc_grp_list->get(n, i)) != VLAD_NOTFOUND)
    return retval;
  return obj_grp_list->get(n, i);
}

/* get the ith identifier of type t */
int symtab::get(unsigned int i, unsigned char t, identifier **id)
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  switch(t) { 
    case VLAD_IDENT_SUBJECT :
      return sub_list->get(i, id);
    case VLAD_IDENT_ACCESS :
      return sub_list->get(i, id);
    case VLAD_IDENT_OBJECT :
      return obj_list->get(i, id);
    case VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP :
      return sub_grp_list->get(i, id);
    case VLAD_IDENT_ACCESS | VLAD_IDENT_GROUP :
      return acc_grp_list->get(i, id);
    case VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP :
      return obj_grp_list->get(i, id);
  }
  return VLAD_INVALIDINPUT; 
}

/* return the number of identifiers that are of type t */
int symtab::length(unsigned char t)
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  switch(t) { 
    case VLAD_IDENT_SUBJECT :
      return sub_list->length();
    case VLAD_IDENT_ACCESS :
      return acc_list->length();
    case VLAD_IDENT_OBJECT :
      return obj_list->length();
    case VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP :
      return sub_grp_list->length();
    case VLAD_IDENT_ACCESS | VLAD_IDENT_GROUP :
      return acc_grp_list->length();
    case VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP :
      return obj_grp_list->length();
  }
  return VLAD_INVALIDINPUT;
}

/* return true if symbol is in the table */
int symtab::find(const char *n)
{
  int retval;

  if (!initialised)
    return VLAD_UNINITIALISED;

  if (n == NULL)
    return VLAD_NULLPTR;

  /* try to get n from all the lists sequentially */
  if ((retval = sub_list->find(n)) != VLAD_NOTFOUND)
    return retval;
  if ((retval = acc_list->find(n)) != VLAD_NOTFOUND)
    return retval;
  if ((retval = obj_list->find(n)) != VLAD_NOTFOUND)
    return retval;
  if ((retval = sub_grp_list->find(n)) != VLAD_NOTFOUND)
    return retval;
  if ((retval = acc_grp_list->find(n)) != VLAD_NOTFOUND)
    return retval;
  return obj_grp_list->find(n);
}
