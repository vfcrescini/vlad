/*
 * wrapper.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
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

  initialised = true;

  return VLAD_OK;
}
