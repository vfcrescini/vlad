/*
 * atomlist.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <new>

#include <config.h>
#include <vlad.h>
#include <atomlist.h>

atomlist::atomlist(bool gr) : list(true)
{
  ground = gr;
}

atomlist::~atomlist()
{
  purge(true);
}

/* add atom into list */
int atomlist::add(atom *a)
{
  if (a == NULL)
    return VLAD_NULLPTR;

  if (!a->verify(ground))
    return VLAD_INVALIDINPUT;

  return list::add(a);
}

/* del atom from list */
int atomlist::del(atom *a)
{
  if (a == NULL)
    return VLAD_NULLPTR;

  return list::del_d(a, true);
}

/* get reference of atom that matches in (may have wildcards) */
int atomlist::get(atom *in, atom ***out, unsigned int *s)
{
  if (in == NULL || out == NULL || s == NULL)
    return VLAD_NULLPTR;

  return list::get_d(in, (list_item ***) out, s);
}

/* few more interfaces for adding atoms */
int atomlist::add_holds(identifier *s, identifier *a, identifier *o, bool t)
{
  holds_atom *tmp = NULL;

  if ((tmp = VLAD_NEW(holds_atom(s, a, o, t))) == NULL)
    return VLAD_MALLOCFAILED;

  if (!tmp->verify(ground)) {
    delete tmp;
    return VLAD_INVALIDINPUT;
  }
   
  return add(tmp);
}

int atomlist::add_member(identifier *e, identifier *g, bool t)
{
  member_atom *tmp = NULL;

  if ((tmp = VLAD_NEW(member_atom(e, g, t))) == NULL)
    return VLAD_MALLOCFAILED;

  if (!tmp->verify(ground)) {
    delete tmp;
    return VLAD_INVALIDINPUT;
  }

  return add(tmp);
}

int atomlist::add_subset(identifier *g1, identifier *g2, bool t)
{
  subset_atom *tmp = NULL;

  if ((tmp = VLAD_NEW(subset_atom(g1, g2, t))) == NULL)
    return VLAD_MALLOCFAILED;

  if (!tmp->verify(ground)) {
    delete tmp;
    return VLAD_INVALIDINPUT;
  }
   
  return add(tmp);
}
