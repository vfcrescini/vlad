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

constraint::constraint(expression *e, expression *c, expression *n)
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

  if (tmp->exp == NULL && exp != NULL)
    return false;
  else if (tmp->exp != NULL && exp == NULL)
    return false;
  else if (tmp->exp != NULL && exp != NULL && !exp->cmp(tmp->exp))
    return false;

  if (tmp->cond == NULL && cond != NULL)
    return false;
  else if (tmp->cond != NULL && cond == NULL)
    return false;
  else if (tmp->cond != NULL && cond != NULL && !cond->cmp(tmp->cond))
    return false;

  if (tmp->ncond == NULL && ncond != NULL)
    return false;
  else if (tmp->ncond != NULL && ncond == NULL)
    return false;
  else if (tmp->ncond != NULL && ncond != NULL && !ncond->cmp(tmp->ncond))
    return false;

  return true;
}

consttab::consttab() : list(true)
{
}

consttab::consttab(const char *n) : list(n, true)
{
}

consttab::~consttab()
{
  purge(true);
}

int consttab::add(expression *e, expression *c, expression *n)
{
  constraint *tmp;

  if (e == NULL || c == NULL || n == NULL)
    return VLAD_NULLPTR;

  if ((tmp = VLAD_NEW(constraint(e, c, n))) == NULL)
    return VLAD_MALLOCFAILED;

  return list::add((list_item *) tmp);
}
