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

atom::atom(unsigned char ty, bool tr)
{
  type = ty;
  truth = tr;
}

atom::~atom()
{
}

unsigned char atom::get_type()
{
  return type;
}

bool atom::get_truth()
{
  return truth;
}

holds_atom::holds_atom(identifier *s, identifier *a, identifier *o, bool t) :
atom(VLAD_ATOM_HOLDS, t)
{
  sub = s;
  acc = a;
  obj = o;
}

holds_atom::~holds_atom()
{
}

bool holds_atom::verify(bool ground)
{
  /* only check for nullness if we are dealing with a ground atom */
  if (ground && (sub == NULL || acc == NULL || obj == NULL))
    return false;

  /* check ground subject */
  if (sub != NULL &&
      VLAD_IDENT_STRING(*sub) == NULL &&
      !VLAD_IDENT_IS_SUBJECT(*sub))
    return false;

  /* check ground access */
  if (acc != NULL &&
      VLAD_IDENT_STRING(*acc) == NULL &&
      !VLAD_IDENT_IS_ACCESS(*acc))
    return false;

   /* check ground object */
  if (obj != NULL &&
      VLAD_IDENT_STRING(*obj) == NULL &&
      !VLAD_IDENT_IS_OBJECT(*obj))
    return false; 

  return true;
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

bool holds_atom::cmp(list_item *item)
{
  atom *tmp_atom = NULL; 
  holds_atom *tmp_holds = NULL;

  /* a NULL will not match anything */
  if (item == NULL)
    return false;
  
  /* check type and truth value */
  if (!VLAD_ATOM_IS_HOLDS(*(tmp_atom = dynamic_cast<atom *> (item))))
    return false;

  if (truth != tmp_atom->get_truth())
    return false;

  tmp_holds = dynamic_cast<holds_atom *> (tmp_atom);

  return 
    VLAD_IDENT_EQ(sub, tmp_holds->sub) &&
    VLAD_IDENT_EQ(acc, tmp_holds->acc) &&
    VLAD_IDENT_EQ(obj, tmp_holds->obj);
}

member_atom::member_atom(identifier *e, identifier *g, bool t) :
atom(VLAD_ATOM_MEMBER, t)
{
  elt = e;
  grp = g;
}

member_atom::~member_atom()
{
}

bool member_atom::verify(bool ground)
{
  /* check only for nullness if we are dealing with a ground atom */
  if (ground && (elt == NULL || grp == NULL))
    return false;

  /* check names for nullness */
  if (elt != NULL && VLAD_IDENT_STRING(*elt) == NULL)
    return false;

  if (grp != NULL && VLAD_IDENT_STRING(*grp) == NULL)
    return false;

  return 
    VLAD_IDENT_BASETYPE(*elt) == VLAD_IDENT_BASETYPE(*grp) && 
    !VLAD_IDENT_IS_GROUP(*elt) &&
    VLAD_IDENT_IS_GROUP(*grp);
}

void member_atom::get(identifier **e, identifier **g)
{
  if (e != NULL)
    *e = elt;
  if (g != NULL)
    *g = grp;
}

bool member_atom::cmp(list_item *item)
{
  atom *tmp_atom = NULL; 
  member_atom *tmp_member = NULL;

  /* a NULL will not match anything */
  if (item == NULL)
    return false;
  
  /* check type and truth value */
  if (!VLAD_ATOM_IS_MEMBER(*(tmp_atom = dynamic_cast<atom *> (item))))
    return false;

  if (truth != tmp_atom->get_truth())
    return false;

  tmp_member = dynamic_cast<member_atom *> (tmp_atom);

  return
    VLAD_IDENT_EQ(elt, tmp_member->elt) &&
    VLAD_IDENT_EQ(grp, tmp_member->grp);
}

subset_atom::subset_atom(identifier *g1, identifier *g2, bool t) :
atom(VLAD_ATOM_SUBSET, t)
{
  grp1 = g1;
  grp2 = g2;
}

subset_atom::~subset_atom()
{
}

bool subset_atom::verify(bool ground)
{
  /* check only for nullness if we are dealing with a ground atom */
  if (ground && (grp1 == NULL || grp2 == NULL))
    return false;

  /* check names for nullness */
  if (grp1 != NULL && VLAD_IDENT_STRING(*grp1) == NULL)
    return false;

  if (grp2 != NULL && VLAD_IDENT_STRING(*grp2) == NULL)
    return false;

  return 
    VLAD_IDENT_BASETYPE(*grp1) == VLAD_IDENT_BASETYPE(*grp2) && 
    VLAD_IDENT_IS_GROUP(*grp1) &&
    VLAD_IDENT_IS_GROUP(*grp2);
}

void subset_atom::get(identifier **g1, identifier **g2)
{
  if (g1 != NULL)
    *g1 = grp1;
  if (g2 != NULL)
    *g2 = grp2;
}

bool subset_atom::cmp(list_item *item)
{
  atom *tmp_atom = NULL; 
  subset_atom *tmp_subset = NULL;

  /* a NULL means not match anything */
  if (item == NULL)
    return false;
  
  /* check type and truth value */
  if (!VLAD_ATOM_IS_SUBSET(*(tmp_atom = dynamic_cast<atom *> (item))))
    return false;

  if (truth != tmp_atom->get_truth())
    return false;

  tmp_subset = dynamic_cast<subset_atom *> (tmp_atom);

  return
    VLAD_IDENT_EQ(grp1, tmp_subset->grp1) &&
    VLAD_IDENT_EQ(grp2, tmp_subset->grp2);
}
