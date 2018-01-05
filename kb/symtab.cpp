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
  if ((sub_list = VLAD_NEW(stringlist("sub"))) == NULL)
    return VLAD_MALLOCFAILED;
  if ((acc_list = VLAD_NEW(stringlist("acc"))) == NULL)
    return VLAD_MALLOCFAILED;
  if ((obj_list = VLAD_NEW(stringlist("obj"))) == NULL)
    return VLAD_MALLOCFAILED;
  if ((sub_grp_list = VLAD_NEW(stringlist("sub-grp"))) == NULL)
    return VLAD_MALLOCFAILED;
  if ((acc_grp_list = VLAD_NEW(stringlist("acc-grp"))) == NULL)
    return VLAD_MALLOCFAILED;
  if ((obj_grp_list = VLAD_NEW(stringlist("obj-grp"))) == NULL)
    return VLAD_MALLOCFAILED;

  initialised = true;

  return VLAD_OK;
}

/* add symbol in symbol table */
int symtab::add(const char *s, unsigned char t)
{
  int retval;

  if (!initialised)
    return VLAD_UNINITIALISED;

  switch(retval = find(s)) {
    case VLAD_NOTFOUND :
      break;
    case VLAD_OK :
      return VLAD_DUPLICATE;
    default :
      return retval;
  }

  switch(t) { 
    case VLAD_IDENT_SUBJECT :
      return sub_list->add(s);
    case VLAD_IDENT_ACCESS :
      return acc_list->add(s);
    case VLAD_IDENT_OBJECT :
      return obj_list->add(s);
    case VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP :
      return sub_grp_list->add(s);
    case VLAD_IDENT_ACCESS | VLAD_IDENT_GROUP :
      return acc_grp_list->add(s);
    case VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP :
      return obj_grp_list->add(s);
  }

  return VLAD_INVALIDINPUT;
}

/* get the index and type of the identifier based on name */
int symtab::get(const char *s, unsigned int *i, unsigned char *t)
{
  int retval;

  if (!initialised)
    return VLAD_UNINITIALISED;

  if (s == NULL || i == NULL || t == NULL)
    return VLAD_NULLPTR;

  /* try to get s from all the lists sequentially */
  if ((retval = sub_list->get(s, i)) != VLAD_NOTFOUND) {
    if ((retval == VLAD_OK))
      *t = VLAD_IDENT_SUBJECT;
    return retval;
  }

  if ((retval = acc_list->get(s, i)) != VLAD_NOTFOUND) {
    if ((retval == VLAD_OK))
      *t = VLAD_IDENT_ACCESS;
    return retval;
  }

  if ((retval = obj_list->get(s, i)) != VLAD_NOTFOUND) {
    if ((retval == VLAD_OK))
      *t = VLAD_IDENT_OBJECT;
    return retval;
  }

  if ((retval = sub_grp_list->get(s, i)) != VLAD_NOTFOUND) {
    if ((retval == VLAD_OK))
      *t = VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP;
    return retval;
  }

  if ((retval = acc_grp_list->get(s, i)) != VLAD_NOTFOUND) {
    if ((retval == VLAD_OK))
      *t = VLAD_IDENT_ACCESS | VLAD_IDENT_GROUP;
    return retval;
  }

  if ((retval = obj_grp_list->get(s, i)) != VLAD_NOTFOUND) {
    if ((retval == VLAD_OK))
      *t = VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP;
    return retval;
  }

  return VLAD_NOTFOUND;
}

/* get the ith identifier of type t */
int symtab::get(unsigned int i, unsigned char t, char **s)
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  switch(t) { 
    case VLAD_IDENT_SUBJECT :
      return sub_list->get(i, s);
    case VLAD_IDENT_ACCESS :
      return acc_list->get(i, s);
    case VLAD_IDENT_OBJECT :
      return obj_list->get(i, s);
    case VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP :
      return sub_grp_list->get(i, s);
    case VLAD_IDENT_ACCESS | VLAD_IDENT_GROUP :
      return acc_grp_list->get(i, s);
    case VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP :
      return obj_grp_list->get(i, s);
  }
  return VLAD_INVALIDINPUT; 
}

/* return the number of identifiers that are of type t */
unsigned int symtab::length(unsigned char t)
{
  if (!initialised)
    return 0;

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
  return 0;
}

/* return true if symbol is in the table */
int symtab::find(const char *s)
{
  int retval;

  if (!initialised)
    return VLAD_UNINITIALISED;

  if (s == NULL)
    return VLAD_NULLPTR;

  /* try to get n from all the lists sequentially */
  if ((retval = sub_list->find(s)) != VLAD_NOTFOUND)
    return retval;

  if ((retval = acc_list->find(s)) != VLAD_NOTFOUND)
    return retval;

  if ((retval = obj_list->find(s)) != VLAD_NOTFOUND)
    return retval;

  if ((retval = sub_grp_list->find(s)) != VLAD_NOTFOUND)
    return retval;

  if ((retval = acc_grp_list->find(s)) != VLAD_NOTFOUND)
    return retval;

  return obj_grp_list->find(s);
}
