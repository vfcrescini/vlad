/*
 * simplelist.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include "simplelist.h"

/* initialise list */
int simplelist_init(simplelist_type *list)
{
  if (list == NULL)
    return -1; 

  list->list = NULL;
  list->length = 0;

  return 0;
}

/* return length */
int simplelist_length(simplelist_type list, unsigned int *length)
{
  if (length == NULL)
    return -1;

  *length = list.length;

  return 0;
}

/* gives the index of the first instance of the node that matches data */
int simplelist_index(simplelist_type list,
                     void *data,
                     unsigned int *index,
                     int (*cmp)(void *, void *))
{
  simplelist_node *curr;

  if (list.length <= 0 ||
      data == NULL ||
      index == NULL ||
      cmp == NULL)
    return -1;

  curr = list.list;
  *index = 0;

  while (curr != NULL) {
    if (cmp(curr->data, data) == 0)
      return 0;

    curr = curr->next;
    *index = *index + 1;
  }

  return -1;
}

/* add pointer to list, assumes memory has been allocated to it */
int simplelist_add(simplelist_type *list, void *data)
{
  simplelist_node *new_node;

  if (list == NULL || data == NULL)
    return -1;

  if ((new_node = (simplelist_node *) malloc(sizeof(simplelist_node))) == NULL)
    return -1;

  new_node->data = data;
  new_node->next = list->list;
  list->list = new_node;
  (list->length)++;

  return 0;
}

/* deletes index'th data, give fr function to free the pointer 
 * or NULL to not free it */
int simplelist_del_index(simplelist_type *list, 
                         unsigned int index, 
                         int (*fr)(void *))
{
  simplelist_node *prev;
  simplelist_node *curr;
  unsigned int i;

  if (list == NULL || list->length <= 0 || index >= list->length)
    return -1;

  prev = NULL;
  curr = list->list;

  for (i = 0; i < list->length - index - 1; i++) {
    prev = curr;
    curr = curr->next;
  }

  if (prev == NULL)
    list->list = list->list->next;
  else
    prev->next = curr->next;

  if (fr != NULL)
    fr(curr->data);
  free(curr);

  (list->length)--;

  return 0;
}

/* deletes all the nodes that matches data, uses cmp to compare, 
 * fr to free or NULL to not free them */
int simplelist_del_data(simplelist_type *list, 
                        void *data,
                        int (*cmp)(void *, void *),
                        int (*fr)(void *))
{
  simplelist_node *prev;
  simplelist_node *curr;

  if (list == NULL ||
      list->length <= 0 ||
      data == NULL ||
      cmp == NULL ||
      fr == NULL)
    return -1;

  prev = NULL;
  curr = list->list;

  while (curr != NULL) {
    if (cmp(curr->data, data) == 0) {
      if (prev == NULL)
        list->list = list->list->next;
      else
        prev->next = curr->next;

      if (fr != NULL)
        fr(curr->data);

      free(curr);

      (list->length)--;
    }
    prev = curr;
    curr = curr->next;
  }

  return 0;
}

/* gives a reference to the index'th data */
int simplelist_get_index(simplelist_type list, 
                         unsigned int index, 
                         void **ref)
{
  unsigned int i;
  simplelist_node *curr;

  if (list.length <= 0 || index >= list.length || ref == NULL)
   return -1;

  curr = list.list;

  for (i = 0; i < list.length - index - 1; i++)
    curr = curr->next;

  *ref = curr->data;

  return 0;
}

/* returns a list of nodes that matches the given data. uses cmp to compare.
 * resulting list will have the same pointer references as the nodes in the
 * first list */
int simplelist_get_data(simplelist_type list,
                        simplelist_type *res,
                        void *data,
                        int (*cmp)(void *, void*))
{
  simplelist_node *curr;
  char notfound = -1;

  if (list.length <= 0 ||
      data == NULL ||
      res == NULL ||
      cmp == NULL) 
    return -1;

  if (simplelist_init(res) != 0)
    return -1;

  curr = list.list;

  while (curr != NULL) {
    if (cmp(curr->data, data) == 0) {
      notfound = 0;
      if (simplelist_add(res, curr->data) != 0)
        return -1;
    }
    curr = curr->next;
  }

  return notfound;
}

/* returns 0 if data is in the list, uses cmp to compare pointers */
int simplelist_find_data(simplelist_type list, 
                         void *data, 
                         int (*cmp)(void *, void *))
{
  simplelist_node *curr;

  if (list.length <= 0 ||
      data == NULL ||
      cmp == NULL) 
    return -1;

  curr = list.list;

  while (curr != NULL) {
    if (cmp(curr->data, data) == 0) 
      return 0;

    curr = curr->next;
  }

  return -1;
}
