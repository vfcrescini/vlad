/*
 * atom.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <new>

#include <config.h>
#include <vlad.h>
#include <atom.h>

holds_atom::holds_atom()
{
  sub = NULL;
  acc = NULL;
  obj = NULL;
}

holds_atom::holds_atom(identifier *s, identifier *a, identifier *o)
{
  sub = s;
  acc = a;
  obj = o;
}

holds_atom::~holds_atom()
{
}

void holds_atom::get(identifier **s, identifier **a, identifier **o)
{
  if (s != NULL)
    *s = sub;
  if (a != NULL)
    *a = acc;
  if (o != NULL)
    *o = obj;
}

member_atom::member_atom()
{
  elt = NULL;
  grp = NULL;
}

member_atom::member_atom(identifier *e, identifier *g)
{
  elt = e;
  grp = g;
}

member_atom::~member_atom()
{
}

void member_atom::get(identifier **e, identifier **g)
{
  if (e != NULL)
    *e = elt;
  if (g != NULL)
    *g = grp;
}

subset_atom::subset_atom()
{
  grp1 = NULL;
  grp2 = NULL;
  
}

subset_atom::subset_atom(identifier *g1, identifier *g2)
{
  grp1 = g1;
  grp2 = g2;
}

subset_atom::~subset_atom()
{
}

void subset_atom::get(identifier **g1, identifier **g2)
{
  if (g1 != NULL)
    *g1 = grp1;
  if (g2 != NULL)
    *g2 = grp2;
}

atom::atom()
{
  type = VLAD_ATOM_NULL;
  truth = true;
}

atom::~atom()
{
  switch(type) {
    case VLAD_ATOM_NULL :
      break;
    case VLAD_ATOM_HOLDS :
      delete holds;
      break;
    case VLAD_ATOM_MEMBER :
      delete member;
      break;
    case VLAD_ATOM_SUBSET :
      delete subset;
      break;
  }
}

int atom::init_holds(identifier *s, identifier *a, identifier *o, bool t)
{
  if (s == NULL || a == NULL || o == NULL)
    return VLAD_NULLPTR;

  if (!VLAD_IDENT_IS_SUBJECT(*s) ||
      !VLAD_IDENT_IS_ACCESS(*a) ||
      !VLAD_IDENT_IS_OBJECT(*o))
    return VLAD_INVALIDINPUT;

  if ((holds =  VLAD_NEW(holds_atom(s, a, o))) == NULL)
    return VLAD_MALLOCFAILED;

  type = VLAD_ATOM_HOLDS;
  truth = t;

  return VLAD_OK;
}

int atom::init_member(identifier *e, identifier *g, bool t)
{
  if (e == NULL || g == NULL)
    return VLAD_NULLPTR;

  if (VLAD_IDENT_IS_GROUP(*e) ||
      !VLAD_IDENT_IS_GROUP(*g) || 
      VLAD_IDENT_BASETYPE(*e) != VLAD_IDENT_BASETYPE(*g))
    return VLAD_INVALIDINPUT;

  if ((member = VLAD_NEW(member_atom(e, g))) == NULL)
    return VLAD_MALLOCFAILED;

  type = VLAD_ATOM_MEMBER;
  truth = t;
  
  return VLAD_OK;
}

int atom::init_subset(identifier *g1, identifier *g2, bool t)
{
  if (g1 == NULL || g2 == NULL)
    return VLAD_NULLPTR;

  if (!VLAD_IDENT_IS_GROUP(*g1) ||
      !VLAD_IDENT_IS_GROUP(*g2) || 
      VLAD_IDENT_BASETYPE(*g1) != VLAD_IDENT_BASETYPE(*g2))
    return VLAD_INVALIDINPUT;

  if ((subset = VLAD_NEW(subset_atom(g1, g2))) == NULL)
    return VLAD_MALLOCFAILED;

  type = VLAD_ATOM_MEMBER;
  truth = t;

  return VLAD_OK;
}

unsigned char atom::get_type()
{
  return type;
}

int atom::get(identifier **i1, identifier **i2, identifier **i3, bool *tr, unsigned char *ty)
{
  switch(type) {
    case VLAD_ATOM_NULL :
      return VLAD_UNINITIALISED;
    case VLAD_ATOM_HOLDS :
      holds->get(i1, i2, i3);
      break;
    case VLAD_ATOM_MEMBER :
      member->get(i1, i2);
      if (i3 != NULL)
        *i3 = NULL;
      break;
    case VLAD_ATOM_SUBSET :
      subset->get(i1, i2);
      if (i3 != NULL)
        *i3 = NULL;
      break;
  }

  if (tr != NULL)
    *tr = truth;

  if (ty != NULL)
    *ty = type;

  return VLAD_OK;
}

bool atom::cmp(list_item *item)
{
  atom *tmp = dynamic_cast<atom *> (item);
  identifier *i11 = NULL;
  identifier *i12 = NULL;
  identifier *i13 = NULL;
  identifier *i21 = NULL;
  identifier *i22 = NULL;
  identifier *i23 = NULL;

  /* if type == VLAD_ATOM_NULL, match any atom */
  if (tmp->type == VLAD_ATOM_NULL)
    return true;

  if (tmp->type != type || tmp->truth != truth)
    return false;

  /* extract identifiers, there should be no errors here but if it occurs,
   * assume that we have no match */
  if (get(&i11, &i12, &i13, NULL, NULL) != VLAD_OK)
    return false;

  if (tmp->get(&i21, &i22, &i23, NULL, NULL) != VLAD_OK)
    return false;

  /* note: identifier::cmp() treats identifiers with NULL names and 0 types as 
   * wildcards */

  switch(type) {
    case VLAD_ATOM_NULL :
      /* this should not be allowed */
      return false;
    case VLAD_ATOM_HOLDS :
      return i11->cmp(i21) && i12->cmp(i22) && i13->cmp(i23);
    case VLAD_ATOM_MEMBER :
      return i11->cmp(i21) && i12->cmp(i22);
    case VLAD_ATOM_SUBSET :
      return i11->cmp(i21) && i12->cmp(i22);
  }
  return false;
}