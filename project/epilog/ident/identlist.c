/*
 * identlist.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <string.h>
#include "identlist.h"

int identlist_compare(void *p1, void *p2);
int identlist_destroy(void *p);

/* initialise list */
int identlist_init(identlist_type *list)
{
  return simplelist_init(list);
}

/* return 0 if name is already used */
int identlist_find(identlist_type list, char *name)
{
  ident_type tmp_ident;

  if ((tmp_ident.name = name) == NULL)
    return -1;

  return simplelist_find_data(list,
                              (void *) &tmp_ident,
                              identlist_compare);
}

/* add identifier */
int identlist_add_new(identlist_type *list, char *name, unsigned short type)
{
  ident_type *new_ident = NULL;
  
  if (list == NULL || name == NULL)
    return -1;

  if (ident_create(&new_ident, name, type) != 0)
    return -1;

  return simplelist_add(list, (void *) new_ident);
}

/* add ref without allocating mem */
int identlist_add_ref(identlist_type *list, ident_type *ident)
{
  if (list == NULL || ident == NULL)
    return -1;

  return simplelist_add(list, (void *) ident);
}

/* get identifier structure based on name */
int identlist_get(identlist_type list, char *name, ident_type **ident)
{
  ident_type tmp_ident;

  if (name == NULL || ident == NULL)
    return -1;

  tmp_ident.name = name;

  return simplelist_get_data(list, 
                             (void *) &tmp_ident,
                             (void **) ident,
                             identlist_compare);
}

/* delete identifier entry based on name */
int identlist_del(identlist_type *list, char *name)
{
  ident_type tmp_ident;

  if (list == NULL || name == NULL)
    return -1;

  tmp_ident.name = name;

  return simplelist_del_data(list,
                             (void *) &tmp_ident,
                             identlist_compare,
                             identlist_destroy);
}

/* delete all entries */
int identlist_purge(identlist_type *list)
{
  unsigned int len;
  unsigned int i;

  if (list == NULL)
    return -1;

  if (simplelist_length(*list, &len) != 0)
    return -1;

  for (i = 0; i < len; i++)
    if (simplelist_del_index(list, 0, NULL) != 0)
      return -1;

  return 0;
}

/* delete all entries and free members */
int identlist_purge_all(identlist_type *list)
{
  unsigned int len;
  unsigned int i;

  if (list == NULL)
    return -1;

  if (simplelist_length(*list, &len) != 0)
    return -1;

  for (i = 0; i < len; i++)
    if (simplelist_del_index(list, 0, identlist_destroy) != 0)
      return -1;

  return 0;
}

/* return 0 if the lists are equivalent */
int identlist_cmp(identlist_type l1, identlist_type l2)
{
  ident_type *i1;
  ident_type *i2;
  unsigned int len1;
  unsigned int len2;
  unsigned int i;
  
  if (simplelist_length(l1, &len1) != 0 ||
      simplelist_length(l2, &len2) != 0 ||
      len1 != len2)
    return -1;

  for (i = 0; i < len1; i++) {
    if (simplelist_get_index(l1, i, (void **) &i1) != 0 ||
        simplelist_get_index(l2, i, (void **) &i2) != 0)
      return -1;

    if (ident_compare(*i1, *i2) != 0)
      return -1;
  }

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
