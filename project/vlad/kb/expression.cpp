/*
 * expression.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <new>
  
#include <config.h>
#include <vlad.h>
#include <expression.h>

expression::expression(const char *n) : list(n, true)
{
}

expression::~expression()
{
  purge(true);
}

/* add pre-malloc'ed atom */
int expression::add(atom *a)
{
  if (a == NULL)
    return VLAD_NULLPTR;

  return list::add(a);
}

/* return true if atom is in the expression */
int expression::find(atom a)
{
  return list::find(&a);
}
