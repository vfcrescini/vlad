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

#include <vlad/vlad.h>
#include <vlad/mem.h>
#include <vlad/identifier.h>
#include <vlad/atom.h>
#include <vlad/mapper.h>

/* some shorthand macros */
#define VLAD_LEN_SS (m_stab->length(VLAD_IDENT_ENT_SUB_SIN))
#define VLAD_LEN_SG (m_stab->length(VLAD_IDENT_ENT_SUB_GRP))
#define VLAD_LEN_AS (m_stab->length(VLAD_IDENT_ENT_ACC_SIN))
#define VLAD_LEN_AG (m_stab->length(VLAD_IDENT_ENT_ACC_GRP))
#define VLAD_LEN_OS (m_stab->length(VLAD_IDENT_ENT_OBJ_SIN))
#define VLAD_LEN_OG (m_stab->length(VLAD_IDENT_ENT_OBJ_GRP))
#define VLAD_LEN_IN (m_stab->length(VLAD_IDENT_INT))

vlad_mapper::vlad_mapper()
{
  m_init = false;
  m_stab = NULL;
}

vlad_mapper::~vlad_mapper()
{
}

/* (re)init mapper */
int vlad_mapper::init(vlad_symtab *a_stab)
{
  if (a_stab == NULL) {
    m_init = false;
    return VLAD_NULLPTR;
  }

  m_init = true;
  m_stab = a_stab;

  return VLAD_OK;
}

/* get the total numbers */
unsigned int vlad_mapper::get_totals(unsigned int a_type)
{
  if (!m_init)
    return 0;

  switch(a_type) {
    case VLAD_ATOM_HOLDS :
      /* total number of holds atoms */
      return
        (VLAD_LEN_SS + VLAD_LEN_SG) *
        (VLAD_LEN_AS + VLAD_LEN_AG) *
        (VLAD_LEN_OS + VLAD_LEN_OG) *
        (VLAD_LEN_IN);
    case VLAD_ATOM_MEMBER :
      /* total number of member atoms */
      return
        (VLAD_LEN_SS * VLAD_LEN_SG * VLAD_LEN_IN) +
        (VLAD_LEN_AS * VLAD_LEN_AG * VLAD_LEN_IN) +
        (VLAD_LEN_OS * VLAD_LEN_OG * VLAD_LEN_IN);
    case VLAD_ATOM_SUBSET :
      /* total number of subset atoms */
      return
        (VLAD_LEN_SG * VLAD_LEN_SG * VLAD_LEN_IN) +
        (VLAD_LEN_AG * VLAD_LEN_AG * VLAD_LEN_IN) +
        (VLAD_LEN_OG * VLAD_LEN_OG * VLAD_LEN_IN);
    case VLAD_ATOM_TOTAL :
      /* total number of all possible atoms */
      return
        get_totals(VLAD_ATOM_HOLDS) +
        get_totals(VLAD_ATOM_MEMBER) +
        get_totals(VLAD_ATOM_SUBSET);
  }

  return 0;
}

/* given the state, truth value and indices, map a holds fact into an id */
unsigned int vlad_mapper::compute_holds(unsigned int a_sub,
                                        unsigned int a_acc,
                                        unsigned int a_obj,
                                        unsigned int a_int,
                                        unsigned int a_state,
                                        bool a_truth)
{
  if (!m_init)
    return 0;

  return compute_fact((a_sub * (VLAD_LEN_AS + VLAD_LEN_AG) * (VLAD_LEN_OS + VLAD_LEN_OG) * VLAD_LEN_IN) +
                      (a_acc * (VLAD_LEN_OS + VLAD_LEN_OG) * VLAD_LEN_IN) +
                      (a_obj * VLAD_LEN_IN) +
                      (a_int),
                      a_state,
                      a_truth);
}

