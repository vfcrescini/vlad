/*
 * translist.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <string.h>
#include "translist.h"

int translist_compare(void *p1, void *p2);
int translist_destroy(void *p);
int translist_destroy_all(void *p);

/* initialise list */
int translist_init(translist_type *list)
{
  return simplelist_init(list);
}

/* add into list given the name, varlist, precondition and postcondition */
int translist_add_name(translist_type *list,
                       char *name,
                       stringlist_type varlist,
                       expression_type precond,
                       expression_type postcond)
{
  trans_type *new_trans = NULL;

  if (list == NULL)
    return -1;

  if (trans_create(&new_trans, name, varlist, precond, postcond) != 0)
    return -1;

  return simplelist_add(list, (void *) new_trans);
}

/* same as above, but with a trans_type structure */
int translist_add_trans(translist_type *list, trans_type trans)
{
  trans_type *new_trans = NULL;
  
  if (list == NULL)
    return -1;

  if (trans_copy(&new_trans, trans) != 0)
    return -1;

  return simplelist_add(list, (void *) new_trans);
}

/* gives the length of the list */
int translist_length(translist_type list, unsigned int *len)
{
  return simplelist_length(list, len);  
}

/* gives a reference to a trans_type structure */
int translist_get_name(translist_type list,
                       char *name,
                       trans_type **trans)
{
  trans_type tmp_trans;

  if (name == NULL || trans == NULL)
    return -1;

  tmp_trans.name = name;

  return simplelist_get_data(list,
                             (void *) &tmp_trans,
                             (void **) trans,
                             translist_compare);
}

/* same as above, but with index */
int translist_get_index(translist_type list,
                        unsigned int index,
                        trans_type **trans)
{
  return simplelist_get_index(list, index, (void **) trans);
}

/* returns 0 if the transformation name is in the list */
int translist_find(translist_type list, char *name)
{
  trans_type tmp_trans;

  if ((tmp_trans.name = name) == NULL)
    return -1;

  return simplelist_find_data(list,
                              (void *) &tmp_trans,
                              translist_compare);
}

/* empty the list */
int translist_purge(translist_type *list)
{
  unsigned int len;
  unsigned int i;
  
  if (list == NULL)
    return -1;
    
  if (simplelist_length(*list, &len) != 0)
    return -1;
    
  for (i = 0; i < len; i++)
    if (simplelist_del_index(list, 0, translist_destroy) != 0)
      return -1;

  return 0;
}

/* USE WITH CAUTION. as above, also purges varlist, precond and postcond */
int translist_purge_all(translist_type *list)
{
  unsigned int len;
  unsigned int i;
  
  if (list == NULL)
    return -1;
    
  if (simplelist_length(*list, &len) != 0)
    return -1;
    
  for (i = 0; i < len; i++)
    if (simplelist_del_index(list, 0, translist_destroy_all) != 0)
      return -1;

  return 0;
}

/* compare ONLY THE NAME component of p1 and p2 */
int translist_compare(void *p1, void *p2)
{
  if (p1 == NULL || p2 == NULL)
    return -1;

  return strcmp(((trans_type *) p1)->name, ((trans_type *) p2)->name);
}

/* free memory */
int translist_destroy(void *p)
{
  return trans_destroy((trans_type *) p);
}

/* free all memory */
int translist_destroy_all(void *p)
{
  return trans_destroy_all((trans_type *) p);
}
