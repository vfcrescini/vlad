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
  qtable = NULL;
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

  if (qtable != NULL)
    delete qtable;
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

  /* query table */

  if (qtable != NULL)
    delete qtable;

  if ((qtable = VLAD_NEW(querytab("query table"))) == NULL)
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
  if ((retval = verify_atom(a, NULL)) != VLAD_OK)
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
    if ((retval = verify_atom(tmp1, NULL)) != VLAD_OK)
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
      if ((retval = verify_atom(tmp1, NULL)) != VLAD_OK)
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
      if ((retval = verify_atom(tmp1, NULL)) != VLAD_OK)
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
  int retval;
  unsigned int i;
  char *name;
  stringlist *vlist;
  expression *precond;
  expression *postcond;

  /* we only allow this function after consttab is closed */
  if (stage != 4)
    return VLAD_FAILURE;

  if (n == NULL || po == NULL)
    return VLAD_NULLPTR;

  /* copy name */
  if ((name = VLAD_STRING_MALLOC(n)) == NULL)
    return VLAD_MALLOCFAILED;

  strcpy(name, n);

  /* verify and copy varlist */
  if ((vlist = VLAD_NEW(stringlist(NULL))) == NULL)
    return VLAD_MALLOCFAILED;

  if (v != NULL) {
    for (i = 0; i < v->length(); i++) {
      char *tmp;
      if ((retval = v->get(i, &tmp)) != VLAD_OK)
        return retval;
      /* check if the variable is already used as an identifier */
      if ((retval = stable->find(tmp)) != VLAD_NOTFOUND)
        return (retval == VLAD_OK) ? VLAD_FAILURE : retval;
      if ((retval = vlist->add(tmp)) != VLAD_OK)
        return retval;
    }
  }

  /* verify and copy precondition */
  if ((precond = VLAD_NEW(expression(NULL))) == NULL)
    return VLAD_MALLOCFAILED;

  if (pr != NULL) {
    for (i = 0; i < pr->length(); i++) {
      atom *tmp1;
      atom *tmp2;
      if ((retval = pr->get(i, &tmp1)) != VLAD_OK)
        return retval;
      if ((retval = verify_atom(tmp1, v)) != VLAD_OK)
        return retval;
      if ((tmp2 = VLAD_NEW(atom())) == NULL)
        return VLAD_MALLOCFAILED;
      if ((retval = tmp2->init_atom(tmp1)) != VLAD_OK)
        return retval;
      if ((retval = precond->add(tmp2)) != VLAD_OK)
        return retval;
    }
  }

  /* verify and copy the postcondition */
  if ((postcond = VLAD_NEW(expression(NULL))) == NULL)
    return VLAD_MALLOCFAILED;

  for (i = 0; i < po->length(); i++) {
    atom *tmp1;
    atom *tmp2;
    if ((retval = po->get(i, &tmp1)) != VLAD_OK)
      return retval;
    if ((retval = verify_atom(tmp1, v)) != VLAD_OK)
      return retval;
    if ((tmp2 = VLAD_NEW(atom())) == NULL)
      return VLAD_MALLOCFAILED;
    if ((retval = tmp2->init_atom(tmp1)) != VLAD_OK)
      return retval;
    if ((retval = postcond->add(tmp2)) != VLAD_OK)
      return retval;
  }

  /* if all went  well, add to the trans table */
  return ttable->add(name, vlist, precond, postcond);
}

/* add a query in the query table */
int kb::add_querytab(expression *e, transreflist *r)
{
  int retval;
  unsigned int i;
  expression *exp;
  transreflist *rlist;

  /* we only allow this function after transtab is closed */
  if (stage != 5)
    return VLAD_FAILURE;

  if (e == NULL)
    return VLAD_NULLPTR;

  /* verify and copy query expression */
  if ((exp = VLAD_NEW(expression(NULL))) == NULL)
    return VLAD_MALLOCFAILED;

  for (i = 0; i < e->length(); i++) {
    atom *tmp1;
    atom *tmp2;
    if ((retval = e->get(i, &tmp1)) != VLAD_OK)
      return retval;
    if ((retval = verify_atom(tmp1, NULL)) != VLAD_OK)
      return retval;
    if ((tmp2 = VLAD_NEW(atom())) == NULL)
      return VLAD_MALLOCFAILED;
    if ((retval = tmp2->init_atom(tmp1)) != VLAD_OK)
      return retval;
    if ((retval = exp->add(tmp2)) != VLAD_OK)
      return retval;
  }

  /* now verify and copy transref */
  if ((rlist = VLAD_NEW(transreflist(NULL))) == NULL)
    return VLAD_MALLOCFAILED;

  if (r != NULL) {
    for (i = 0; i < r->length(); i++) {
      transref *tmp1;
      transref *tmp2;
      if ((retval = r->get(i, &tmp1)) != VLAD_OK)
        return retval;
      if ((retval = verify_transref(tmp1)) != VLAD_OK)
        return retval;
      if ((tmp2 = VLAD_NEW(transref(tmp1->get_name(), tmp1->get_ilist()))) == NULL)
        return VLAD_MALLOCFAILED;
      if ((retval = rlist->add(tmp2)) != VLAD_OK)
        return retval;
    }
  }

  /* if all went well, add to querytab */
  return qtable->add(exp, rlist);
}

