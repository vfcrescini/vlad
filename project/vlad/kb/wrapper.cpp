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

  stage = 2;

  return VLAD_OK;
}

/* after this no more calls to add_rule_*() are allowed */
int wrapper::close_rule()
{
  if (stage != 2)
    return VLAD_FAILURE;

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

/* constant head & single body */
int wrapper::add_rule_chead_sbody(bool h, unsigned int pb, unsigned int nb)
{
  char tmp_name[VLAD_MAXLEN_NUM];

  if (stage != 2)
    return VLAD_FAILURE;

  pr_api->begin_rule(BASICRULE);

  /* add head */
  pr_api->add_head(pr_api->get_atom(VLAD_BOOL_STRING(h)));

  /* now for the positive body */
  sprintf(tmp_name, "%d", pb);
  pr_api->add_body(pr_api->get_atom(tmp_name), true);

  /* then the negative body */
  sprintf(tmp_name, "%d", nb);
  pr_api->add_body(pr_api->get_atom(tmp_name), false);

  pr_api->end_rule();

  return VLAD_OK;
}

/* constant head & multiple body */
int wrapper::add_rule_chead_mbody(bool h, numberlist *pb, numberlist *nb)
{
  int retval;
  unsigned int i;
  char tmp_name[VLAD_MAXLEN_NUM];
  unsigned int tmp_num;

  if (stage != 2)
    return VLAD_FAILURE;

  pr_api->begin_rule(BASICRULE);

  /* add head */
  pr_api->add_head(pr_api->get_atom(VLAD_BOOL_STRING(h)));

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

/* single head & constant body */
int wrapper::add_rule_shead_cbody(unsigned int h, bool b)
{
  char tmp_name[VLAD_MAXLEN_NUM];

  if (stage != 2)
    return VLAD_FAILURE;

  pr_api->begin_rule(BASICRULE);

  /* add head */
  sprintf(tmp_name, "%d", h);
  pr_api->add_head(pr_api->get_atom(tmp_name));

  /* now for the body */
  pr_api->add_body(pr_api->get_atom(VLAD_BOOL_STRING(b)), true);

  pr_api->end_rule();

  return VLAD_OK;
}

/* single head & single body */
int wrapper::add_rule_shead_sbody(unsigned int h, unsigned int pb, unsigned int nb)
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

/* single head & multiple body */
int wrapper::add_rule_shead_mbody(unsigned int h, numberlist *pb, numberlist *nb)
{
  int retval;
  unsigned int i;
  char tmp_name[VLAD_MAXLEN_NUM];
  unsigned int tmp_num;

  if (stage != 2)
    return VLAD_FAILURE;

  pr_api->begin_rule(BASICRULE);

  /* add head */
  sprintf(tmp_name, "%d", h);
  pr_api->add_body(pr_api->get_atom(tmp_name), true);

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

/* multiple head & constant body */
int wrapper::add_rule_mhead_cbody(numberlist *h, bool b)
{
  int retval;
  unsigned int i;
  char tmp_name[VLAD_MAXLEN_NUM];
  unsigned int tmp_num;

  if (stage != 2)
    return VLAD_FAILURE;

  pr_api->begin_rule(CHOICERULE);

  /* add head */
  for (i = 0; i < VLAD_LIST_LENGTH(h); i++) {
    if ((retval = h->get(i, &tmp_num)) != VLAD_OK)
      return retval;

    sprintf(tmp_name, "%d", tmp_num);
    pr_api->add_head(pr_api->get_atom(tmp_name));
  }

  pr_api->add_body(pr_api->get_atom(VLAD_BOOL_STRING(b)), true);

  pr_api->end_rule();

  return VLAD_OK;
}

/* multiple head & single body */
int wrapper::add_rule_mhead_sbody(numberlist *h, unsigned int pb, unsigned int nb)
{
  int retval;
  unsigned int i;
  char tmp_name[VLAD_MAXLEN_NUM];
  unsigned int tmp_num;

  if (stage != 2)
    return VLAD_FAILURE;

  pr_api->begin_rule(CHOICERULE);

  /* add head */
  for (i = 0; i < VLAD_LIST_LENGTH(h); i++) {
    if ((retval = h->get(i, &tmp_num)) != VLAD_OK)
      return retval;

    sprintf(tmp_name, "%d", tmp_num);
    pr_api->add_head(pr_api->get_atom(tmp_name));
  }

  /* positive body */
  sprintf(tmp_name, "%d", pb);
  pr_api->add_body(pr_api->get_atom(tmp_name), true);

  /* negative body */
  sprintf(tmp_name, "%d", nb);
  pr_api->add_body(pr_api->get_atom(tmp_name), false);

  pr_api->end_rule();

  return VLAD_OK;
}

/* multiple head & multiple body */
int wrapper::add_rule_mhead_mbody(numberlist *h, numberlist *pb, numberlist *nb)
{
  int retval;
  unsigned int i;
  char tmp_name[VLAD_MAXLEN_NUM];
  unsigned int tmp_num;

  if (stage != 2)
    return VLAD_FAILURE;

  pr_api->begin_rule(CHOICERULE);

  /* add head */
  for (i = 0; i < VLAD_LIST_LENGTH(h); i++) {
    if ((retval = h->get(i, &tmp_num)) != VLAD_OK)
      return retval;

    sprintf(tmp_name, "%d", tmp_num);
    pr_api->add_head(pr_api->get_atom(tmp_name));
  }

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
