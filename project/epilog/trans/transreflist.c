/*
 * transreflist.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <string.h>
#include "transreflist.h"

int transreflist_destroy(void *p);

/* initialise list */
int transreflist_init(transreflist_type *list)
{
  return simplelist_init(list);
}

/* add into list given the transref_type structure */
int transreflist_add(transreflist_type *list, transref_type trans)
{
  transref_type *new_trans = NULL;
  
  if (list == NULL)
    return -1;

  if (transref_copy(&new_trans, trans) != 0)
    return -1;

  return simplelist_add(list, (void *) new_trans);
}

/* gives the length of the list */
int transreflist_length(transreflist_type list, unsigned int *len)
{
  return simplelist_length(list, len);  
}

/* gives ar reference to the index'th transref */
int transreflist_get_index(transreflist_type list,
                           unsigned int index,
                           transref_type **trans)
{
  return simplelist_get_index(list, index, (void **) trans);
}

/* empty the list along with its members */
int transreflist_purge(transreflist_type *list)
{
  unsigned int len;
  unsigned int i;
  
  if (list == NULL)
    return -1;
    
  if (simplelist_length(*list, &len) != 0)
    return -1;
    
  for (i = 0; i < len; i++)
    if (simplelist_del_index(list, 0, transreflist_destroy) != 0)
      return -1;

  return 0;
}

/* free memory */
int transreflist_destroy(void *p)
{
  return transref_destroy((transref_type *) p);
}
