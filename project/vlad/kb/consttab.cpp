/*
 * consttab.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <new>

#include <config.h>
#include <vlad.h>
#include <consttab.h>

constraint::constraint(numberlist *e, numberlist *c, numberlist *n)
{
  exp = e;
  cond = c;
  ncond = n;
}

constraint::~constraint()
{
  if (exp != NULL)
    delete exp;
  if (cond != NULL)
    delete cond;
  if (ncond != NULL)
    delete ncond;
}

bool constraint::cmp(list_item *item)
{
  constraint *tmp = NULL;

  if (item == NULL)
    return false;

  if ((tmp = dynamic_cast<constraint *> (item)) == NULL)
    return false; 

  return tmp->exp->cmp(exp) && tmp->cond->cmp(cond) && tmp->ncond->cmp(ncond);
}

consttab::consttab() : list("consttab", true)
{
}

consttab::~consttab()
{
  purge(true);
}

int consttab::add(numberlist *e, numberlist *c, numberlist *n)
{
  constraint *tmp;

  if (e == NULL || c == NULL || n == NULL)
    return VLAD_NULLPTR;

  if ((tmp = VLAD_NEW(constraint(e, c, n))) == NULL)
    return VLAD_MALLOCFAILED;

  return list::add((list_item *) tmp);
}
