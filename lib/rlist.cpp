/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004, 2005, 2006 University of Western Sydney
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
#include <vlad/rlist.h>

vlad_rlist::vlad_rlist() : vlad_list(true)
{
}

vlad_rlist::~vlad_rlist()
{
  purge(true);
}

/* adds a relation to the list */
int vlad_rlist::add(vlad_rel *a_rel)
{
  int retval;
  vlad_rel *rptr = NULL;
  vlad_rel **rarray = NULL;
  unsigned int size;

  if (a_rel == NULL)
    return VLAD_NULLPTR;

  retval = VLAD_OK;

  /* make a copy */
  if (retval == VLAD_OK)
    retval = a_rel->copy(&rptr);
   
  if (retval == VLAD_OK) {
    retval = vlad_list::get((vlad_list_item *) rptr,
                            (vlad_list_item ***) &rarray,
                            &size);
  }

  /* if not already in the list, we add it */
  if (retval == VLAD_NOTFOUND)
    return vlad_list::add(rptr);

  /* if already in, we just join the relset */
  if (retval == VLAD_OK)
    retval = (rarray[0])->join(rptr);

  /* cleanup */
  if (rarray != NULL)
    VLAD_MEM_FREE(rarray);
  if (rptr != NULL)
    VLAD_MEM_DELETE(rptr);

  return retval; 
}

/* gives a reference to the ith relation */
int vlad_rlist::get(unsigned int a_i, vlad_rel **a_rel)
{
  return vlad_list::get(a_i, (vlad_list_item **) a_rel);
}

/* make a copy */
int vlad_rlist::copy(vlad_rlist **a_rlist)
{
  int retval;
  unsigned int i;
  vlad_rlist *rlist;

  if (a_rlist == NULL)
    return VLAD_NULLPTR;

  if ((rlist = VLAD_MEM_NEW(vlad_rlist())) == NULL)
    return VLAD_MALLOCFAILED;

  retval = VLAD_OK;

  /* go through each rel in this rlist */
  for (i = 0; retval == VLAD_OK && i < vlad_list::length(); i++) {
    vlad_rel *old_rel = NULL;
    vlad_rel *new_rel = NULL;

    if (retval == VLAD_OK)
      retval = vlad_rlist::get(i, &old_rel);
    if (retval == VLAD_OK)
      retval = old_rel->copy(&new_rel);
    if (retval == VLAD_OK)
      retval = rlist->vlad_list::add((vlad_list_item *) new_rel);
    if (retval != VLAD_OK && new_rel != NULL)
      VLAD_MEM_DELETE(new_rel);
  }

  if (retval != VLAD_OK) {
    VLAD_MEM_DELETE(rlist);
    return retval;
  }

  *a_rlist = rlist;

  return VLAD_OK;
}

/* verify then copy */
int vlad_rlist::vcopy(vlad_symtab *a_stab,
                      vlad_varlist *a_vlist,
                      vlad_rlist **a_rlist)
{
  int retval;
  unsigned int i;
  vlad_rlist *rlist;

  if (a_stab == NULL || a_rlist == NULL)
    return VLAD_NULLPTR;

  if ((rlist = VLAD_MEM_NEW(vlad_rlist())) == NULL)
    return VLAD_MALLOCFAILED;

  retval = VLAD_OK;

  /* go through each rel in this rlist */
  for (i = 0; retval == VLAD_OK && i < vlad_list::length(); i++) {
    vlad_rel *old_rel = NULL;
    vlad_rel *new_rel = NULL;

    if (retval == VLAD_OK)
      retval = vlad_rlist::get(i, &old_rel);
    if (retval == VLAD_OK)
      retval = old_rel->vcopy(a_stab, a_vlist, &new_rel);
    if (retval == VLAD_OK)
      retval = rlist->vlad_list::add((vlad_list_item *) new_rel);
    if (retval != VLAD_OK && new_rel != NULL)
      VLAD_MEM_DELETE(new_rel); 
  }

  if (retval != VLAD_OK) {
    VLAD_MEM_DELETE(rlist);
    return retval;
  }

  *a_rlist = rlist;

  return VLAD_OK;
}

