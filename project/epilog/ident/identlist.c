/*
 * identlist.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <string.h>
#include "identlist.h"

int identlist_compare(void *p1, void *p2);
void identlist_destroy(void *p);

/* initialise list */
int identlist_init(identlist_type *list)
{
  return simplelist_init(list);
}

/* gives the number of identifiers in the list */
unsigned int identlist_length(identlist_type list)
{
  return simplelist_length(list);
}

/* return 0 if name is already used */
int identlist_find(identlist_type list, char *name)
{
  ident_type tmp_ident;

  if ((tmp_ident.name = name) == NULL)
    return EPI_NULLPTR;

  return simplelist_find_data(list,
                              (void *) &tmp_ident,
                              identlist_compare);
}

/* add identifier */
int identlist_add(identlist_type *list, ident_type *ident)
{
  return simplelist_add(list, (void *) ident);
}

/* get identifier structure based on index */
int identlist_get(identlist_type list, unsigned int index, ident_type **ident)
{
  return simplelist_get_index(list, index, (void **) ident);
}

/* delete identifier entry based on index but not free the indentifier */
int identlist_del(identlist_type *list, unsigned int index)
{
  return simplelist_del_index(list, index, NULL);
}

/* delete all entries but not free the identifiers */
void identlist_purge(identlist_type *list)
{
  simplelist_purge(list, NULL);
}

/* compare ONLY THE NAME component of p1 and p2 */
int identlist_compare(void *p1, void *p2)
{
  if (p1 == NULL || p2 == NULL)
    return -1;

  return strcmp(((ident_type *) p1)->name, ((ident_type *) p2)->name) ? -1 : 0;
}

/* free memory */
void identlist_destroy(void *p)
{
  ident_destroy((ident_type *) p);
}
