/*
 * kb.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <new>

#include <config.h>
#include <vlad.h>
#include <kb.h>

kb::kb()
{
  stable = NULL;
  initialised = false;
  closed = false;
  s_len = 0;
  a_len = 0;
  o_len = 0;
  sg_len = 0;
  ag_len = 0;
  og_len = 0;
  h_tot = 0;
  m_tot = 0;
  s_tot = 0;
  pos_tot = 0;
  neg_tot = 0;
  h_index = 0;
  m_index = 0;
  s_index = 0;
}

kb::~kb()
{
  if (stable != NULL);
    delete stable;
}

/* (re)init kb */
int kb::init()
{
  int retval;

  if (stable != NULL)
    delete stable;

  if ((stable = VLAD_NEW(symtab())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = stable->init()) != VLAD_OK)
    return retval;

  initialised = true;

  return VLAD_OK;
}

/* after this is called, no further calls to add_ident() is allowed */
int kb::close()
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  /* first get some needed values */

  /* get list lengths */
  s_len = stable->length(VLAD_IDENT_SUBJECT);
  a_len = stable->length(VLAD_IDENT_ACCESS);
  o_len = stable->length(VLAD_IDENT_OBJECT);
  sg_len = stable->length(VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP);
  ag_len = stable->length(VLAD_IDENT_ACCESS | VLAD_IDENT_GROUP);
  og_len = stable->length(VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP);

  /* total atoms (only +) */
  h_tot = (s_len + sg_len) * (a_len + ag_len) * (o_len + og_len);
  m_tot = (s_len * sg_len) + (a_len * ag_len) + (o_len * og_len);
  s_tot = (sg_len * sg_len) + (ag_len * ag_len) + (og_len * og_len);

  /* the indices at which + atoms start */
  h_index = 2;
  m_index = 2 + h_tot;
  s_index = 2 + h_tot + m_tot;

  /* total + atoms, the 2 is for the constant true and false */
  pos_tot = h_tot + m_tot + s_tot + 2;

  /* of course we have an equal number of + and - atoms */
  neg_tot = pos_tot;

  closed = true;

  return VLAD_OK;
}

/* register an identifier in the kb */
int kb::add_ident(const char *n, unsigned char t)
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  if (closed)
    return VLAD_FAILURE;

  return stable->add(n, t);
}

