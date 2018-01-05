/*
 * symtab.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <cstring>
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
  t_const = NULL;
  f_const = NULL;
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
  if (t_const != NULL)
    free(t_const); 
  if (f_const != NULL)
    free(f_const);
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
  if (t_const != NULL)
    free(t_const); 
  if (f_const != NULL)
    free(f_const);

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
  if ((t_const = VLAD_STRING_MALLOC(VLAD_CONST_TRUE)) == NULL)
    return VLAD_MALLOCFAILED;
  if ((f_const = VLAD_STRING_MALLOC(VLAD_CONST_FALSE)) == NULL)
    return VLAD_MALLOCFAILED;

  /* strcpy never fails */
  strcpy(t_const, VLAD_CONST_TRUE);
  strcpy(f_const, VLAD_CONST_FALSE);

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
    /* of course, we do not allow the constants to be added */
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

  /* first see if they are constants */
  if (!strcmp(s, VLAD_CONST_FALSE)) {
    *t = VLAD_IDENT_CONST;
    *i = 0;
    return VLAD_OK;
  }

  if (!strcmp(s, VLAD_CONST_TRUE)) {
    *t = VLAD_IDENT_CONST;
    *i = 1;
    return VLAD_OK;
  }

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
    case VLAD_IDENT_CONST :
      if (i == 0) {
        *s = f_const;
        return VLAD_OK;
      }
      if (i == 1) {
        *s = t_const;
        return VLAD_OK;
      }
      return VLAD_INVALIDINPUT;
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

  if (VLAD_IDENT_IS_CONST(t)) {

    *s = 2;

    if ((*a = VLAD_ADT_MALLOC(char *, *s)) == NULL)
      return VLAD_MALLOCFAILED;

    (*a)[0] = f_const;
    (*a)[1] = t_const;

    return VLAD_OK;
  }
  else if (VLAD_IDENT_IS_SUBJECT(t)) {
    unsigned int s_len;
    unsigned int sg_len;

    s_len = sub_list->length();
    sg_len = sub_grp_list->length();
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

    a_len = acc_list->length();
    ag_len = acc_grp_list->length();
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

    o_len = obj_list->length();
    og_len = obj_grp_list->length();
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

/* return the number of identifiers that are of type t */
unsigned int symtab::length(unsigned char t)
{
  if (!initialised)
    return 0;

  switch(t) { 
    case VLAD_IDENT_CONST :
      return 2;
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

/* give the type of the given identifier */
int symtab::type(const char *s, unsigned char *t)
{
  int retval;

  if (!initialised)
    return VLAD_UNINITIALISED;

  if (s == NULL || t == NULL)
    return VLAD_NULLPTR;

  /* first see if they are constants */
  if (!strcmp(s, VLAD_CONST_FALSE)) {
    *t = VLAD_IDENT_CONST;
    return VLAD_OK;
  }

  if (!strcmp(s, VLAD_CONST_TRUE)) {
    *t = VLAD_IDENT_CONST;
    return VLAD_OK;
  }

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
