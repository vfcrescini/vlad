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
#include <vlad/stringlist.h>

string::string()
{
  m_string = NULL;
}

string::~string()
{
  if (m_string != NULL)
    free(m_string);
}

/* compare item with this string */
bool string::cmp(list_item *a_item)
{
  string *tmp = NULL;

  /* a NULL will not match anything */
  if (a_item == NULL)
    return false;

  if ((tmp = dynamic_cast<string *>(a_item)) == NULL)
    return false;

  /* only return true if they are both NULL */
  if (tmp->m_string == NULL)
      return (m_string == NULL);

  return (strcmp(tmp->m_string, m_string) == 0);
}

int string::init(const char *a_str)
{
  if (a_str == NULL)
    return VLAD_NULLPTR;

  if (m_string != NULL)
    free(m_string);

  if ((m_string = VLAD_MEM_STR_MALLOC(a_str)) == NULL)
    return VLAD_MALLOCFAILED;

  return ((strcpy(m_string, a_str) == NULL) ? VLAD_FAILURE : VLAD_OK);
}

char *string::get()
{
  return m_string;
}

#ifdef VLAD_DEBUG
/* assuming s has enough memory allocation */
void string::print(char *a_str)
{
  strcpy(a_str, (m_string ? m_string : ""));
}
#endif

stringlist::stringlist() : list(true)
{
}

stringlist::~stringlist()
{
  purge(true);
}

/* add a string in the list */
int stringlist::add(const char *a_str)
{
  int retval;
  string *tmp = NULL;

  if (a_str == NULL)
    return VLAD_NULLPTR;

  if ((tmp = VLAD_MEM_NEW(string())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = tmp->init(a_str)) != VLAD_OK)
    return retval;

  if ((retval = list::add(tmp)) != VLAD_OK) {
    delete tmp;
    return retval;
  }
  return VLAD_OK;
}

/* get the index of the string */
int stringlist::get(const char *a_str, unsigned int *a_index)
{
  int retval;
  string tmp;
  unsigned int size;
  unsigned int *array;

  if (a_str == NULL || a_index == NULL)
    return VLAD_NULLPTR;

  if ((retval = tmp.init(a_str)) != VLAD_OK)
    return retval;

  if ((retval = list::get(&tmp, &array, &size)) != VLAD_OK)
    return retval;

  /* there should be exactly one in the array */
  *a_index = array[0];

  free(array);

  return VLAD_OK;
}

/* get the ith string in the list */
int stringlist::get(unsigned int a_index, char **a_str)
{
  int retval;
  string *tmp = NULL;

  /*
   * this will give a reference to the actual string and not a copy,
   * so care must be taken to ensure that s is not changed.
   */

  if ((retval = list::get(a_index, (list_item **) &tmp)) != VLAD_OK)
    return retval;

  *a_str = tmp->get();

  return VLAD_OK;
}

/* return true if string is in the list */
int stringlist::find(const char *a_str)
{
  int retval;
  string tmp;

  if ((retval = tmp.init(a_str)) != VLAD_OK)
    return retval;

  return list::find(&tmp);
}

#ifdef VLAD_DEBUG
/* assumimg s has enough memory allocation */
void stringlist::print(char *a_str)
{
  unsigned int i;
  char tmp_str[VLAD_MAXLEN_STR];
  string *tmp_obj;

  memset(tmp_str, 0, VLAD_MAXLEN_STR);

  for (i = 0; i < list::length(); i++) {
    if (list::get(i, (list_item **) &tmp_obj) != VLAD_OK)
      break;

    tmp_obj->print(tmp_str);
    sprintf(a_str, "%s %s", a_str, tmp_str);
  }
}
#endif