/* gives an atom id based on the identifiers already given */
int kb::encode_atom(const char *n1,
                    const char *n2,
                    const char *n3,
                    unsigned char ty,
                    bool tr,
                    unsigned int *a)
{
  int retval;

  if (!initialised)
    return VLAD_UNINITIALISED;

  /* of course, symtab must be closed first */
  if (!closed)
    return VLAD_FAILURE;

  if (a == NULL)
    return VLAD_NULLPTR;

  switch(ty) {
    case VLAD_ATOM_CONST : {
      if (n1 != NULL) {
        if (!strcmp(VLAD_STRING_FALSE, n1))
          *a = (tr) ? 0 : 1;
        else if (!strcmp(VLAD_STRING_TRUE, n1))
          *a = (tr) ? 1 : 0;
        else
          return VLAD_NOTFOUND;
      }
      else
        return VLAD_NULLPTR;

      break;
    } 
    case VLAD_ATOM_HOLDS : {
      unsigned int s_index;
      unsigned int a_index;
      unsigned int o_index;
      unsigned char s_type;
      unsigned char a_type;
      unsigned char o_type;
      unsigned int hs;
      unsigned int ha;
      unsigned int ho;

      /* get the indices of the identifiers */
      if ((retval = stable->get(n1, &s_index, &s_type)) != VLAD_OK)
        return retval;
      if ((retval = stable->get(n2, &a_index, &a_type)) != VLAD_OK)
        return retval;
      if ((retval = stable->get(n3, &o_index, &o_type)) != VLAD_OK)
        return retval;

      /* check validity */
      if (!VLAD_IDENT_IS_SUBJECT(s_type))
        return VLAD_INVALIDINPUT;
      if (!VLAD_IDENT_IS_ACCESS(a_type))
        return VLAD_INVALIDINPUT;
      if (!VLAD_IDENT_IS_OBJECT(o_type))
        return VLAD_INVALIDINPUT;

      /* now compute */
      hs = VLAD_IDENT_IS_GROUP(s_type) ? s_index + s_len : s_index;
      ha = VLAD_IDENT_IS_GROUP(a_type) ? a_index + a_len : a_index;
      ho = VLAD_IDENT_IS_GROUP(o_type) ? o_index + o_len : o_index;
      *a = ((tr) ? 0 : pos_tot) + 2 + (hs * (a_len + ag_len) * (o_len + og_len)) + (ha * (o_len + og_len)) + ho;

      break;
    }
    case VLAD_ATOM_MEMBER : {
      unsigned int e_index;
      unsigned int g_index;
      unsigned char e_type;
      unsigned char g_type;

      /* get the indices of the identifiers */
      if ((retval = stable->get(n1, &e_index, &e_type)) != VLAD_OK)
        return retval;
      if ((retval = stable->get(n2, &g_index, &g_type)) != VLAD_OK)
        return retval;

      /* check validity */
      if (VLAD_IDENT_BASETYPE(e_type) != VLAD_IDENT_BASETYPE(g_type) ||
         VLAD_IDENT_IS_GROUP(e_type) || !VLAD_IDENT_IS_GROUP(g_type))
        return VLAD_INVALIDINPUT;

      /* now compute */
      switch(VLAD_IDENT_BASETYPE(e_type)) {
        case VLAD_IDENT_SUBJECT :
          *a = ((tr) ? 0 : pos_tot) + 2 + h_tot + (e_index * sg_len) + g_index;
          break;
        case VLAD_IDENT_ACCESS :
          *a = ((tr) ? 0 : pos_tot) + 2 + h_tot + (s_len * sg_len) + (e_index * ag_len) + g_index;
          break;
        case VLAD_IDENT_OBJECT :
          *a = ((tr) ? 0 : pos_tot) + 2 + h_tot + (s_len * sg_len) + (a_len * ag_len) + (e_index * og_len) + g_index;
          break;
        default :
          /* this should never happen */
          return VLAD_FAILURE;
      }
      break;
    }
    case VLAD_ATOM_SUBSET : {
      unsigned int g1_index;
      unsigned int g2_index;
      unsigned char g1_type;
      unsigned char g2_type;

      /* get the indices of the identifiers */
      if ((retval = stable->get(n1, &g1_index, &g1_type)) != VLAD_OK)
        return retval;
      if ((retval = stable->get(n2, &g2_index, &g2_type)) != VLAD_OK)
        return retval;

      /* check validity */
      if (VLAD_IDENT_BASETYPE(g1_type) != VLAD_IDENT_BASETYPE(g2_type) ||
         !VLAD_IDENT_IS_GROUP(g1_type) || !VLAD_IDENT_IS_GROUP(g2_type))
        return VLAD_INVALIDINPUT;

      /* now we compute */
      switch(VLAD_IDENT_BASETYPE(g1_type)) {
        case VLAD_IDENT_SUBJECT :
          *a = ((tr) ? 0 : pos_tot) + 2 + h_tot + m_tot + (g1_index * sg_len) + g2_index;
          break;
        case VLAD_IDENT_ACCESS :
          *a = ((tr) ? 0 : pos_tot) + 2 + h_tot + m_tot + (sg_len * sg_len) + (g1_index * ag_len) + g2_index;
          break;
        case VLAD_IDENT_OBJECT :
          *a = ((tr) ? 0 : pos_tot) + 2 + h_tot + m_tot + (sg_len * sg_len) + (ag_len * ag_len) + (g1_index * og_len) + g2_index;
          break;
        default :
          /* this should never happen */
          return VLAD_FAILURE;
      }     
      
      break;
    }
    default :
      return VLAD_INVALIDINPUT;
  }
  return VLAD_OK;
}

/* returns the negation of the given atom */
int kb::negate_atom(unsigned int in, unsigned int *out)
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  /* of course it has to be closed first */
  if (!closed)
    return VLAD_FAILURE; 

  /* check if it is within range */
  if (in < 0 || in > (pos_tot + neg_tot))
    return VLAD_INVALIDINPUT;

  *out = ((in < pos_tot) ? 0 : pos_tot) + in;

  return VLAD_OK;
}

