/*
 * stringlist.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <string.h>
#include "stringlist.h"

int stringlist_compare(void *p1, void *p2);
int stringlist_destroy(void *p);

/* initialise list */
int stringlist_init(stringlist_type *list)
{
  return simplelist_init(list);
}

/* gives the number of strings in the stringlist */
int stringlist_length(stringlist_type list, unsigned int *len)
{
  return simplelist_length(list, len);
}

/* return 0 if the string is in the stringlist */
int stringlist_find(stringlist_type list, char *string)
{
  return simplelist_find_data(list, (void *) string, stringlist_compare);
}

/* gives a reference to the index'th string in the stringlist */
int stringlist_get(stringlist_type list, unsigned int index, char **string)
{
  return simplelist_get_index(list, index, (void **) string);
}

/* add an string into the stringlist */
int stringlist_add(stringlist_type *list, char *string)
{
  char *new_string = NULL;

  if (list == NULL || string == NULL)
    return -1;

  if ((new_string = (char *) malloc(sizeof(char) * strlen(string))) == NULL)
    return -1;

  strcpy(new_string, string);

  return simplelist_add(list, (void *) new_string);
}

/* delete an string from the stringlist */
int stringlist_del(stringlist_type *list, char *string)
{
  if (list == NULL)
    return -1;

  return simplelist_del_data(list,
                             (void *) string,
                             stringlist_compare,
                             stringlist_destroy);
}

/* delete all strings from this stringlist */
int stringlist_purge(stringlist_type *list)
{
  unsigned int len;
  unsigned int i;

  if (list == NULL)
    return -1;

  if (simplelist_length(*list, &len) != 0)
    return -1;

  for (i = 0; i < len; i++)
    if (simplelist_del_index(list, 0, stringlist_destroy) != 0)
      return -1;

  return 0;
}

/* returns 0 if the ATOMS pointed to by p1 and p2 are equivalent */
int stringlist_compare(void *p1, void *p2)
{
  if (p1 == NULL || p2 == NULL)
    return -1;

  return strcmp((char *) p1, (char *) p2);
}

/* free memory */
int stringlist_destroy(void *p)
{
  if (p == NULL)
    return -1;

  free(p);

  return 0;
}
