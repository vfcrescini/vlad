/*
 *  This file is part of PolicyUpdater.
 *
 *  Copyright (C) 2003, 2004, 2005 University of Western Sydney
 *  by Vino Fernando Crescini <jcrescin@cit.uws.edu.au>
 *
 *  PolicyUpdater is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  PolicyUpdater is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with PolicyUpdater; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <cstdlib>

#include <vlad/vlad.h>
#include <vlad/mem.h>
#include <vlad/tnet.h>

vlad_tnet::vlad_tnet()
{
  m_init = false;
  m_net = NULL;
}

vlad_tnet::~vlad_tnet()
{
  if (m_net != NULL)
    tbe_net_destroy(&m_net);
}

/* (re)initialise temporal network */
int vlad_tnet::init()
{
  if (m_net != NULL)
    tbe_net_destroy(&m_net);

  if (tbe_net_create(&m_net) != TBE_OK)
    return VLAD_MALLOCFAILED;

  m_init = true;

  return VLAD_OK;
}

/* add a new interval (a_int must already be in the symtab */
int vlad_tnet::add_interval(const char *a_int, vlad_symtab *a_stab)
{
  int retval;
  unsigned int index;

  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_int == NULL || a_stab == NULL)
    return VLAD_NULLPTR;

  /* try to get the index of interval a_int from the symtab */
  if ((retval = a_stab->get(a_int, VLAD_IDENT_INT, &index)) != VLAD_OK)
    return retval;

  switch(tbe_net_add_interval(m_net, index)) {
    case TBE_OK :
      return VLAD_OK;
    case TBE_MALLOCFAILED :
      return VLAD_MALLOCFAILED;
    case TBE_DUPLICATE :
      return VLAD_DUPLICATE;
  }

  return VLAD_FAILURE;
}

/* bind the given (existing) interval with the given endpoints */
int vlad_tnet::add_endpoints(const char *a_int,
                             unsigned int a_ep1,
                             unsigned int a_ep2,
                             vlad_symtab *a_stab)
{
  int retval;
  unsigned int index;
  tbe_interval interval;

  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_int == NULL || a_stab == NULL)
    return VLAD_NULLPTR;

  /* try to get the index of interval a_int from the symtab */
  if ((retval = a_stab->get(a_int, VLAD_IDENT_INT, &index)) != VLAD_OK)
    return retval;

  interval.ep_1 = a_ep1;
  interval.ep_2 = a_ep2;
  interval.ep_mask = TBE_INTERVAL_EP_ALL;
 
  switch(tbe_net_add_endpoints(m_net, index, interval)) {
    case TBE_OK :
      return VLAD_OK;
    case TBE_MALLOCFAILED :
      return VLAD_MALLOCFAILED;
  }

  return VLAD_FAILURE;
}

/* add the given temporal constraints */
int vlad_tnet::add_constraints(vlad_rlist *a_rlist, vlad_symtab *a_stab)
{
  int retval;
  unsigned int i;
  vlad_rel *rel;

  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_rlist == NULL || a_stab == NULL)
    return VLAD_NULLPTR;

  /* go through each relation in the list */
  for (i = 0; i < VLAD_LIST_LENGTH(a_rlist); i++) {
    char *int1;
    char *int2;
    unsigned int rs;
    unsigned int index1;
    unsigned int index2;

    if ((retval = a_rlist->get(i, &rel)) != VLAD_OK)
      return retval;
    if ((retval = rel->get(&int1, &int2, &rs)) != VLAD_OK)
      return retval;

    /* try to get the indices of intervals from the symtab */
    if ((retval = a_stab->get(int1, VLAD_IDENT_INT, &index1)) != VLAD_OK)
      return retval;
    if ((retval = a_stab->get(int2, VLAD_IDENT_INT, &index2)) != VLAD_OK)
      return retval;

    /* add to the network */
    if (tbe_net_add_relation(m_net, index1, index2, rs) != TBE_OK)
      return VLAD_FAILURE;
  }

  return VLAD_OK;
}

/* gives the relation set that exists between the given 2 intervals */
int vlad_tnet::get_relset(const char *a_int1,
                          const char *a_int2,
                          vlad_symtab *a_stab,
                          unsigned int *a_rs)
{
  int retval;
  unsigned int index1;
  unsigned int index2;

  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_int1 == NULL || a_int2 == NULL || a_stab == NULL || a_rs == NULL)
    return VLAD_NULLPTR;

  /* get the indices of the given intervals from the symtab */
  if ((retval = a_stab->get(a_int1, VLAD_IDENT_INT, &index1)) != VLAD_OK)
    return retval;
  if ((retval = a_stab->get(a_int2, VLAD_IDENT_INT, &index2)) != VLAD_OK)
    return retval;

  *a_rs = tbe_net_get_relation1(m_net, index1, index2);

  return VLAD_OK;
}

/* checks whether the given tuple satisfies the given constraints */
int vlad_tnet::check_tuple(vlad_stringlist *a_tuple,
                           vlad_varlist *a_vlist,
                           vlad_rlist *a_rlist,
                           vlad_symtab *a_stab)
{
  int retval;
  unsigned int i;

  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_tuple == NULL || a_vlist == NULL || a_rlist == NULL || a_stab == NULL)
    return VLAD_NULLPTR;

  for (i = 0; i < a_rlist->length(); i++) {
    vlad_rel *rel;
    char *int1;
    char *int2;
    unsigned int c_rs;
    unsigned int n_rs;
    unsigned int index;

    if ((retval = a_rlist->get(i, &rel)) != VLAD_OK)
      return retval;

    /* each rel in the rlist should have at least one variable */
    if (rel->is_ground())
      return VLAD_INVALIDINPUT;

    if ((retval = rel->get(&int1, &int2, &c_rs)) != VLAD_OK)
      return retval;

    if ((retval = vlad_identifier::validate_var_ident(int1)) == VLAD_OK) {
      /* first one is a var, so get it's index from the varlist */
      if (a_vlist->get(int1, &index) != VLAD_OK)
        return VLAD_INVALIDINPUT;
      /* using the index of the var from the varlist, get the crspndg int */
      if (a_tuple->get(index, &int1) != VLAD_OK)
        return VLAD_INVALIDINPUT;
    }

    if ((retval = vlad_identifier::validate_var_ident(int2)) == VLAD_OK) {
      /* second one is a var, so get it's index from the varlist */
      if (a_vlist->get(int2, &index) != VLAD_OK)
        return VLAD_INVALIDINPUT;
      /* using the index of the var from the varlist, get the crspndg int */
      if (a_tuple->get(index, &int2) != VLAD_OK)
        return VLAD_INVALIDINPUT;
    }

    /* get the actual relset of these intervals from the network */
    if ((retval = get_relset(int1, int2, a_stab, &n_rs)) != VLAD_OK)
      return retval;

    /* the relset in the network must be a subset of the constraint relset */
    if (TBE_REL_SET_INTERSECT(n_rs, c_rs) != n_rs)
      return VLAD_FAILURE;
  }

  return VLAD_OK;
}