/* given the state, truth value and indices, map a memb fact into an id */
unsigned int vlad_mapper::compute_memb(unsigned char a_type,
                                       unsigned int a_elt,
                                       unsigned int a_grp,
                                       unsigned int a_int,
                                       unsigned int a_state,
                                       bool a_truth)
{
  if (!m_init)
    return 0;

  switch(a_type & VLAD_IDENT_MASK_ENT_BASE) {
    case VLAD_IDENT_ENT_SUB :
      return compute_fact(get_totals(VLAD_ATOM_HOLDS) +
                          (a_elt * VLAD_LEN_SG * VLAD_LEN_IN) +
                          (a_grp * VLAD_LEN_IN) +
                          (a_int),
                          a_state,
                          a_truth);
    case VLAD_IDENT_ENT_ACC :
      return compute_fact(get_totals(VLAD_ATOM_HOLDS) +
                          (VLAD_LEN_SS * VLAD_LEN_SG * VLAD_LEN_IN) +
                          (a_elt * VLAD_LEN_AG * VLAD_LEN_IN) +
                          (a_grp * VLAD_LEN_IN) +
                          (a_int),
                          a_state,
                          a_truth);
    case VLAD_IDENT_ENT_OBJ :
      return compute_fact(get_totals(VLAD_ATOM_HOLDS) +
                          (VLAD_LEN_SS * VLAD_LEN_SG * VLAD_LEN_IN) +
                          (VLAD_LEN_AS * VLAD_LEN_AG * VLAD_LEN_IN) +
                          (a_elt * VLAD_LEN_OG * VLAD_LEN_IN) +
                          (a_grp * VLAD_LEN_IN) +
                          (a_int),
                          a_state,
                          a_truth);
  }

  return 0;
}

/* given the state, truth value and indices, map a subst fact into an id */
unsigned int vlad_mapper::compute_subst(unsigned char a_type,
                                        unsigned int a_grp1,
                                        unsigned int a_grp2,
                                        unsigned int a_int,
                                        unsigned int a_state,
                                        bool a_truth)
{
  if (!m_init)
    return 0;

  switch(a_type & VLAD_IDENT_MASK_ENT_BASE) {
    case VLAD_IDENT_ENT_SUB :
      return compute_fact(get_totals(VLAD_ATOM_HOLDS) +
                          get_totals(VLAD_ATOM_MEMBER) +
                          (a_grp1 * VLAD_LEN_SG * VLAD_LEN_IN) +
                          (a_grp2 * VLAD_LEN_IN) +
                          (a_int),
                          a_state,
                          a_truth);
    case VLAD_IDENT_ENT_ACC :
      return compute_fact(get_totals(VLAD_ATOM_HOLDS) +
                          get_totals(VLAD_ATOM_MEMBER) +
                          (VLAD_LEN_SG * VLAD_LEN_SG * VLAD_LEN_IN) +
                          (a_grp1 * VLAD_LEN_AG * VLAD_LEN_IN) +
                          (a_grp2 * VLAD_LEN_IN) +
                          (a_int),
                          a_state,
                          a_truth);
    case VLAD_IDENT_ENT_OBJ :
      return compute_fact(get_totals(VLAD_ATOM_HOLDS) +
                          get_totals(VLAD_ATOM_MEMBER) +
                          (VLAD_LEN_SG * VLAD_LEN_SG * VLAD_LEN_IN) +
                          (VLAD_LEN_AG * VLAD_LEN_AG * VLAD_LEN_IN) +
                          (a_grp1 * VLAD_LEN_OG * VLAD_LEN_IN) +
                          (a_grp2 * VLAD_LEN_IN) +
                          (a_int),
                          a_state,
                          a_truth);
  }

  return 0;
}

/* used by the other compute_*() functions */
unsigned int vlad_mapper::compute_fact(unsigned int a_fact,
                                       unsigned int a_state,
                                       bool a_truth)
{
  if (!m_init)
    return 0;

  /* state offset  + truth offset + fact offset */
  return
    (a_state * get_totals(VLAD_ATOM_TOTAL) * 2) +
    (a_truth ? get_totals(VLAD_ATOM_TOTAL) : 0) +
    a_fact;
}

