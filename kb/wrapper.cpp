/*
 * wrapper.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <new>

#include <config.h>
#include <vlad/vlad.h>
#include <vlad/wrapper.h>

wrapper::wrapper()
{
  pr_smod = NULL;
  pr_api = NULL;
  stage = 0;
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

  if (stage > 0) {
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

  stage = 1;

  return VLAD_OK;
}

/* after this no more calls to add_atom() are allowed */
int wrapper::close_atom()
{
  if (stage != 1)
    return VLAD_FAILURE;

  /* now we add the rule that the atom "true" is always true */

  pr_api->begin_rule(BASICRULE);
  pr_api->add_head(pr_api->get_atom(VLAD_STR_TRUE));
  pr_api->end_rule();

  stage = 2;

  return VLAD_OK;
}

/* after this no more calls to add_rule_*() are allowed */
int wrapper::close_rule()
{
  if (stage != 2)
    return VLAD_FAILURE;

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

  stage = 3;

  return VLAD_OK;
}

/* register an atom */
int wrapper::add_atom(unsigned int a)
{
  Atom *tmp_atom;
  char tmp_name[VLAD_MAXLEN_NUM];

  if (stage != 1)
    return VLAD_FAILURE;

  if ((tmp_atom = pr_api->new_atom()) == NULL)
    return VLAD_MALLOCFAILED;

  sprintf(tmp_name, "%d", a);
  pr_api->set_name(tmp_atom, tmp_name);

  return VLAD_OK;
}
/* add a single axiom (always true or always false) */
int wrapper::add_axiom(unsigned int a, bool t)
{
  char tmp_name[VLAD_MAXLEN_NUM];

  if (stage != 2)
    return VLAD_FAILURE;

  sprintf(tmp_name, "%d", a);

  pr_api->begin_rule(BASICRULE);

  if (t) {
    /* body is TRUE */
    pr_api->add_head(pr_api->get_atom(tmp_name));
    pr_api->add_body(pr_api->get_atom(VLAD_STR_TRUE), true);
  }
  else {
    /* head is FALSE */
    pr_api->add_head(pr_api->get_atom(VLAD_STR_FALSE));
    pr_api->add_body(pr_api->get_atom(tmp_name), true);
  }

  pr_api->end_rule();

  return VLAD_OK;
}

/* add multiple axiom (always true or always false) */
int wrapper::add_axiom(numberlist *a, bool t)
{
  int retval;
  unsigned int i;
  unsigned int tmp_num;
  char tmp_name[VLAD_MAXLEN_NUM];

  if (stage != 2)
    return VLAD_FAILURE;

  /* of course if the list is empty, we do nothing */
  if (VLAD_LIST_LENGTH(a) == 0)
    return VLAD_OK;

  if (t) {
    /* if positive, we simply add each axiom */
    for (i = 0; i < VLAD_LIST_LENGTH(a); i++) {
      if ((retval = a->get(i, &tmp_num)) != VLAD_OK)
        return retval;
      if ((retval = add_axiom(tmp_num, true)) != VLAD_OK)
        return retval;
    }
  }
  else {
    /* false, meaning each atom in the list cannot all be true */
    pr_api->begin_rule(BASICRULE);

    pr_api->add_head(pr_api->get_atom(VLAD_STR_FALSE));

    for (i = 0; i < VLAD_LIST_LENGTH(a); i++) {
      if ((retval = a->get(i, &tmp_num)) != VLAD_OK)
        return retval;
      sprintf(tmp_name, "%d", tmp_num);
      pr_api->add_body(pr_api->get_atom(tmp_name), true);
    }  

    pr_api->end_rule();
  }

  return VLAD_OK;
}

/* add single rule (single head, pos body and neg body */
int wrapper::add_rule(unsigned int h, unsigned int pb, unsigned int nb)
{
  char tmp_name[VLAD_MAXLEN_NUM];

  if (stage != 2)
    return VLAD_FAILURE;

  pr_api->begin_rule(BASICRULE);

  /* add head */
  sprintf(tmp_name, "%d", h);
  pr_api->add_head(pr_api->get_atom(tmp_name));

  /* now for the positive body */
  sprintf(tmp_name, "%d", pb);
  pr_api->add_body(pr_api->get_atom(tmp_name), true);

  /* then the negative body */
  sprintf(tmp_name, "%d", nb);
  pr_api->add_body(pr_api->get_atom(tmp_name), false);

  pr_api->end_rule();

  return VLAD_OK;
}

/* add multiple rule (single head, pos body list and neg body list */
int wrapper::add_rule(unsigned int h, numberlist *pb, numberlist *nb)
{
  int retval;
  unsigned int i;
  char tmp_name[VLAD_MAXLEN_NUM];
  unsigned int tmp_num;

  if (stage != 2)
    return VLAD_FAILURE;

  /* the special case when pb and nb are NULL: h becomes an axiom */
  if (VLAD_LIST_LENGTH(pb) == 0 && VLAD_LIST_LENGTH(nb) == 0)
    return add_axiom(h, true);

  pr_api->begin_rule(BASICRULE);

  /* add head */
  sprintf(tmp_name, "%d", h);
  pr_api->add_head(pr_api->get_atom(tmp_name));

  /* now for the positive body */
  for (i = 0; i < VLAD_LIST_LENGTH(pb); i++) {
    if ((retval = pb->get(i, &tmp_num)) != VLAD_OK)
      return retval;

    sprintf(tmp_name, "%d", tmp_num);
    pr_api->add_body(pr_api->get_atom(tmp_name), true);
  }

  /* then the negative body */
  for (i = 0; i < VLAD_LIST_LENGTH(nb); i++) {
    if ((retval = nb->get(i, &tmp_num)) != VLAD_OK)
      return retval;

    sprintf(tmp_name, "%d", tmp_num);
    pr_api->add_body(pr_api->get_atom(tmp_name), false);
  }

  pr_api->end_rule();

  return VLAD_OK;
}

/* return true, false or unknown */
int wrapper::ask(unsigned int a, char *r)
{
  char tmp_name[VLAD_MAXLEN_NUM];

  if (stage != 3)
    return VLAD_FAILURE;

  if (r == NULL)
    return VLAD_NULLPTR;

  sprintf(tmp_name, "%d", a);

  if ((pr_api->get_atom(tmp_name)->Bpos))
    *r = VLAD_RESULT_TRUE;
  else if ((pr_api->get_atom(tmp_name)->Bneg))
    *r = VLAD_RESULT_FALSE;
  else
    *r = VLAD_RESULT_UNKNOWN;

  return VLAD_OK;
}
