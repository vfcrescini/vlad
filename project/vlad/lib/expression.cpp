/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004, 2005 University of Western Sydney
 * by Vino Fernando Crescini <jcrescin@cit.uws.edu.au>
 *
 * PolicyUpdater is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * PolicyUpdater is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PolicyUpdater; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <cstdlib>
#include <cstddef>
#include <cstring>

#include <vlad/vlad.h>
#include <vlad/mem.h>
#include <vlad/expression.h>

vlad_expression::vlad_expression() : vlad_list(true)
{
}

vlad_expression::~vlad_expression()
{
  purge(true);
}

/* add pre-malloc'ed fact */
int vlad_expression::add(vlad_fact *a_fact)
{
  if (a_fact == NULL)
    return VLAD_NULLPTR;

  return vlad_list::add(a_fact);
}

/* return true if fact is in the expression */
int vlad_expression::find(vlad_fact *a_fact)
{
  if (a_fact == NULL)
    return VLAD_NULLPTR;

  return vlad_list::find(a_fact);
}
/* give a reference to the i'th fact */
int vlad_expression::get(unsigned int a_index, vlad_fact **a_fact)
{
  if (a_fact == NULL)
    return VLAD_NULLPTR;

  return vlad_list::get(a_index, (vlad_list_item **) a_fact);
}