/* gives a unique id based on the fact given */
int vlad_mapper::encode_fact(vlad_fact *a_fact,
                             unsigned int a_state,
                             unsigned int *a_id)
{
  int retval;
  unsigned char type;
  bool truth;
  char *name[4];

  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_fact == NULL || a_id == NULL)
    return VLAD_NULLPTR;

  retval = a_fact->get(&(name[0]),
                       &(name[1]),
                       &(name[2]),
                       &(name[3]),
                       &type,
                       &truth);

  if (retval!= VLAD_OK)
    return retval;

  switch(type) {
    case VLAD_ATOM_HOLDS :
      return encode_holds(name[0], name[1], name[2], name[3], a_state, truth, a_id);
    case VLAD_ATOM_MEMBER :
      return encode_memb(name[0], name[1], name[3], a_state, truth, a_id);
    case VLAD_ATOM_SUBSET :
      return encode_subst(name[0], name[1], name[3], a_state, truth, a_id);
  }

  return VLAD_FAILURE;
}

/* gives the fact based on the id given */
int vlad_mapper::decode_fact(unsigned int a_id,
                             unsigned int *a_state,
                             vlad_fact **a_fact)
{
  int retval;
  char *name[4];
  unsigned char type;
  bool truth;

  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_fact == NULL || a_state == NULL)
    return VLAD_NULLPTR;

  /* extract the state */
  *a_state = a_id / (get_totals(VLAD_ATOM_TOTAL) * 2);
  a_id = a_id % (get_totals(VLAD_ATOM_TOTAL) * 2);

  /* now the truth value */
  truth = (a_id >= get_totals(VLAD_ATOM_TOTAL));
  a_id = a_id % get_totals(VLAD_ATOM_TOTAL);

  /* then the fact type */
  if (a_id < get_totals(VLAD_ATOM_HOLDS)) {
    type = VLAD_ATOM_HOLDS;

    retval = decode_holds(a_id,
                          &(name[0]),
                          &(name[1]),
                          &(name[2]),
                          &(name[3]));

    if (retval != VLAD_OK)
      return retval;
  }
  else if (a_id < get_totals(VLAD_ATOM_HOLDS) + get_totals(VLAD_ATOM_MEMBER)) {
    type = VLAD_ATOM_MEMBER;

    retval = decode_memb(a_id - get_totals(VLAD_ATOM_HOLDS),
                         &(name[0]),
                         &(name[1]),
                         &(name[3]));

    if (retval != VLAD_OK)
      return retval;
  }
  else {
    type = VLAD_ATOM_SUBSET;

    retval = decode_subst(a_id - (get_totals(VLAD_ATOM_HOLDS) + get_totals(VLAD_ATOM_MEMBER)),
                          &(name[0]),
                          &(name[1]),
                          &(name[3]));

    if (retval != VLAD_OK)
      return retval;
  }

  /*  now create a new fact */
  if ((*a_fact = VLAD_MEM_NEW(vlad_fact())) == NULL)
    return VLAD_MALLOCFAILED;

  retval = (*a_fact)->init(name[0], name[1], name[2], name[3], type, truth);

  if (retval != VLAD_OK) {
    VLAD_MEM_DELETE(*a_fact);
    return retval;
  }

  return VLAD_OK;
}

/* returns the id of the negation of the given fact id */
unsigned int vlad_mapper::negate_fact(unsigned int a_fact)
{
  unsigned int state;
  bool truth;

  if (!m_init)
    return 0;

  /* first extract the state */
  state = a_fact / (get_totals(VLAD_ATOM_TOTAL) * 2);
  a_fact = a_fact % (get_totals(VLAD_ATOM_TOTAL) * 2);

  /* then get truth value */
  truth = ! (a_fact >= get_totals(VLAD_ATOM_TOTAL));
  a_fact = a_fact % get_totals(VLAD_ATOM_TOTAL);

  return compute_fact(a_fact, state, truth);
}

