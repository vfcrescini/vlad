/*
 * name.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <string.h>
#include "name.h"

/* composes a variable name_type, allocates space for the string */
int name_create_var(char *string, name_type *name)
{
  char *tmp_string = NULL;
  
  if (string == NULL || name == NULL)
    return -1;

  if ((tmp_string = (char *) malloc(sizeof(char) * (strlen(string) + 1))) == NULL)
    return -1;

  name->type = EPI_NAME_VAR;
  name->name.var = tmp_string;

  strcpy(name->name.var, string);

  return 0;
}

/* composes an ident name_type */
int name_create_ident(ident_type *ident, name_type *name)
{
  if (ident == NULL || name == NULL)
    return -1;

  name->type = EPI_NAME_IDENT;
  name->name.ident = ident;

  return 0;
}

/* copies a name_type */
int name_copy(name_type source, name_type *dest)
{
  if (dest == NULL)
    return -1;

  if (EPI_NAME_IS_VAR(source))
    return name_create_var(source.name.var, dest);
  
  if (EPI_NAME_IS_IDENT(source))
    return name_create_ident(source.name.ident, dest);

  return -1;
}

/* returns 0 if the two names are equivalent */
int name_compare(name_type one, name_type two)
{
  if (one.type != two.type)
    return -1;

  if (EPI_NAME_IS_VAR(one))
    return (strcmp(one.name.var, two.name.var) ? -1 : 0);

  if (EPI_NAME_IS_IDENT(one))
    return (ident_compare(*(one.name.ident), *(two.name.ident)));

  return -1;
}

/* destroy name. only var name strings are freed */
int name_destroy(name_type name)
{
  if (EPI_NAME_IS_IDENT(name))
    return 0;

  if (EPI_NAME_IS_VAR(name)) {
    if (name.name.var != NULL)
      free(name.name.var);
    return 0;
  }

  return -1;
}
