/*
 * stringlist.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <string.h>
#include "stringlist.h"

int stringlist_compare(void *p1, void *p2);
void stringlist_destroy(void *p);

/* initialise list */
int stringlist_init(stringlist_type *list)
{
  return simplelist_init(list);
}

/* gives the number of strings in the stringlist */
unsigned int stringlist_length(stringlist_type list)
{
  return simplelist_length(list);
}

/* return 0 if the string is in the stringlist */
int stringlist_find(stringlist_type list, char *string)
{
  return simplelist_find_data(list, (void *) string, stringlist_compare);
}

/* gives the index of the string given, if in the list */
int stringlist_index(stringlist_type list, char *string, unsigned int *index)
{
  return simplelist_index(list, (void *) string, index, stringlist_compare);
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
    return EPI_NULLPTR;

  if ((new_string = (char *) malloc(sizeof(char) * (strlen(string) + 1))) == NULL)
    return EPI_MALLOCFAILED;

  strcpy(new_string, string);

  return simplelist_add(list, (void *) new_string);
}

/* delete all instances of string in the list */
int stringlist_del(stringlist_type *list, char *string)
{
  return simplelist_del_data(list,
                             (void *) string,
                             stringlist_compare,
                             stringlist_destroy);
}

/* delete all strings from this stringlist */
void stringlist_purge(stringlist_type *list)
{
  simplelist_purge(list, stringlist_destroy);
}

/* returns 0 if the strings pointed to by p1 and p2 are equivalent */
int stringlist_compare(void *p1, void *p2)
{
  if (p1 == NULL || p2 == NULL)
    return -1;

  return strcmp((char *) p1, (char *) p2) ? -1 : 0;
}

/* free memory */
void stringlist_destroy(void *p)
{
  if (p != NULL)
    free(p);
}
