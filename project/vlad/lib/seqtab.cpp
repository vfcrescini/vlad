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
#include <new>

#include <vlad/vlad.h>
#include <vlad/seqtab.h>

transref::transref()
{
  name = NULL;
  ilist = NULL;
  initialised = false;	
}

transref::~transref()
{
  if (name != NULL)
    free(name);
  if (ilist != NULL)
    delete ilist;
}

bool transref::cmp(list_item *item)
{
  transref *tmp = NULL;

  if (item == NULL)
    return false;

  if ((tmp = dynamic_cast<transref *> (item)) == NULL)
    return false;

  /* if both are uninitialised return true. if only one -- false */
  if (!initialised)
    return !tmp->initialised;

  if (!tmp->initialised)
    return false;

  /* name cannot be NULL */
  if (strcmp(name, tmp->name))
    return false;

  return VLAD_LIST_ITEMCMP(ilist, tmp->ilist);
}

int transref::init(const char *n, stringlist *il)
{
  if (n == NULL)
    return VLAD_NULLPTR;

  name = (char *) n;
  ilist = il;
  initialised = true;

  return VLAD_OK;
}

int transref::get(char **n, stringlist **il)
{
  if (n == NULL || il == NULL)
    return VLAD_NULLPTR;

  if (!initialised)
    return VLAD_UNINITIALISED;

  *n = name;
  *il = ilist;

  return VLAD_OK;
}

#ifdef VLAD_DEBUG
void transref::print(char *s)
{
  char tmps[VLAD_MAXLEN_STR];

  if (initialised) {
    memset(tmps, 0, VLAD_MAXLEN_STR);

    if (ilist != NULL)
      ilist->print(tmps);

    sprintf(s, "%s(%s)", name, tmps);
  }
}
#endif

seqtab::seqtab() : list(false)
{
}

seqtab::~seqtab()
{
  purge(true);
}

/* add pre-malloc'ed transref */
int seqtab::add(transref *t)
{
  if (t == NULL)
    return VLAD_NULLPTR;

  return list::add((list_item *) t);
}

/* add pre-malloc'ed name and ilist */
int seqtab::add(const char *n, stringlist *il)
{
  int retval;
  transref *tmp_ref;

  if ((tmp_ref = VLAD_NEW(transref())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = tmp_ref->init(n, il)) != VLAD_OK)
    return retval;

  return list::add((list_item *) tmp_ref);
}

/* delete i'th item */
int seqtab::del(unsigned int i)
{
  return list::del(i, true);
}

/* get i'th name and ilist */
int seqtab::get(unsigned int i, char **n, stringlist **il)
{
  int retval;
  transref *tmp_ref;

  if (n == NULL || il == NULL)
    return VLAD_NULLPTR;

  if ((retval = list::get(i, (list_item **) &tmp_ref)) != VLAD_OK)
    return retval;

  return tmp_ref->get(n, il);
}

#ifdef VLAD_DEBUG
void seqtab::print(char *s)
{
  unsigned int i;
  char tmps[VLAD_MAXLEN_STR];
  transref *tmpr;

  strcpy(s, "");

  for (i = 0; i < list::length(); i++) {
    if (list::get(i, (list_item **) &tmpr) != VLAD_OK)
      break;

    memset(tmps, 0, VLAD_MAXLEN_STR);
    if (tmpr != NULL)
      tmpr->print(tmps);
    sprintf(s, "%s %s", s, tmps);
  }
}
#endif
