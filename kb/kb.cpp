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
  stage1 = false;
  stage2 = false;
}

kb::~kb()
{
  if (stable != NULL)
    delete stable;

  if (itable != NULL)
    delete itable;
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

  /* initial state table */
  if (itable != NULL)
    delete itable;

  if ((itable = VLAD_NEW(expression("initial state table"))) == NULL)
    return VLAD_MALLOCFAILED;

  initialised = true;
  stage1 = false;
  stage2 = false;

  return VLAD_OK;
}

/* after this is called, no further calls to add_inittab() is allowed */
int kb::close_symtab()
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  stage1 = true;

  return VLAD_OK;
}

/* after this is called, no further calls to add_inittab() is allowed */
int kb::close_inittab()
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  if (!stage1)
    return VLAD_FAILURE;

  stage2 = true;

  return VLAD_OK;
}

/* register an identifier in the kb */
int kb::add_symtab(const char *n, unsigned char t)
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  if (stage1)
    return VLAD_FAILURE;

  return stable->add(n, t);
}

/* add an atom into the initial state table */
int kb::add_inittab(atom *a)
{
  int retval;
  atom *tmp;

  /* first check if the atom is valid */
  if ((retval = verify_atom(a)) != VLAD_OK)
    return retval;

  /* now copy and add to inittab */
  if ((tmp = VLAD_NEW(atom())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = tmp->init_atom(a)) != VLAD_OK)
    return retval;

  /* ignore duplicates */
  if ((retval = itable->add(tmp)) == VLAD_DUPLICATE)
    return VLAD_OK;

  return retval;
}

/* make sure atom a is valid */
int kb::verify_atom(atom *a)
{
  int retval;
  char *tmp1;
  char *tmp2;
  char *tmp3;
  unsigned char type1;
  unsigned char type2;
  unsigned char type3;

  if (!stage1)
    return VLAD_FAILURE;

  switch(a->get_type()) {
    case VLAD_ATOM_CONST :
      /* constants need not be checked */
      break;
    case VLAD_ATOM_HOLDS : {
      a->get_holds(&tmp1, &tmp2, &tmp3);

      if ((retval = stable->type(tmp1, &type1)) != VLAD_OK)
        return retval;
      if ((retval = stable->type(tmp2, &type2)) != VLAD_OK)
        return retval;
      if ((retval = stable->type(tmp3, &type3)) != VLAD_OK)
        return retval;

      if (!VLAD_IDENT_IS_SUBJECT(type1) ||
          !VLAD_IDENT_IS_ACCESS(type2) ||
          !VLAD_IDENT_IS_OBJECT(type3))

        return VLAD_INVALIDINPUT;

      break;
    }
    case VLAD_ATOM_MEMBER : {

      a->get_member(&tmp1, &tmp2);

      if ((retval = stable->type(tmp1, &type1)) != VLAD_OK)
        return retval;
      if ((retval = stable->type(tmp2, &type2)) != VLAD_OK)
        return retval;

      if (VLAD_IDENT_IS_GROUP(type1) || 
          !VLAD_IDENT_IS_GROUP(type2) ||
          VLAD_IDENT_BASETYPE(type1) != VLAD_IDENT_BASETYPE(type2))

        return VLAD_INVALIDINPUT;

      break;
    }
    case VLAD_ATOM_SUBSET : {
      a->get_subset(&tmp1, &tmp2);

      if ((retval = stable->type(tmp1, &type1)) != VLAD_OK)
        return retval;
      if ((retval = stable->type(tmp2, &type2)) != VLAD_OK)
        return retval;

      if (!VLAD_IDENT_IS_GROUP(type1) || 
          !VLAD_IDENT_IS_GROUP(type2) ||
          VLAD_IDENT_BASETYPE(type1) != VLAD_IDENT_BASETYPE(type2))

        return VLAD_INVALIDINPUT;

      break;
    }
    default :
      return VLAD_INVALIDINPUT;
  }

  return VLAD_OK;
}
