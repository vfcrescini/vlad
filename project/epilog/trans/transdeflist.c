/*
 * transdeflist.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <string.h>
#include "transdeflist.h"

int transdeflist_compare(void *p1, void *p2);
int transdeflist_destroy(void *p);
int transdeflist_destroy_all(void *p);

/* initialise list */
int transdeflist_init(transdeflist_type *list)
{
  return simplelist_init(list);
}

/* add into list given the name, varlist, precondition and postcondition */
int transdeflist_add_name(transdeflist_type *list,
                          char *name,
                          stringlist_type varlist,
                          expression_type precond,
                          expression_type postcond)
{
  transdef_type *new_trans = NULL;

  if (list == NULL)
    return -1;

  if (transdef_create(&new_trans, name, varlist, precond, postcond) != 0)
    return -1;

  return simplelist_add(list, (void *) new_trans);
}

/* same as above, but with a transdef_type structure */
int transdeflist_add_trans(transdeflist_type *list, transdef_type trans)
{
  transdef_type *new_trans = NULL;
  
  if (list == NULL)
    return -1;

  if (transdef_copy(&new_trans, trans) != 0)
    return -1;

  return simplelist_add(list, (void *) new_trans);
}

/* gives the length of the list */
int transdeflist_length(transdeflist_type list, unsigned int *len)
{
  return simplelist_length(list, len);  
}

/* gives a reference to a transdef_type structure */
int transdeflist_get_name(transdeflist_type list,
                          char *name,
                          transdef_type **trans)
{
  transdef_type tmp_trans;

  if (name == NULL || trans == NULL)
    return -1;

  tmp_trans.name = name;

  return simplelist_get_data(list,
                             (void *) &tmp_trans,
                             (void **) trans,
                             transdeflist_compare);
}

/* same as above, but with index */
int transdeflist_get_index(transdeflist_type list,
                           unsigned int index,
                           transdef_type **trans)
{
  return simplelist_get_index(list, index, (void **) trans);
}

/* returns 0 if the transformation name is in the list */
int transdeflist_find(transdeflist_type list, char *name)
{
  transdef_type tmp_trans;

  if ((tmp_trans.name = name) == NULL)
    return -1;

  return simplelist_find_data(list,
                              (void *) &tmp_trans,
                              transdeflist_compare);
}

/* empty the list */
int transdeflist_purge(transdeflist_type *list)
{
  unsigned int len;
  unsigned int i;
  
  if (list == NULL)
    return -1;
    
  if (simplelist_length(*list, &len) != 0)
    return -1;
    
  for (i = 0; i < len; i++)
    if (simplelist_del_index(list, 0, transdeflist_destroy) != 0)
      return -1;

  return 0;
}

/* USE WITH CAUTION. as above, also purges varlist, precond and postcond */
int transdeflist_purge_all(transdeflist_type *list)
{
  unsigned int len;
  unsigned int i;
  
  if (list == NULL)
    return -1;
    
  if (simplelist_length(*list, &len) != 0)
    return -1;
    
  for (i = 0; i < len; i++)
    if (simplelist_del_index(list, 0, transdeflist_destroy_all) != 0)
      return -1;

  return 0;
}

/* compare ONLY THE NAME component of p1 and p2 */
int transdeflist_compare(void *p1, void *p2)
{
  if (p1 == NULL || p2 == NULL)
    return -1;

  return strcmp(((transdef_type *) p1)->name, ((transdef_type *) p2)->name);
}

/* free memory */
int transdeflist_destroy(void *p)
{
  return transdef_destroy((transdef_type *) p);
}

/* free all memory */
int transdeflist_destroy_all(void *p)
{
  return transdef_destroy_all((transdef_type *) p);
}
