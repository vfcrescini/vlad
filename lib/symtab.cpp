/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004 University of Western Sydney
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
#include <new>

#include <vlad/vlad.h>
#include <vlad/symtab.h>

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
  if ((sub_list = VLAD_NEW(stringlist())) == NULL)
    return VLAD_MALLOCFAILED;
  if ((acc_list = VLAD_NEW(stringlist())) == NULL)
    return VLAD_MALLOCFAILED;
  if ((obj_list = VLAD_NEW(stringlist())) == NULL)
    return VLAD_MALLOCFAILED;
  if ((sub_grp_list = VLAD_NEW(stringlist())) == NULL)
    return VLAD_MALLOCFAILED;
  if ((acc_grp_list = VLAD_NEW(stringlist())) == NULL)
    return VLAD_MALLOCFAILED;
  if ((obj_grp_list = VLAD_NEW(stringlist())) == NULL)
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

/* get the i'th identifier */
int symtab::get(unsigned int i, char **s)
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  if (i < VLAD_LIST_LENGTH(sub_list))
    return get(i, VLAD_IDENT_SUBJECT, s);
  else {
    i -= VLAD_LIST_LENGTH(sub_list);
    if (i < VLAD_LIST_LENGTH(acc_list))
      return get(i, VLAD_IDENT_ACCESS, s);
    else {
      i -= VLAD_LIST_LENGTH(acc_list);
      if (i < VLAD_LIST_LENGTH(obj_list))
        return get(i, VLAD_IDENT_OBJECT, s);
      else {
        i -= VLAD_LIST_LENGTH(obj_list);
        if (i < VLAD_LIST_LENGTH(sub_grp_list))
          return get(i, VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP, s);
        else {
          i -= VLAD_LIST_LENGTH(sub_grp_list);
          if (i < VLAD_LIST_LENGTH(acc_grp_list))
            return get(i, VLAD_IDENT_ACCESS | VLAD_IDENT_GROUP, s);
          else {
            i -= VLAD_LIST_LENGTH(acc_grp_list);
            if (i < VLAD_LIST_LENGTH(obj_grp_list))
              return get(i, VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP, s);
            else
              return VLAD_OUTOFBOUNDS;
          }
        }
      }
    }
  }
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

/* get an array of identifiers that matches the given type */
int symtab::get(unsigned char t, char ***a, unsigned int *s)
{
  int retval;
  unsigned int i;

  if (!initialised)
    return VLAD_UNINITIALISED;

  if (a == NULL || s == NULL)
    return VLAD_NULLPTR;

  if (VLAD_IDENT_IS_SUBJECT(t)) {
    unsigned int s_len;
    unsigned int sg_len;

    s_len = VLAD_LIST_LENGTH(sub_list);
    sg_len = VLAD_LIST_LENGTH(sub_grp_list);
    *s = s_len + sg_len;

    if ((*a = VLAD_ADT_MALLOC(char *, *s)) == NULL)
      return VLAD_MALLOCFAILED;

    /* non-group subject */
    for (i = 0; i < s_len; i++)
      if ((retval = sub_list->get(i, &((*a)[i]))) != VLAD_OK)
        return retval;

    /* group subject */
    for (i = s_len; i < *s; i++)
      if ((retval = sub_grp_list->get(i - s_len, &((*a)[i]))) != VLAD_OK)
        return retval;

    return VLAD_OK;
  }
  else if (VLAD_IDENT_IS_ACCESS(t)) {
    unsigned int a_len;
    unsigned int ag_len;

    a_len = VLAD_LIST_LENGTH(acc_list);
    ag_len = VLAD_LIST_LENGTH(acc_grp_list);
    *s = a_len + ag_len;

    if ((*a = VLAD_ADT_MALLOC(char *, *s)) == NULL)
      return VLAD_MALLOCFAILED;

    /* non-group access */
    for (i = 0; i < a_len; i++)
      if ((retval = acc_list->get(i, &((*a)[i]))) != VLAD_OK)
        return retval;

    /* group access */
    for (i = a_len; i < *s; i++)
      if ((retval = acc_grp_list->get(i - a_len, &((*a)[i]))) != VLAD_OK)
        return retval;

    return VLAD_OK;
  }
  else if (VLAD_IDENT_IS_OBJECT(t)) {
    unsigned int o_len;
    unsigned int og_len;

    o_len = VLAD_LIST_LENGTH(obj_list);
    og_len = VLAD_LIST_LENGTH(obj_grp_list);
    *s = o_len + og_len;

    if ((*a = VLAD_ADT_MALLOC(char *, *s)) == NULL)
      return VLAD_MALLOCFAILED;

    /* non-group object */
    for (i = 0; i < o_len; i++)
      if ((retval = obj_list->get(i, &((*a)[i]))) != VLAD_OK)
        return retval;

    /* group object */
    for (i = o_len; i < *s; i++)
      if ((retval = obj_grp_list->get(i - o_len, &((*a)[i]))) != VLAD_OK)
        return retval;
    return VLAD_OK;
  }

  return VLAD_INVALIDINPUT;
}