/* make sure atom a is valid */
int kb::verify_atom(atom *a, stringlist *v)
{
  int retval;
  char *tmp1;
  char *tmp2;
  char *tmp3;

  /* this function is only valid after the symtab is closed */
  if (stage < 2)
    return VLAD_FAILURE;

  switch(a->get_type()) {
    case VLAD_ATOM_CONST :
      /* constants need not be checked */
      break;
    case VLAD_ATOM_HOLDS :
      if ((retval = a->get_holds(&tmp1, &tmp2, &tmp3)) != VLAD_OK)
        return retval;
      return verify_atom_holds(tmp1, tmp2, tmp3, v);
    case VLAD_ATOM_MEMBER :
      if ((retval = a->get_member(&tmp1, &tmp2)) != VLAD_OK)
        return retval;
      return verify_atom_member(tmp1, tmp2, v);
    case VLAD_ATOM_SUBSET :
      if ((retval = a->get_subset(&tmp1, &tmp2)) != VLAD_OK)
        return retval;
      return verify_atom_subset(tmp1, tmp2, v);
    default :
      return VLAD_INVALIDINPUT;
  }

  return VLAD_OK;
}

/* 
 * verifies that s, a and o are in the symtab and that they are of the right 
 * type, or listed in v if v is non-null
 */
int kb::verify_atom_holds(const char *s, 
                         const char *a, 
                         const char *o, 
                         stringlist *v)
{
  int retval;
  unsigned char type;

  if (s == NULL || a == NULL || o == NULL)
    return VLAD_NULLPTR;

  /* check subject */
  switch((retval = stable->type(s, &type))) {
    case VLAD_OK :
      /* in symtab so check if it is a subject */
      if (!VLAD_IDENT_IS_SUBJECT(type))
        return VLAD_INVALIDINPUT;
      break;
    case VLAD_NOTFOUND :
      /* not in symtab */
      if (v == NULL)
        return VLAD_INVALIDINPUT;
      /* if this is a non-ground atom, check in the var list */
      if ((retval = v->find(s)) != VLAD_OK)
        return (retval == VLAD_NOTFOUND) ? VLAD_INVALIDINPUT : retval;
      break;
    default :
      return retval;
  }

  /* check access */
  switch((retval = stable->type(a, &type))) {
    case VLAD_OK :
      /* in symtab so check if it is an access */
      if (!VLAD_IDENT_IS_ACCESS(type))
        return VLAD_INVALIDINPUT;
      break;
    case VLAD_NOTFOUND :
      /* not in symtab */
      if (v == NULL)
        return VLAD_INVALIDINPUT;
      /* if this is a non-ground atom, check in the var list */
      if ((retval = v->find(a)) != VLAD_OK)
        return (retval == VLAD_NOTFOUND) ? VLAD_INVALIDINPUT : retval;
      break;
    default :
      return retval;
  }

  /* check object */
  switch((retval = stable->type(o, &type))) {
    case VLAD_OK :
      /* in symtab so check if it is an object */
      if (!VLAD_IDENT_IS_OBJECT(type))
        return VLAD_INVALIDINPUT;
      break;
    case VLAD_NOTFOUND :
      /* not in symtab */
      if (v == NULL)
        return VLAD_INVALIDINPUT;
      /* if this is a non-ground atom, check in the var list */
      if ((retval = v->find(o)) != VLAD_OK)
        return (retval == VLAD_NOTFOUND) ? VLAD_INVALIDINPUT : retval;
      break;
    default :
      return retval;
  }
  
  return VLAD_OK;
}

/* 
 * verifies that e and g are in the symtab and that they are of the right 
 * type, or listed in v if v is non-null
 */
