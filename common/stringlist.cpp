/*
 * stringlist.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <new>

#include <config.h>
#include <vlad.h>
#include <stringlist.h>

string::string()
{
  str = NULL;
}

string::~string()
{
  if (str != NULL)
    free(str);
}

/* compare item with this string */
bool string::cmp(list_item *item)
{
  string *tmp = NULL;

  /* a NULL will not match anything */
  if (item == NULL)
    return false;

  tmp = dynamic_cast<string *> (item);

  /* only return true if they are both NULL */
  if (tmp->str == NULL)
      return (str == NULL);

  return (strcmp(tmp->str, str) == 0);
}

int string::init(const char *s)
{
  if (s == NULL)
    return VLAD_NULLPTR;

  if (str != NULL)
    free(str);

  if ((str = VLAD_STRING_MALLOC(s)) == NULL)
    return VLAD_MALLOCFAILED;

  return ((strcpy(str, s) == NULL) ? VLAD_FAILURE : VLAD_OK);
}

char *string::get()
{
  return str;
}

/* n is the optional tmp for the list */
stringlist::stringlist(const char *n) : list(n, true)
{
}

stringlist::~stringlist()
{
  purge(true);
}

/* add a string in the list */
int stringlist::add(const char *s)
{
  int retval;
  string *tmp;

  if (s == NULL)
    return VLAD_NULLPTR;

  if ((tmp = VLAD_NEW(string())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = tmp->init(s)) != VLAD_OK)
    return retval;

  if ((retval = list::add(tmp)) != VLAD_OK) {
    delete tmp;
    return retval;
  }
  return VLAD_OK;
}

/* get the index of the string */
int stringlist::get(const char *s, unsigned int *i)
{
  int retval;
  string tmp;
  unsigned int size;
  unsigned int *array;

  if (s == NULL || i == NULL)
    return VLAD_NULLPTR;

  if ((retval = tmp.init(s)) != VLAD_OK)
    return retval;

  if ((retval = list::get(&tmp, &array, &size)) != VLAD_OK)
    return retval;

  /* there should be exactly one in the array */
  *i = array[0];

  free(array);

  return VLAD_OK;
}

/* get the ith string in the list */
int stringlist::get(unsigned int i, char **s)
{
  string *tmp = NULL;
  int retval;

  /*
   * this will give a reference to the actual string and not a copy,
   * so care must be taken to ensure that s is not changed. 
   */

  if ((retval = list::get(i, (list_item **) &tmp)) != VLAD_OK)
    return retval;

  *s = tmp->get();

  return VLAD_OK;
}

/* return true if string is in the list */
int stringlist::find(const char *s)
{
  int retval;
  string tmp;

  if ((retval = tmp.init(s)) != VLAD_OK)
    return retval;

  return list::find(&tmp);
}
