/*
 * transtab.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <string.h>
#include <simplelist.h>
#include "transtab.h"

int transtab_compare(void *p1, void *p2);
int transtab_destroy(void *p);

simplelist_type list;

/* initialise list */
int transtab_init(void)
{
  return simplelist_init(&list);
}

/* add transformation into table */
int transtab_add(transdef_type trans)
{
  transdef_type *new_trans = NULL;
  
  if (simplelist_find_data(list, (void *) &trans, transtab_compare) == 0)
    return -1;

  if (transdef_copy(&new_trans, trans) != 0)
    return -1;

  return simplelist_add(&list, (void *) new_trans);
}

/* gives the length of the list */
int transtab_length(unsigned int *len)
{
  return simplelist_length(list, len);
}

/* gives a reference to a transdef_type structure */
int transtab_get(char *name, transdef_type **trans)
{
  simplelist_type tmp_list;
  transdef_type tmp_trans;

  if (name == NULL || trans == NULL)
    return -1;

  tmp_trans.name = name;

  if (simplelist_get_data(list, (void *) &tmp_trans, &tmp_list, transtab_compare) != 0)
    return -1;

  /* we know that there are no duplicates in this list so either the resulting
   * list is empty or the transformation we're looking for is the first and
   * only entry. */

  if (simplelist_get_index(tmp_list, 0, (void **) trans) != 0)
    return -1;

  return simplelist_del_index(&tmp_list, 0, NULL);
}

/* returns 0 if the transformation name is in the list */
int transtab_find(char *name)
{
  transdef_type tmp_trans;

  if ((tmp_trans.name = name) == NULL)
    return -1;

  return simplelist_find_data(list,
                              (void *) &tmp_trans,
                              transtab_compare);
}

/* empty the list */
int transtab_purge(void)
{
  unsigned int len;
  unsigned int i;
  
  if (simplelist_length(list, &len) != 0)
    return -1;
    
  for (i = 0; i < len; i++)
    if (simplelist_del_index(&list, 0, transtab_destroy) != 0)
      return -1;

  return 0;
}

/* compare ONLY THE NAME component of p1 and p2 */
int transtab_compare(void *p1, void *p2)
{
  if (p1 == NULL || p2 == NULL)
    return -1;

  return strcmp(((transdef_type *) p1)->name, ((transdef_type *) p2)->name);
}

/* free memory */
int transtab_destroy(void *p)
{
  return transdef_destroy((transdef_type *) p);
}
