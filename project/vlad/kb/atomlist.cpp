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

atomlist::atomlist() : list(true)
{
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

  return list::add(a);
}

/* del atom from list */
int atomlist::del(atom *a)
{
  if (a == NULL)
    return VLAD_NULLPTR;

  return list::del_d(a, true);
}

/* get reference of atom that matches in */
int atomlist::get(atom *in, atom **out)
{
  int retval;
  unsigned int size;
  atom **array;

  if (in == NULL || out == NULL)
    return VLAD_NULLPTR;

  if ((retval = list::get_d(in, (list_item ***) &array, &size)) != VLAD_OK)
    return retval;

  *out = array[0];

  free(array);

  return VLAD_OK;
}

/* few more interfaces for adding atoms */
int atomlist::add_holds(identifier *s, identifier *a, identifier *o, bool t)
{
  int retval;
  atom *tmp = NULL;

  if ((tmp = VLAD_NEW(atom())) == NULL)
    return VLAD_MALLOCFAILED;
   
  if ((retval = tmp->init_holds(s, a, o, t)) != VLAD_OK) {
    delete tmp;
    return retval;
  }

  return add(tmp);
}

int atomlist::add_member(identifier *e, identifier *g, bool t)
{
  int retval;
  atom *tmp = NULL;

  if ((tmp = VLAD_NEW(atom())) == NULL)
    return VLAD_MALLOCFAILED;
   
  if ((retval = tmp->init_member(e, g, t)) != VLAD_OK) {
    delete tmp;
    return retval;
  }

  return add(tmp);
}

int atomlist::add_subset(identifier *g1, identifier *g2, bool t)
{
  int retval;
  atom *tmp = NULL;

  if ((tmp = VLAD_NEW(atom())) == NULL)
    return VLAD_MALLOCFAILED;
   
  if ((retval = tmp->init_subset(g1, g2, t)) != VLAD_OK) {
    delete tmp;
    return retval;
  }

  return add(tmp);
}
