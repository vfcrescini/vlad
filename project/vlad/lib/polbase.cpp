/*
 * polbase.cpp
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <new>

#include <vlad/vlad.h>
#include <vlad/polbase.h>

/* some shorthand macros */
#define sslen (stable->length(VLAD_IDENT_SUB_SIN))
#define aslen (stable->length(VLAD_IDENT_ACC_SIN))
#define oslen (stable->length(VLAD_IDENT_OBJ_SIN))
#define sglen (stable->length(VLAD_IDENT_SUB_GRP))
#define aglen (stable->length(VLAD_IDENT_ACC_GRP))
#define oglen (stable->length(VLAD_IDENT_OBJ_GRP))

polbase::polbase()
{
  m_stage = 0;
  stable = NULL;
}

polbase::~polbase()
{
  if (stable != NULL)
    delete stable;
}

/* (re)init the policy base */
int polbase::init()
{
  int i;
  int retval;

  /* reset all the totals */
  for (i = VLAD_ATOM_FIRST; i <= VLAD_ATOM_LAST; i++)
    m_tot_atoms[i] = 0;

  /* initialise symbol table */
  if (stable != NULL)
    delete stable;
  if ((stable = VLAD_NEW(symtab())) == NULL)
    return VLAD_MALLOCFAILED;
  if ((retval = stable->init()) != VLAD_OK)
    return retval;

  m_stage = 1;

  return VLAD_OK;
}

/* close symbol table */
int polbase::close_symtab()
{
  if (m_stage != 1)
    return VLAD_INVALIDOP;

  /* compute totals */
  m_tot_atoms[VLAD_ATOM_HOLDS] = (sslen + sglen) *
                                 (aslen + aglen) *
                                 (oslen + oglen);
  m_tot_atoms[VLAD_ATOM_MEMBER] = (sslen * sglen) +
                                  (aslen * aglen) +
                                  (oslen * oglen);
  m_tot_atoms[VLAD_ATOM_SUBSET]= (sglen * sglen) +
                                 (aglen * aglen) +
                                 (oglen * oglen);

  m_stage = 2;

  return VLAD_OK;
}

/* gives a unique id based on the holds entities given */
int polbase::encode_holds(const char *a_sub, const char *a_acc, const char *a_obj, unsigned int *a_id)
{
  int retval;
  unsigned int index[3];
  unsigned char type[3];

  /* get indices and types */
  if ((retval = stable->get(a_sub, &(index[0]), &(type[0]))) != VLAD_OK)
    return retval;
  if ((retval = stable->get(a_acc, &(index[1]), &(type[1]))) != VLAD_OK)
    return retval;
  if ((retval = stable->get(a_obj, &(index[2]), &(type[2]))) != VLAD_OK)
    return retval;

  /* verify types */
  if (!VLAD_IDENT_IS_SUBJECT(type[0]) ||
      !VLAD_IDENT_IS_ACCESS(type[1]) ||
      !VLAD_IDENT_IS_OBJECT(type[2]))
    return VLAD_INVALIDINPUT;

  /* now compute */
  *a_id = (index[0] * (aslen + aglen) * (oslen + oglen)) +
          (index[1] * (oslen + oglen)) +
          (index[2]);
  
  return VLAD_OK;
}

/* gives a unique id based on the member entities given */
int polbase::encode_memb(const char *a_elt, const char *a_grp, unsigned int *a_id)
{ 
  int retval;
  unsigned int index[2];
  unsigned char type[2];

  /* get indices and types */
  if ((retval = stable->get(a_elt, &(index[0]), &(type[0]))) != VLAD_OK)
    return retval;
  if ((retval = stable->get(a_grp, &(index[1]), &(type[1]))) != VLAD_OK)
    return retval;

  /* verify */
  if (VLAD_IDENT_IS_GROUP(type[0]) || !VLAD_IDENT_IS_GROUP(type[1]))
    return VLAD_INVALIDINPUT;
  if (type[0] != VLAD_IDENT_BASETYPE(type[1]))
    return VLAD_INVALIDINPUT;

  *a_id = m_tot_atoms[VLAD_ATOM_HOLDS];

  /* now compute */
  switch(type[0]) {
    case VLAD_IDENT_SUB_SIN :
      *a_id = *a_id +
              (index[0] * sglen) +
              index[1];
      break;
    case VLAD_IDENT_ACC_SIN :
      *a_id = *a_id +
              (sslen * sglen) +
              (index[0] * aglen) +
              index[1];
      break;
    case VLAD_IDENT_OBJ_SIN :
      *a_id = *a_id +
              (sslen * sglen) +
              (aslen * aglen) +
              (index[0] * oglen) +
              index[1];
      break;
  }

  return VLAD_OK;
}

