/*
 * kb.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <new>

#include <vlad/vlad.h>
#include <vlad/kb.h>
#include <vlad/numberlist.h>
#ifdef VLAD_SMODELS
  #include <vlad/smwrap.h>
#endif

kb::kb()
{
  stable = NULL;
  itable = NULL;
  ctable = NULL;
  ttable = NULL;
  setable = NULL;
#ifdef VLAD_SMODELS
  smobject = NULL;
#endif
  stage = 0;
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

  if (itable != NULL)
    delete itable;

  if (ctable != NULL)
    delete ctable;

  if (ttable != NULL)
    delete ttable;

  if (setable != NULL)
    delete setable;

#ifdef VLAD_SMODELS
  if (smobject != NULL)
    delete smobject;
#endif
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

  if ((itable = VLAD_NEW(expression())) == NULL)
    return VLAD_MALLOCFAILED;

  /* constraints table */
  if (ctable != NULL)
    delete ctable;

  if ((ctable = VLAD_NEW(consttab())) == NULL)
    return VLAD_MALLOCFAILED;

  /* transformation declaration table */
  if (ttable != NULL)
    delete ttable;

  if ((ttable = VLAD_NEW(transtab())) == NULL)
    return VLAD_MALLOCFAILED;

  /* sequence table */
  if (setable != NULL)
    delete setable;

  if ((setable = VLAD_NEW(seqtab())) == NULL)
    return VLAD_MALLOCFAILED;

#ifdef VLAD_SMODELS
  /* smodels smwrap */
  if (smobject != NULL)
    delete smobject;

  smobject = NULL;
#endif

  stage = 1;

  return VLAD_OK;
}

/* after this is called, no further calls to add_inittab() is allowed */
int kb::close_symtab()
{
  if (stage != 1)
    return VLAD_INVALIDOP;

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

  /* total +ve atoms */
  pos_tot = h_tot + m_tot + s_tot;

  stage = 2;

  return VLAD_OK;
}

/* checks whether n of type t is in symtab */
int kb::check_symtab(const char *n, unsigned char t)
{
  if (stage < 2)
    return VLAD_INVALIDOP;

  return stable->find(n, t);
}

/* after this is called, no further calls to add_inittab(), add_consttab()
 * or add_transtab() is allowed */
int kb::close_kb()
{
  if (stage != 2)
    return VLAD_INVALIDOP;

  stage = 3;

  return VLAD_OK;
}

/* gives the length of the symtab */
int kb::length_symtab(unsigned int *l)
{
  if (stage < 2)
    return VLAD_INVALIDOP;

  if (l == NULL)
    return VLAD_NULLPTR;

  *l = stable->length();

  return VLAD_OK;
}

/* gets the i'th identifier from symtab */
int kb::get_symtab(unsigned int i, char **s)
{
  if (stage < 2)
    return VLAD_INVALIDOP;

  if (s == NULL)
    return VLAD_NULLPTR;

  return stable->get(i, s);
}

/* register an identifier in the kb */
int kb::add_symtab(const char *n, unsigned char t)
{
  if (stage != 1)
    return VLAD_INVALIDOP;

  return stable->add(n, t);
}

/* add an atom into the initial state table */
int kb::add_inittab(atom *a)
{
  int retval;
  atom *tmp;

  if (stage != 2)
    return VLAD_INVALIDOP;

  if (a == NULL)
    return VLAD_NULLPTR;

  /* first check if the atom is valid */
  if ((retval = verify_atom(a, NULL)) != VLAD_OK)
    return retval;

  /* now copy and add to inittab */
  if ((retval = a->copy(&tmp)) != VLAD_OK)
    return retval;

  return itable->add(tmp);
}

