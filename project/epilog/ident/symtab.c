/*
 * symtab.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <string.h>
#include <simplelist.h>
#include "symtab.h"

int symtab_compare(void *p1, void *p2);
int symtab_destroy(void *p);

simplelist_type list;

/* initialise list */
int symtab_init(void)
{
  return simplelist_init(&list);
}

/* return 0 if name is already in the list */
int symtab_find(char *name)
{
  ident_type tmp_ident;

  if ((tmp_ident.name = name) == NULL)
    return -1;

  return simplelist_find_data(list,
                              (void *) &tmp_ident,
                              symtab_compare);
}

/* add identifier */
int symtab_add(char *name, unsigned short type)
{
  ident_type *new_ident = NULL;
  
  if (name == NULL)
    return -1;

  if (ident_create(&new_ident, name, type) != 0)
    return -1;

  if (simplelist_find_data(list, (void *) new_ident, symtab_compare) == 0)
    return -1;

  return simplelist_add(&list, (void *) new_ident);
}

/* get identifier structure based on name */
int symtab_get(char *name, ident_type **ident)
{
  simplelist_type tmp_list;
  ident_type tmp_ident;

  if (name == NULL || ident == NULL)
    return -1;

  tmp_ident.name = name;
  
  if (simplelist_get_data(list, &tmp_list, (void *) &tmp_ident, symtab_compare) != 0)
    return -1;
  
  /* we know that there are no duplicates in this list so either the resulting
   * list is empty or the identifier we're looking for is the first and only
   * entry. */
  if (simplelist_get_index(tmp_list, 0, (void **) ident) != 0)
    return -1;
	
  return simplelist_del_index(&tmp_list, 0, NULL);
}

/* delete identifier entry based on name */
int symtab_del(char *name)
{
  ident_type tmp_ident;

  if (name == NULL)
    return -1;

  tmp_ident.name = name;

  return simplelist_del_data(&list,
                             (void *) &tmp_ident,
                             symtab_compare,
                             symtab_destroy);
}

/* delete all entries and free everything */
int symtab_purge(void)
{
  unsigned int len;
  unsigned int i;

  if (simplelist_length(list, &len) != 0)
    return -1;

  for (i = 0; i < len; i++)
    if (simplelist_del_index(&list, 0, symtab_destroy) != 0)
      return -1;

  return 0;
}

/* compare ONLY THE NAME component of p1 and p2 */
int symtab_compare(void *p1, void *p2)
{
  if (p1 == NULL || p2 == NULL)
    return -1;

  return strcmp(((ident_type *) p1)->name, ((ident_type *) p2)->name) ? -1 : 0;
}

/* free memory */
int symtab_destroy(void *p)
{
  return ident_destroy((ident_type *) p);
}
