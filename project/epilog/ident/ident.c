/* ident.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <string.h>
#include "ident.h"

/* create a new ident structure. allocate space for name */
int ident_create(ident_type **ident, char *name, unsigned short type)
{
  if (ident == NULL || name == NULL)
    return -1;

  if ((*ident = (ident_type *) malloc(sizeof(ident_type))) == NULL)
    return -1;

  if (((*ident)->name = (char *) malloc(sizeof(char) * (strlen(name) + 1))) == NULL) {
    free(*ident);
    return -1;
  }

  strcpy((*ident)->name, name);
  (*ident)->type = type;

  return 0;
}

/* free ident struct, including allocated space for name */
void ident_destroy(ident_type *ident)
{
  if (ident != NULL) {
    if (ident->name != NULL)
      free(ident->name);

    free(ident);
  }
}

/* returns 0 if the contents of ident1 and ident2 are identical */
int ident_compare(ident_type ident1, ident_type ident2)
{
  if (ident1.type != ident2.type)
    return -1;

  return (strcmp(ident1.name, ident2.name) ? -1 : 0);
}