/* return the total number of indentifers */
unsigned int symtab::length()
{
  if (!initialised)
    return 0;

  return
    VLAD_LIST_LENGTH(sub_list) +
    VLAD_LIST_LENGTH(acc_list) +
    VLAD_LIST_LENGTH(obj_list) +
    VLAD_LIST_LENGTH(sub_grp_list) +
    VLAD_LIST_LENGTH(acc_grp_list) +
    VLAD_LIST_LENGTH(obj_grp_list);
}

/* return the number of identifiers that are of type t */
unsigned int symtab::length(unsigned char t)
{
  if (!initialised)
    return 0;

  switch(t) {
    case VLAD_IDENT_SUBJECT :
      return VLAD_LIST_LENGTH(sub_list);
    case VLAD_IDENT_ACCESS :
      return VLAD_LIST_LENGTH(acc_list);
    case VLAD_IDENT_OBJECT :
      return VLAD_LIST_LENGTH(obj_list);
    case VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP :
      return VLAD_LIST_LENGTH(sub_grp_list);
    case VLAD_IDENT_ACCESS | VLAD_IDENT_GROUP :
      return VLAD_LIST_LENGTH(acc_grp_list);
    case VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP :
      return VLAD_LIST_LENGTH(obj_grp_list);
  }
  return 0;
}

/* return 0 if symbol is in the table */
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

/* return 0 if symbol of type t is in the table */
int symtab::find(const char *s, unsigned char t)
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  if (s == NULL)
    return VLAD_NULLPTR;

  switch(t) {
    case VLAD_IDENT_SUBJECT :
      return sub_list->find(s);
    case VLAD_IDENT_ACCESS :
      return acc_list->find(s);
    case VLAD_IDENT_OBJECT :
      return obj_list->find(s);
    case VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP:
      return sub_grp_list->find(s);
    case VLAD_IDENT_ACCESS | VLAD_IDENT_GROUP:
      return acc_grp_list->find(s);
    case VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP:
      return obj_grp_list->find(s);
  }

  return VLAD_INVALIDINPUT;
}

/* give the type of the given identifier */
int symtab::type(const char *s, unsigned char *t)
{
  int retval;

  if (!initialised)
    return VLAD_UNINITIALISED;

  if (s == NULL || t == NULL)
    return VLAD_NULLPTR;

  /* try to find s from all the lists sequentially */
  if ((retval = sub_list->find(s)) != VLAD_NOTFOUND) {
    if (retval == VLAD_OK)
      *t = VLAD_IDENT_SUBJECT;
    return retval;
  }

  if ((retval = acc_list->find(s)) != VLAD_NOTFOUND) {
    if (retval == VLAD_OK)
      *t = VLAD_IDENT_ACCESS;
    return retval;
  }

  if ((retval = obj_list->find(s)) != VLAD_NOTFOUND) {
    if (retval == VLAD_OK)
      *t = VLAD_IDENT_OBJECT;
    return retval;
  }

  if ((retval = sub_grp_list->find(s)) != VLAD_NOTFOUND) {
    if (retval == VLAD_OK)
      *t = VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP;
    return retval;
  }

  if ((retval = acc_grp_list->find(s)) != VLAD_NOTFOUND) {
    if (retval == VLAD_OK)
      *t = VLAD_IDENT_ACCESS | VLAD_IDENT_GROUP;
    return retval;
  }

  if ((retval = obj_grp_list->find(s)) != VLAD_NOTFOUND) {
    if (retval == VLAD_OK)
      *t = VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP;
    return retval;
  }

  return VLAD_NOTFOUND;
}