int kb::verify_atom_member(const char *e, const char *g, stringlist *v)
{
  int retval;
  unsigned char type_e;
  unsigned char type_g;
  bool var = false;

  if (e == NULL || g == NULL)
    return VLAD_NULLPTR;

  /* check element */
  switch((retval = stable->type(e, &type_e))) {
    case VLAD_OK :
      /* in symtab so check if it is a non-group */
      if (VLAD_IDENT_IS_GROUP(type_e))
        return VLAD_INVALIDINPUT;
      break;
    case VLAD_NOTFOUND :
      /* not in symtab */
      if (v == NULL)
        return VLAD_INVALIDINPUT;
      /* if this is a non-ground atom, check in the var list */
      if ((retval = v->find(e)) != VLAD_OK)
        return (retval == VLAD_NOTFOUND) ? VLAD_INVALIDINPUT : retval;
      var = true;
      break;
    default :
      return retval;
  }

  /* check group */
  switch((retval = stable->type(g, &type_g))) {
    case VLAD_OK :
      /* in symtab so check if it is a group */
      if (!VLAD_IDENT_IS_GROUP(type_g))
        return VLAD_INVALIDINPUT;
      /* also check if it has the same base type as e if e is not a var */
      if (!var && VLAD_IDENT_BASETYPE(type_e) != VLAD_IDENT_BASETYPE(type_g))
        return VLAD_INVALIDINPUT;
      break;
    case VLAD_NOTFOUND :
      /* not in symtab */
      if (v == NULL)
        return VLAD_INVALIDINPUT;
      /* if this is a non-ground atom, check in the var list */
      if ((retval = v->find(g)) != VLAD_OK)
        return (retval == VLAD_NOTFOUND) ? VLAD_INVALIDINPUT : retval;
      break;
    default :
      return retval;
  }
  
  return VLAD_OK;
}

/* 
 * verifies that g1 and g2 are in the symtab and that they are of the right 
 * type, or listed in v if v is non-null
 */
int kb::verify_atom_subset(const char *g1, const char *g2, stringlist *v)
{
  int retval;
  unsigned char type_g1;
  unsigned char type_g2;
  bool var = false;

  if (g1 == NULL || g2 == NULL)
    return VLAD_NULLPTR;

  /* check group1 */
  switch((retval = stable->type(g1, &type_g1))) {
    case VLAD_OK :
      /* in symtab so check if it is a group */
      if (!VLAD_IDENT_IS_GROUP(type_g1))
        return VLAD_INVALIDINPUT;
      break;
    case VLAD_NOTFOUND :
      /* not in symtab */
      if (v == NULL)
        return VLAD_INVALIDINPUT;
      /* if this is a non-ground atom, check in the var list */
      if ((retval = v->find(g1)) != VLAD_OK)
        return (retval == VLAD_NOTFOUND) ? VLAD_INVALIDINPUT : retval;
      var = true;
      break;
    default :
      return retval;
  }

  /* check group2 */
  switch((retval = stable->type(g2, &type_g2))) {
    case VLAD_OK :
      /* in symtab so check if it is a group */
      if (!VLAD_IDENT_IS_GROUP(type_g2))
        return VLAD_INVALIDINPUT;
      /* also check if it has the same base type as g1 if g1 is not a var */
      if (!var && VLAD_IDENT_BASETYPE(type_g1) != VLAD_IDENT_BASETYPE(type_g2))
        return VLAD_INVALIDINPUT;
      break;
    case VLAD_NOTFOUND :
      /* not in symtab */
      if (v == NULL)
        return VLAD_INVALIDINPUT;
      /* if this is a non-ground atom, check in the var list */
      if ((retval = v->find(g2)) != VLAD_OK)
        return (retval == VLAD_NOTFOUND) ? VLAD_INVALIDINPUT : retval;
      break;
    default :
      return retval;
  }
  
  return VLAD_OK;
}

/* make sure transref is valid */
int kb::verify_transref(transref *r)
{
  int retval;
  unsigned int i;
  transdef *tdef;

  if (stage < 5)
    return VLAD_FAILURE;

  if (r == NULL)
    return VLAD_NULLPTR;

  /* retrieve respective trans in transtab */
  if ((retval = ttable->get(r->get_name(), &tdef)) != VLAD_OK)
    return retval;
  
  /* check that the name transformation is valid */
  if (strcmp(r->get_name(), tdef->get_name()))
    return VLAD_INVALIDINPUT;

  /* check that the number of ident listed is the same as the transformation */
  if (tdef->get_vlist()->length() != r->get_ilist()->length())
    return VLAD_INVALIDINPUT;

  /* check that every ident is valid in symtab */
  for (i = 0; i < r->get_ilist()->length(); i++) {
    char *s;

    if ((retval = r->get_ilist()->get(i, &s)) != VLAD_OK)
      return retval;

    if ((retval = stable->find(s)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}