/* gives a unique id based on the holds entities given */
int vlad_mapper::encode_holds(const char *a_sub,
                              const char *a_acc,
                              const char *a_obj,
                              const char *a_int,
                              unsigned int a_state,
                              bool a_truth,
                              unsigned int *a_id)
{
  int retval;
  unsigned int index[4];
  unsigned char type[4];

  /* get indices and types */
  if ((retval = m_stab->get(a_sub, &(index[0]), &(type[0]))) != VLAD_OK)
    return retval;
  if ((retval = m_stab->get(a_acc, &(index[1]), &(type[1]))) != VLAD_OK)
    return retval;
  if ((retval = m_stab->get(a_obj, &(index[2]), &(type[2]))) != VLAD_OK)
    return retval;
  if ((retval = m_stab->get(a_int, &(index[3]), &(type[3]))) != VLAD_OK)
    return retval;

  /* verify types */
  if (!VLAD_IDENT_TYPE_IS_SUB(type[0]) ||
      !VLAD_IDENT_TYPE_IS_ACC(type[1]) ||
      !VLAD_IDENT_TYPE_IS_OBJ(type[2]) ||
      !VLAD_IDENT_TYPE_IS_INT(type[3]))
    return VLAD_INVALIDINPUT;

  /* adjust for groups */
  index[0] += (VLAD_IDENT_TYPE_IS_GRP(type[0]) ? VLAD_LEN_SS : 0);
  index[1] += (VLAD_IDENT_TYPE_IS_GRP(type[1]) ? VLAD_LEN_AS : 0);
  index[2] += (VLAD_IDENT_TYPE_IS_GRP(type[2]) ? VLAD_LEN_OS : 0);

  *a_id = compute_holds(index[0],
                        index[1],
                        index[2],
                        index[3],
                        a_state,
                        a_truth);

  return VLAD_OK;
}

/* gives a unique id based on the member entities given */
int vlad_mapper::encode_memb(const char *a_elt,
                             const char *a_grp,
                             const char *a_int,
                             unsigned int a_state,
                             bool a_truth,
                             unsigned int *a_id)
{
  int retval;
  unsigned int index[3];
  unsigned char type[3];

  /* get indices and types */
  if ((retval = m_stab->get(a_elt, &(index[0]), &(type[0]))) != VLAD_OK)
    return retval;
  if ((retval = m_stab->get(a_grp, &(index[1]), &(type[1]))) != VLAD_OK)
    return retval;
  if ((retval = m_stab->get(a_int, &(index[2]), &(type[2]))) != VLAD_OK)
    return retval;

  /* verify */
  if (VLAD_IDENT_TYPE_IS_GRP(type[0]) || !VLAD_IDENT_TYPE_IS_GRP(type[1]))
    return VLAD_INVALIDINPUT;
  if (!VLAD_IDENT_TYPE_IS_INT(type[2]))
    return VLAD_INVALIDINPUT;
  if ((type[0] & VLAD_IDENT_MASK_ENT_BASE) != (type[1] & VLAD_IDENT_MASK_ENT_BASE))
    return VLAD_INVALIDINPUT;

  *a_id = compute_memb(type[0], index[0], index[1], index[2], a_state, a_truth);

  return VLAD_OK;
}

/* gives a unique id based on the subset entities given */
int vlad_mapper::encode_subst(const char *a_grp1,
                              const char *a_grp2,
                              const char *a_int,
                              unsigned int a_state,
                              bool a_truth,
                              unsigned int *a_id)
{
  int retval;
  unsigned int index[3];
  unsigned char type[3];

  /* get indices and types */
  if ((retval = m_stab->get(a_grp1, &(index[0]), &(type[0]))) != VLAD_OK)
    return retval;
  if ((retval = m_stab->get(a_grp2, &(index[1]), &(type[1]))) != VLAD_OK)
    return retval;
  if ((retval = m_stab->get(a_int, &(index[2]), &(type[2]))) != VLAD_OK)
    return retval;

  /* verify */
  if (!VLAD_IDENT_TYPE_IS_GRP(type[0]) || !VLAD_IDENT_TYPE_IS_GRP(type[1]))
    return VLAD_INVALIDINPUT;
  if (!VLAD_IDENT_TYPE_IS_INT(type[2]))
    return VLAD_INVALIDINPUT;
  if ((type[0] & VLAD_IDENT_MASK_ENT_BASE) != (type[1] & VLAD_IDENT_MASK_ENT_BASE))
    return VLAD_INVALIDINPUT;

  *a_id = compute_subst(type[0], index[0], index[1], index[2], a_state, a_truth);

  return VLAD_OK;
}

