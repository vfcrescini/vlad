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

numberlist::numberlist() : list(false)
{
}

numberlist::~numberlist()
{
  purge(true);
}

/* compare list with this list */
bool numberlist::cmp(list_item *item)
{
  return list::cmp((list *)item);
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

/* get the i'th number in the list */
int numberlist::get(unsigned int i, unsigned int *n)
{
  int retval;
  number *tmp = NULL;

  if ((retval = list::get(i, (list_item **) &tmp)) != VLAD_OK)
    return retval;

  *n = tmp->get();

  return VLAD_OK;
}

numberlistlist::numberlistlist()
{
}

numberlistlist::~numberlistlist()
{
  purge(true);
}

/* add a number list to this list */
int numberlistlist::add(numberlist *l)
{
  if (l == NULL)
    return VLAD_NULLPTR;

  /* we assume it is pre-malloc'ed */
  return list::add(l);
}

/* get the i'th number list in this list */
int numberlistlist::get(unsigned int i, numberlist **l)
{
  return list::get(i, (list_item **) l);
}