/* make a copy */
int vlad_expression::copy(vlad_expression **a_exp)
{
  int retval;
  unsigned int i;

  if (a_exp == NULL)
    return VLAD_NULLPTR;

  if ((*a_exp = VLAD_MEM_NEW(vlad_expression())) == NULL)
    return VLAD_MALLOCFAILED;

  /* go through each fact in this expression */
  for (i = 0; i < vlad_list::length(); i++) {
    vlad_fact *old_fact;
    vlad_fact *new_fact;

    if ((retval = vlad_expression::get(i, &old_fact)) != VLAD_OK)
      return retval;
    if ((retval = old_fact->copy(&new_fact)) != VLAD_OK)
      return retval;
    if ((retval = (*a_exp)->add(new_fact)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

/* verify and copy */
int vlad_expression::vcopy(vlad_symtab *a_stab,
                           vlad_varlist *a_vlist,
                           vlad_expression **a_exp)
{
  int retval;
  unsigned int i;

  if (a_exp == NULL)
    return VLAD_NULLPTR;

  if ((*a_exp = VLAD_MEM_NEW(vlad_expression())) == NULL)
    return VLAD_MALLOCFAILED;

  /* go through each fact in this expression */
  for (i = 0; i < vlad_list::length(); i++) {
    vlad_fact *old_fact;
    vlad_fact *new_fact;

    if ((retval = vlad_expression::get(i, &old_fact)) != VLAD_OK)
      return retval;

    /* instead of a copy, we use vcopy */
    retval = old_fact->vcopy(a_stab, a_vlist, &new_fact);
    if (retval != VLAD_OK)
      return retval;

    if ((retval = (*a_exp)->add(new_fact)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

/* replace vars in vlist to ident in ilist. create a new expression */
int vlad_expression::replace(vlad_varlist *a_vlist,
                             vlad_stringlist *a_ilist,
                             vlad_expression **a_exp)
{
  int retval;
  unsigned int i;

  if (a_exp == NULL)
    return VLAD_NULLPTR;

  if ((*a_exp = VLAD_MEM_NEW(vlad_expression())) == NULL)
    return VLAD_MALLOCFAILED;

  for (i = 0; i < vlad_list::length(); i++) {
    vlad_fact *old_fact;
    vlad_fact *new_fact;

    if ((retval = get(i, &old_fact)) != VLAD_OK)
      return retval;
    if ((retval = old_fact->replace(a_vlist, a_ilist, &new_fact)) != VLAD_OK)
      return retval;
    if ((retval = (*a_exp)->add(new_fact)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

/* replace then verify */
int vlad_expression::vreplace(vlad_symtab *a_stab,
                              vlad_varlist *a_vlist,
                              vlad_stringlist *a_ilist,
                              vlad_expression **a_exp)
{
  int retval;
  unsigned int i;

  if (a_exp == NULL)
    return VLAD_NULLPTR;

  if ((*a_exp = VLAD_MEM_NEW(vlad_expression())) == NULL)
    return VLAD_MALLOCFAILED;

  for (i = 0; i < vlad_list::length(); i++) {
    vlad_fact *old_fact;
    vlad_fact *new_fact;

    if ((retval = get(i, &old_fact)) != VLAD_OK)
      return retval;

    if ((retval = old_fact->replace(a_vlist, a_ilist, &new_fact)) != VLAD_OK)
      return retval;

    if ((retval = new_fact->verify(a_stab, NULL)) != VLAD_OK) {
      /* if the new fact failed to verify, we stop right here */
      VLAD_MEM_DELETE(new_fact);
      VLAD_MEM_DELETE(*a_exp);
      return retval;
    }

    if ((retval = (*a_exp)->add(new_fact)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

/* gives a list of vars occuring in the expr. assumes list is init'ed */
int vlad_expression::varlist(vlad_varlist *a_list)
{
  int retval;
  unsigned int i;

  if (a_list == NULL)
    return VLAD_NULLPTR;

  for (i = 0; i < vlad_list::length(); i++) {
    vlad_fact *fact;

    if ((retval = get(i, &fact)) != VLAD_OK)
      return retval;

    if ((retval = fact->varlist(a_list)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

/* as above, but verify first */
int vlad_expression::vvarlist(vlad_symtab *a_stab, vlad_varlist *a_list)
{
  int retval;
  unsigned int i;

  if (a_stab == NULL || a_list == NULL)
    return VLAD_NULLPTR;

  for (i = 0; i < vlad_list::length(); i++) {
    vlad_fact *fact;

    if ((retval = get(i, &fact)) != VLAD_OK)
      return retval;

    if ((retval = fact->vvarlist(a_stab, a_list)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

/* check if exp is valid, any variables that occur must be in a_vlist */
int vlad_expression::verify(vlad_symtab *a_stab, vlad_varlist *a_vlist)
{
  int retval;
  unsigned int i;

  if (a_stab == NULL)
    return VLAD_NULLPTR;

  for (i = 0; i < vlad_list::length(); i++) {
    vlad_fact *fact;

    if ((retval = get(i, &fact)) != VLAD_OK)
      return retval;

    if ((retval = fact->verify(a_stab, a_vlist)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

/* returns VLAD_OK if all facts are ground */
int vlad_expression::is_ground()
{
  int retval;
  unsigned int i;

  for (i = 0; i < vlad_list::length(); i++) {
    vlad_fact *fact;

    if ((retval = get(i, &fact)) != VLAD_OK)
      return retval;

    if ((retval = fact->is_ground()) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

#ifdef VLAD_DEBUG
/* assumimg s has enough memory allocation */
void vlad_expression::print(char *a_str)
{
  unsigned int i;
  char str[VLAD_MAXLEN_STR];
  vlad_fact *obj;

  for (i = 0; i < vlad_list::length(); i++) {
    if (vlad_list::get(i, (vlad_list_item **) &obj) != VLAD_OK)
      break;

    memset(str, 0, VLAD_MAXLEN_STR);
    obj->print(str);
    sprintf(a_str, "%s %s", a_str, str);
  }
}
#endif

/* called by vlad_list::traverse() */
int vlad_expression::trav(vlad_list_item *a_item, vlad_list_trav *a_trav)
{
  vlad_fact *nitem;
  vlad_expression_trav *ntrav;

  if ((nitem = dynamic_cast<vlad_fact *>(a_item)) == NULL)
    return VLAD_NULLPTR;

  if ((ntrav = dynamic_cast<vlad_expression_trav *>(a_trav)) == NULL)
    return VLAD_NULLPTR;

  return ntrav->trav(nitem);
}