/* add an expression into the constraints table */
int kb::add_consttab(expression *e, expression *c, expression *n)
{
  int retval;
  unsigned int i;
  expression *exp = NULL;
  expression *cond = NULL;
  expression *ncond = NULL;
  atom *tmp1;
  atom *tmp2;

  if (stage != 2)
    return VLAD_INVALIDOP;

  /* only e is required to be non-null */
  if (e == NULL)
    return VLAD_NULLPTR;

  /*
   * now, we must go through every atom of every exression to ensure
   * their validity. while we are going through them, we might as well
   * make a copy.
   */

  /* exression */
  if ((exp = VLAD_NEW(expression())) == NULL)
    return VLAD_MALLOCFAILED;

  for (i = 0; i < VLAD_LIST_LENGTH(e); i++) {
    if ((retval = e->get(i, &tmp1)) != VLAD_OK)
      return retval;
    if ((retval = verify_atom(tmp1, NULL)) != VLAD_OK)
      return retval;
    if ((retval = tmp1->copy(&tmp2)) != VLAD_OK)
      return retval;
    if ((retval = exp->add(tmp2)) != VLAD_OK)
      return retval;
  }

  /* condition */
  if (c != NULL) {
    if ((cond = VLAD_NEW(expression())) == NULL)
      return VLAD_MALLOCFAILED;

    for (i = 0; i < VLAD_LIST_LENGTH(c); i++) {
      if ((retval = c->get(i, &tmp1)) != VLAD_OK)
        return retval;
      if ((retval = verify_atom(tmp1, NULL)) != VLAD_OK)
        return retval;
      if ((retval = tmp1->copy(&tmp2)) != VLAD_OK)
        return retval;
      if ((retval = cond->add(tmp2)) != VLAD_OK)
        return retval;
    }
  }

  /* negative condition */
  if (n != NULL) {
    if ((ncond = VLAD_NEW(expression())) == NULL)
      return VLAD_MALLOCFAILED;

    for (i = 0; i < VLAD_LIST_LENGTH(n); i++) {
      if ((retval = n->get(i, &tmp1)) != VLAD_OK)
        return retval;
      if ((retval = verify_atom(tmp1, NULL)) != VLAD_OK)
        return retval;
      if ((retval = tmp1->copy(&tmp2)) != VLAD_OK)
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
  stringlist *vlist = NULL;
  expression *precond = NULL;
  expression *postcond = NULL;

  /* we only allow this function after symtab is closed */
  if (stage != 2)
    return VLAD_INVALIDOP;

  /* precondition and vlist are allowed to be NULL */
  if (n == NULL || po == NULL)
    return VLAD_NULLPTR;

  /* copy name */
  if ((name = VLAD_STRING_MALLOC(n)) == NULL)
    return VLAD_MALLOCFAILED;

  strcpy(name, n);

  /* verify and copy varlist */
  if (v != NULL) {
    if ((vlist = VLAD_NEW(stringlist())) == NULL)
      return VLAD_MALLOCFAILED;

    for (i = 0; i < VLAD_LIST_LENGTH(v); i++) {
      char *tmp;
      if ((retval = v->get(i, &tmp)) != VLAD_OK)
        return retval;
      /* check if the variable is already used as an identifier */
      if ((retval = stable->find(tmp)) != VLAD_NOTFOUND)
        return (retval == VLAD_OK) ? VLAD_DUPLICATE : retval;
      if ((retval = vlist->add(tmp)) != VLAD_OK)
        return retval;
    }
  }

  /* verify and copy precondition */
  if (pr != NULL) {
    if ((precond = VLAD_NEW(expression())) == NULL)
      return VLAD_MALLOCFAILED;

    for (i = 0; i < VLAD_LIST_LENGTH(pr); i++) {
      atom *tmp1;
      atom *tmp2;
      if ((retval = pr->get(i, &tmp1)) != VLAD_OK)
        return retval;
      if ((retval = verify_atom(tmp1, v)) != VLAD_OK)
        return retval;
      if ((retval = tmp1->copy(&tmp2)) != VLAD_OK)
        return retval;
      if ((retval = precond->add(tmp2)) != VLAD_OK)
        return retval;
    }
  }

  /* verify and copy the postcondition */
  if ((postcond = VLAD_NEW(expression())) == NULL)
    return VLAD_MALLOCFAILED;

  for (i = 0; i < VLAD_LIST_LENGTH(po); i++) {
    atom *tmp1;
    atom *tmp2;
    if ((retval = po->get(i, &tmp1)) != VLAD_OK)
      return retval;
    if ((retval = verify_atom(tmp1, v)) != VLAD_OK)
      return retval;
    if ((retval = tmp1->copy(&tmp2)) != VLAD_OK)
      return retval;
    if ((retval = postcond->add(tmp2)) != VLAD_OK)
      return retval;
  }

  /* if all went well, add to the trans table */
  return ttable->add(name, vlist, precond, postcond);
}

/* gives the length of the transformation table */
int kb::length_transtab(unsigned int *l)
{
  if (l == NULL)
    return VLAD_NULLPTR;

  if (stage < 2)
    return VLAD_INVALIDOP;

  *l = ttable->length();

  return VLAD_OK;
}

/* gives the i'th entry in the transformation table */
int kb::get_transtab(unsigned int i,
                     char **n,
                     stringlist **v,
                     expression **pr,
                     expression **po)
{
  if (stage < 2)
    return VLAD_INVALIDOP;

  return ttable->get(i, n, v, pr, po);
}

/* add a transformation reference to the sequence table */
int kb::add_seqtab(transref *t)
{
  int retval;
  char *tmp_name;
  stringlist *tmp_ilist;

  /* we only allow this function after kb is closed */
  if (stage < 3)
    return VLAD_INVALIDOP;

  if (t == NULL)
    return VLAD_NULLPTR;

  /* get components */
  if ((retval = t->get(&tmp_name, &tmp_ilist)) != VLAD_OK)
    return retval;

  /* now verify the sequence */
  if ((retval = verify_transref(tmp_name, tmp_ilist)) != VLAD_OK)
    return retval;

  /* if all is well, add */
  if ((retval = setable->add(t)) != VLAD_OK)
    return retval;

  /* set back to stage 3 to prevent query before compute */
  stage = 3;

  return VLAD_OK;
}

/* delete a transformation reference from the sequence table */
int kb::del_seqtab(unsigned int i)
{
  int retval;

  /* we only allow this function after kb is closed */
  if (stage < 3)
    return VLAD_INVALIDOP;

  if ((retval = setable->del(i)) != VLAD_OK)
    return retval;

  /* set back to stage 3 to prevent query before compute */
  stage = 3;

  return retval;
}

/* enumerate the sequences in the sequence table, output to f */
int kb::list_seqtab(FILE *f)
{
  int retval;
  unsigned int i;
  unsigned int j;
  char *tmp_name;
  stringlist *tmp_ilist;

  /* we only allow this function after kb is closed */
  if (stage < 3)
    return VLAD_INVALIDOP;

  if (f == NULL)
    return VLAD_NULLPTR;

  for (i = 0; i < VLAD_LIST_LENGTH(setable); i++) {
    /* get the trans ref */
    if ((retval = setable->get(i, &tmp_name, &tmp_ilist)) != VLAD_OK)
      return retval;
    /* now print */
    fprintf(f, "%d %s(", i, tmp_name);
    /* also do the stringlist */
    for (j = 0; j < VLAD_LIST_LENGTH(tmp_ilist); j++) {
      char *tmp_arg;
      if ((retval = tmp_ilist->get(j, &tmp_arg)) != VLAD_OK) {
        fprintf(f, ")\n");
        return retval;
      }
      fprintf(f, "%s%s", ((j == 0) ? "" : ","), tmp_arg);
    }
    fprintf(f, ")\n");
  }

  return VLAD_OK;
}

/* gives the length of the sequence table */
int kb::length_seqtab(unsigned int *l)
{
  if (l == NULL)
    return VLAD_NULLPTR;

  if (stage < 2)
    return VLAD_INVALIDOP;

  *l = setable->length();

  return VLAD_OK;
}

/* gives the i'th entry in the sequence table */
int kb::get_seqtab(unsigned int i, char **n, stringlist **il)
{
  if (stage < 2)
    return VLAD_INVALIDOP;

  return setable->get(i, n, il);
}

/* generate the query */
int kb::query_generate(expression *e, FILE *f)
{
  int retval;
  unsigned int i;

  if (stage != 5)
    return VLAD_INVALIDOP;

  /* make sure the filestream is not NULL */
  if (f == NULL)
    return VLAD_NULLPTR;

  /* verify expression */
  if ((retval = verify_expression(e)) != VLAD_OK)
    return retval;

  /* and now for the queries */
  fprintf(f, "Queries\n");

  for (i = 0; i < VLAD_LIST_LENGTH(e); i++) {
    atom *tmp_atom;
    unsigned int tmp_num;

    if ((retval = e->get(i, &tmp_atom)) != VLAD_OK)
      return retval;
    if ((retval = encode_atom(tmp_atom, VLAD_LIST_LENGTH(setable), &tmp_num)) != VLAD_OK)
      return retval;

    if (i == 0)
      fprintf(f, "  ");

    if (i + 1 == VLAD_LIST_LENGTH(e))
      fprintf(f, "%d %s\n", tmp_num, VLAD_STR_QUERY);
    else
      fprintf(f, "%d %s ", tmp_num, VLAD_STR_AND);
  }

  return VLAD_OK;
}

/* generate the rules necessary to evaluate queries */
int kb::compute_generate(FILE *f)
{
  int retval;
  unsigned int i;

  /* we only allow this function after kb is closed */
  if (stage != 3 && stage != 5)
    return VLAD_INVALIDOP;

  /* make sure the filestream is not NULL */
  if (f == NULL)
    return VLAD_NULLPTR;

  /* first we print out all the possible atoms in the kb */
  fprintf(f, "Atoms\n");

  for (i = 0; i < (pos_tot * 2 * (VLAD_LIST_LENGTH(setable) + 1)); i++) {
    atom *tmp_atom;
    unsigned char tmp_ty;
    unsigned int tmp_s;
    bool tmp_tr;
    char *tmp_param1;
    char *tmp_param2;
    char *tmp_param3;

    if ((retval = decode_atom(&tmp_atom, &tmp_s, i)) != VLAD_OK)
      return retval;

    if ((retval = tmp_atom->get(&tmp_param1, &tmp_param2, &tmp_param3, &tmp_ty, &tmp_tr)) != VLAD_OK)
      return retval;

    switch(tmp_ty) {
      case VLAD_ATOM_HOLDS :
        fprintf(f,
                "  %d = %s(S%d, %c, %s, %s, %s, %s)\n",
                i,
                VLAD_STR_HOLDS,
                tmp_s,
                tmp_tr ? 'T' : 'F',
                VLAD_STR_HOLDS,
                tmp_param1,
                tmp_param2,
                tmp_param3);
        break;
      case VLAD_ATOM_MEMBER :
          fprintf(f,
                  "  %d = %s(S%d, %c, %s, %s, %s)\n",
                  i, VLAD_STR_HOLDS,
                  tmp_s,
                  tmp_tr ? 'T' : 'F',
                  VLAD_STR_MEMBER,
                  tmp_param1,
                  tmp_param2);
        break;
      case VLAD_ATOM_SUBSET :
          fprintf(f,
                  "  %d = %s(S%d, %c, %s, %s, %s)\n",
                  i,
                  VLAD_STR_HOLDS,
                  tmp_s,
                  tmp_tr ? 'T' : 'F',
                  VLAD_STR_SUBSET,
                  tmp_param1,
                  tmp_param2);
        break;
    }

    delete tmp_atom;
  }

  /* identity rules */
  fprintf(f, "Identity  Rules\n");

  /* state loop */
  for (i = 0; i <= VLAD_LIST_LENGTH(setable); i++) {
    unsigned int i_grp;
    /* subject groups */
    for (i_grp = 0; i_grp < sg_len; i_grp++)
      fprintf(f,
              "  %d %s %s\n",
              compute_subset(i, true, VLAD_IDENT_SUBJECT, i_grp, i_grp),
              VLAD_STR_ARROW,
              VLAD_STR_TRUE);
    /* access groups */
    for (i_grp = 0; i_grp < ag_len; i_grp++)
      fprintf(f,
              "  %d %s %s\n",
              compute_subset(i, true, VLAD_IDENT_ACCESS, i_grp, i_grp),
              VLAD_STR_ARROW,
              VLAD_STR_TRUE);
    /* object groups */
    for (i_grp = 0; i_grp < og_len; i_grp++)
      fprintf(f,
              "  %d %s %s\n",
              compute_subset(i, true, VLAD_IDENT_OBJECT, i_grp, i_grp),
              VLAD_STR_ARROW,
              VLAD_STR_TRUE);
  }

  /* inheritance rules */
  fprintf(f, "Inheritance Rules\n");

  /* state loop */
  for (i = 0; i <= VLAD_LIST_LENGTH(setable); i++) {
    unsigned int i_grp1;
    unsigned int i_grp2;
    unsigned int i_sub;
    unsigned int i_acc;
    unsigned int i_obj;
    /* subset inheritance */

     /* subject groups */
    for (i_grp1 = 0; i_grp1 < sg_len; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < sg_len; i_grp2++) {
        if (i_grp1 == i_grp2)
          continue;
        for (i_acc = 0; i_acc < a_len + ag_len; i_acc++) {
          for (i_obj = 0; i_obj < o_len + og_len; i_obj++) {
            fprintf(f,
                    "  %d %s %d %s %d %s %s %d\n",
                    compute_holds(i, true, i_grp1 + s_len, i_acc, i_obj),
                    VLAD_STR_ARROW,
                    compute_holds(i, true, i_grp2 + s_len, i_acc, i_obj),
                    VLAD_STR_AND,
                    compute_subset(i, true, VLAD_IDENT_SUBJECT, i_grp1, i_grp2),
                    VLAD_STR_AND,
                    VLAD_STR_NOT,
                    compute_holds(i, false, i_grp1 + s_len, i_acc, i_obj));
            fprintf(f,
                    "  %d %s %d %s %d\n",
                    compute_holds(i, false, i_grp1 + s_len, i_acc, i_obj),
                    VLAD_STR_ARROW,
                    compute_holds(i, false, i_grp2 + s_len, i_acc, i_obj),
                    VLAD_STR_AND,
                    compute_subset(i, true, VLAD_IDENT_SUBJECT, i_grp1, i_grp2));
          }
        }
      }
    }
    /* access groups */
    for (i_grp1 = 0; i_grp1 < ag_len; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < ag_len; i_grp2++) {
        if (i_grp1 == i_grp2)
          continue;
        for (i_sub = 0; i_sub < s_len + sg_len; i_sub++) {
          for (i_obj = 0; i_obj < o_len + og_len; i_obj++) {
            fprintf(f,
                    "  %d %s %d %s %d %s %s %d\n",
                    compute_holds(i, true, i_sub, i_grp1 + a_len, i_obj),
                    VLAD_STR_ARROW,
                    compute_holds(i, true, i_sub, i_grp2 + a_len, i_obj),
                    VLAD_STR_AND,
                    compute_subset(i, true, VLAD_IDENT_ACCESS, i_grp1, i_grp2),
                    VLAD_STR_AND,
                    VLAD_STR_NOT,
                    compute_holds(i, false, i_sub, i_grp1 + a_len, i_obj));
            fprintf(f,
                    "  %d %s %d %s %d\n",
                    compute_holds(i, false, i_sub, i_grp1 + a_len, i_obj),
                    VLAD_STR_ARROW,
                    compute_holds(i, false, i_sub, i_grp2 + a_len, i_obj),
                    VLAD_STR_AND,
                    compute_subset(i, true, VLAD_IDENT_ACCESS, i_grp1, i_grp2));
          }
        }
      }
    }
    /* object groups */
    for (i_grp1 = 0; i_grp1 < og_len; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < og_len; i_grp2++) {
        if (i_grp1 == i_grp2)
          continue;
        for (i_sub = 0; i_sub < s_len + sg_len; i_sub++) {
          for (i_acc = 0; i_acc < a_len + ag_len; i_acc++) {
            fprintf(f,
                    "  %d %s %d %s %d %s %s %d\n",
                    compute_holds(i, true, i_sub, i_acc, i_grp1 + o_len),
                    VLAD_STR_ARROW,
                    compute_holds(i, true, i_sub, i_acc, i_grp2 + o_len),
                    VLAD_STR_AND,
                    compute_subset(i, true, VLAD_IDENT_OBJECT, i_grp1, i_grp2),
                    VLAD_STR_AND,
                    VLAD_STR_NOT,
                    compute_holds(i, false, i_sub, i_acc, i_grp1 + o_len));
            fprintf(f,
                    "  %d %s %d %s %d\n",
                    compute_holds(i, false, i_sub, i_acc, i_grp1 + o_len),
                    VLAD_STR_ARROW,
                    compute_holds(i, false, i_sub, i_acc, i_grp2 + o_len),
                    VLAD_STR_AND,
                    compute_subset(i, true, VLAD_IDENT_OBJECT, i_grp1, i_grp2));
          }
        }
      }
    }

    /* member inheritance */

    /* subject groups */
    for (i_grp1 = 0; i_grp1 < sg_len; i_grp1++) {
      for (i_sub = 0; i_sub < s_len; i_sub++) {
        for (i_acc = 0; i_acc < a_len + ag_len; i_acc++) {
          for (i_obj = 0; i_obj < o_len + og_len; i_obj++) {
            fprintf(f,
                    "  %d %s %d %s %d %s %s %d\n",
                    compute_holds(i, true, i_sub, i_acc, i_obj),
                    VLAD_STR_ARROW,
                    compute_holds(i, true, i_grp1 + s_len, i_acc, i_obj),
                    VLAD_STR_AND,
                    compute_member(i, true, VLAD_IDENT_SUBJECT, i_sub, i_grp1),
                    VLAD_STR_AND,
                    VLAD_STR_NOT,
                    compute_holds(i, false, i_sub, i_acc, i_obj));
            fprintf(f,
                    "  %d %s %d %s %d\n",
                    compute_holds(i, false, i_sub, i_acc, i_obj),
                    VLAD_STR_ARROW,
                    compute_holds(i, false, i_grp1 + s_len, i_acc, i_obj),
                    VLAD_STR_AND,
                    compute_member(i, true, VLAD_IDENT_SUBJECT, i_sub, i_grp1));
          }
        }
      }
    }
    /* access groups */
    for (i_grp1 = 0; i_grp1 < ag_len; i_grp1++) {
      for (i_sub = 0; i_sub < s_len + sg_len; i_sub++) {
        for (i_acc = 0; i_acc < a_len; i_acc++) {
          for (i_obj = 0; i_obj < o_len + og_len; i_obj++) {
            fprintf(f,
                    "  %d %s %d %s %d %s %s %d\n",
                    compute_holds(i, true, i_sub, i_acc, i_obj),
                    VLAD_STR_ARROW,
                    compute_holds(i, true, i_sub, i_grp1 + a_len, i_obj),
                    VLAD_STR_AND,
                    compute_member(i, true, VLAD_IDENT_ACCESS, i_acc, i_grp1),
                    VLAD_STR_AND,
                    VLAD_STR_NOT,
                    compute_holds(i, false, i_sub, i_acc, i_obj));
            fprintf(f,
                    "  %d %s %d %s %d\n",
                    compute_holds(i, false, i_sub, i_acc, i_obj),
                    VLAD_STR_ARROW,
                    compute_holds(i, false, i_sub, i_grp1 + a_len, i_obj),
                    VLAD_STR_AND,
                    compute_member(i, true, VLAD_IDENT_ACCESS, i_acc, i_grp1));
          }
        }
      }
    }
    /* object groups */
    for (i_grp1 = 0; i_grp1 < og_len; i_grp1++) {
      for (i_sub = 0; i_sub < s_len + sg_len; i_sub++) {
        for (i_acc = 0; i_acc < a_len + ag_len; i_acc++) {
          for (i_obj = 0; i_obj < o_len; i_obj++) {
            fprintf(f,
                    "  %d %s %d %s %d %s %s %d\n",
                    compute_holds(i, true, i_sub, i_acc, i_obj),
                    VLAD_STR_ARROW,
                    compute_holds(i, true, i_sub, i_acc, i_grp1 + o_len),
                    VLAD_STR_AND,
                    compute_member(i, true, VLAD_IDENT_OBJECT, i_obj, i_grp1),
                    VLAD_STR_AND,
                    VLAD_STR_NOT,
                    compute_holds(i, false, i_sub, i_acc, i_obj));
            fprintf(f,
                    "  %d %s %d %s %d\n",
                    compute_holds(i, false, i_sub, i_acc, i_obj),
                    VLAD_STR_ARROW,
                    compute_holds(i, false, i_sub, i_acc, i_grp1 + o_len),
                    VLAD_STR_AND,
                    compute_member(i, true, VLAD_IDENT_OBJECT, i_obj, i_grp1));
          }
        }
      }
    }
  }

  /* transitivity */
  fprintf(f, "Transitivity Rules\n");

  /* state loop */
  for (i = 0; i <= VLAD_LIST_LENGTH(setable); i++) {
    unsigned int i_grp1;
    unsigned int i_grp2;
    unsigned int i_grp3;
    /* subject groups */
    for (i_grp1 = 0; i_grp1 < sg_len; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < sg_len; i_grp2++) {
        for (i_grp3 = 0; i_grp3 < sg_len; i_grp3++) {
          /* ignore if any 2 are the same */
          if (i_grp1 == i_grp2 || i_grp1 == i_grp3 || i_grp2 == i_grp3)
            continue;

          fprintf(f,
                  "  %d %s %d %s %d\n",
                  compute_subset(i, true, VLAD_IDENT_SUBJECT, i_grp1, i_grp3),
                  VLAD_STR_ARROW,
                  compute_subset(i, true, VLAD_IDENT_SUBJECT, i_grp1, i_grp2),
                  VLAD_STR_AND,
                  compute_subset(i, true, VLAD_IDENT_SUBJECT, i_grp2, i_grp3));
        }
      }
    }
    /* access groups */
    for (i_grp1 = 0; i_grp1 < ag_len; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < ag_len; i_grp2++) {
        for (i_grp3 = 0; i_grp3 < ag_len; i_grp3++) {
          /* ignore if any 2 are the same */
          if (i_grp1 == i_grp2 || i_grp1 == i_grp3 || i_grp2 == i_grp3)
            continue;
          fprintf(f,
                  "  %d %s %d %s %d\n",
                  compute_subset(i, true, VLAD_IDENT_ACCESS, i_grp1, i_grp3),
                  VLAD_STR_ARROW,
                  compute_subset(i, true, VLAD_IDENT_ACCESS, i_grp1, i_grp2),
                  VLAD_STR_AND,
                  compute_subset(i, true, VLAD_IDENT_ACCESS, i_grp2, i_grp3));
        }
      }
    }
    /* object groups */
    for (i_grp1 = 0; i_grp1 < og_len; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < og_len; i_grp2++) {
        for (i_grp3 = 0; i_grp3 < og_len; i_grp3++) {
          /* ignore if any 2 are the same */
          if (i_grp1 == i_grp2 || i_grp1 == i_grp3 || i_grp2 == i_grp3)
            continue;
          fprintf(f,
                  "  %d %s %d %s %d\n",
                  compute_subset(i, true, VLAD_IDENT_OBJECT, i_grp1, i_grp3),
                  VLAD_STR_ARROW,
                  compute_subset(i, true, VLAD_IDENT_OBJECT, i_grp1, i_grp2),
                  VLAD_STR_AND,
                  compute_subset(i, true, VLAD_IDENT_OBJECT, i_grp2, i_grp3));
        }
      }
    }
  }

  /* complementary rules */
  fprintf(f, "Complementary Rules\n");

  /* state loop */
  for (i = 0; i <= VLAD_LIST_LENGTH(setable); i++) {
    unsigned int i_atom;
    for (i_atom = 0; i_atom < pos_tot; i_atom++) {
      fprintf(f,
              "  %s %s %d %s %d\n",
              VLAD_STR_FALSE,
              VLAD_STR_ARROW,
              compute_atom(i, true, i_atom),
              VLAD_STR_AND,
              compute_atom(i, false, i_atom));
    }
  }

  /* inertial rules */
  fprintf(f, "Inertial Rules\n");

  /* state loop */
  for (i = 0; i < VLAD_LIST_LENGTH(setable); i++) {
    unsigned int i_atom;
    for (i_atom = 0; i_atom < pos_tot; i_atom++) {
      fprintf(f,
              "  %d %s %d %s %s %d\n",
              compute_atom(i + 1, true, i_atom),
              VLAD_STR_ARROW,
              compute_atom(i, true, i_atom),
              VLAD_STR_AND,
              VLAD_STR_NOT,
              compute_atom(i + 1, false, i_atom));
      fprintf(f,
              "  %d %s %d %s %s %d\n",
              compute_atom(i + 1, false, i_atom),
              VLAD_STR_ARROW,
              compute_atom(i, false, i_atom),
              VLAD_STR_AND,
              VLAD_STR_NOT,
              compute_atom(i + 1, true, i_atom));
    }
  }

  /* initial state */
  fprintf(f, "Initial State\n");

  for (i = 0; i < VLAD_LIST_LENGTH(itable); i++) {
    atom *tmp_atom;
    unsigned int tmp_num;

    if ((retval = itable->get(i, &tmp_atom)) != VLAD_OK)
      return retval;
    if ((retval = encode_atom(tmp_atom, 0, &tmp_num)) != VLAD_OK)
      return retval;

    fprintf(f, "  %d %s %s\n", tmp_num, VLAD_STR_ARROW, VLAD_STR_TRUE);
  }

  /* constraints */
  fprintf(f, "Constraints\n");

  for (i = 0; i <= VLAD_LIST_LENGTH(setable); i++) {
    unsigned int  i_const;
    unsigned int i_exp;

    /* constraint loop */
    for (i_const = 0; i_const < VLAD_LIST_LENGTH(ctable); i_const++) {
      expression *tmp_e;
      expression *tmp_c;
      expression *tmp_n;
      atom *tmp_atom;
      unsigned int tmp_num;

      if ((retval = ctable->get(i_const, &tmp_e, &tmp_c, &tmp_n)) != VLAD_OK)
        return retval;

      fprintf(f, " ");

      /* constaint expression */
      for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_e); i_exp++) {
        if ((retval = tmp_e->get(i_exp, &tmp_atom)) != VLAD_OK)
          return retval;
        if ((retval = encode_atom(tmp_atom, i, &tmp_num)) != VLAD_OK)
          return retval;
        fprintf(f, " %d", tmp_num);
      }

      fprintf(f, " %s", VLAD_STR_ARROW);

      /* constraint condition */
      for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_c); i_exp++) {
        if ((retval = tmp_c->get(i_exp, &tmp_atom)) != VLAD_OK)
          return retval;
        if ((retval = encode_atom(tmp_atom, i, &tmp_num)) != VLAD_OK)
          return retval;
        fprintf(f, " %d", tmp_num);
      }

      /* constraint negative condition */
      for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_n); i_exp++) {
        if ((retval = tmp_n->get(i_exp, &tmp_atom)) != VLAD_OK)
          return retval;
        if ((retval = encode_atom(tmp_atom, i, &tmp_num)) != VLAD_OK)
          return retval;
        fprintf(f, " %s %d", VLAD_STR_NOT, tmp_num);
      }

      if (tmp_c == NULL && tmp_n == NULL)
        fprintf(f, " %s\n", VLAD_STR_TRUE);
      else
        fprintf(f, "\n");
    }
  }

  /* transformation rules */
  fprintf(f, "Transformation Rules\n");

  /* state loop */
  for (i = 0; i < VLAD_LIST_LENGTH(setable); i++) {
    unsigned int i_exp;
    char *tmp_name;
    atom *tmp_atom;
    unsigned int tmp_num;
    stringlist *tmp_ilist = NULL;
    expression *tmp_pre = NULL;
    expression *tmp_post = NULL;

    if ((retval = setable->get(i, &tmp_name, &tmp_ilist)) != VLAD_OK)
      return retval;

    if ((retval = ttable->replace(tmp_name, tmp_ilist, &tmp_pre, &tmp_post)) != VLAD_OK)
      return retval;

    fprintf(f, " ");

    /* postcondition loop */
    for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_post); i_exp++) {
      if ((retval = tmp_post->get(i_exp, &tmp_atom)) != VLAD_OK)
        return retval;
      if ((retval = encode_atom(tmp_atom, i + 1, &tmp_num)) != VLAD_OK)
        return retval;
      fprintf(f, " %d", tmp_num);
    }

    fprintf(f, " %s", VLAD_STR_ARROW);

    /* precondition loop */
    for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_pre); i_exp++) {
      if ((retval = tmp_pre->get(i_exp, &tmp_atom)) != VLAD_OK)
        return retval;
      if ((retval = encode_atom(tmp_atom, i, &tmp_num)) != VLAD_OK)
        return retval;
      fprintf(f, " %d", tmp_num);
    }

    if (tmp_pre == NULL)
      fprintf(f, " %s\n", VLAD_STR_TRUE);
    else
      fprintf(f, "\n");
  }

  stage = 5;

  return VLAD_OK;
}

