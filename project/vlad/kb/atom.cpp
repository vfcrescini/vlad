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

atom::atom()
{
  type = VLAD_ATOM_NULL;
  truth = true;
}

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

holds_atom::holds_atom() 
{
  sub = NULL;
  acc = NULL;
  obj = NULL;
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

bool holds_atom::verify()
{
  if (sub == NULL || acc == NULL || obj == NULL)
    return false;

  if (VLAD_IDENT_STRING(*sub) == NULL ||
      VLAD_IDENT_STRING(*obj) == NULL ||
      VLAD_IDENT_STRING(*acc) == NULL)
    return false;

  return 
    VLAD_IDENT_IS_SUBJECT(*sub) && 
    VLAD_IDENT_IS_ACCESS(*acc) && 
    VLAD_IDENT_IS_OBJECT(*obj);
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
  identifier *s = NULL;
  identifier *a = NULL;
  identifier *o = NULL;

  /* if this atom has any NULL identifier, a segfault will occur */

  /* NULL means match all */
  if (item == NULL)
    return true;
  
  /* check type and truth value */
  if (!VLAD_ATOM_IS_HOLDS(*(tmp_atom = dynamic_cast<atom *> (item))))
    return false;

  if (get_truth() != tmp_atom->get_truth())
    return false;

  /* now get the identifiers and compare them */
  tmp_holds = dynamic_cast<holds_atom *> (tmp_atom);
  
  tmp_holds->get(&s, &a, &o);

  /* treat NULL identifiers as wildcards */
  if (s != NULL && !sub->cmp(s))
    return false;

  if (a != NULL && !acc->cmp(a))
    return false;

  if (o != NULL && !obj->cmp(o))
    return false;

  return true;
}

member_atom::member_atom()
{
  elt = NULL;
  grp = NULL;
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

bool member_atom::verify()
{
  if (elt == NULL || grp == NULL)
    return false;

  if (VLAD_IDENT_STRING(*elt) == NULL || VLAD_IDENT_STRING(*grp) == NULL)
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
  identifier *e = NULL;
  identifier *g = NULL;

  /* if this atom has any NULL identifier, a segfault will occur */

  /* NULL means match all */
  if (item == NULL)
    return true;
  
  /* check type and truth value */
  if (!VLAD_ATOM_IS_MEMBER(*(tmp_atom = dynamic_cast<atom *> (item))))
    return false;

  if (get_truth() != tmp_atom->get_truth())
    return false;

  /* now get the identifiers and compare them */
  tmp_member = dynamic_cast<member_atom *> (tmp_atom);
  
  tmp_member->get(&e, &g);

  /* treat NULL identifiers as wildcards */
  if (e != NULL && !elt->cmp(e))
    return false;

  if (g != NULL && !grp->cmp(g))
    return false;

  return true;
}

subset_atom::subset_atom()
{
  grp1 = NULL;
  grp2 = NULL;
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

bool subset_atom::verify()
{
  if (grp1 == NULL || grp2 == NULL)
    return false;

  if (VLAD_IDENT_STRING(*grp1) == NULL || VLAD_IDENT_STRING(*grp2) == NULL)
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
  identifier *g1 = NULL;
  identifier *g2 = NULL;

  /* if this atom has any NULL identifier, a segfault will occur */

  /* NULL means match all */
  if (item == NULL)
    return true;
  
  /* check type and truth value */
  if (!VLAD_ATOM_IS_SUBSET(*(tmp_atom = dynamic_cast<atom *> (item))))
    return false;

  if (get_truth() != tmp_atom->get_truth())
    return false;

  /* now get the identifiers and compare them */
  tmp_subset = dynamic_cast<subset_atom *> (tmp_atom);
  
  tmp_subset->get(&g1, &g2);

  /* treat NULL identifiers as wildcards */
  if (g1 != NULL && !grp1->cmp(g1))
    return false;

  if (g2 != NULL && !grp2->cmp(g2))
    return false;

  return true;
}
