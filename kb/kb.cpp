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
  istate = NULL;
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
}

kb::~kb()
{
  if (stable != NULL)
    delete stable;
  if (istate != NULL)
    delete istate;
}

/* (re)init kb */
int kb::init()
{
  int retval;

  /* symbol table */
  if (stable != NULL)
    delete stable;

  if ((stable = VLAD_NEW(symtab())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = stable->init()) != VLAD_OK)
    return retval;

  /* initial state */
  if (istate != NULL)
    delete istate;

  if ((istate = VLAD_NEW(numberlist("init_state"))) == NULL)
    return VLAD_MALLOCFAILED;

  initialised = true;
  closed = false;

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

  /* total atoms (only +ve) */
  h_tot = (s_len + sg_len) * (a_len + ag_len) * (o_len + og_len);
  m_tot = (s_len * sg_len) + (a_len * ag_len) + (o_len * og_len);
  s_tot = (sg_len * sg_len) + (ag_len * ag_len) + (og_len * og_len);

  /* total +ve atoms, the 2 is for the constant true and false */
  pos_tot = h_tot + m_tot + s_tot + 2;

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

/* add an atom into the initial state list */
int kb::add_init_atom(unsigned int a)
{
  int retval;

  if (!initialised)
    return VLAD_UNINITIALISED;

  if (!closed)
    return VLAD_FAILURE;

  if (a >= pos_tot + pos_tot)
    return VLAD_INVALIDINPUT;

  /* ignore duplicates */
  switch(retval = istate->add(a)) {
    case VLAD_OK :
    case VLAD_DUPLICATE :
      break;
    default :
      return retval;
  }

  return VLAD_OK;
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
  unsigned int tmp;

  if (a == NULL)
    return VLAD_NULLPTR;

  /* make sure it has been initialised */
  if (!initialised)
    return VLAD_UNINITIALISED;

  /* of course, symtab must be closed first */
  if (!closed)
    return VLAD_FAILURE;

  switch(ty) {
    case VLAD_ATOM_CONST :
      if ((retval = encode_const(n1, &tmp)) != VLAD_OK)
        return retval;
      break;
    case VLAD_ATOM_HOLDS :
      if ((retval = encode_holds(n1, n2, n3, &tmp)) != VLAD_OK)
        return retval;
      tmp = tmp + 2;
      break;
    case VLAD_ATOM_MEMBER :
      if ((retval = encode_member(n1, n2, &tmp)) != VLAD_OK)
        return retval;
      tmp = tmp + 2 + h_tot;
      break;
    case VLAD_ATOM_SUBSET :
      if ((retval = encode_subset(n1, n2, &tmp)) != VLAD_OK)
        return retval;
      tmp = tmp + 2 + h_tot + m_tot;
      break;
    default :
      return VLAD_INVALIDINPUT;
  }

  *a = tmp + (tr ? 0 : pos_tot);

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
  if (ty == NULL || tr == NULL)
    return VLAD_NULLPTR;

  /* make sure it has been initialised */
  if (!initialised)
    return VLAD_UNINITIALISED;

  /* of course it has to be closed first */
  if (!closed)
    return VLAD_FAILURE;

  /* check if it is within range */
  if (a < 0 || a > (pos_tot + pos_tot))
    return VLAD_INVALIDINPUT;

  /* if negative, set truth value and proceed as positive */
  if (!(*tr = (a < pos_tot)))
    a = a - pos_tot;

  if (a >= 0 && a < 2) {
    /* const atom */

    if (n1 == NULL)
      return VLAD_NULLPTR;

    *ty = VLAD_ATOM_CONST;

    return decode_const(n1, a);
  }
  else if (a < (2 + h_tot)) {
    /* holds atom */

    if (n1 == NULL || n2 == NULL || n3 == NULL)
      return VLAD_NULLPTR;

    *ty = VLAD_ATOM_HOLDS;

    return decode_holds(n1, n2, n3, a - 2);
  }
  else if (a < (2 + h_tot + m_tot)) {
    /* member atom */

    if (n1 == NULL || n2 == NULL)
      return VLAD_NULLPTR;

    *ty = VLAD_ATOM_MEMBER;

    return decode_member(n1, n2, a - (2 + h_tot));
  }
  else {
    /* subset atom */

    if (n1 == NULL || n2 == NULL)
      return VLAD_NULLPTR;

    *ty = VLAD_ATOM_SUBSET;

    return decode_subset(n1, n2, a - (2 + h_tot + m_tot));
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
  if (in < 0 || in > (pos_tot + pos_tot))
    return VLAD_INVALIDINPUT;

  *out = ((in < pos_tot) ? in + pos_tot : in - pos_tot);

  return VLAD_OK;
}

int kb::encode_const(const char *c, unsigned int *n)
{
  int retval;
  unsigned int c_index;
  unsigned char c_type;

  /* get from symbol table */
  if ((retval = stable->get(c, &c_index, &c_type)) != VLAD_OK)
    return retval;

  /* check type */
  if (!VLAD_IDENT_IS_CONST(c_type))
    return VLAD_INVALIDINPUT; 

  *n = c_index;

  return VLAD_OK;
}

int kb::encode_holds(const char *s, const char *a, const char *o, unsigned int *n)
{
  int retval;
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
  if ((retval = stable->get(s, &s_index, &s_type)) != VLAD_OK)
    return retval;
  if ((retval = stable->get(a, &a_index, &a_type)) != VLAD_OK)
    return retval;
  if ((retval = stable->get(o, &o_index, &o_type)) != VLAD_OK)
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

  *n = (hs * (a_len + ag_len) * (o_len + og_len)) + (ha * (o_len + og_len)) + ho;

  return VLAD_OK;
}

int kb::encode_member(const char *e, const char *g, unsigned int *n)
{
  int retval;
  unsigned int e_index;
  unsigned int g_index;
  unsigned char e_type;
  unsigned char g_type;
 
  /* get the indices of the identifiers */
  if ((retval = stable->get(e, &e_index, &e_type)) != VLAD_OK)
    return retval;
  if ((retval = stable->get(g, &g_index, &g_type)) != VLAD_OK)
    return retval;
 
  /* check validity */
  if (VLAD_IDENT_BASETYPE(e_type) != VLAD_IDENT_BASETYPE(g_type) ||
     VLAD_IDENT_IS_GROUP(e_type) || !VLAD_IDENT_IS_GROUP(g_type))
    return VLAD_INVALIDINPUT;

  /* now compute */
  switch(VLAD_IDENT_BASETYPE(e_type)) {
    case VLAD_IDENT_SUBJECT :
      *n = (e_index * sg_len) + g_index;
      break;
    case VLAD_IDENT_ACCESS :
      *n = (s_len * sg_len) + (e_index * ag_len) + g_index;
      break;
    case VLAD_IDENT_OBJECT :
      *n = (s_len * sg_len) + (a_len * ag_len) + (e_index * og_len) + g_index;
      break;
    default :
      /* this should never happen */
      return VLAD_FAILURE;
  }

  return VLAD_OK;
}

int kb::encode_subset(const char *g1, const char *g2, unsigned int *n)
{
  int retval;
  unsigned int g1_index;
  unsigned int g2_index;
  unsigned char g1_type;
  unsigned char g2_type;

  /* get the indices of the identifiers */
  if ((retval = stable->get(g1, &g1_index, &g1_type)) != VLAD_OK)
    return retval;
  if ((retval = stable->get(g2, &g2_index, &g2_type)) != VLAD_OK)
    return retval;

  /* check validity */
  if (VLAD_IDENT_BASETYPE(g1_type) != VLAD_IDENT_BASETYPE(g2_type) ||
     !VLAD_IDENT_IS_GROUP(g1_type) || !VLAD_IDENT_IS_GROUP(g2_type))
    return VLAD_INVALIDINPUT;

  /* now we compute */
  switch(VLAD_IDENT_BASETYPE(g1_type)) {
    case VLAD_IDENT_SUBJECT :
      *n = (g1_index * sg_len) + g2_index;
      break;
    case VLAD_IDENT_ACCESS :
      *n = (sg_len * sg_len) + (g1_index * ag_len) + g2_index;
      break;
    case VLAD_IDENT_OBJECT :
      *n = (sg_len * sg_len) + (ag_len * ag_len) + (g1_index * og_len) + g2_index;
      break;
    default :
      /* this should never happen */
      return VLAD_FAILURE;
  }
  return VLAD_OK;
}

int kb::decode_const(char **c, unsigned int n)
{
  return stable->get(n, VLAD_IDENT_CONST, c);
}

int kb::decode_holds(char **s, char **a, char **o, unsigned int n)
{
  int retval;
  unsigned int s_tmp;
  unsigned int a_tmp;
  unsigned int o_tmp;
  unsigned int rem;

  rem = n % ((a_len + ag_len) * (o_len + og_len));
  s_tmp = n / ((a_len + ag_len) * (o_len + og_len));
  a_tmp = rem / (o_len + og_len);
  o_tmp = rem % (o_len + og_len);

  if ((retval = stable->get(s_tmp - ((s_tmp < s_len) ? 0 : s_len), VLAD_IDENT_SUBJECT | ((s_tmp < s_len) ? 0 : VLAD_IDENT_GROUP), s)) != VLAD_OK)
    return retval;
  if ((retval = stable->get(a_tmp - ((a_tmp < a_len) ? 0 : a_len), VLAD_IDENT_ACCESS | ((a_tmp < a_len) ? 0 : VLAD_IDENT_GROUP), a)) != VLAD_OK)
    return retval;
  if ((retval = stable->get(o_tmp - ((o_tmp < o_len) ? 0 : o_len), VLAD_IDENT_OBJECT | ((o_tmp < o_len) ? 0 : VLAD_IDENT_GROUP), o)) != VLAD_OK)
    return retval;

  return VLAD_OK;
}

int kb::decode_member(char **e, char **g, unsigned int n)
{
  int retval;

  if (n < s_len * sg_len) {
    /* subject member atom */
    if ((retval = stable->get(n / sg_len, VLAD_IDENT_SUBJECT, e)) != VLAD_OK)
      return retval;
    if ((retval = stable->get(n % sg_len, VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP, g)) != VLAD_OK)
      return retval;
  }
  else if (n < (s_len * sg_len) + (a_len * ag_len)) {
    /* access member atom */
    n = n - (s_len * sg_len);
    if ((retval = stable->get(n / ag_len, VLAD_IDENT_ACCESS, e)) != VLAD_OK)
      return retval;
    if ((retval = stable->get(n % ag_len, VLAD_IDENT_ACCESS | VLAD_IDENT_GROUP, g)) != VLAD_OK)
      return retval;
  }
  else {
    /* object member atom */
    n = n - ((s_len * sg_len) + (a_len * ag_len));
    if ((retval = stable->get(n / og_len, VLAD_IDENT_OBJECT, e)) != VLAD_OK)
      return retval;
    if ((retval = stable->get(n % og_len, VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP, g)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

int kb::decode_subset(char **g1, char **g2, unsigned int n)
{
  int retval;

  if (n < sg_len * sg_len) {
    /* subject subset atom */
    if ((retval = stable->get(n / sg_len, VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP, g1)) != VLAD_OK)
      return retval;
    if ((retval = stable->get(n % sg_len, VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP, g2)) != VLAD_OK)
      return retval;
  }
  else if (n < (sg_len * sg_len) + (ag_len * ag_len)) {
    /* access subset atom */
    n = n - (sg_len * sg_len);
    if ((retval = stable->get(n / ag_len, VLAD_IDENT_ACCESS | VLAD_IDENT_GROUP, g1)) != VLAD_OK)
      return retval;
    if ((retval = stable->get(n % ag_len, VLAD_IDENT_ACCESS | VLAD_IDENT_GROUP, g2)) != VLAD_OK)
      return retval;
  }
  else {
    /* object subset atom */
    n = n - ((sg_len * sg_len) + (ag_len * ag_len));
    if ((retval = stable->get(n / og_len, VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP, g1)) != VLAD_OK)
      return retval;
    if ((retval = stable->get(n % og_len, VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP, g2)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}
