/*
 * smwrap.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <new>

#include <vlad/vlad.h>
#include <vlad/smwrap.h>

smwrap::smwrap()
{
  pr_smod = NULL;
  pr_api = NULL;
  pr_stage = 0;
}

smwrap::~smwrap()
{
  if (pr_smod != NULL)
    delete pr_smod;
  if (pr_api != NULL)
    delete pr_api;
}

int smwrap::init()
{
  Atom *tmp_atom;

  if (pr_stage > 0) {
    if (pr_smod != NULL)
      delete pr_smod;
    if (pr_api != NULL)
      delete pr_api;
  }

  if ((pr_smod = VLAD_NEW(Smodels())) == NULL)
    return VLAD_MALLOCFAILED;
  if ((pr_api = VLAD_NEW(Api(&(pr_smod->program)))) == NULL)
    return VLAD_MALLOCFAILED;

  /* so we don't forget */
  pr_api->remember();

  /* we have to add the two constant atoms */
  if ((tmp_atom = pr_api->new_atom()) == NULL)
    return VLAD_MALLOCFAILED;

  pr_api->set_name(tmp_atom, VLAD_STR_TRUE);

  if ((tmp_atom = pr_api->new_atom()) == NULL)
    return VLAD_MALLOCFAILED;

  pr_api->set_name(tmp_atom, VLAD_STR_FALSE);

  pr_stage = 1;

  return VLAD_OK;
}

/* after this no more calls to add_atom() are allowed */
int smwrap::close_atom()
{
  if (pr_stage != 1)
    return VLAD_INVALIDOP;

  /* now we add the rule that the atom "true" is always true */

  pr_api->begin_rule(BASICRULE);
  pr_api->add_head(pr_api->get_atom(VLAD_STR_TRUE));
  pr_api->end_rule();

  pr_stage = 2;

  return VLAD_OK;
}

/* after this no more calls to add_rule_*() are allowed */
int smwrap::close_rule()
{
  if (pr_stage != 2)
    return VLAD_INVALIDOP;

  pr_api->done();

#ifdef VLAD_DEBUG
  pr_smod->program.print();
#endif

  pr_smod->init();

  if (!pr_smod->model())
    return VLAD_NOMODEL;

#ifdef VLAD_DEBUG
    pr_smod->printAnswer();
#endif

  pr_stage = 3;

  return VLAD_OK;
}

/* register an atom */
int smwrap::add_atom(unsigned int a_atom)
{
  Atom *tmp_atom;
  char tmp_name[VLAD_MAXLEN_NUM];

  if (pr_stage != 1)
    return VLAD_INVALIDOP;

  if ((tmp_atom = pr_api->new_atom()) == NULL)
    return VLAD_MALLOCFAILED;

  memset(tmp_name, 0, VLAD_MAXLEN_NUM);
  sprintf(tmp_name, "%d", a_atom);
  pr_api->set_name(tmp_atom, tmp_name);

  return VLAD_OK;
}

/* add an axiom (always true or always false) */
int smwrap::add_axiom(bool a_tr, unsigned int a_count, ...)
{
  unsigned int i;
  char tmp_name[VLAD_MAXLEN_NUM];
  va_list tmp_ap;

  if (pr_stage != 2)
    return VLAD_INVALIDOP;

  /* of course if the list is empty, we do nothing */
  if (a_count == 0)
    return VLAD_OK;

  if (a_tr) {
    va_start(tmp_ap, a_count);

    /* if positive, we simply add each axiom */
    for (i = 0; i < a_count; i++) {
      memset(tmp_name, 0, VLAD_MAXLEN_NUM);
      sprintf(tmp_name, "%d", va_arg(tmp_ap, unsigned int));

      pr_api->begin_rule(BASICRULE);
      pr_api->add_head(pr_api->get_atom(tmp_name));
      pr_api->add_body(pr_api->get_atom(VLAD_STR_TRUE), true);
      pr_api->end_rule();
    }

    va_end(tmp_ap);
  }
  else {
    /* false, meaning each atom in the list cannot all be true */
    pr_api->begin_rule(BASICRULE);
    pr_api->add_head(pr_api->get_atom(VLAD_STR_FALSE));

    va_start(tmp_ap, a_count);

    for (i = 0; i < a_count; i++) {
      memset(tmp_name, 0, VLAD_MAXLEN_NUM);
      sprintf(tmp_name, "%d", va_arg(tmp_ap, unsigned int));
      pr_api->add_body(pr_api->get_atom(tmp_name), true);
    }

    va_end(tmp_ap);

    pr_api->end_rule();
  }

  return VLAD_OK;
}

/* add rule: variable argument list */
int smwrap::add_rule(unsigned int a_pcount, unsigned int a_ncount, unsigned int a_head, ...)
{
  unsigned int i;
  char tmp_name[VLAD_MAXLEN_NUM];
  va_list tmp_ap;

  if (pr_stage != 2)
    return VLAD_INVALIDOP;

  /* the special case when there is no body: head becomes an axiom */
  if (a_pcount == 0 && a_ncount == 0)
    return add_axiom(true, 1, a_head);

  pr_api->begin_rule(BASICRULE);

  /* add head */
  memset(tmp_name, 0, VLAD_MAXLEN_NUM);
  sprintf(tmp_name, "%d", a_head);
  pr_api->add_head(pr_api->get_atom(tmp_name));

  va_start(tmp_ap, a_head);

  /* now for the positive body */
  for (i = 0; i < a_pcount; i++) {
    memset(tmp_name, 0, VLAD_MAXLEN_NUM);
    sprintf(tmp_name, "%d", va_arg(tmp_ap, unsigned int));
    pr_api->add_body(pr_api->get_atom(tmp_name), true);
  }

  /* then the negative body */
  for (i = 0; i < a_ncount; i++) {
    memset(tmp_name, 0, VLAD_MAXLEN_NUM);
    sprintf(tmp_name, "%d", va_arg(tmp_ap, unsigned int));
    pr_api->add_body(pr_api->get_atom(tmp_name), false);
  }

  va_end(tmp_ap);

  pr_api->end_rule();

  return VLAD_OK;
}