#ifdef VLAD_SMODELS
/* prepares the kb for queries */
int kb::compute_evaluate()
{
  int retval;
  unsigned int i;

  /* we only allow this after kb is closed */
  if (stage != 3 && stage != 4)
    return VLAD_INVALIDOP;

  /* create a new instance of the smodels smwrap and init it */
  if (smobject != NULL)
    delete smobject;

  if ((smobject = VLAD_NEW(smwrap())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = smobject->init()) != VLAD_OK)
    return retval;

  /* first we register all the possible atoms in smodels */
  for (i = 0; i < (pos_tot * 2 * (VLAD_LIST_LENGTH(setable) + 1)); i++) {
    if ((retval = smobject->add_atom(i)) != VLAD_OK)
      return retval;
  }

  smobject->close_atom();

  /* now *sigh* we register all the built-in rules */

  /* identity rules */

  /* state loop */
  for (i = 0; i <= VLAD_LIST_LENGTH(setable); i++) {
    unsigned int i_grp;
    /* subject groups */
    for (i_grp = 0; i_grp < sg_len; i_grp++) {
      unsigned int tmp_num = compute_subset(i, true, VLAD_IDENT_SUBJECT, i_grp, i_grp);
      if ((retval = smobject->add_axiom(true, 1, tmp_num)) != VLAD_OK)
        return retval;
    }
    /* access groups */
    for (i_grp = 0; i_grp < ag_len; i_grp++) {
      unsigned int tmp_num = compute_subset(i, true, VLAD_IDENT_ACCESS, i_grp, i_grp);
      if ((retval = smobject->add_axiom(true, 1, tmp_num)) != VLAD_OK)
        return retval;
    }
    /* object groups */
    for (i_grp = 0; i_grp < og_len; i_grp++) {
      unsigned int tmp_num = compute_subset(i, true, VLAD_IDENT_OBJECT, i_grp, i_grp);
      if ((retval = smobject->add_axiom(true, 1, tmp_num)) != VLAD_OK)
        return retval;
    }
  }

  /* inheritance rules */

  /* state loop */
  for (i = 0; i <= VLAD_LIST_LENGTH(setable); i++) {
    unsigned int i_grp1;
    unsigned int i_grp2;
    unsigned int i_sub;
    unsigned int i_acc;
    unsigned int i_obj;

    /* subset inheritance */

    /* subject groups */
    for (i_grp1 = 0; i_grp1 < sg_len; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < sg_len; i_grp2++) {
        if (i_grp1 == i_grp2)
          continue;
        for (i_acc = 0; i_acc < a_len + ag_len; i_acc++) {
          for (i_obj = 0; i_obj < o_len + og_len; i_obj++) {
            /* positive */
            if ((retval = smobject->add_rule(2,
                                             1,
                                             compute_holds(i, true, i_grp1 + s_len, i_acc, i_obj),
                                             compute_holds(i, true, i_grp2 + s_len, i_acc, i_obj),
                                             compute_subset(i, true, VLAD_IDENT_SUBJECT, i_grp1, i_grp2),
                                             compute_holds(i, false, i_grp1 + s_len, i_acc, i_obj))) != VLAD_OK)
              return retval;
            /* negative */
            if ((retval = smobject->add_rule(2,
                                             0,
                                             compute_holds(i, false, i_grp1 + s_len, i_acc, i_obj),
                                             compute_holds(i, false, i_grp2 + s_len, i_acc, i_obj),
                                             compute_subset(i, true, VLAD_IDENT_SUBJECT, i_grp1, i_grp2))) != VLAD_OK)
              return retval;
          }
        }
      }
    }
    /* access groups */
    for (i_grp1 = 0; i_grp1 < ag_len; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < ag_len; i_grp2++) {
        if (i_grp1 == i_grp2)
          continue;
        for (i_sub = 0; i_sub < s_len + sg_len; i_sub++) {
          for (i_obj = 0; i_obj < o_len + og_len; i_obj++) {
            /* positive */
            if ((retval = smobject->add_rule(2,
                                             1,
                                             compute_holds(i, true, i_sub, i_grp1 + a_len, i_obj),
                                             compute_holds(i, true, i_sub, i_grp2 + a_len, i_obj),
                                             compute_subset(i, true, VLAD_IDENT_ACCESS, i_grp1, i_grp2),
                                             compute_holds(i, false, i_sub, i_grp1 + a_len, i_obj))) != VLAD_OK)
              return retval;
            /* negative */
            if ((retval = smobject->add_rule(2,
                                             0,
                                             compute_holds(i, false, i_sub, i_grp1 + a_len, i_obj),
                                             compute_holds(i, false, i_sub, i_grp2 + a_len, i_obj),
                                             compute_subset(i, true, VLAD_IDENT_ACCESS, i_grp1, i_grp2))) != VLAD_OK)
              return retval;
          }
        }
      }
    }
    /* object groups */
    for (i_grp1 = 0; i_grp1 < og_len; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < og_len; i_grp2++) {
        if (i_grp1 == i_grp2)
          continue;
        for (i_sub = 0; i_sub < s_len + sg_len; i_sub++) {
          for (i_acc = 0; i_acc < a_len + ag_len; i_acc++) {
            /* positive */
            if ((retval = smobject->add_rule(2,
                                             1,
                                             compute_holds(i, true, i_sub, i_acc, i_grp1 + o_len),
                                             compute_holds(i, true, i_sub, i_acc, i_grp2 + o_len),
                                             compute_subset(i, true, VLAD_IDENT_OBJECT, i_grp1, i_grp2),
                                             compute_holds(i, false, i_sub, i_acc, i_grp1 + o_len))) != VLAD_OK)
              return retval;
            /* negative */
            if ((retval = smobject->add_rule(2,
                                             0,
                                             compute_holds(i, false, i_sub, i_acc, i_grp1 + o_len),
                                             compute_holds(i, false, i_sub, i_acc, i_grp2 + o_len),
                                             compute_subset(i, true, VLAD_IDENT_OBJECT, i_grp1, i_grp2))) != VLAD_OK)
              return retval;
          }
        }
      }
    }

    /* member inheritance */

    /* subject groups */
    for (i_grp1 = 0; i_grp1 < sg_len; i_grp1++) {
      for (i_sub = 0; i_sub < s_len; i_sub++) {
        for (i_acc = 0; i_acc < a_len + ag_len; i_acc++) {
          for (i_obj = 0; i_obj < o_len + og_len; i_obj++) {
            /* positive */
            if ((retval = smobject->add_rule(2,
                                             1,
                                             compute_holds(i, true, i_sub, i_acc, i_obj),
                                             compute_holds(i, true, i_grp1 + s_len, i_acc, i_obj),
                                             compute_member(i, true, VLAD_IDENT_SUBJECT, i_sub, i_grp1),
                                             compute_holds(i, false, i_sub, i_acc, i_obj))) != VLAD_OK)
              return retval;
            /* negative */
            if ((retval = smobject->add_rule(2,
                                             0,
                                             compute_holds(i, false, i_sub, i_acc, i_obj),
                                             compute_holds(i, false, i_grp1 + s_len, i_acc, i_obj),
                                             compute_member(i, true, VLAD_IDENT_SUBJECT, i_sub, i_grp1))) != VLAD_OK)
              return retval;
          }
        }
      }
    }
    /* access groups */
    for (i_grp1 = 0; i_grp1 < ag_len; i_grp1++) {
      for (i_sub = 0; i_sub < s_len + sg_len; i_sub++) {
        for (i_acc = 0; i_acc < a_len; i_acc++) {
          for (i_obj = 0; i_obj < o_len + og_len; i_obj++) {
            /* positive */
            if ((retval = smobject->add_rule(2,
                                             1,
                                             compute_holds(i, true, i_sub, i_acc, i_obj),
                                             compute_holds(i, true, i_sub, i_grp1 + a_len, i_obj),
                                             compute_member(i, true, VLAD_IDENT_ACCESS, i_acc, i_grp1),
                                             compute_holds(i, false, i_sub, i_acc, i_obj))) != VLAD_OK)
              return retval;
            /* negative */
            if ((retval = smobject->add_rule(2,
                                             0,
                                             compute_holds(i, false, i_sub, i_acc, i_obj),
                                             compute_holds(i, false, i_sub, i_grp1 + a_len, i_obj),
                                             compute_member(i, true, VLAD_IDENT_ACCESS, i_acc, i_grp1))) != VLAD_OK)
              return retval;
          }
        }
      }
    }
    /* object groups */
    for (i_grp1 = 0; i_grp1 < og_len; i_grp1++) {
      for (i_sub = 0; i_sub < s_len + sg_len; i_sub++) {
        for (i_acc = 0; i_acc < a_len + ag_len; i_acc++) {
          for (i_obj = 0; i_obj < o_len; i_obj++) {
            /* positive */
            if ((retval = smobject->add_rule(2,
                                             1,
                                             compute_holds(i, true, i_sub, i_acc, i_obj),
                                             compute_holds(i, true, i_sub, i_acc, i_grp1 + o_len),
                                             compute_member(i, true, VLAD_IDENT_OBJECT, i_obj, i_grp1),
                                             compute_holds(i, false, i_sub, i_acc, i_obj))) != VLAD_OK)
              return retval;
            /* negative */
            if ((retval = smobject->add_rule(2,
                                             0,
                                             compute_holds(i, false, i_sub, i_acc, i_obj),
                                             compute_holds(i, false, i_sub, i_acc, i_grp1 + o_len),
                                             compute_member(i, true, VLAD_IDENT_OBJECT, i_obj, i_grp1))) != VLAD_OK)
              return retval;
          }
        }
      }
    }
  }

  /* transitivity */

  /* state loop */
  for (i = 0; i <= VLAD_LIST_LENGTH(setable); i++) {
    unsigned int i_grp1;
    unsigned int i_grp2;
    unsigned int i_grp3;
    /* subject groups */
    for (i_grp1 = 0; i_grp1 < sg_len; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < sg_len; i_grp2++) {
        /* ignore if the 2 are the same */
        if (i_grp1 == i_grp2)
          continue;
        for (i_grp3 = 0; i_grp3 < sg_len; i_grp3++) {
          unsigned int tmp_num1;
          unsigned int tmp_num2;
          unsigned int tmp_num3;

          /* ignore if any 2 are the same */
          if (i_grp1 == i_grp3 || i_grp2 == i_grp3)
            continue;

          tmp_num1 = compute_subset(i, true, VLAD_IDENT_SUBJECT, i_grp1, i_grp3);
          tmp_num2 = compute_subset(i, true, VLAD_IDENT_SUBJECT, i_grp1, i_grp2);
          tmp_num3 = compute_subset(i, true, VLAD_IDENT_SUBJECT, i_grp2, i_grp3);

          if ((retval = smobject->add_rule(2, 0, tmp_num1, tmp_num2, tmp_num3)) != VLAD_OK)
            return retval;
        }
      }
    }
    /* access groups */
    for (i_grp1 = 0; i_grp1 < ag_len; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < ag_len; i_grp2++) {
        /* ignore if the 2 are the same */
        if (i_grp1 == i_grp2)
          continue;
        for (i_grp3 = 0; i_grp3 < ag_len; i_grp3++) {
          unsigned int tmp_num1;
          unsigned int tmp_num2;
          unsigned int tmp_num3;

          /* ignore if any 2 are the same */
          if (i_grp1 == i_grp3 || i_grp2 == i_grp3)
            continue;

          tmp_num1 = compute_subset(i, true, VLAD_IDENT_ACCESS, i_grp1, i_grp3);
          tmp_num2 = compute_subset(i, true, VLAD_IDENT_ACCESS, i_grp1, i_grp2);
          tmp_num3 = compute_subset(i, true, VLAD_IDENT_ACCESS, i_grp2, i_grp3);

          if ((retval = smobject->add_rule(2, 0, tmp_num1, tmp_num2, tmp_num3)) != VLAD_OK)
            return retval;
        }
      }
    }
    /* object groups */
    for (i_grp1 = 0; i_grp1 < og_len; i_grp1++) {
      for (i_grp2 = 0; i_grp2 < og_len; i_grp2++) {
        /* ignore if the 2 are the same */
        if (i_grp1 == i_grp2)
          continue;
        for (i_grp3 = 0; i_grp3 < og_len; i_grp3++) {
          unsigned int tmp_num1;
          unsigned int tmp_num2;
          unsigned int tmp_num3;

          /* ignore if any 2 are the same */
          if (i_grp1 == i_grp3 || i_grp2 == i_grp3)
            continue;

          tmp_num1 = compute_subset(i, true, VLAD_IDENT_OBJECT, i_grp1, i_grp3);
          tmp_num2 = compute_subset(i, true, VLAD_IDENT_OBJECT, i_grp1, i_grp2);
          tmp_num3 = compute_subset(i, true, VLAD_IDENT_OBJECT, i_grp2, i_grp3);

          if ((retval = smobject->add_rule(2, 0, tmp_num1, tmp_num2, tmp_num3)) != VLAD_OK)
            return retval;
        }
      }
    }
  }

  /* complementary rules */

  /* state loop */
  for (i = 0; i <= VLAD_LIST_LENGTH(setable); i++) {
    unsigned int i_atom;
    for (i_atom = 0; i_atom < pos_tot; i_atom++) {
      unsigned int tmp_num1 = compute_atom(i, true, i_atom);
      unsigned int tmp_num2 = compute_atom(i, false, i_atom);
      if ((retval = smobject->add_axiom(false, 2, tmp_num1, tmp_num2)) != VLAD_OK)
        return retval;
    }
  }

  /* inertial rules */

  /* state loop */
  for (i = 0; i < VLAD_LIST_LENGTH(setable); i++) {
    unsigned int i_atom;
    for (i_atom = 0; i_atom < pos_tot; i_atom++) {
      unsigned int tmp_num1;
      unsigned int tmp_num2;
      unsigned int tmp_num3;

      tmp_num1 = compute_atom(i + 1, true, i_atom);
      tmp_num2 = compute_atom(i, true, i_atom);
      tmp_num3 = compute_atom(i + 1, false, i_atom);

      if ((retval = smobject->add_rule(1, 1, tmp_num1, tmp_num2, tmp_num3)) != VLAD_OK)
        return retval;

      tmp_num1 = compute_atom(i + 1, false, i_atom);
      tmp_num2 = compute_atom(i, false, i_atom);
      tmp_num3 = compute_atom(i + 1, true, i_atom);

      if ((retval = smobject->add_rule(1, 1, tmp_num1, tmp_num2, tmp_num3)) != VLAD_OK)
        return retval;
    }
  }

  /* initial state */

  for (i = 0; i < VLAD_LIST_LENGTH(itable); i++) {
    atom *tmp_atom;
    unsigned int tmp_num;

    if ((retval = itable->get(i, &tmp_atom)) != VLAD_OK)
      return retval;
    if ((retval = encode_atom(tmp_atom, 0, &tmp_num)) != VLAD_OK)
      return retval;
    if ((retval = smobject->add_axiom(true, 1, tmp_num)) != VLAD_OK)
      return retval;
  }

  /* constraints */

  for (i = 0; i <= VLAD_LIST_LENGTH(setable); i++) {
    unsigned int  i_const;
    unsigned int i_exp;

    /* constraint loop */
    for (i_const = 0; i_const < VLAD_LIST_LENGTH(ctable); i_const++) {
      expression *tmp_e;
      expression *tmp_c;
      expression *tmp_n;
      atom *tmp_atom;
      unsigned int tmp_num;
      numberlist *tmp_list1;
      numberlist *tmp_list2;

      if ((tmp_list1 = VLAD_NEW(numberlist())) == NULL)
        return VLAD_MALLOCFAILED;
      if ((tmp_list2 = VLAD_NEW(numberlist())) == NULL)
        return VLAD_MALLOCFAILED;

      if ((retval = ctable->get(i_const, &tmp_e, &tmp_c, &tmp_n)) != VLAD_OK)
        return retval;

      /* constraint condition */
      for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_c); i_exp++) {
        if ((retval = tmp_c->get(i_exp, &tmp_atom)) != VLAD_OK)
          return retval;
        if ((retval = encode_atom(tmp_atom, i, &tmp_num)) != VLAD_OK)
          return retval;
        if ((retval = tmp_list1->add(tmp_num)) != VLAD_OK)
          return retval;
      }

      /* constraint negative condition */
      for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_n); i_exp++) {
        if ((retval = tmp_n->get(i_exp, &tmp_atom)) != VLAD_OK)
          return retval;
        if ((retval = encode_atom(tmp_atom, i, &tmp_num)) != VLAD_OK)
          return retval;
        if ((retval = tmp_list2->add(tmp_num)) != VLAD_OK)
          return retval;
      }

      /* constaint expression */
      for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_e); i_exp++) {
        if ((retval = tmp_e->get(i_exp, &tmp_atom)) != VLAD_OK)
          return retval;
        if ((retval = encode_atom(tmp_atom, i, &tmp_num)) != VLAD_OK)
          return retval;
	/* for every atom in the exression, we add a separate rule */
        if ((retval = smobject->add_rule(tmp_num, tmp_list1, tmp_list2)) != VLAD_OK)
          return retval;
      }

      delete tmp_list1;
      delete tmp_list2;
    }
  }

  /* transformation rules */

  /* state loop */
  for (i = 0; i < VLAD_LIST_LENGTH(setable); i++) {
    unsigned int i_exp;
    char *tmp_name;
    atom *tmp_atom;
    unsigned int tmp_num;
    stringlist *tmp_ilist = NULL;
    expression *tmp_pre = NULL;
    expression *tmp_post = NULL;
    numberlist *tmp_list;

   if ((tmp_list = VLAD_NEW(numberlist())) == NULL)
     return VLAD_MALLOCFAILED;

    if ((retval = setable->get(i, &tmp_name, &tmp_ilist)) != VLAD_OK)
      return retval;

    if ((retval = ttable->replace(tmp_name, tmp_ilist, &tmp_pre, &tmp_post)) != VLAD_OK)
      return retval;

    /* precondition loop */
    for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_pre); i_exp++) {
      if ((retval = tmp_pre->get(i_exp, &tmp_atom)) != VLAD_OK)
        return retval;
      if ((retval = encode_atom(tmp_atom, i, &tmp_num)) != VLAD_OK)
        return retval;
      if ((retval = tmp_list->add(tmp_num)) != VLAD_OK)
        return retval;
    }

    /* postcondition loop */
    for (i_exp = 0; i_exp < VLAD_LIST_LENGTH(tmp_post); i_exp++) {
      if ((retval = tmp_post->get(i_exp, &tmp_atom)) != VLAD_OK)
        return retval;
      if ((retval = encode_atom(tmp_atom, i + 1, &tmp_num)) != VLAD_OK)
        return retval;
      /* for every atom in the postcondition we add a rule */
      if ((retval = smobject->add_rule(tmp_num, tmp_list, NULL)) != VLAD_OK)
        return retval;
    }

    delete tmp_list;
  }

  /* this might not succeed as there might not exist a model for this query */
  if ((retval = smobject->close_rule()) != VLAD_OK)
    return retval;

  stage = 4;

  return VLAD_OK;
}
#endif

