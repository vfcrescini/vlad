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
  c_len = 0;
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

  stage = 1;

  return VLAD_OK;
}

/* after this is called, no further calls to add_inittab() is allowed */
int kb::close_symtab()
{
  if (stage != 1)
    return VLAD_FAILURE;

  /* first get some needed values */
  
  /* get list lengths */
  c_len = stable->length(VLAD_IDENT_CONST);
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
  pos_tot = h_tot + m_tot + s_tot + c_len;

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
  if ((retval = a->copy(&tmp)) != VLAD_OK)
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
  if ((exp = VLAD_NEW(expression())) == NULL)
    return VLAD_MALLOCFAILED;
  if ((cond = VLAD_NEW(expression())) == NULL)
    return VLAD_MALLOCFAILED;
  if ((ncond = VLAD_NEW(expression())) == NULL)
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
    if ((retval = tmp1->copy(&tmp2)) != VLAD_OK)
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
      if ((retval = tmp1->copy(&tmp2)) != VLAD_OK)
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
  if ((vlist = VLAD_NEW(stringlist())) == NULL)
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
  if ((precond = VLAD_NEW(expression())) == NULL)
    return VLAD_MALLOCFAILED;

  if (pr != NULL) {
    for (i = 0; i < pr->length(); i++) {
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

  for (i = 0; i < po->length(); i++) {
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

  /* if all went  well, add to the trans table */
  return ttable->add(name, vlist, precond, postcond);
}

/* generate a human-readable general logic program and dump output to f */
int kb::generate_nlp(expression *e, sequence *s, FILE *f)
{
  int retval;
  unsigned int i;
  unsigned int j;
  unsigned int k;
  unsigned int l;
  unsigned int m;
  unsigned int n;

  /* we only allow this function after transtab is closed */
  if (stage != 5)
    return VLAD_FAILURE;

  if (e == NULL || f == NULL)
    return VLAD_NULLPTR;

  /* verify the query expression */
  for (i = 0; i < e->length(); i++) {
    atom *tmp1;
    if ((retval = e->get(i, &tmp1)) != VLAD_OK)
      return retval;
    if ((retval = verify_atom(tmp1, NULL)) != VLAD_OK)
      return retval;
  }

  /* now verify transref */
  if (s != NULL) {
    for (i = 0; i < s->length(); i++) {
      transref *tmp1;
      if ((retval = s->get(i, &tmp1)) != VLAD_OK)
        return retval;
      if ((retval = verify_transref(tmp1)) != VLAD_OK)
        return retval;
    }
  }

  /* first we print out all the possible atoms in the kb */
  fprintf(f, "Atoms\n");

  for (i = 0; i < pos_tot * 2 * ((s == NULL) ? 1 : s->length() + 1); i++) {
    atom *tmp_atom;
    unsigned char tmp_ty;
    unsigned int tmp_s;
    bool tmp_tr;
    char *tmp1;
    char *tmp2;
    char *tmp3;

    if ((retval = decode_atom(&tmp_atom, &tmp_s, i)) != VLAD_OK)
      return retval;

    if ((retval = tmp_atom->get(&tmp1, &tmp2, &tmp3, &tmp_ty, &tmp_tr)) != VLAD_OK)
      return retval;

    switch(tmp_ty) {
      case VLAD_ATOM_CONST :
        fprintf(f, "  %d = holds(S%d, %c, constant, %s)\n", i, tmp_s, tmp_tr ? 'T' : 'F', tmp1);
        break;
      case VLAD_ATOM_HOLDS :
        fprintf(f, "  %d = holds(S%d, %c, holds, %s, %s, %s)\n", i, tmp_s, tmp_tr ? 'T' : 'F', tmp1, tmp2, tmp3);
        break;
      case VLAD_ATOM_MEMBER :
          fprintf(f, "  %d = holds(S%d, %c, member, %s, %s)\n", i, tmp_s, tmp_tr ? 'T' : 'F', tmp1, tmp2);
        break;
      case VLAD_ATOM_SUBSET :
          fprintf(f, "  %d = holds(S%d, %c, subset, %s, %s)\n", i, tmp_s, tmp_tr ? 'T' : 'F', tmp1, tmp2);
        break;
    }

    delete tmp_atom;
  }

  /* inheritance rules */
  fprintf(f, "Inheritance Rules\n");

  /* state loop */
  for (i = 0; i <= ((s == NULL) ? 0 : s->length()); i++) {
    /* truth loop */
    for (j = 0; j < 2; j++) {
      /* subject group loop */
      for (k = 0; k < sg_len; k++) {
        /* subject loop */
        for (l = 0; l < s_len; l++) {
          /* access loop */
          for (m = 0; m < a_len + ag_len; m++) {
            /* object loop */
            for (n = 0; n < o_len + og_len; n++) {
              fprintf(f, 
                      "  %d <- %d AND %d\n",
                      (j ? pos_tot : 0) + (i * pos_tot * 2) + c_len + (k * (a_len + ag_len) * (o_len + og_len)) + (m * (o_len + og_len)) + n,
                      (j ? pos_tot : 0) + (i * pos_tot * 2) + c_len + (l * (a_len + ag_len) * (o_len + og_len)) + (m * (o_len + og_len)) + n + o_len,
                      pos_tot + (i * pos_tot * 2) + c_len + h_tot + (l * sg_len) + k);
            }
          }
        }
      }
      /* access group loop */
      for (k = 0; k < ag_len; k++) {
        /* access loop */
        for (l = 0; l < a_len; l++) {
          /* subject loop */
          for (m = 0; m < s_len + sg_len; m++) {
            /* object loop */
            for (n = 0; n < o_len + og_len; n++) {
              fprintf(f, 
                      "  %d <- %d AND %d\n",
                      (j ? pos_tot : 0) + (i * pos_tot * 2) + c_len + (m * (a_len + ag_len) * (o_len + og_len)) + (l * (o_len + og_len)) + n,
                      (j ? pos_tot : 0) + (i * pos_tot * 2) + c_len + (m * (a_len + ag_len) * (o_len + og_len)) + (k * (o_len + og_len)) + n + o_len,
                      pos_tot + (i * pos_tot * 2) + c_len + h_tot + (sg_len * sg_len) + (l * sg_len) + k);
            }
          }
        }
      }
      /* object group loop */
      for (k = 0; k < og_len; k++) {
        /* object loop */
        for (l = 0; l < o_len; l++) {
          /* subject loop */
          for (m = 0; m < s_len + sg_len; m++) {
            /* access loop */
            for (n = 0; n < a_len + ag_len; n++) {
              fprintf(f, 
                      "  %d <- %d AND %d\n",
                      (j ? pos_tot : 0) + (i * pos_tot * 2) + c_len + (m * (a_len + ag_len) * (o_len + og_len)) + (n * (o_len + og_len)) + l,
                      (j ? pos_tot : 0) + (i * pos_tot * 2) + c_len + (m * (a_len + ag_len) * (o_len + og_len)) + (n * (o_len + og_len)) + k + o_len,
                      pos_tot + (i * pos_tot * 2) + c_len + h_tot + (sg_len * sg_len) + (ag_len * ag_len) + (l * og_len) + k);
            }
          }
        }
      }
    }
  }

  /* transitivity */
  fprintf(f, "Transitivity Rules\n");

  /* state loop */
  for (i = 0; i <= ((s == NULL) ? 0 : s->length()); i++) {
    /* truth loop */
    for (j = 0; j < 2; j++) {
      /* subject */
      for (k = 0; k < sg_len; k++) {
        for (l = 0; l < sg_len; l++) {
          for (m = 0; m < sg_len; m++) {

            /* ignore if any 2 are the same */
            if (k == l || l == m || m == k)
              continue;

            fprintf(f,
                    "  %d <- %d AND %d\n",
                    (j ? pos_tot : 0) + (i * pos_tot * 2) + c_len + h_tot + m_tot + (k * sg_len) + m,
                    (j ? pos_tot : 0) + (i * pos_tot * 2) + c_len + h_tot + m_tot + (k * sg_len) + l,
                    (j ? pos_tot : 0) + (i * pos_tot * 2) + c_len + h_tot + m_tot + (l * sg_len) + m);
          }
        }
      }
      /* access */
      for (k = 0; k < ag_len; k++) {
        for (l = 0; l < ag_len; l++) {
          for (m = 0; m < ag_len; m++) {

            /* ignore if any 2 are the same */
            if (k == l || l == m || m == k)
              continue;

            fprintf(f,
                    "  %d <- %d AND %d\n",
                    (j ? pos_tot : 0) + (i * pos_tot * 2) + c_len + h_tot + m_tot + (sg_len * sg_len) + (k * ag_len) + m,
                    (j ? pos_tot : 0) + (i * pos_tot * 2) + c_len + h_tot + m_tot + (sg_len * sg_len) + (k * ag_len) + l,
                    (j ? pos_tot : 0) + (i * pos_tot * 2) + c_len + h_tot + m_tot + (sg_len * sg_len) + (l * ag_len) + m);
          }
        }
      }
      /* object */
      for (k = 0; k < og_len; k++) {
        for (l = 0; l < og_len; l++) {
          for (m = 0; m < og_len; m++) {

            /* ignore if any 2 are the same */
            if (k == l || l == m || m == k)
              continue;

            fprintf(f,
                    "  %d <- %d AND %d\n",
                    (j ? pos_tot : 0) + (i * pos_tot * 2) + c_len + h_tot + m_tot + (sg_len * sg_len) + (ag_len * ag_len) + (k * og_len) + m,
                    (j ? pos_tot : 0) + (i * pos_tot * 2) + c_len + h_tot + m_tot + (sg_len * sg_len) + (ag_len * ag_len) + (k * og_len) + l,
                    (j ? pos_tot : 0) + (i * pos_tot * 2) + c_len + h_tot + m_tot + (sg_len * sg_len) + (ag_len * ag_len) + (l * og_len) + m);
          }
        }
      }
    }
  }

  /* complementary rules */
  fprintf(f, "Complementary Rules\n");

  /* state loop */
  for (i = 0; i <= ((s == NULL) ? 0 : s->length()); i++) {
    for (j = 0; j < pos_tot; j++) {
      fprintf(f,
              "  %d <- %d AND %d\n", 
              pos_tot + (i * pos_tot * 2), 
              j + pos_tot + (i * pos_tot * 2), 
              j + (i * pos_tot * 2));
    }
  }

  /* inertial rules */
  fprintf(f, "Inertial Rules\n");

  /* state loop */
  for (i = 0; i < ((s == NULL) ? 0 : s->length()); i++) {
    for (j = 0; j < pos_tot; j++) {
      fprintf(f, 
              "  %d <- %d AND NOT %d\n", 
              j + pos_tot + ((i + 1) * pos_tot * 2),
              j + pos_tot + (i * pos_tot * 2),
              j + ((i + 1) * pos_tot * 2));
      fprintf(f, 
              "  %d <- %d AND NOT %d\n", 
              j + ((i + 1) * pos_tot * 2),
              j + (i * pos_tot * 2),
              j + pos_tot + ((i + 1) * pos_tot * 2));
    } 
  }

  /* initial state */
  fprintf(f, "Initial State\n");

  for (i = 0; i < itable->length(); i++) {
    atom *tmp;
    unsigned int a;
    if ((retval = itable->get(i, &tmp)) != VLAD_OK)
      return retval;
    if ((retval = encode_atom(tmp, 0, &a)) != VLAD_OK)
      return retval;
    fprintf(f, "  %d <-\n", a);
  }

  /* constraints */
  fprintf(f, "Constraints\n");

  for (i = 0; i <= ((s == NULL) ? 0 : s->length()); i++) {
    for (j = 0; j < ctable->length(); j++) {
      expression *tmp_e;
      expression *tmp_c;
      expression *tmp_n;
   
      if ((retval = ctable->get(j, &tmp_e, &tmp_c, &tmp_n)) != VLAD_OK)
        return retval;
  
      /* constaint expression */
      for (k = 0; k < tmp_e->length(); k++) {
        atom *tmp_a;
        unsigned int a;
        if ((retval = tmp_e->get(k, &tmp_a)) != VLAD_OK)
          return retval;
        if ((retval = encode_atom(tmp_a, i, &a)) != VLAD_OK)
          return retval;
        fprintf(f, "  %d", a);
      } 
  
      fprintf(f, " <-");
      
      /* constraint condition */
      for (k = 0; k < tmp_c->length(); k++) {
        atom *tmp_a;
        unsigned int a;
        if ((retval = tmp_c->get(k, &tmp_a)) != VLAD_OK)
          return retval;
        if ((retval = encode_atom(tmp_a, i, &a)) != VLAD_OK)
          return retval;
  
        fprintf(f, "  %d", a);
      }
  
      /* constraint negative condition */
      for (k = 0; k < tmp_n->length(); k++) {
        atom *tmp_a;
        unsigned int a;
        if ((retval = tmp_n->get(k, &tmp_a)) != VLAD_OK)
          return retval;
        if ((retval = encode_atom(tmp_a, i, &a)) != VLAD_OK)
          return retval;
  
        fprintf(f, "  NOT %d", a);
      }
  
      fprintf(f, "\n");
    }
  }

  /* transformation rules */
  fprintf(f, "Transformation Rules\n");

  for (i = 0; i < ((s == NULL) ? 0 : s->length()); i++) {
    transref *tmp_ref;
    expression *tmp_pre;
    expression *tmp_post;
    atom *tmp_atom;
    unsigned int a;

    s->get(i, &tmp_ref);
    ttable->get(tmp_ref->get_name(), tmp_ref->get_ilist(), &tmp_pre, &tmp_post);
    for (j = 0; j < tmp_post->length(); j++) {
       if ((retval = tmp_post->get(j, &tmp_atom)) != VLAD_OK)
         return retval;
       if ((retval = encode_atom(tmp_atom, i + 1, &a)) != VLAD_OK)
         return retval;
       fprintf(f, "  %d", a);
    }
    fprintf(f, "  <-");
    for (j = 0; j < tmp_pre->length(); j++) {
       if ((retval = tmp_pre->get(j, &tmp_atom)) != VLAD_OK)
         return retval;
       if ((retval = encode_atom(tmp_atom, i, &a)) != VLAD_OK)
         return retval;
       fprintf(f, "  %d", a);
    }
    fprintf(f, "\n");
    delete tmp_pre;
    delete tmp_post;
  }

  return VLAD_OK;
}

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
    return VLAD_FAILURE;

  if ((retval = a->get(&tmp1, &tmp2, &tmp3, &ty, &tr)) != VLAD_OK)
    return retval;

  switch(ty) {
    case VLAD_ATOM_CONST :
      /* constants need not be checked */
      break;
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

int kb::encode_const(const char *c, unsigned int *n)
{
  int retval;
  unsigned int c_index;
  unsigned char c_type;

  /* get from symbol table */
  if ((retval = stable->get(c, &c_index, &c_type)) != VLAD_OK)
    return retval;

  *n = c_index;

  return VLAD_OK;
}

int kb::encode_holds(const char *s, const char *a, const char *o, unsigned int *n)
{
  int retval;
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
  if ((retval = stable->get(s, &s_index, &s_type)) != VLAD_OK)
    return retval;
  if ((retval = stable->get(a, &a_index, &a_type)) != VLAD_OK)
    return retval;
  if ((retval = stable->get(o, &o_index, &o_type)) != VLAD_OK)
    return retval;

  /* now compute */
  hs = s_index + (VLAD_IDENT_IS_GROUP(s_type) ? s_len : 0);
  ha = a_index + (VLAD_IDENT_IS_GROUP(a_type) ? a_len : 0);
  ho = o_index + (VLAD_IDENT_IS_GROUP(o_type) ? o_len : 0);

  *n = (hs * (a_len + ag_len) * (o_len + og_len)) + (ha * (o_len + og_len)) + ho;

  return VLAD_OK;
}

int kb::encode_member(const char *e, const char *g, unsigned int *n)
{
  int retval;
  unsigned int e_index;
  unsigned int g_index;
  unsigned char e_type;
  unsigned char g_type;

  /* get the indices of the identifiers */
  if ((retval = stable->get(e, &e_index, &e_type)) != VLAD_OK)
    return retval;
  if ((retval = stable->get(g, &g_index, &g_type)) != VLAD_OK)
    return retval;

  /* now compute */
  switch(VLAD_IDENT_BASETYPE(e_type)) {
    case VLAD_IDENT_SUBJECT :
      *n = (e_index * sg_len) + g_index;
      break;
    case VLAD_IDENT_ACCESS :
      *n = (s_len * sg_len) + (e_index * ag_len) + g_index;
      break;
    case VLAD_IDENT_OBJECT :
      *n = (s_len * sg_len) + (a_len * ag_len) + (e_index * og_len) + g_index;
      break;
    default :
      /* this should never happen */
      return VLAD_FAILURE;
  }

  return VLAD_OK;
}

int kb::encode_subset(const char *g1, const char *g2, unsigned int *n)
{
  int retval;
  unsigned int g1_index;
  unsigned int g2_index;
  unsigned char g1_type;
  unsigned char g2_type;

  /* get the indices of the identifiers */
  if ((retval = stable->get(g1, &g1_index, &g1_type)) != VLAD_OK)
    return retval;
  if ((retval = stable->get(g2, &g2_index, &g2_type)) != VLAD_OK)
    return retval;

  /* now we compute */
  switch(VLAD_IDENT_BASETYPE(g1_type)) {
    case VLAD_IDENT_SUBJECT :
      *n = (g1_index * sg_len) + g2_index;
      break;
    case VLAD_IDENT_ACCESS :
      *n = (sg_len * sg_len) + (g1_index * ag_len) + g2_index;
      break;
    case VLAD_IDENT_OBJECT :
      *n = (sg_len * sg_len) + (ag_len * ag_len) + (g1_index * og_len) + g2_index;
      break;
    default :
      /* this should never happen */
      return VLAD_FAILURE;
  }
  return VLAD_OK;
}

int kb::decode_const(char **c, unsigned int n)
{
  return stable->get(n, VLAD_IDENT_CONST, c);
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
  unsigned int num;
  char *tmp1;
  char *tmp2;
  char *tmp3;
  unsigned char ty;
  bool tr;

  if (stage < 5)
    return VLAD_FAILURE;

  if (a == NULL || n == NULL)
    return VLAD_NULLPTR;

  if ((retval = a->get(&tmp1, &tmp2, &tmp3, &ty, &tr)) != VLAD_OK)
    return retval;

  /* get the unsigned, unstated id of the atom */
  switch(ty) {
    case VLAD_ATOM_CONST :
      if ((retval = encode_const(tmp1, &num)) != VLAD_OK)
        return retval;
      break;
    case VLAD_ATOM_HOLDS :
      if ((retval = encode_holds(tmp1, tmp2, tmp3, &num)) != VLAD_OK)
        return retval;
      num = num + c_len;
      break;
    case VLAD_ATOM_MEMBER :
      if ((retval = encode_member(tmp1, tmp2, &num)) != VLAD_OK)
        return retval;
      num = num + c_len + h_tot;
      break;
    case VLAD_ATOM_SUBSET :
      if ((retval = encode_subset(tmp1, tmp2, &num)) != VLAD_OK)
        return retval;
      num = num + c_len + h_tot + m_tot;
      break;
    default :
      return VLAD_INVALIDINPUT;
  }

  /* consider the truth value */
  num = num + (tr ? pos_tot : 0);
   
  /* now the state */
  *n = num + (s * (pos_tot * 2));

  return VLAD_OK;
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

  if (stage < 5)
    return VLAD_FAILURE;

  if (a == NULL || s == NULL)
    return VLAD_NULLPTR;

  /* first extract the state */
  *s = n / (pos_tot * 2);
  n = n % (pos_tot * 2);

  /* then get truth value */
  tr = (n >= pos_tot);
  n = n % pos_tot;

  /* now get the type and the atom details */
  if (n < c_len) {
    ty = VLAD_ATOM_CONST;

    if ((retval = decode_const(&tmp1, n)) != VLAD_OK)
      return retval;
  }
  else if (n < c_len + h_tot) {
    ty = VLAD_ATOM_HOLDS;

    if ((retval = decode_holds(&tmp1, &tmp2, &tmp3, n - c_len)) != VLAD_OK)
      return retval;
  }
  else if (n < c_len + h_tot + m_tot) {
    ty = VLAD_ATOM_MEMBER;

    if ((retval = decode_member(&tmp1, &tmp2, n - (c_len + h_tot))) != VLAD_OK)
      return retval;
  }
  else {
    ty = VLAD_ATOM_SUBSET;

    if ((retval = decode_subset(&tmp1, &tmp2, n - (c_len + h_tot + m_tot))) != VLAD_OK)
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
