/*
 * identifier.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <string.h>
#include <vlad.h>
#include "identifier.h"

identifier::identifier()
{
  name = NULL;
  type = subject;
  group = false;
}

identifier::~identifier()
{
  if (name != NULL)
    free(name);

  name = NULL;
  type = subject;
  group = false;
}

bool identifier::cmp(list_item *item)
{
  identifier *tmp = dynamic_cast<identifier *> (item);

  /* compare only the name */
  return (strcmp(name, tmp->name) ? false : true);
}

int identifier::init(const char *n, ident_type t, bool g)
{
  if (n == NULL)
    return VLAD_NULLPTR;

  if (name != NULL)
    free(name);

  if ((name = VLAD_STRING_MALLOC(n)) == NULL)
    return VLAD_MALLOCFAILED;

  type = t;
  group = g;

  return ((strcpy(name, n) == NULL) ? VLAD_FAILURE : VLAD_OK);
}

const char *identifier::get_name()
{
  return name;
}

ident_type identifier::get_type()
{
  return type;
}

bool identifier::get_group()
{
  return group;
}
	
