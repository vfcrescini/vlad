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
  initialised = false;
  stage1 = false;
  stage2 = false;
}

kb::~kb()
{
  if (stable != NULL)
    delete stable;
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

  initialised = true;
  stage1 = false;
  stage2 = false;

  return VLAD_OK;
}

/* after this is called, no further calls to add_inittab() is allowed */
int kb::close_symtab()
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  stage1 = true;

  return VLAD_OK;
}

/* after this is called, no further calls to add_inittab() is allowed */
int kb::close_inittab()
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  if (!stage1)
    return VLAD_FAILURE;

  stage2 = true;

  return VLAD_OK;
}

/* register an identifier in the kb */
int kb::add_symtab(const char *n, unsigned char t)
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  if (stage1)
    return VLAD_FAILURE;

  return stable->add(n, t);
}