#ifdef VLAD_SMODELS
/* use smwrap class to evaluate a query */
int kb::query_evaluate(expression *e, unsigned char *r)
{
  int retval;
  unsigned int i;

  /* we only allow this after a call to compute() */
  if (stage != 4)
    return VLAD_INVALIDOP;

  /* verify expression */
  if ((retval = verify_expression(e)) != VLAD_OK)
    return retval;

  *r = VLAD_RESULT_UNKNOWN;

  /* go through the atoms to test */
  for (i = 0; i < VLAD_LIST_LENGTH(e); i++) {
    atom *tmp_atom;
    unsigned int tmp_num;
    char tmp_res;

    if ((retval = e->get(i, &tmp_atom)) != VLAD_OK)
      return retval;
    if ((retval = encode_atom(tmp_atom, VLAD_LIST_LENGTH(setable), &tmp_num)) != VLAD_OK)
      return retval;
    if ((retval = smobject->ask(tmp_num, &tmp_res)) != VLAD_OK)
      return retval;
    if (tmp_res == VLAD_RESULT_TRUE) {
      *r = VLAD_RESULT_TRUE;
      continue;
    }
    else if (tmp_res == VLAD_RESULT_FALSE) {
      /* if false, we try the negation of the atom */
      tmp_atom->negate();

      if ((retval = encode_atom(tmp_atom, VLAD_LIST_LENGTH(setable), &tmp_num)) != VLAD_OK)
        return retval;
      if ((retval = smobject->ask(tmp_num, &tmp_res)) != VLAD_OK)
        return retval;
      if (tmp_res == VLAD_RESULT_TRUE)
        *r = VLAD_RESULT_FALSE;
      else
        *r = VLAD_RESULT_UNKNOWN;

      /* of course, we have to change it back */
      tmp_atom->negate();

      /* no sense in continuing further: false or unknown we stop */
      break;
    }
    else {
      /* nothing we can do */
      *r = VLAD_RESULT_UNKNOWN;
      break;
    }
  }

  return VLAD_OK;
}
#endif