/* replace vars in vlist to ident in ilist. create a new rlist */
int vlad_rlist::replace(vlad_varlist *a_vlist,
                        vlad_stringlist *a_ilist,
                        vlad_rlist **a_rlist)
{
  int retval;
  unsigned int i;
  vlad_rlist *rlist;

  if (a_rlist == NULL)
    return VLAD_NULLPTR;

  if ((rlist = VLAD_MEM_NEW(vlad_rlist())) == NULL)
    return VLAD_MALLOCFAILED;

  retval = VLAD_OK;

  for (i = 0; retval == VLAD_OK && i < vlad_list::length(); i++) {
    vlad_rel *old_rel = NULL;
    vlad_rel *new_rel = NULL;

    if (retval == VLAD_OK)
      retval = vlad_list::get(i, (vlad_list_item **) &old_rel);
    if (retval == VLAD_OK)
      retval = old_rel->replace(a_vlist, a_ilist, &new_rel);
    if (retval == VLAD_OK)
      retval = rlist->vlad_list::add((vlad_list_item *) new_rel);
    if (retval != VLAD_OK && new_rel != NULL)
      VLAD_MEM_DELETE(new_rel); 
  }

  /* cleanup */
  if (retval != VLAD_OK) {
    VLAD_MEM_DELETE(rlist);
    return retval;
  }

  *a_rlist = rlist;

  return VLAD_OK;
}

/* replace then verify */
int vlad_rlist::vreplace(vlad_symtab *a_stab,
                         vlad_varlist *a_vlist,
                         vlad_stringlist *a_ilist,
                         vlad_rlist **a_rlist)
{
  int retval;
  unsigned int i;
  vlad_rlist *rlist;

  if (a_stab == NULL || a_rlist == NULL)
    return VLAD_NULLPTR;

  if ((rlist = VLAD_MEM_NEW(vlad_rlist())) == NULL)
    return VLAD_MALLOCFAILED;

  retval = VLAD_OK;

  for (i = 0; retval == VLAD_OK && i < vlad_list::length(); i++) {
    vlad_rel *old_rel = NULL;
    vlad_rel *new_rel = NULL;

    if (retval == VLAD_OK)
      retval = vlad_list::get(i, (vlad_list_item **) &old_rel);
    if (retval == VLAD_OK)
      retval = old_rel->vreplace(a_stab, a_vlist, a_ilist, &new_rel);
    if (retval == VLAD_OK)
      retval = rlist->vlad_list::add((vlad_list_item *) new_rel);
    if (retval != VLAD_OK && new_rel != NULL)
      VLAD_MEM_DELETE(new_rel); 
  }

  /* cleanup */
  if (retval != VLAD_OK) {
    VLAD_MEM_DELETE(rlist);
    return retval;
  }

  *a_rlist = rlist;

  return VLAD_OK;
}

/* gives a list of vars occurring in the rlist. assumes list is init'ed */
int vlad_rlist::varlist(vlad_varlist *a_vlist)
{
  int retval;
  unsigned int i;

  if (a_vlist == NULL)
    return VLAD_NULLPTR;

  for (i = 0; i < vlad_list::length(); i++) {
    vlad_rel *rel;

    if ((retval = get(i, &rel)) != VLAD_OK)
      return retval;

    if ((retval = rel->varlist(a_vlist)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

/* as above, but verify first */
int vlad_rlist::vvarlist(vlad_symtab *a_stab, vlad_varlist *a_vlist)
{
  int retval;
  unsigned int i;

  if (a_stab == NULL || a_vlist == NULL)
    return VLAD_NULLPTR;

  for (i = 0; i < vlad_list::length(); i++) {
    vlad_rel *rel;

    if ((retval = get(i, &rel)) != VLAD_OK)
      return retval;

    if ((retval = rel->vvarlist(a_stab, a_vlist)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

/* ensure that each interval is in symtab and each var is in varlist */
int vlad_rlist::verify(vlad_symtab *a_stab, vlad_varlist *a_vlist)
{
  int retval;
  unsigned int i;

  if (a_stab == NULL)
    return VLAD_NULLPTR;

  for (i = 0; i < vlad_list::length(); i++) {
    vlad_rel *rel;

    if ((retval = get(i, &rel)) != VLAD_OK)
      return retval;

    if ((retval = rel->verify(a_stab, a_vlist)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

/* returns VLAD_OK if the list contains no variables */
int vlad_rlist::is_ground()
{
  int retval;
  unsigned int i;

  for (i = 0; i < vlad_list::length(); i++) {
    vlad_rel *rel;

    if ((retval = get(i, &rel)) != VLAD_OK)
      return retval;

    if ((retval = rel->is_ground()) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

/* returns VLAD_OK if all the rels contain at least one variable */
int vlad_rlist::is_allnonground()
{
  int retval;
  unsigned int i;

  for (i = 0; i < vlad_list::length(); i++) {
    vlad_rel *rel;

    if ((retval = get(i, &rel)) != VLAD_OK)
      return retval;

    switch((retval = rel->is_ground())) {
      case VLAD_FAILURE :
        /* VLAD_FAILURE means non-ground */
        continue;
      case VLAD_OK :
        /* VLAD_OK means ground, which means it failed the test */
        return VLAD_FAILURE;
      default :
        return retval;
    }
  }

  return VLAD_OK;
}

