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
  initialised = false;
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

  if (initialised) {
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

  initialised = true;

  return VLAD_OK;
}

int wrapper::add_atom(unsigned int a)
{
  Atom *tmp_atom;
  char tmp_name[VLAD_MAXLEN_NUM];

  if (!initialised)
    return VLAD_UNINITIALISED;

  if ((tmp_atom = pr_api->new_atom()) == NULL)
    return VLAD_MALLOCFAILED;

  sprintf(tmp_name, "%d", a);
  pr_api->set_name(tmp_atom, tmp_name);

  return VLAD_OK;
}

/* add rule with constant head */
int wrapper::add_rule_const_head(numberlist *pbody, numberlist *nbody, bool h)
{
  int retval;
  unsigned int i;
  char tmp_name[VLAD_MAXLEN_NUM];
  unsigned int tmp_num;

  if (!initialised)
    return VLAD_UNINITIALISED;

  pr_api->begin_rule(BASICRULE);

  /* add head */
  pr_api->add_head(pr_api->get_atom(VLAD_BOOL_STRING(h)));

  /* now for the positive body */
  for (i = 0; i < VLAD_LIST_LENGTH(pbody); i++) {
    if ((retval = pbody->get(i, &tmp_num)) != VLAD_OK)
      return retval;

    sprintf(tmp_name, "%d", tmp_num);
    pr_api->add_body(pr_api->get_atom(tmp_name), true);
  }

  /* then the negative body */
  for (i = 0; i < VLAD_LIST_LENGTH(nbody); i++) {
    if ((retval = nbody->get(i, &tmp_num)) != VLAD_OK)
      return retval;

    sprintf(tmp_name, "%d", tmp_num);
    pr_api->add_body(pr_api->get_atom(tmp_name), false);
  }

  pr_api->end_rule();

  return VLAD_OK;
}

/* add rule with constant body */
int wrapper::add_rule_const_body(numberlist *head, bool b)
{
  int retval;
  unsigned int i;
  char tmp_name[VLAD_MAXLEN_NUM];
  unsigned int tmp_num;

  if (!initialised)
    return VLAD_UNINITIALISED;

  pr_api->begin_rule(BASICRULE);

  /* add head */
  for (i = 0; i < VLAD_LIST_LENGTH(head); i++) {
    if ((retval = head->get(i, &tmp_num)) != VLAD_OK)
      return retval;

    sprintf(tmp_name, "%d", tmp_num);
    pr_api->add_head(pr_api->get_atom(tmp_name));
  }

  pr_api->add_body(pr_api->get_atom(VLAD_BOOL_STRING(b)), true);
  pr_api->end_rule();

  return VLAD_OK;
}

/* add rule with a single head */
int wrapper::add_rule_single_head(unsigned int head, numberlist *pbody, numberlist *nbody)
{
  int retval;
  unsigned int i;
  char tmp_name[VLAD_MAXLEN_NUM];
  unsigned int tmp_num;

  if (!initialised)
    return VLAD_UNINITIALISED;

  pr_api->begin_rule(BASICRULE);

  /* add head */
  sprintf(tmp_name, "%d", head);
  pr_api->add_head(pr_api->get_atom(tmp_name));

  /* add positive body */
  for (i = 0; i < VLAD_LIST_LENGTH(pbody); i++) {
    if ((retval = pbody->get(i, &tmp_num)) != VLAD_OK)
      return retval;

    sprintf(tmp_name, "%d", tmp_num);
    pr_api->add_body(pr_api->get_atom(tmp_name), true);
  }

  /* add negative body */
  for (i = 0; i < VLAD_LIST_LENGTH(nbody); i++) {
    if ((retval = nbody->get(i, &tmp_num)) != VLAD_OK)
      return VLAD_OK;

    sprintf(tmp_name, "%d", tmp_num);
    pr_api->add_body(pr_api->get_atom(tmp_name), false);
  }

  pr_api->end_rule();

  return VLAD_OK;
}
