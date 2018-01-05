/*
 * numberlist.cpp
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <new>

#include <vlad/vlad.h>
#include <vlad/numberlist.h>

number::number()
{
  m_number = 0;
}

number::~number()
{
}

/* compare item with this number */
bool number::cmp(list_item *a_item)
{
  number *tmp = NULL;

  /* a NULL will not match anything */
  if (a_item == NULL)
    return false;

  if ((tmp = dynamic_cast<number *>(a_item)) == NULL)
    return false;

  return (tmp->m_number == m_number);
}

int number::init(unsigned int a_num)
{
  m_number = a_num;
  return VLAD_OK;
}

unsigned int number::get()
{
  return m_number;
}

numberlist::numberlist() : list(false)
{
}

numberlist::~numberlist()
{
  list::purge(true);
}

/* add a number in the list */
int numberlist::add(unsigned int a_num)
{
  int retval;
  number *tmp = NULL;

  if ((tmp = VLAD_NEW(number())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = tmp->init(a_num)) != VLAD_OK)
    return retval;

  if ((retval = list::add(tmp)) != VLAD_OK) {
    delete tmp;
    return retval;
  }
  return VLAD_OK;
}

/* get the i'th number in the list */
int numberlist::get(unsigned int a_index, unsigned int *a_num)
{
  int retval;
  number *tmp = NULL;

  if ((retval = list::get(a_index, (list_item **) &tmp)) != VLAD_OK)
    return retval;

  *a_num = tmp->get();

  return VLAD_OK;
}

/* empty the list */
void numberlist::purge()
{
  list::purge(true);
}