/* add rule: numberlist */
int smwrap::add_rule(unsigned int a_head, numberlist *a_pbody, numberlist *a_nbody)
{
  int retval;
  unsigned int i;
  char tmp_name[VLAD_MAXLEN_NUM];
  unsigned int tmp_num;

  if (pr_stage != 2)
    return VLAD_INVALIDOP;

  /* the special case when there is no body: head becomes an axiom */
  if (VLAD_LIST_LENGTH(a_pbody) == 0 && VLAD_LIST_LENGTH(a_nbody) == 0)
    return add_axiom(true, 1, a_head);

  pr_api->begin_rule(BASICRULE);

  /* add head */
  memset(tmp_name, 0, VLAD_MAXLEN_NUM);
  sprintf(tmp_name, "%d", a_head);
  pr_api->add_head(pr_api->get_atom(tmp_name));

  /* now for the positive body */
  for (i = 0; i < VLAD_LIST_LENGTH(a_pbody); i++) {
    if ((retval = a_pbody->get(i, &tmp_num)) != VLAD_OK)
      return retval;

    memset(tmp_name, 0, VLAD_MAXLEN_NUM);
    sprintf(tmp_name, "%d", tmp_num);
    pr_api->add_body(pr_api->get_atom(tmp_name), true);
  }

  /* then the negative body */
  for (i = 0; i < VLAD_LIST_LENGTH(a_nbody); i++) {
    if ((retval = a_nbody->get(i, &tmp_num)) != VLAD_OK)
      return retval;

    memset(tmp_name, 0, VLAD_MAXLEN_NUM);
    sprintf(tmp_name, "%d", tmp_num);
    pr_api->add_body(pr_api->get_atom(tmp_name), false);
  }

  pr_api->end_rule();

  return VLAD_OK;
}

/* return true, false or unknown */
int smwrap::ask(numberlist *a_exp,
                unsigned int (*a_negate)(unsigned int, unsigned int),
                unsigned int a_parm,
                unsigned char *a_res)
{
  unsigned int i;
  char tmp_name[VLAD_MAXLEN_NUM];

  if (pr_stage != 3)
    return VLAD_INVALIDOP;

  if (a_exp == NULL || a_negate == NULL || a_res == NULL)
    return VLAD_NULLPTR;

  *a_res = VLAD_RESULT_UNKNOWN;

  /* go through all the models */
  do {
    char tmp_res = VLAD_RESULT_TRUE;

    /* go through all the atoms given */
    for (i = 0; i < VLAD_LIST_LENGTH(a_exp); i++) {
      int retval;
      Atom *tmp_atom = NULL;
      unsigned int tmp_num;

      if ((retval = a_exp->get(i, &tmp_num)) != VLAD_OK) {
        *a_res = VLAD_RESULT_UNKNOWN;
        return retval;
      }

      memset(tmp_name, 0, VLAD_MAXLEN_NUM);
      sprintf(tmp_name, "%d", tmp_num);
      tmp_atom = pr_api->get_atom(tmp_name);

      /*
       * an expression is :
       *
       *   true    iff all atoms are true
       *   false   iff at least one atom is false
       *   unknown iff at least one atom is unknown and all other atoms
       *           are true
       */

      if (tmp_atom->Bpos) {
        /* atom holds in this model */
        continue;
      }
      else if (tmp_atom->Bneg) {
        char tmp_name_neg[VLAD_MAXLEN_NUM];

        /* atom doesn't hold in this model, so we try the negation */
        memset(tmp_name_neg, 0, VLAD_MAXLEN_NUM);
        sprintf(tmp_name_neg, "%d", a_negate(a_parm, tmp_num));

        if ((pr_api->get_atom(tmp_name_neg))->Bpos) {
          tmp_res = VLAD_RESULT_FALSE;
          break;
        }
        else {
          tmp_res = VLAD_RESULT_UNKNOWN;
          continue;
        }
      }
      else {
        /* atom is undefined in this model */
        tmp_res = VLAD_RESULT_UNKNOWN;
        continue;
      }
    }

    /*
     * the order in which we choose the "right" model is as follows
     * (top having the highest priority) :
     *
     * false
     * true
     * unknown
     *
     * if, upon reaching the end of evaluation of the expression in one
     * model, the result is :
     *
     *   true    -> see if it is false in other models
     *   false   -> return this result
     *   unknown -> see if it is true or false in other models
     */

    if (tmp_res == VLAD_RESULT_FALSE) {
      *a_res = VLAD_RESULT_FALSE;
      break;
    }
    else if (tmp_res == VLAD_RESULT_TRUE) {
      *a_res = VLAD_RESULT_TRUE;
      continue;
    }
    else
      continue;

  } while (pr_smod->model());

  /* of course we have to reset for the next call */
  pr_smod->revert();
  pr_smod->model();

  return VLAD_OK;
}
