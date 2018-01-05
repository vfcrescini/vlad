/*
 * numberlist.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <new>

#include <config.h>
#include <vlad.h>
#include <numberlist.h>

number::number()
{
  num = 0;
}

number::~number()
{
}

/* compare item with this number */
bool number::cmp(list_item *item)
{
  number *tmp = NULL;

  /* a NULL will not match anything */
  if (item == NULL)
    return false;

  if ((tmp = dynamic_cast<number *> (item)) == NULL)
    return false;

  return (tmp->num == num);
}

int number::init(unsigned int n)
{
  num = n;
  return VLAD_OK;
}

unsigned int number::get()
{
  return num;
}

numberlist::numberlist() : list(true)
{
}

numberlist::~numberlist()
{
  purge(true);
}

/* add a number in the list */
int numberlist::add(unsigned int n)
{
  int retval;
  number *tmp = NULL;

  if ((tmp = VLAD_NEW(number())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = tmp->init(n)) != VLAD_OK)
    return retval;

  if ((retval = list::add(tmp)) != VLAD_OK) {
    delete tmp;
    return retval;
  }
  return VLAD_OK;
}

/* get the index of the number */
int numberlist::geti(unsigned int n, unsigned int *i)
{
  int retval;
  number tmp;
  unsigned int size;
  unsigned int *array;

  if (i == NULL)
    return VLAD_NULLPTR;

  if ((retval = tmp.init(n)) != VLAD_OK)
    return retval;

  if ((retval = list::get(&tmp, &array, &size)) != VLAD_OK)
    return retval;

  /* there should be exactly one in the array */
  *i = array[0];

  free(array);

  return VLAD_OK;
}

/* get the ith number in the list */
int numberlist::getn(unsigned int i, unsigned int *n)
{
  int retval;
  number *tmp = NULL;

  /*
   * this will give a reference to the actual number and not a copy,
   * so care must be taken to ensure that s is not changed. 
   */

  if ((retval = list::get(i, (list_item **) &tmp)) != VLAD_OK)
    return retval;

  *n = tmp->get();

  return VLAD_OK;
}

/* return true if number is in the list */
int numberlist::find(unsigned int n)
{
  int retval;
  number tmp;

  if ((retval = tmp.init(n)) != VLAD_OK)
    return retval;

  return list::find(&tmp);
}