/* make sure atom a is valid */
int kb::verify_atom(atom *a, stringlist *v)
{
  int retval;
  char *tmp1;
  char *tmp2;
  char *tmp3;
  unsigned char ty;
  bool tr;

  /* this function is only valid after the symtab is closed */
  if (stage < 2)
    return VLAD_INVALIDOP;

  if ((retval = a->get(&tmp1, &tmp2, &tmp3, &ty, &tr)) != VLAD_OK)
    return retval;

  switch(ty) {
    case VLAD_ATOM_HOLDS :
      return verify_atom_holds(tmp1, tmp2, tmp3, v);
    case VLAD_ATOM_MEMBER :
      return verify_atom_member(tmp1, tmp2, v);
    case VLAD_ATOM_SUBSET :
      return verify_atom_subset(tmp1, tmp2, v);
    default :
      return VLAD_INVALIDINPUT;
  }

  return VLAD_OK;
}

/* make sure expression e is valid */
int kb::verify_expression(expression *e)
{
  int retval;
  unsigned int i;
  atom *tmp_atom;

  if (e == NULL)
    return VLAD_NULLPTR;

  for (i = 0; i < VLAD_LIST_LENGTH(e); i++) {
    if ((retval = e->get(i, &tmp_atom)) != VLAD_OK)
      return retval;
    if ((retval = verify_atom(tmp_atom, NULL)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

/* make sure transref is valid */
int kb::verify_transref(char *n, stringlist *il)
{
  int retval;
  unsigned int i;
  stringlist *tmp_vlist;
  expression *tmp_pr;
  expression *tmp_po;

  if (n == NULL)
    return VLAD_NULLPTR;

  /* retrieve respective trans in transtab */
  if ((retval = ttable->get(n, &tmp_vlist, &tmp_pr, &tmp_po)) != VLAD_OK)
    return retval;

  /* if both lists are NULL, there is nothing to check */
  if (il == NULL && tmp_vlist == NULL)
    return VLAD_OK;

  /* check that the number of ident listed is the same as the transformation */
  if (il == NULL || tmp_vlist == NULL || VLAD_LIST_LENGTH(tmp_vlist) != VLAD_LIST_LENGTH(il))
    return VLAD_INVALIDINPUT;

  /* check that every ident is valid in symtab */
  for (i = 0; i < VLAD_LIST_LENGTH(il); i++) {
    char *tmp_ident;

    if ((retval = il->get(i, &tmp_ident)) != VLAD_OK)
      return retval;

    if ((retval = stable->find(tmp_ident)) != VLAD_OK)
      return retval;
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

/* gives an atom id based on the identifiers already given */
int kb::encode_atom(atom *a, unsigned int s, unsigned int *n)
{
  int retval;
  char *tmp1;
  char *tmp2;
  char *tmp3;
  unsigned char ty;
  bool tr;

  if (stage < 2)
    return VLAD_INVALIDOP;

  if (a == NULL || n == NULL)
    return VLAD_NULLPTR;

  if ((retval = a->get(&tmp1, &tmp2, &tmp3, &ty, &tr)) != VLAD_OK)
    return retval;

  /* get the unsigned, unstated id of the atom */
  switch(ty) {
    case VLAD_ATOM_HOLDS : {
      unsigned int s_index;
      unsigned int a_index;
      unsigned int o_index;
      unsigned char s_type;
      unsigned char a_type;
      unsigned char o_type;

      /* get incices */
      if ((retval = stable->get(tmp1, &s_index, &s_type)) != VLAD_OK)
        return retval;
      if ((retval = stable->get(tmp2, &a_index, &a_type)) != VLAD_OK)
        return retval;
      if ((retval = stable->get(tmp3, &o_index, &o_type)) != VLAD_OK)
        return retval;

      /* adjust for groups */
      s_index += (VLAD_IDENT_IS_GROUP(s_type) ? s_len : 0);
      a_index += (VLAD_IDENT_IS_GROUP(a_type) ? a_len : 0);
      o_index += (VLAD_IDENT_IS_GROUP(o_type) ? o_len : 0);

      *n = compute_holds(s, tr, s_index, a_index, o_index);

      return VLAD_OK;
    }
    case VLAD_ATOM_MEMBER : {
      unsigned int e_index;
      unsigned int g_index;
      unsigned char e_type;
      unsigned char g_type;

      /* get incices */
      if ((retval = stable->get(tmp1, &e_index, &e_type)) != VLAD_OK)
        return retval;
      if ((retval = stable->get(tmp2, &g_index, &g_type)) != VLAD_OK)
        return retval;

      *n = compute_member(s, tr, e_type, e_index, g_index);

      return VLAD_OK;
    }
    case VLAD_ATOM_SUBSET : {
      unsigned int g1_index;
      unsigned int g2_index;
      unsigned char g1_type;
      unsigned char g2_type;

      /* get incices */
      if ((retval = stable->get(tmp1, &g1_index, &g1_type)) != VLAD_OK)
        return retval;
      if ((retval = stable->get(tmp2, &g2_index, &g2_type)) != VLAD_OK)
        return retval;

      *n = compute_subset(s, tr, g1_type, g1_index, g2_index);

      return VLAD_OK;
    }
  }

  return VLAD_INVALIDINPUT;
}

/* returns the atom given the id */
int kb::decode_atom(atom **a, unsigned int *s, unsigned int n)
{
  int retval;
  char *tmp1;
  char *tmp2;
  char *tmp3;
  unsigned char ty;
  bool tr;

  if (stage < 2)
    return VLAD_INVALIDOP;

  if (a == NULL || s == NULL)
    return VLAD_NULLPTR;

  /* first extract the state */
  *s = n / (pos_tot * 2);
  n = n % (pos_tot * 2);

  /* then get truth value */
  tr = (n >= pos_tot);
  n = n % pos_tot;

  /* now get the type and the atom details */
  if (n < h_tot) {
    ty = VLAD_ATOM_HOLDS;

    if ((retval = decode_holds(&tmp1, &tmp2, &tmp3, n)) != VLAD_OK)
      return retval;
  }
  else if (n < h_tot + m_tot) {
    ty = VLAD_ATOM_MEMBER;

    if ((retval = decode_member(&tmp1, &tmp2, n - h_tot)) != VLAD_OK)
      return retval;
  }
  else {
    ty = VLAD_ATOM_SUBSET;

    if ((retval = decode_subset(&tmp1, &tmp2, n - (h_tot + m_tot))) != VLAD_OK)
      return retval;
  }

  /* now create a new atom */
  if ((*a = VLAD_NEW(atom())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = (*a)->init(tmp1, tmp2, tmp3, ty, tr)) != VLAD_OK) {
    delete (*a);
    return retval;
  }

  return VLAD_OK;
}

/* returns an atom id based on the info given */
unsigned int kb::compute_atom(unsigned int a_st, bool a_tr, unsigned int a_atm)
{
  return (a_st * pos_tot * 2) + (a_tr ? pos_tot : 0) + a_atm;
}

/* returns a holds atom id based on the info given */
unsigned int kb::compute_holds(unsigned int a_st, bool a_tr, unsigned int a_sub, unsigned int a_acc, unsigned int a_obj)
{
  return compute_atom(a_st, a_tr, (a_sub * (a_len + ag_len) * (o_len + og_len)) + (a_acc * (o_len + og_len)) + a_obj);
}

/* returns a member atom id based on the info given */
unsigned int kb::compute_member(unsigned int a_st, bool a_tr, char a_ty, unsigned int a_elt, unsigned int a_grp)
{
  switch(VLAD_IDENT_BASETYPE(a_ty)) {
    case VLAD_IDENT_SUBJECT :
      return compute_atom(a_st, a_tr, h_tot + (a_elt * sg_len) + a_grp);
    case VLAD_IDENT_ACCESS :
      return compute_atom(a_st, a_tr, h_tot + (s_len * sg_len) + (a_elt * ag_len) + a_grp);
    case VLAD_IDENT_OBJECT :
      return compute_atom(a_st, a_tr, h_tot + (s_len * sg_len) + (a_len * ag_len) + (a_elt * og_len) + a_grp);
  }

  return 0;
}

/* returns a subset atom id based on the info given */
unsigned int kb::compute_subset(unsigned int a_st, bool a_tr, char a_ty, unsigned int a_grp1, unsigned int a_grp2)
{
  switch(VLAD_IDENT_BASETYPE(a_ty)) {
    case VLAD_IDENT_SUBJECT :
      return compute_atom(a_st, a_tr, h_tot + m_tot + (a_grp1 * sg_len) + a_grp2);
    case VLAD_IDENT_ACCESS :
      return compute_atom(a_st, a_tr, h_tot + m_tot + (sg_len * sg_len) + (a_grp1 * ag_len) + a_grp2);
    case VLAD_IDENT_OBJECT :
      return compute_atom(a_st, a_tr, h_tot + m_tot + (sg_len * sg_len) + (ag_len * ag_len) + (a_grp1 * og_len) + a_grp2);
  }

  return 0;
}
