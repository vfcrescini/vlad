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
  itable = NULL;
  ctable = NULL;
  ttable = NULL;
  stage = 0;
}

kb::~kb()
{
  if (stable != NULL)
    delete stable;

  if (itable != NULL)
    delete itable;

  if (ctable != NULL)
    delete ctable;

  if (ttable != NULL)
    delete ttable;
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

  /* constraints table */
  if (ctable != NULL)
    delete ctable;

  if ((ctable = VLAD_NEW(consttab("constraint table"))) == NULL)
    return VLAD_MALLOCFAILED;

  /* transformation declaration table */
  if (ttable != NULL)
    delete ttable;

  if ((ttable = VLAD_NEW(transtab("tranformation declaration table"))) == NULL)
    return VLAD_MALLOCFAILED;

  stage = 1;

  return VLAD_OK;
}

/* after this is called, no further calls to add_inittab() is allowed */
int kb::close_symtab()
{
  if (stage != 1)
    return VLAD_FAILURE;

  stage = 2;
  
  return VLAD_OK;
}

/* after this is called, no further calls to add_inittab() is allowed */
int kb::close_inittab()
{
  if (stage != 2)
    return VLAD_FAILURE;

  stage = 3;

  return VLAD_OK;
}

/* after this is called, no further calls to add_consttab() is allowed */
int kb::close_consttab()
{
  if (stage != 3)
    return VLAD_FAILURE;

  stage = 4;
  return VLAD_OK;
}

/* after this is called, no further calls to add_transtab() is allowed */
int kb::close_transtab()
{
  if (stage != 4)
    return VLAD_FAILURE;

  stage = 5;
  return VLAD_OK;
}

/* register an identifier in the kb */
int kb::add_symtab(const char *n, unsigned char t)
{
  if (stage != 1)
    return VLAD_FAILURE;

  return stable->add(n, t);
}

/* add an atom into the initial state table */
int kb::add_inittab(atom *a)
{
  int retval;
  atom *tmp;

  if (stage != 2)
    return VLAD_FAILURE;

  if (a == NULL)
    return VLAD_NULLPTR;

  /* first check if the atom is valid */
  if ((retval = verify_atom(a)) != VLAD_OK)
    return retval;

  /* now copy and add to inittab */
  if ((tmp = VLAD_NEW(atom())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = tmp->init_atom(a)) != VLAD_OK)
    return retval;

  return itable->add(tmp);
}

/* add an expression into the constraints table */
int kb::add_consttab(expression *e, expression *c, expression *n)
{
  int retval;
  unsigned int i;
  expression *exp;
  expression *cond;
  expression *ncond;
  atom *tmp1;
  atom *tmp2;

  if (stage != 3)
    return VLAD_FAILURE;

  /* only e is required to be non-null */
  if (e == NULL)
    return VLAD_NULLPTR;

  /* create new expressions */
  if ((exp = VLAD_NEW(expression(NULL))) == NULL)
    return VLAD_MALLOCFAILED;
  if ((cond = VLAD_NEW(expression(NULL))) == NULL)
    return VLAD_MALLOCFAILED;
  if ((ncond = VLAD_NEW(expression(NULL))) == NULL)
    return VLAD_MALLOCFAILED;

  /* 
   * now, we must go through every atom of every exression to ensure
   * their validity. while we are going through them, we might as well
   * make a copy.
   */

  /* exression */
  for (i = 0; i < e->length(); i++) {
    if ((retval = e->get(i, &tmp1)) != VLAD_OK)
      return retval;
    if ((retval = verify_atom(tmp1)) != VLAD_OK)
      return retval;
    if ((tmp2 = VLAD_NEW(atom())) == NULL)
      return VLAD_MALLOCFAILED;
    if ((retval = tmp2->init_atom(tmp1)) != VLAD_OK)
      return retval;
    if ((retval = exp->add(tmp2)) != VLAD_OK)
      return retval;
  }

  /* condition: if there is no condition, simply use an empty expression */
  if (c != NULL) {
    for (i = 0; i < c->length(); i++) {
      if ((retval = c->get(i, &tmp1)) != VLAD_OK)
        return retval;
      if ((retval = verify_atom(tmp1)) != VLAD_OK)
        return retval;
      if ((tmp2 = VLAD_NEW(atom())) == NULL)
        return VLAD_MALLOCFAILED;
      if ((retval = tmp2->init_atom(tmp1)) != VLAD_OK)
        return retval;
      if ((retval = cond->add(tmp2)) != VLAD_OK)
        return retval;
    }
  }

  /* negative condition: if there is no negative cond, use empty expression */
  if (n != NULL) {
    for (i = 0; i < n->length(); i++) {
      if ((retval = n->get(i, &tmp1)) != VLAD_OK)
        return retval;
      if ((retval = verify_atom(tmp1)) != VLAD_OK)
        return retval;
      if ((tmp2 = VLAD_NEW(atom())) == NULL)
        return VLAD_MALLOCFAILED;
      if ((retval = tmp2->init_atom(tmp1)) != VLAD_OK)
        return retval;
      if ((retval = ncond->add(tmp2)) != VLAD_OK)
        return retval;
    } 
  }

  /* finally, we add the expressions into the cosntraints table */
  return ctable->add(exp, cond, ncond);
}

/* add a transformation declaration in the trans table */
int kb::add_transtab(const char *n,
                     stringlist *v,
                     expression *pr,
                     expression *po)
{
  stringlist *vlist;
  expression *precond;
  expression *postcond;

  if ((vlist = VLAD_NEW(stringlist(NULL))) == NULL)
    return VLAD_MALLOCFAILED;
  if ((precond = VLAD_NEW(expression(NULL))) == NULL)
    return VLAD_MALLOCFAILED;
  if ((postcond = VLAD_NEW(expression(NULL))) == NULL)
    return VLAD_MALLOCFAILED;

  if (n == NULL)
    return VLAD_NULLPTR;

  return VLAD_OK;
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

  /* this function is only valid after the symtab is closed */
  if (stage < 2)
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