/* gives entities based on the holds id given */
int vlad_mapper::decode_holds(unsigned int a_id,
                              char **a_sub,
                              char **a_acc,
                              char **a_obj,
                              char **a_int)
{
  int retval;
  unsigned int rem;
  unsigned int index[4];

  index[0] = a_id / ((VLAD_LEN_AS + VLAD_LEN_AG) * (VLAD_LEN_OS + VLAD_LEN_OG) * VLAD_LEN_IN);
  rem      = a_id % ((VLAD_LEN_AS + VLAD_LEN_AG) * (VLAD_LEN_OS + VLAD_LEN_OG) * VLAD_LEN_IN);
  index[1] = rem / ((VLAD_LEN_OS + VLAD_LEN_OG) * VLAD_LEN_IN);
  rem      = rem % ((VLAD_LEN_OS + VLAD_LEN_OG) * VLAD_LEN_IN);
  index[2] = rem / VLAD_LEN_IN;
  rem      = rem % VLAD_LEN_IN;
  index[3] = rem;

  if ((retval = m_stab->get(index[0] - ((index[0] < VLAD_LEN_SS) ? 0 : VLAD_LEN_SS), ((index[0] < VLAD_LEN_SS) ? VLAD_IDENT_ENT_SUB_SIN : VLAD_IDENT_ENT_SUB_GRP), a_sub)) != VLAD_OK)
    return retval;
  if ((retval = m_stab->get(index[1] - ((index[1] < VLAD_LEN_AS) ? 0 : VLAD_LEN_AS), ((index[1] < VLAD_LEN_AS) ? VLAD_IDENT_ENT_ACC_SIN : VLAD_IDENT_ENT_ACC_GRP), a_acc)) != VLAD_OK)
    return retval;
  if ((retval = m_stab->get(index[2] - ((index[2] < VLAD_LEN_OS) ? 0 : VLAD_LEN_OS), ((index[2] < VLAD_LEN_OS) ? VLAD_IDENT_ENT_OBJ_SIN : VLAD_IDENT_ENT_OBJ_GRP), a_obj)) != VLAD_OK)
    return retval;
  if ((retval = m_stab->get(index[3], VLAD_IDENT_INT, a_int)) != VLAD_OK)
    return retval;

  return VLAD_OK;
}

