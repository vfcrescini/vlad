/*
 * expression.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <new>
  
#include <config.h>
#include <vlad/vlad.h>
#include <vlad/expression.h>

expression::expression() : list(true)
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
int expression::find(atom *a)
{
  if (a == NULL)
    return VLAD_NULLPTR;

  return list::find(a);
}
/* give a reference to the i'th atom */
int expression::get(unsigned int i, atom **a)
{
  if (a == NULL)
    return VLAD_NULLPTR;

  return list::get(i, (list_item **) a);
}

/* replace vars in vlist to ident in ilist. create a new expression */
int expression::replace(stringlist *vlist, stringlist *ilist, expression **e)
{
  int retval;
  unsigned int i;
  atom *old_atom;
  atom *new_atom;

  if (e == NULL)
    return VLAD_NULLPTR;

  if ((*e = VLAD_NEW(expression())) == NULL)
    return VLAD_MALLOCFAILED;

  for (i = 0; i < list::length(); i++) {
    if ((retval = get(i, &old_atom)) != VLAD_OK)
      return retval;
    if ((retval = old_atom->replace(vlist, ilist, &new_atom)) != VLAD_OK)
      return retval;
    if ((retval = (*e)->add(new_atom)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

#ifdef DEBUG
/* assumimg s has enough memory allocation */
void expression::print(char *s)
{
  unsigned int i;
  char tmps[VLAD_MAXLEN_STR];
  atom *tmpa;

  strcpy(s, "");

  for (i = 0; i < list::length(); i++) {
    if (list::get(i, (list_item **) &tmpa) != VLAD_OK)
      break;

    strcpy(tmps, "");
    tmpa->print(tmps);
    sprintf(s, "%s %s", s, tmps);
  }
}
#endif
