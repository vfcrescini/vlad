/*
 * translist.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <string.h>
#include "translist.h"

void translist_destroy(void *p);

/* initialise list */
int translist_init(translist_type *list)
{
  return simplelist_init(list);
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
  int tmp_res;
  
  if (list == NULL)
    return EPI_NULLPTR;

  if ((tmp_res = transref_copy(&new_trans, trans)) != EPI_OK)
    return tmp_res;

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
void translist_purge(translist_type *list)
{
  simplelist_purge(list, translist_destroy);
}

/* free memory */
void translist_destroy(void *p)
{
  transref_destroy((transref_type *) p);
}