/* gives entities based on the member id given */
int vlad_mapper::decode_memb(unsigned int a_id,
                             char **a_elt,
                             char **a_grp,
                             char **a_int)
{
 int retval;

  if (a_id < (VLAD_LEN_SS * VLAD_LEN_SG * VLAD_LEN_IN)) {
    /* subject member fact */
    if ((retval = m_stab->get(a_id / (VLAD_LEN_SG * VLAD_LEN_IN), VLAD_IDENT_ENT_SUB_SIN, a_elt)) != VLAD_OK)
      return retval;
    if ((retval = m_stab->get((a_id % (VLAD_LEN_SG * VLAD_LEN_IN)) / VLAD_LEN_IN, VLAD_IDENT_ENT_SUB_GRP, a_grp)) != VLAD_OK)
      return retval;
    if ((retval = m_stab->get((a_id % (VLAD_LEN_SG * VLAD_LEN_IN)) % VLAD_LEN_IN, VLAD_IDENT_INT, a_int)) != VLAD_OK)
      return retval;
  }
  else if (a_id < ((VLAD_LEN_SS * VLAD_LEN_SG * VLAD_LEN_IN) + (VLAD_LEN_AS * VLAD_LEN_AG * VLAD_LEN_IN))) {
    /* access member fact */
    a_id = a_id - (VLAD_LEN_SS * VLAD_LEN_SG * VLAD_LEN_IN);

    if ((retval = m_stab->get(a_id / (VLAD_LEN_AG * VLAD_LEN_IN), VLAD_IDENT_ENT_ACC_SIN, a_elt)) != VLAD_OK)
      return retval;
    if ((retval = m_stab->get((a_id % (VLAD_LEN_AG * VLAD_LEN_IN)) / VLAD_LEN_IN, VLAD_IDENT_ENT_ACC_GRP, a_grp)) != VLAD_OK)
      return retval;
    if ((retval = m_stab->get((a_id % (VLAD_LEN_AG * VLAD_LEN_IN)) % VLAD_LEN_IN, VLAD_IDENT_INT, a_int)) != VLAD_OK)
      return retval;
  }
  else {
    /* object member fact */
    a_id = a_id - ((VLAD_LEN_SS * VLAD_LEN_SG * VLAD_LEN_IN) + (VLAD_LEN_AS * VLAD_LEN_AG * VLAD_LEN_IN));

    if ((retval = m_stab->get(a_id / (VLAD_LEN_OG * VLAD_LEN_IN), VLAD_IDENT_ENT_OBJ_SIN, a_elt)) != VLAD_OK)
      return retval;
    if ((retval = m_stab->get((a_id % (VLAD_LEN_OG * VLAD_LEN_IN)) / VLAD_LEN_IN, VLAD_IDENT_ENT_OBJ_GRP, a_grp)) != VLAD_OK)
      return retval;
    if ((retval = m_stab->get((a_id % (VLAD_LEN_OG * VLAD_LEN_IN)) % VLAD_LEN_IN, VLAD_IDENT_INT, a_int)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

/* gives entities based on the subset id given */
int vlad_mapper::decode_subst(unsigned int a_id,
                              char **a_grp1,
                              char **a_grp2,
                              char **a_int)
{
  int retval;

  if (a_id < (VLAD_LEN_SG * VLAD_LEN_SG * VLAD_LEN_IN)) {
    /* subject subset fact */
    if ((retval = m_stab->get(a_id / (VLAD_LEN_SG * VLAD_LEN_IN), VLAD_IDENT_ENT_SUB_GRP, a_grp1)) != VLAD_OK)
      return retval;
    if ((retval = m_stab->get((a_id % (VLAD_LEN_SG * VLAD_LEN_IN)) / VLAD_LEN_IN, VLAD_IDENT_ENT_SUB_GRP, a_grp2)) != VLAD_OK)
      return retval;
    if ((retval = m_stab->get((a_id % (VLAD_LEN_SG * VLAD_LEN_IN)) % VLAD_LEN_IN, VLAD_IDENT_INT, a_int)) != VLAD_OK)
      return retval;
  }
  else if (a_id < (VLAD_LEN_SG * VLAD_LEN_SG * VLAD_LEN_IN) + (VLAD_LEN_AG * VLAD_LEN_AG * VLAD_LEN_IN)) {
    /* access subset fact */
    a_id = a_id - (VLAD_LEN_SG * VLAD_LEN_SG * VLAD_LEN_IN);

    if ((retval = m_stab->get(a_id / (VLAD_LEN_AG * VLAD_LEN_IN), VLAD_IDENT_ENT_ACC_GRP, a_grp1)) != VLAD_OK)
      return retval;
    if ((retval = m_stab->get((a_id % (VLAD_LEN_AG * VLAD_LEN_IN)) / VLAD_LEN_IN, VLAD_IDENT_ENT_ACC_GRP, a_grp2)) != VLAD_OK)
      return retval;
    if ((retval = m_stab->get((a_id % (VLAD_LEN_AG * VLAD_LEN_IN)) % VLAD_LEN_IN, VLAD_IDENT_INT, a_int)) != VLAD_OK)
      return retval;
  }
  else {
    /* object subset fact */
    a_id = a_id - ((VLAD_LEN_SG * VLAD_LEN_SG * VLAD_LEN_IN) + (VLAD_LEN_AG * VLAD_LEN_AG * VLAD_LEN_IN));

    if ((retval = m_stab->get(a_id / (VLAD_LEN_OG * VLAD_LEN_IN), VLAD_IDENT_ENT_OBJ_GRP, a_grp1)) != VLAD_OK)
      return retval;
    if ((retval = m_stab->get((a_id % (VLAD_LEN_OG * VLAD_LEN_IN)) / VLAD_LEN_IN, VLAD_IDENT_ENT_OBJ_GRP, a_grp2)) != VLAD_OK)
      return retval;
    if ((retval = m_stab->get((a_id % (VLAD_LEN_OG * VLAD_LEN_IN)) % VLAD_LEN_IN, VLAD_IDENT_INT, a_int)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}
