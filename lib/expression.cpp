/*
 * expression.cpp
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <new>

#include <vlad/vlad.h>
#include <vlad/expression.h>

expression::expression() : list(true)
{
}

expression::~expression()
{
  purge(true);
}

/* add pre-malloc'ed fact */
int expression::add(fact *a_fact)
{
  if (a_fact == NULL)
    return VLAD_NULLPTR;

  return list::add(a_fact);
}

/* return true if fact is in the expression */
int expression::find(fact *a_fact)
{
  if (a_fact == NULL)
    return VLAD_NULLPTR;

  return list::find(a_fact);
}
/* give a reference to the i'th fact */
int expression::get(unsigned int a_index, fact **a_fact)
{
  if (a_fact == NULL)
    return VLAD_NULLPTR;

  return list::get(a_index, (list_item **) a_fact);
}

/* replace occurences of var with ident, creates a new expression */
int expression::replace(const char *a_var,
                        const char *a_ident,
                        expression **a_exp)
{
  int retval;
  unsigned int i;
  fact *old_fact;
  fact *new_fact;

  if (a_exp == NULL)
    return VLAD_NULLPTR;

  if ((*a_exp = VLAD_NEW(expression())) == NULL)
    return VLAD_MALLOCFAILED;

  for (i = 0; i < list::length(); i++) {
    if ((retval = get(i, &old_fact)) != VLAD_OK)
      return retval;
    if ((retval = old_fact->replace(a_var, a_ident, &new_fact)) != VLAD_OK)
      return retval;
    if ((retval = (*a_exp)->add(new_fact)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

/* replace vars in vlist to ident in ilist. create a new expression */
int expression::replace(stringlist *a_vlist, stringlist *a_ilist, expression **a_exp)
{
  int retval;
  unsigned int i;
  fact *old_fact;
  fact *new_fact;

  if (a_exp == NULL)
    return VLAD_NULLPTR;

  if ((*a_exp = VLAD_NEW(expression())) == NULL)
    return VLAD_MALLOCFAILED;

  for (i = 0; i < list::length(); i++) {
    if ((retval = get(i, &old_fact)) != VLAD_OK)
      return retval;
    if ((retval = old_fact->replace(a_vlist, a_ilist, &new_fact)) != VLAD_OK)
      return retval;
    if ((retval = (*a_exp)->add(new_fact)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

/* gives a list of vars occuring in the expr. assumes list is init'ed */
int expression::varlist(stringlist **a_list)
{
  int retval;
  unsigned int i;
  fact *tmp;

  for (i = 0; i < list::length(); i++) {
    if ((retval = get(i, &tmp)) != VLAD_OK)
      return retval;
    if ((retval = tmp->varlist(a_list)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}


#ifdef VLAD_DEBUG
/* assumimg s has enough memory allocation */
void expression::print(char *a_str)
{
  unsigned int i;
  char tmp_str[VLAD_MAXLEN_STR];
  fact *tmp_obj;

  for (i = 0; i < list::length(); i++) {
    if (list::get(i, (list_item **) &tmp_obj) != VLAD_OK)
      break;

    memset(tmp_str, 0, VLAD_MAXLEN_STR);
    tmp_obj->print(tmp_str);
    sprintf(a_str, "%s %s", a_str, tmp_str);
  }
}
#endif
