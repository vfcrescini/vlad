/*
 * wrapper.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <new>

#include <config.h>
#include <vlad/vlad.h>
#include <vlad/wrapper.h>

wrapper::wrapper()
{
  pr_smod = NULL;
  pr_api = NULL;
  pr_stage = 0;
}

wrapper::~wrapper()
{
  if (pr_smod != NULL)
    delete pr_smod;
  if (pr_api != NULL)
    delete pr_api;
}

int wrapper::init()
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
int wrapper::close_atom()
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
int wrapper::close_rule()
{
  if (pr_stage != 2)
    return VLAD_INVALIDOP;

  pr_api->done();

#ifdef DEBUG
  pr_smod->program.print();
#endif

  pr_smod->init();

  if (!pr_smod->model())
    return VLAD_NOMODEL;

#ifdef DEBUG
    pr_smod->printAnswer();
#endif

  pr_stage = 3;

  return VLAD_OK;
}

/* register an atom */
int wrapper::add_atom(unsigned int a_atom)
{
  Atom *tmp_atom;
  char tmp_name[VLAD_MAXLEN_NUM];

  if (pr_stage != 1)
    return VLAD_INVALIDOP;

  if ((tmp_atom = pr_api->new_atom()) == NULL)
    return VLAD_MALLOCFAILED;

  sprintf(tmp_name, "%d", a_atom);
  pr_api->set_name(tmp_atom, tmp_name);

  return VLAD_OK;
}

/* add an axiom (always true or always false) */
int wrapper::add_axiom(bool a_tr, unsigned int a_count, ...)
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
      sprintf(tmp_name, "%d", va_arg(tmp_ap, unsigned int));
      pr_api->add_body(pr_api->get_atom(tmp_name), true);
    }

    va_end(tmp_ap);

    pr_api->end_rule();
  }

  return VLAD_OK;
}

/* add rule: variable argument list */
int wrapper::add_rule(unsigned int a_pcount, unsigned int a_ncount, unsigned int a_head, ...)
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
  sprintf(tmp_name, "%d", a_head);
  pr_api->add_head(pr_api->get_atom(tmp_name));

  va_start(tmp_ap, a_head);

  /* now for the positive body */
  for (i = 0; i < a_pcount; i++) {
    sprintf(tmp_name, "%d", va_arg(tmp_ap, unsigned int));
    pr_api->add_body(pr_api->get_atom(tmp_name), true);
  }

  /* then the negative body */
  for (i = 0; i < a_ncount; i++) {
    sprintf(tmp_name, "%d", va_arg(tmp_ap, unsigned int));
    pr_api->add_body(pr_api->get_atom(tmp_name), false);
  }

  va_end(tmp_ap);

  pr_api->end_rule();

  return VLAD_OK;
}

/* add rule: numberlist */
int wrapper::add_rule(unsigned int a_head, numberlist *a_pbody, numberlist *a_nbody)
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
  sprintf(tmp_name, "%d", a_head);
  pr_api->add_head(pr_api->get_atom(tmp_name));

  /* now for the positive body */
  for (i = 0; i < VLAD_LIST_LENGTH(a_pbody); i++) {
    if ((retval = a_pbody->get(i, &tmp_num)) != VLAD_OK)
      return retval;

    sprintf(tmp_name, "%d", tmp_num);
    pr_api->add_body(pr_api->get_atom(tmp_name), true);
  }

  /* then the negative body */
  for (i = 0; i < VLAD_LIST_LENGTH(a_nbody); i++) {
    if ((retval = a_nbody->get(i, &tmp_num)) != VLAD_OK)
      return retval;

    sprintf(tmp_name, "%d", tmp_num);
    pr_api->add_body(pr_api->get_atom(tmp_name), false);
  }

  pr_api->end_rule();

  return VLAD_OK;
}

/* return true, false or unknown */
int wrapper::ask(unsigned int a_atom, char *a_result)
{
  char tmp_name[VLAD_MAXLEN_NUM];

  if (pr_stage != 3)
    return VLAD_INVALIDOP;

  if (a_result == NULL)
    return VLAD_NULLPTR;

  sprintf(tmp_name, "%d", a_atom);

  if ((pr_api->get_atom(tmp_name)->Bpos)) {
    /* positive in the first model so we check all other models */
    while (pr_smod->model()) {
      if ((pr_api->get_atom(tmp_name)->Bpos))
        continue;
      else {
        *a_result = VLAD_RESULT_UNKNOWN;
        break;
      }
    }
    *a_result = VLAD_RESULT_TRUE;
  }
  else if ((pr_api->get_atom(tmp_name)->Bneg)) {
    /* negative in the first model so we check all other models */
    while (pr_smod->model()) {
      if ((pr_api->get_atom(tmp_name)->Bneg))
        continue;
      else {
        *a_result = VLAD_RESULT_UNKNOWN;
        break;
      }
    }
    *a_result = VLAD_RESULT_FALSE;
  }
  else
    *a_result = VLAD_RESULT_UNKNOWN;

  /* of course we have to reset for the next call */
  pr_smod->revert();
  pr_smod->model();

  return VLAD_OK;
}
