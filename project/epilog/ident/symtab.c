/*
 * symtab.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <string.h>
#include <simplelist.h>
#include "symtab.h"

int symtab_compare(void *p1, void *p2);
void symtab_destroy(void *p);

static simplelist_type list;

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
    return EPI_NULLPTR;

  return simplelist_find_data(list,
                              (void *) &tmp_ident,
                              symtab_compare);
}

/* add identifier */
int symtab_add(char *name, unsigned short type)
{
  int tmp_res;
  ident_type *new_ident = NULL;
  
  if ((tmp_res = ident_create(&new_ident, name, type)) != EPI_OK)
    return tmp_res;

  if ((tmp_res = simplelist_find_data(list, (void *) new_ident, symtab_compare)) == EPI_OK)
    return EPI_DUPLICATE;

  if (tmp_res != EPI_NOTFOUND)
    return tmp_res;

  return simplelist_add(&list, (void *) new_ident);
}

/* get identifier structure based on name */
int symtab_get(char *name, ident_type **ident)
{
  simplelist_type tmp_list;
  ident_type tmp_ident;
  int tmp_res;

  if (name == NULL || ident == NULL)
    return EPI_NULLPTR;

  tmp_ident.name = name;
  
  if ((tmp_res = simplelist_get_data(list, &tmp_list, (void *) &tmp_ident, symtab_compare)) != EPI_OK)
    return tmp_res;

  /* we know that there are no duplicates in this list so either the resulting
   * list is empty or the identifier we're looking for is the first and only
   * entry. */
  if ((tmp_res = simplelist_get_index(tmp_list, 0, (void **) ident)) != EPI_OK)
    return tmp_res;

  simplelist_purge(&tmp_list, NULL);

  return EPI_OK;
}

/* delete identifier entry based on name */
int symtab_del(char *name)
{
  ident_type tmp_ident;

  if (name == NULL)
    return EPI_NULLPTR;

  tmp_ident.name = name;

  return simplelist_del_data(&list,
                             (void *) &tmp_ident,
                             symtab_compare,
                             symtab_destroy);
}

/* delete all entries and free everything */
void symtab_purge(void)
{
  simplelist_purge(&list, symtab_destroy);
}

/* compare ONLY THE NAME component of p1 and p2 */
int symtab_compare(void *p1, void *p2)
{
  if (p1 == NULL || p2 == NULL)
    return -1;

  return strcmp(((ident_type *) p1)->name, ((ident_type *) p2)->name) ? -1 : 0;
}

/* free memory */
void symtab_destroy(void *p)
{
  ident_destroy((ident_type *) p);
}
