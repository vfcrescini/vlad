/*
 * identifier.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <string.h>

#include <config.h>
#include <vlad.h>
#include <identifier.h>

identifier::identifier()
{
  name = NULL;
  type = 0;
}

identifier::~identifier()
{
  if (name != NULL)
    free(name);
}

bool identifier::cmp(list_item *item)
{
  identifier *tmp = dynamic_cast<identifier *> (item);

  /* we treat type == 0 and name == NULL as wildcards */

  if (tmp->name != NULL) {
    if (name == NULL)
      return false;

    if (strcmp(name, tmp->name))
      return false;
  }

  if ((tmp->type != 0) && (type != tmp->type))
    return false;

  return true;
}

int identifier::init(const char *n, unsigned char t)
{
  /* here we allow identifier instances to be created with NULL names
   * and/or 0 types */

  if (t > VLAD_IDENT_TYPEMAX)
    return VLAD_INVALIDINPUT;

  if (name != NULL)
    free(name);

  type = t;

  if (n != NULL) {
    if ((name = VLAD_STRING_MALLOC(n)) == NULL)
      return VLAD_MALLOCFAILED;
    return ((strcpy(name, n) == NULL) ? VLAD_FAILURE : VLAD_OK);
  }

  return VLAD_OK;
}

const char *identifier::get_name()
{
  return name;
}

unsigned char identifier::get_type()
{
  return type;
}
