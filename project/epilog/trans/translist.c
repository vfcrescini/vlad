/*
 * translist.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <string.h>
#include "translist.h"

int translist_destroy(void *p);

/* initialise list */
void translist_init(translist_type *list)
{
  simplelist_init(list);
}

/* gives the length of the list */
unsigned int translist_length(translist_type list)
{
  return simplelist_length(list);
}

/* add into list given the transref_type structure */
int translist_add(translist_type *list, transref_type trans)
{
  transref_type *new_trans = NULL;
  
  if (list == NULL)
    return -1;

  if (transref_copy(&new_trans, trans) != 0)
    return -1;

  return simplelist_add(list, (void *) new_trans);
}

/* gives ar reference to the index'th transref */
int translist_get(translist_type list,
                  unsigned int index,
                  transref_type **trans)
{
  return simplelist_get_index(list, index, (void **) trans);
}

/* empty the list along with its members */
int translist_purge(translist_type *list)
{
  unsigned int i;
  
  if (list == NULL)
    return -1;
    
  for (i = 0; i < simplelist_length(*list); i++)
    if (simplelist_del_index(list, 0, translist_destroy) != 0)
      return -1;

  return 0;
}

/* free memory */
int translist_destroy(void *p)
{
  return transref_destroy((transref_type *) p);
}
