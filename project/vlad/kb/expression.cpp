/*
 * expression.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <cstring>
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
/* give a reference to the i'th atom */
int expression::get(unsigned int i, atom **a)
{
  if (a == NULL)
    return VLAD_NULLPTR;

  return list::get(i, (list_item **) a);
}

#ifdef DEBUG
/* assumimg s has enough memory allocation */
void expression::print(char *s)
{
  unsigned int i;
  char tmps[128];
  atom *tmpa;

  strcpy(s, "");

  for (i = 0; i < list::length(); i++) {
    if (list::get(i, (list_item **) &tmpa) != VLAD_OK)
      break;

    tmpa->print(tmps);
    sprintf(s, "%s %s", s, tmps);
  }
}
#endif
