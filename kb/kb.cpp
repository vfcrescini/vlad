/*
 * kb.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <new>

#include <config.h>
#include <vlad.h>
#include <identifier.h>
#include <kb.h>

kb::kb()
{
  stable = NULL;
}

kb::~kb()
{
  if (stable != NULL);
    delete stable;
}

/* (re)init kb */
int kb::init()
{
  if (stable != NULL)
    delete stable;

  if ((stable = VLAD_NEW(symtab())) == NULL)
    return VLAD_MALLOCFAILED;

  return stable->init();
}

/* register an identifier in the kb */
int kb::add_ident(const char *n, unsigned char t)
{
  if (stable == NULL)
    return VLAD_UNINITIALISED;

  return stable->add(n, t);
}

/* gives an atom id based on the identifiers already given */
int kb::get_atom(const char *n1,
                 const char *n2,
                 const char *n3,
                 unsigned char t,
                 unsigned int *a)
{
  int retval;
  unsigned int s_len;
  unsigned int a_len;
  unsigned int o_len;
  unsigned int sg_len;
  unsigned int ag_len;
  unsigned int og_len;

  /* 
   * XXX
   *   the id's returned by this function will only remain consistent if no 
   *   further identifiers are registered to the symtab. so care must be
   *   taken to ensure that identifiers are registered first before calling
   *   this function.
   */

  if (stable == NULL)
    return VLAD_UNINITIALISED;

  if (a == NULL)
    return VLAD_NULLPTR;

  /* get the total lengths of the identifier lists */
  s_len = stable->length(VLAD_IDENT_SUBJECT);
  a_len = stable->length(VLAD_IDENT_ACCESS);
  o_len = stable->length(VLAD_IDENT_OBJECT);
  sg_len = stable->length(VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP);
  ag_len = stable->length(VLAD_IDENT_ACCESS | VLAD_IDENT_GROUP);
  og_len = stable->length(VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP);

  switch(t) {
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
      if (!VLAD_IDENT_TYPE_IS_SUBJECT(s_type))
        return VLAD_INVALIDINPUT;
      if (!VLAD_IDENT_TYPE_IS_ACCESS(a_type))
        return VLAD_INVALIDINPUT;
      if (!VLAD_IDENT_TYPE_IS_OBJECT(o_type))
        return VLAD_INVALIDINPUT;

      /* now compute */
      hs = VLAD_IDENT_TYPE_IS_GROUP(s_type) ? s_index + s_len : s_index;
      ha = VLAD_IDENT_TYPE_IS_GROUP(a_type) ? a_index + a_len : a_index;
      ho = VLAD_IDENT_TYPE_IS_GROUP(o_type) ? o_index + o_len : o_index;

      *a = (hs * (a_len + ag_len)* (o_len + og_len)) + (ha * (o_len + og_len)) + ho + 1;

      break;
    }
    case VLAD_ATOM_MEMBER : {
      unsigned int e_index;
      unsigned int g_index;
      unsigned char e_type;
      unsigned char g_type;
      unsigned int holds_len;

      /* get the indices of the identifiers */
      if ((retval = stable->get(n1, &e_index, &e_type)) != VLAD_OK)
        return retval;
      if ((retval = stable->get(n2, &g_index, &g_type)) != VLAD_OK)
        return retval;

      /* check validity */
      if (VLAD_IDENT_TYPE_BASETYPE(e_type) != VLAD_IDENT_TYPE_BASETYPE(g_type) ||
         VLAD_IDENT_TYPE_IS_GROUP(e_type) || !VLAD_IDENT_TYPE_IS_GROUP(g_type))
        return VLAD_INVALIDINPUT;

      /* now compute */
      holds_len = (s_len + sg_len) * (a_len + ag_len) * (o_len + og_len);
      
      switch(VLAD_IDENT_TYPE_BASETYPE(e_type)) {
        case VLAD_IDENT_SUBJECT :
          *a = holds_len + (e_index * sg_len) + g_index + 1;
          break;
        case VLAD_IDENT_ACCESS :
          *a = holds_len + (s_len * sg_len) + (e_index * ag_len) + g_index + 1;
          break;
        case VLAD_IDENT_OBJECT :
          *a = holds_len + (s_len * sg_len) + (a_len * ag_len) + (e_index * og_len) + g_index + 1;
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
      unsigned int holds_len;
      unsigned int memb_len;

      /* get the indices of the identifiers */
      if ((retval = stable->get(n1, &g1_index, &g1_type)) != VLAD_OK)
        return retval;
      if ((retval = stable->get(n2, &g2_index, &g2_type)) != VLAD_OK)
        return retval;

      /* check validity */
      if (VLAD_IDENT_TYPE_BASETYPE(g1_type) != VLAD_IDENT_TYPE_BASETYPE(g2_type) ||
         !VLAD_IDENT_TYPE_IS_GROUP(g1_type) || !VLAD_IDENT_TYPE_IS_GROUP(g2_type))
        return VLAD_INVALIDINPUT;

      /* now we compute */
      holds_len = (s_len + sg_len) * (a_len + ag_len) * (o_len + og_len);
      memb_len = (s_len * sg_len) + (a_len * ag_len) + (o_len * og_len);

      switch(VLAD_IDENT_TYPE_BASETYPE(g1_type)) {
        case VLAD_IDENT_SUBJECT :
          *a = holds_len + memb_len + (g1_index * sg_len) + g2_index + 1;
          break;
        case VLAD_IDENT_ACCESS :
          *a = holds_len + memb_len + (sg_len * sg_len) + (g1_index * ag_len) + g2_index + 1;
          break;
        case VLAD_IDENT_OBJECT :
          *a = holds_len + memb_len + (sg_len * sg_len) + (ag_len * ag_len) + (g1_index * og_len) + g2_index + 1;
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
