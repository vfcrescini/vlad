/*
 * identlist.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <string.h>
#include <simplelist.h>
#include "ident.h"

int identlist_compare(void *p1, void *p2);
int identlist_destroy(void *p);

static simplelist_type list;

/* initialise list */
int identlist_init()
{
  return simplelist_init(&list);
}

/* return 0 if name is already used */
int identlist_find(char *name)
{
  int found = -1;
  ident_type *temp_ident = NULL;

  if (name == NULL)
    return -1;

  if (ident_create(&temp_ident, name, 0) != 0)
    return -1;

  found = simplelist_find_data(list, 
                               (void *) temp_ident,
                               identlist_compare);

  ident_destroy(temp_ident);

  return found;
}

/* add identifier */
int identlist_add(char *name, unsigned short type)
{
  ident_type *new_ident = NULL;
  
  if (name == NULL)
    return -1;

  if (ident_create(&new_ident, name, type) != 0)
    return -1;

  return simplelist_add(&list, (void *) new_ident);
}

/* get identifier structure based on name */
int identlist_get(char *name, ident_type *ident)
{
  ident_type *temp_ident = NULL;

  if (name == NULL || ident == NULL)
    return -1;

  if (ident_create(&temp_ident, name, 0) != 0)
    return -1;

  if (simplelist_get_data(list, 
                          (void *) temp_ident,
                          (void **) &ident,
                          identlist_compare)) {
    ident_destroy(temp_ident);
    return -1;
  }

  return ident_destroy(temp_ident);
}

/* delete identifier entry based on name */
int identlist_del(char *name)
{
  ident_type *temp_ident = NULL;

  if (name == NULL)
    return -1;

  if (ident_create(&temp_ident, name, 0) != 0)
    return -1;

  if (simplelist_del_data(&list,
                          (void *) temp_ident,
                          identlist_compare,
                          identlist_destroy)) {
    ident_destroy(temp_ident);
    return -1;
  }

  return ident_destroy(temp_ident);
}

/* delete all entries */
int identlist_purge()
{
  unsigned int len;
  unsigned int i;

  if (simplelist_length(list, &len) != 0)
    return -1;

  for (i = 0; i < len; i++)
    if (simplelist_del_index(&list, i, identlist_destroy) != 0)
      return -1;

  return 0;
}

/* compare ONLY THE NAME component of p1 and p2 */
int identlist_compare(void *p1, void *p2)
{
  if (p1 == NULL || p2 == NULL)
    return -1;

  return strcmp(((ident_type *) p1)->name, ((ident_type *) p2)->name);
}

/* free memory */
int identlist_destroy(void *p)
{
  return ident_destroy((ident_type *) p);
}
