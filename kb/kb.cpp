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
  expression *exp = NULL;
  expression *cond = NULL;
  expression *ncond = NULL;
  atom *tmp1;
  atom *tmp2;

  if (stage != 3)
    return VLAD_FAILURE;

  /* only e is required to be non-null */
  if (e == NULL)
    return VLAD_NULLPTR;

    if ((ncond = VLAD_NEW(expression())) == NULL)
    return VLAD_MALLOCFAILED;

  /* 
   * now, we must go through every atom of every exression to ensure
   * their validity. while we are going through them, we might as well
   * make a copy.
   */

  /* exression */
  if ((exp = VLAD_NEW(expression())) == NULL)
    return VLAD_MALLOCFAILED;

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

  /* condition */
  if (c != NULL) {
    if ((cond = VLAD_NEW(expression())) == NULL)
      return VLAD_MALLOCFAILED;

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

  /* negative condition */
  if (n != NULL) {
    if ((ncond = VLAD_NEW(expression())) == NULL)
      return VLAD_MALLOCFAILED;

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
  stringlist *vlist = NULL;
  expression *precond = NULL;
  expression *postcond = NULL;

  /* we only allow this function after consttab is closed */
  if (stage != 4)
    return VLAD_FAILURE;

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
  if (pr != NULL) {
    if ((precond = VLAD_NEW(expression())) == NULL)
      return VLAD_MALLOCFAILED;

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

  /* if all went well, add to the trans table */
  return ttable->add(name, vlist, precond, postcond);
}

/* generate a human-readable general logic program and dump output to f */
int kb::generate_nlp(expression *e, sequence *s, FILE *f)
{
  int retval;
  unsigned int i;
  unsigned int tot_trans;
  
  /* we only allow this function after transtab is closed */
  if (stage != 5)
    return VLAD_FAILURE;

  /* make sure the filestream is not NULL */
  if (f == NULL)
    return VLAD_NULLPTR;

  /* verify expression */
  if ((retval = verify_expression(e)) != VLAD_OK)
    return retval;

  /* now verify the sequence */
  if ((retval = verify_sequence(s)) != VLAD_OK)
    return retval;

  /* number of transformations in this sequence */
  tot_trans = (s == NULL) ? 0 : s->length();

  /* first we print out all the possible atoms in the kb */
  fprintf(f, "Atoms\n");

  for (i = 0; i < (pos_tot * 2 * (tot_trans + 1)); i++) {
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
        fprintf(f, "  %d = holds(S%d, %c, holds, %s, %s, %s)\n", i, tmp_s, tmp_tr ? 'T' : 'F', tmp_param1, tmp_param2, tmp_param3);
        break;
      case VLAD_ATOM_MEMBER :
          fprintf(f, "  %d = holds(S%d, %c, member, %s, %s)\n", i, tmp_s, tmp_tr ? 'T' : 'F', tmp_param1, tmp_param2);
        break;
      case VLAD_ATOM_SUBSET :
          fprintf(f, "  %d = holds(S%d, %c, subset, %s, %s)\n", i, tmp_s, tmp_tr ? 'T' : 'F', tmp_param1, tmp_param2);
        break;
    }

    delete tmp_atom;
  }

  /* inheritance rules */
  fprintf(f, "Inheritance Rules\n");

  /* state loop */
  for (i = 0; i <= tot_trans; i++) {
    unsigned int i_truth;
    unsigned int i_group;
    unsigned int i_sub;
    unsigned int i_acc;
    unsigned int i_obj;
    /* truth loop */
    for (i_truth = 0; i_truth < 2; i_truth++) {
      /* subject groups */
      for (i_group = 0; i_group < sg_len; i_group++) {
        for (i_sub = 0; i_sub < s_len; i_sub++) {
          for (i_acc = 0; i_acc < a_len + ag_len; i_acc++) {
            for (i_obj = 0; i_obj < o_len + og_len; i_obj++) {
              fprintf(f, 
                      "  %d <- %d AND %d\n",
                      (i * pos_tot * 2) + (i_truth ? pos_tot : 0) + (i_sub * (a_len + ag_len) * (o_len + og_len)) + (i_acc * (o_len + og_len)) + i_obj,
                      (i * pos_tot * 2) + (i_truth ? pos_tot : 0) + ((i_group + s_len) * (a_len + ag_len) * (o_len + og_len)) + (i_acc * (o_len + og_len)) + i_obj,
                      (i * pos_tot * 2) + pos_tot + h_tot + (i_sub * sg_len) + i_group);
            }
          }
        }
      }
      /* access groups */
      for (i_group = 0; i_group < ag_len; i_group++) {
        for (i_sub = 0; i_sub < s_len + sg_len; i_sub++) {
          for (i_acc = 0; i_acc < a_len; i_acc++) {
            for (i_obj = 0; i_obj < o_len + og_len; i_obj++) {
              fprintf(f, 
                      "  %d <- %d AND %d\n",
                      (i * pos_tot * 2) + (i_truth ? pos_tot : 0) + (i_sub * (a_len + ag_len) * (o_len + og_len)) + (i_acc * (o_len + og_len)) + i_obj,
                      (i * pos_tot * 2) + (i_truth ? pos_tot : 0) + (i_sub * (a_len + ag_len) * (o_len + og_len)) + ((i_group + a_len) * (o_len + og_len)) + i_obj,
                      (i * pos_tot * 2) + pos_tot + h_tot + (sg_len * sg_len) + (i_acc * sg_len) + i_group);
            }
          }
        }
      }
      /* object groups */
      for (i_group = 0; i_group < og_len; i_group++) {
        for (i_sub = 0; i_sub < s_len + sg_len; i_sub++) {
          for (i_acc = 0; i_acc < a_len + ag_len; i_acc++) {
            for (i_obj = 0; i_obj < o_len; i_obj++) {
              fprintf(f, 
                      "  %d <- %d AND %d\n",
                      (i * pos_tot * 2) + (i_truth ? pos_tot : 0) + (i_sub * (a_len + ag_len) * (o_len + og_len)) + (i_acc * (o_len + og_len)) + i_obj,
                      (i * pos_tot * 2) + (i_truth ? pos_tot : 0) + (i_sub * (a_len + ag_len) * (o_len + og_len)) + (i_acc * (o_len + og_len)) + i_group + o_len,
                      (i * pos_tot * 2) + pos_tot + h_tot + (sg_len * sg_len) + (ag_len * ag_len) + (i_obj * og_len) + i_group);
            }
          }
        }
      }
    }
  }

  /* transitivity */
  fprintf(f, "Transitivity Rules\n");

  /* state loop */
  for (i = 0; i <= tot_trans; i++) {
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
                  "  %d <- %d AND %d\n",
                  (i * pos_tot * 2) + pos_tot + h_tot + m_tot + (i_grp1 * sg_len) + i_grp3,
                  (i * pos_tot * 2) + pos_tot + h_tot + m_tot + (i_grp1 * sg_len) + i_grp2,
                  (i * pos_tot * 2) + pos_tot + h_tot + m_tot + (i_grp2 * sg_len) + i_grp3);
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
                  "  %d <- %d AND %d\n",
                 (i * pos_tot * 2) + pos_tot + h_tot + m_tot + (sg_len * sg_len) + (i_grp1 * sg_len) + i_grp3,
                 (i * pos_tot * 2) + pos_tot + h_tot + m_tot + (sg_len * sg_len) + (i_grp1 * sg_len) + i_grp2,
                 (i * pos_tot * 2) + pos_tot + h_tot + m_tot + (sg_len * sg_len) + (i_grp2 * sg_len) + i_grp3);
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
                  "  %d <- %d AND %d\n",
                 (i * pos_tot * 2) + pos_tot + h_tot + m_tot + (sg_len * sg_len) + (ag_len * ag_len) + (i_grp1 * sg_len) + i_grp3,
                 (i * pos_tot * 2) + pos_tot + h_tot + m_tot + (sg_len * sg_len) + (ag_len * ag_len) + (i_grp1 * sg_len) + i_grp2,
                 (i * pos_tot * 2) + pos_tot + h_tot + m_tot + (sg_len * sg_len) + (ag_len * ag_len) + (i_grp2 * sg_len) + i_grp3);
        }
      }
    }
  }

  /* complementary rules */
  fprintf(f, "Complementary Rules\n");

  /* state loop */
  for (i = 0; i <= tot_trans; i++) {
    unsigned int i_atom;
    for (i_atom = 0; i_atom < pos_tot; i_atom++) {
      fprintf(f, 
              "  false <- %d AND %d\n",
              (i * pos_tot * 2) + i_atom + pos_tot,
              (i * pos_tot * 2) + i_atom);
    }
  }

  /* inertial rules */
  fprintf(f, "Inertial Rules\n");

  /* state loop */
  for (i = 0; i < tot_trans; i++) {
    unsigned int i_atom;
    for (i_atom = 0; i_atom < pos_tot; i_atom++) {
      fprintf(f,
              "  %d <- %d AND NOT %d\n",
              ((i + 1) * pos_tot * 2) + i_atom + pos_tot,
              (i * pos_tot * 2) + i_atom + pos_tot,
              ((i + 1) * pos_tot * 2) + i_atom);
      fprintf(f,
              "  %d <- %d AND NOT %d\n",
              ((i + 1) * pos_tot * 2) + i_atom,
              (i * pos_tot * 2) + i_atom,
              ((i + 1) * pos_tot * 2) + i_atom + pos_tot);
    }
  }

  /* initial state */
  fprintf(f, "Initial State\n");

  for (i = 0; i < (itable ? itable->length() : 0); i++) {
    atom *tmp_atom;
    unsigned int tmp_num;

    if ((retval = itable->get(i, &tmp_atom)) != VLAD_OK)
      return retval;
    if ((retval = encode_atom(tmp_atom, 0, &tmp_num)) != VLAD_OK)
      return retval;

    fprintf(f, "  %d <-\n", tmp_num);
  }

  /* constraints */
  fprintf(f, "Constraints\n");

  for (i = 0; i <= tot_trans; i++) {
    unsigned int  i_const;
    unsigned int i_exp;

    /* constraint loop */
    for (i_const = 0; i_const < (ctable ? ctable->length() : 0); i_const++) {
      expression *tmp_e;
      expression *tmp_c;
      expression *tmp_n;
      atom *tmp_atom;
      unsigned int tmp_num;

      if ((retval = ctable->get(i_const, &tmp_e, &tmp_c, &tmp_n)) != VLAD_OK)
        return retval;

      fprintf(f, " ");

      /* constaint expression */
      for (i_exp = 0; i_exp < tmp_e->length(); i_exp++) {
        if ((retval = tmp_e->get(i_exp, &tmp_atom)) != VLAD_OK)
          return retval;
        if ((retval = encode_atom(tmp_atom, i, &tmp_num)) != VLAD_OK)
          return retval;
        fprintf(f, " %d", tmp_num);
      }

      fprintf(f, " <-");

      /* constraint condition */
      for (i_exp = 0; i_exp < (tmp_c ? tmp_c->length() : 0); i_exp++) {
        if ((retval = tmp_c->get(i_exp, &tmp_atom)) != VLAD_OK)
          return retval;
        if ((retval = encode_atom(tmp_atom, i, &tmp_num)) != VLAD_OK)
          return retval;
        fprintf(f, " %d", tmp_num);
      }

      /* constraint negative condition */
      for (i_exp = 0; i_exp < tmp_n->length(); i_exp++) {
        if ((retval = tmp_n->get(i_exp, &tmp_atom)) != VLAD_OK)
          return retval;
        if ((retval = encode_atom(tmp_atom, i, &tmp_num)) != VLAD_OK)
          return retval;
        fprintf(f, " NOT %d", tmp_num);
      }

      fprintf(f, "\n");
    }
  }

  /* transformation rules */
  fprintf(f, "Transformation Rules\n");

  /* state loop */
  for (i = 0; i < tot_trans; i++) {
    unsigned int i_exp;
    char *tmp_name;
    atom *tmp_atom;
    unsigned int tmp_num;
    stringlist *tmp_ilist = NULL;
    expression *tmp_pre = NULL;
    expression *tmp_post = NULL;

    if ((retval = s->get(i, &tmp_name, &tmp_ilist)) != VLAD_OK)
      return retval;

    if ((retval = ttable->replace(tmp_name, tmp_ilist, &tmp_pre, &tmp_post)) != VLAD_OK)
      return retval;
    
    fprintf(f, " ");

    /* postcondition loop */
    for (i_exp = 0; i_exp < (tmp_post ? tmp_post->length() : 0); i_exp++) {
      if ((retval = tmp_post->get(i_exp, &tmp_atom)) != VLAD_OK)
        return retval;
      if ((retval = encode_atom(tmp_atom, i + 1, &tmp_num)) != VLAD_OK)
        return retval;
      fprintf(f, " %d", tmp_num);
    }

    fprintf(f, " <-");

    /* precondition loop */
    for (i_exp = 0; i_exp < (tmp_pre ? tmp_pre->length() : 0); i_exp++) {
      if ((retval = tmp_pre->get(i_exp, &tmp_atom)) != VLAD_OK)
        return retval;
      if ((retval = encode_atom(tmp_atom, i, &tmp_num)) != VLAD_OK)
        return retval;
      fprintf(f, " %d", tmp_num);
    }

    fprintf(f, "\n");
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

  for (i = 0; i < e->length(); i++) {
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

  if (stage < 5)
    return VLAD_FAILURE;

  if (n == NULL)
    return VLAD_NULLPTR;

  /* retrieve respective trans in transtab */
  if ((retval = ttable->get(n, &tmp_vlist, &tmp_pr, &tmp_po)) != VLAD_OK)
    return retval;

  /* if both lists are NULL, there is nothing to check */
  if (il == NULL && tmp_vlist == NULL)
    return VLAD_OK;

  /* check that the number of ident listed is the same as the transformation */
  if (il == NULL || tmp_vlist == NULL || tmp_vlist->length() != il->length())
    return VLAD_INVALIDINPUT;

  /* check that every ident is valid in symtab */
  for (i = 0; i < il->length(); i++) {
    char *tmp_ident;

    if ((retval = il->get(i, &tmp_ident)) != VLAD_OK)
      return retval;

    if ((retval = stable->find(tmp_ident)) != VLAD_OK)
      return retval;
  }

  return VLAD_OK;
}

/* make sure sequence s is valid */
int kb::verify_sequence(sequence *s)
{
  int retval;
  unsigned int i;
  char *tmp_name;
  stringlist *tmp_ilist;

  /* if the sequence is NULL, there is nothing to do */
  if (s == NULL)
    return VLAD_OK;

  /* now verify transref */
  for (i = 0; i < s->length(); i++) {
    if ((retval = s->get(i, &tmp_name, &tmp_ilist)) != VLAD_OK)
      return retval;

    if ((retval = verify_transref(tmp_name, tmp_ilist)) != VLAD_OK)
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
    case VLAD_ATOM_HOLDS :
      if ((retval = encode_holds(tmp1, tmp2, tmp3, &num)) != VLAD_OK)
        return retval;
      break;
    case VLAD_ATOM_MEMBER :
      if ((retval = encode_member(tmp1, tmp2, &num)) != VLAD_OK)
        return retval;
      num = num + h_tot;
      break;
    case VLAD_ATOM_SUBSET :
      if ((retval = encode_subset(tmp1, tmp2, &num)) != VLAD_OK)
        return retval;
      num = num + h_tot + m_tot;
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