/* gives a unique id based on the subset entities given */
int polbase::encode_subst(const char *a_grp1, const char *a_grp2, unsigned int *a_id)
{
  int retval;
  unsigned int index[2];
  unsigned char type[2];

  /* get indices and types */
  if ((retval = stable->get(a_grp1, &(index[0]), &(type[0]))) != VLAD_OK)
    return retval;
  if ((retval = stable->get(a_grp2, &(index[1]), &(type[1]))) != VLAD_OK)
    return retval;

  /* verify */
  if (!VLAD_IDENT_IS_GROUP(type[0]) || !VLAD_IDENT_IS_GROUP(type[1]))
    return VLAD_INVALIDINPUT;
  if (VLAD_IDENT_BASETYPE(type[0]) != VLAD_IDENT_BASETYPE(type[1]))
    return VLAD_INVALIDINPUT;

  *a_id = m_tot_atoms[VLAD_ATOM_HOLDS] + m_tot_atoms[VLAD_ATOM_MEMBER];

  switch(type[0]) {
    case VLAD_IDENT_SUB_GRP :
      *a_id = *a_id +
              (index[0] * sglen) +
              index[1];
      break;
    case VLAD_IDENT_ACC_GRP :
      *a_id = *a_id +
               (sglen * sglen) +
               (index[0] * aglen) +
               index[1];
      break;
    case VLAD_IDENT_OBJ_GRP :
      *a_id = *a_id +
              (sglen * sglen) +
              (aglen * aglen) +
              (index[0] * oglen) +
              index[1];
      break;
  }

  return VLAD_OK;
}

/* gives the entities based on the holds id */
int polbase::decode_holds(unsigned int a_id, char **a_sub, char **a_acc, char **a_obj)
{
  int retval;
  unsigned int index[3];
  unsigned int rem;

  rem = a_id % ((aslen + aglen) * (oslen + oglen));
  index[0] = a_id / ((aslen + aglen) * (oslen + oglen));
  index[1] = rem / (oslen + oglen);
  index[2] = rem % (oslen + oglen);

  if ((retval = stable->get(index[0] - ((index[0] < sslen) ? 0 : sslen), ((index[0] < sslen) ? VLAD_IDENT_SUB_SIN : VLAD_IDENT_SUB_GRP), a_sub)) != VLAD_OK)
    return retval;
  if ((retval = stable->get(index[1] - ((index[1] < aslen) ? 0 : aslen), ((index[1] < aslen) ? VLAD_IDENT_ACC_SIN : VLAD_IDENT_ACC_GRP), a_acc)) != VLAD_OK)
    return retval;
  if ((retval = stable->get(index[2] - ((index[2] < oslen) ? 0 : oslen), ((index[2] < oslen) ? VLAD_IDENT_OBJ_SIN : VLAD_IDENT_OBJ_GRP), a_obj)) != VLAD_OK)
    return retval;

  return VLAD_OK;
}

/* gives the entities based on the member id */
int polbase::decode_memb(unsigned int a_id, char **a_elt, char **a_grp)
{
  int retval;

  a_id = a_id - m_tot_atoms[VLAD_ATOM_HOLDS];

  if (a_id < (sslen * sglen)) {
    /* subject member atom */
    if ((retval = stable->get(a_id / sglen, VLAD_IDENT_SUB_SIN, a_elt)) != VLAD_OK)
      return retval;
    if ((retval = stable->get(a_id % sglen, VLAD_IDENT_SUB_GRP, a_grp)) != VLAD_OK)
      return retval;
  }
  else if (a_id < ((sslen * sglen) + (aslen *aglen))) {
    /* access member atom */
    a_id = a_id - (sslen * sglen);
    if ((retval = stable->get(a_id / aglen, VLAD_IDENT_ACC_SIN, a_elt)) != VLAD_OK)
      return retval;
    if ((retval = stable->get(a_id % aglen, VLAD_IDENT_ACC_GRP, a_grp)) != VLAD_OK)
      return retval;
  }
  else {
    /* object member atom */
    a_id = a_id - ((sslen * sglen) + (aslen * aglen));
    if ((retval = stable->get(a_id / oglen, VLAD_IDENT_OBJ_SIN, a_elt)) != VLAD_OK)
      return retval;
    if ((retval = stable->get(a_id % oglen, VLAD_IDENT_OBJ_GRP, a_grp)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

/* gives the entities based on the subst id */
int polbase::decode_subst(unsigned int a_id, char **a_grp1, char **a_grp2)
{
  int retval;

  a_id = a_id - (m_tot_atoms[VLAD_ATOM_HOLDS] + m_tot_atoms[VLAD_ATOM_MEMBER]);

  if (a_id < (sglen * sglen)) {
    /* subject subset atom */
    if ((retval = stable->get(a_id / sglen, VLAD_IDENT_SUB_GRP, a_grp1)) != VLAD_OK)
      return retval;
    if ((retval = stable->get(a_id & sglen, VLAD_IDENT_SUB_GRP, a_grp2)) != VLAD_OK)
      return retval;
  }
  else if (a_id < (sglen * sglen) + (aglen * aglen)) {
    /* access subset atom */
    a_id = a_id - (sglen * sglen);
    if ((retval = stable->get(a_id / aglen, VLAD_IDENT_ACC_GRP, a_grp1)) != VLAD_OK)
      return retval;
    if ((retval = stable->get(a_id % aglen, VLAD_IDENT_ACC_GRP, a_grp2)) != VLAD_OK)
      return retval;
  }
  else {
    /* object subset atom */
    a_id = a_id - ((sglen * sglen) + (aglen * aglen));
    if ((retval = stable->get(a_id / oglen, VLAD_IDENT_OBJ_GRP, a_grp1)) != VLAD_OK)
      return retval;
    if ((retval = stable->get(a_id % oglen, VLAD_IDENT_OBJ_GRP, a_grp2)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}