/* returns the atom details given the id */
int kb::decode_atom(char **n1,
                    char **n2,
                    char **n3,
                    unsigned char *ty,
                    bool *tr,
                    unsigned int a)
{
  unsigned int tmp;

  if (!initialised)
    return VLAD_UNINITIALISED;

  /* of course it has to be closed first */
  if (!closed)
    return VLAD_FAILURE;

  /* check if it is within range */
  if (a < 0 || a > (pos_tot + neg_tot))
    return VLAD_INVALIDINPUT;

  if (ty == NULL || tr == NULL)
    return VLAD_NULLPTR;

  /* first, determine whether it is positive or negative */
  *tr = (a < pos_tot);

  /* now assume it is positive */
  tmp = a - ((*tr) ? 0 : pos_tot);

  if (tmp >= 0 && tmp < 2) {
    /* constant atom */

    *ty = VLAD_ATOM_CONST;

    if (n1 == NULL)
      return VLAD_NULLPTR;

    if (*tr) {
      if ((*n1 = VLAD_STRING_MALLOC(VLAD_STRING_TRUE)) == NULL)
        return VLAD_MALLOCFAILED;
      return (strcpy(*n1, VLAD_STRING_TRUE) != NULL);
    }
    else {
      if ((*n1 = VLAD_STRING_MALLOC(VLAD_STRING_FALSE)) == NULL)
        return VLAD_MALLOCFAILED;
      return (strcpy(*n1, VLAD_STRING_FALSE) != NULL);
    }
  }
  else if (tmp < m_index) {
    /* holds atom */
    unsigned int rem;
    unsigned int s;
    unsigned int a;
    unsigned int o;

    *ty = VLAD_ATOM_HOLDS;
    tmp = tmp - h_index;

    s = tmp / ((a_len + ag_len) * (o_len + og_len));
    rem = tmp % ((a_len + ag_len) * (o_len + og_len));
    a = rem / (o_len + og_len);
    o = rem % (o_len + og_len);

    return
      stable->get(s - ((s < s_len) ? 0 : s_len), VLAD_IDENT_SUBJECT | ((s < s_len) ? 0 : VLAD_IDENT_GROUP), n1) &&
      stable->get(a - ((a < a_len) ? 0 : a_len), VLAD_IDENT_ACCESS | ((a < a_len) ? 0 : VLAD_IDENT_GROUP), n2) &&
      stable->get(o - ((o < o_len) ? 0 : o_len), VLAD_IDENT_OBJECT | ((o < o_len) ? 0 : VLAD_IDENT_GROUP), n3);
  }
  else if (tmp < s_index) {
    /* member atom */

    *ty = VLAD_ATOM_MEMBER;
    tmp = tmp - m_index;

    if (tmp < s_len * sg_len) {
      /* subject member atom */
      return 
        stable->get(tmp / sg_len, VLAD_IDENT_SUBJECT, n1) &&
        stable->get(tmp % sg_len, VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP, n2);
    }
    else if (tmp < (s_len * sg_len) + (a_len * ag_len)) {
      /* access member atom */
      tmp = tmp - (s_len * sg_len);
      return 
        stable->get(tmp / ag_len, VLAD_IDENT_ACCESS, n1) &&
        stable->get(tmp % ag_len, VLAD_IDENT_ACCESS | VLAD_IDENT_GROUP, n2);
    }
    else {
      /* object member atom */
      tmp = tmp - ((s_len * sg_len) + (a_len * ag_len));
      return
        stable->get(tmp / og_len, VLAD_IDENT_OBJECT, n1) &&
        stable->get(tmp % og_len, VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP, n2);
    } 
  }
  else {
    /* subset atom */

    *ty = VLAD_ATOM_SUBSET;
    tmp = tmp - s_index;

    if (tmp < sg_len * sg_len) {
      /* subject subset atom */
       return
        stable->get(tmp / sg_len, VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP, n1) &&
        stable->get(tmp % sg_len, VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP, n2);
    }
    else if (tmp < (sg_len * sg_len) + (ag_len * ag_len)) {
      /* access subset atom */
      tmp = tmp - (sg_len * sg_len);
       return
        stable->get(tmp / ag_len, VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP, n1) &&
        stable->get(tmp % ag_len, VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP, n2);
    }
    else {
      /* object subset atom */
      tmp = tmp - ((sg_len * sg_len) + (ag_len * ag_len));
       return
        stable->get(tmp / og_len, VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP, n1) &&
        stable->get(tmp % og_len, VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP, n2);
    }
  }

  return VLAD_FAILURE;
}
