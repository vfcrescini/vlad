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
    return EPI_NULLPTR;
  
  list->list = NULL;
  list->length = 0;

  return EPI_OK;
}

/* return length */
unsigned int simplelist_length(simplelist_type list)
{
  return list.length;
}

/* gives the index of the first instance of the node that matches data */
int simplelist_index(simplelist_type list,
                     void *data,
                     unsigned int *index,
                     int (*cmp)(void *, void *))
{
  simplelist_node *curr;

  if (data == NULL ||
      index == NULL ||
      cmp == NULL)
    return EPI_NULLPTR;

  curr = list.list;
  *index = list.length;

  while (curr != NULL) {
    *index = *index - 1;

    if (cmp(curr->data, data) == 0)
      return EPI_OK;

    curr = curr->next;
  }

  return EPI_NOTFOUND;
}

/* add pointer to list, assumes memory has been allocated to it */
int simplelist_add(simplelist_type *list, void *data)
{
  simplelist_node *new_node;

  if (list == NULL || data == NULL)
    return EPI_NULLPTR;

  if ((new_node = EPI_ADT_MALLOC(simplelist_node)) == NULL)
    return EPI_MALLOCFAILED;

  new_node->data = data;
  new_node->next = list->list;
  list->list = new_node;
  (list->length)++;

  return EPI_OK;
}

/* deletes index'th data, give fr function to free the pointer 
 * or NULL to not free it */
int simplelist_del_index(simplelist_type *list, 
                         unsigned int index, 
                         void (*fr)(void *))
{
  simplelist_node *prev;
  simplelist_node *curr;
  unsigned int i;

  if (list == NULL)
    return EPI_NULLPTR;

  if (list->length <= 0 || index >= list->length)
    return EPI_OUTOFBOUNDS;

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

  return EPI_OK;
}

/* deletes all the nodes that matches data, uses cmp to compare, 
 * fr to free or NULL to not free them */
int simplelist_del_data(simplelist_type *list, 
                        void *data,
                        int (*cmp)(void *, void *),
                        void (*fr)(void *))
{
  simplelist_node *prev;
  simplelist_node *curr;
  int found = 0;

  if (list == NULL ||
      data == NULL ||
      cmp == NULL)
    return EPI_NULLPTR;

  prev = NULL;
  curr = list->list;

  while (curr != NULL) {
    if (cmp(curr->data, data) == 0) {

      found = 1;

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

  return (found ? EPI_OK : EPI_NOTFOUND);
}

/* gives a reference to the index'th data */
int simplelist_get_index(simplelist_type list, 
                         unsigned int index, 
                         void **ref)
{
  unsigned int i;
  simplelist_node *curr;

  if (ref == NULL)
    return EPI_NULLPTR;

  if (list.length <= 0 || index >= list.length)
   return EPI_OUTOFBOUNDS;

  curr = list.list;

  for (i = 0; i < list.length - index - 1; i++)
    curr = curr->next;

  *ref = curr->data;

  return EPI_OK;
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
  simplelist_node *new;
  char found = 0;

  if (data == NULL ||
      res == NULL ||
      cmp == NULL) 
    return EPI_NULLPTR;

  res->list = NULL;
  res->length = 0;
  curr = list.list;

  while (curr != NULL) {
    if (cmp(curr->data, data) == 0) {
      found = 1;

      if ((new = EPI_ADT_MALLOC(simplelist_node)) == NULL)
        return EPI_MALLOCFAILED;
    
      new->data = curr->data;
      new->next = res->list;
      res->list= new;
      (res->length)++;
    }

    curr = curr->next;
  }

  return (found ? EPI_OK : EPI_NOTFOUND);
}

/* returns 0 if data is in the list, uses cmp to compare pointers */
int simplelist_find_data(simplelist_type list, 
                         void *data, 
                         int (*cmp)(void *, void *))
{
  simplelist_node *curr;

  if (data == NULL ||
      cmp == NULL) 
    return EPI_NULLPTR;

  curr = list.list;

  while (curr != NULL) {
    if (cmp(curr->data, data) == 0) 
      return EPI_OK;

    curr = curr->next;
  }

  return EPI_NOTFOUND;
}

/* makes an exact copy of l1 to l2, uses cpy to make a new copy */
int simplelist_copy(simplelist_type l1,
                    simplelist_type *l2,
                    int (*cpy)(void *, void **))

{
  simplelist_node *curr;
  simplelist_node *new;
  simplelist_node *last;
  void *data;

  if (l2 == NULL || cpy == NULL)
    return EPI_NULLPTR;

  curr = l1.list;
  l2->list = NULL;
  l2->length = 0;
  last = NULL;

  while (curr != NULL) {
    if (cpy(curr->data, &data) != 0)
      return EPI_FAILURE;
    
    /* we have to attach the new node at the end of the list to
     * preserve the original order */
    if ((new = EPI_ADT_MALLOC(simplelist_node)) == NULL)
      return EPI_MALLOCFAILED;

    if (last == NULL)
      l2->list = new;
    else 
      last->next = new;

    (l2->length)++;
    new->data = data;
    new->next = NULL;
    last = new;
    curr = curr->next;
  }

  return EPI_OK;
}

/* destroys the list. uses the fr function to free the nodes */
void simplelist_purge(simplelist_type *list, void (*fr)(void *))
{
  simplelist_node *curr;
  simplelist_node *prev;

  if (list != NULL) {
    curr = list->list;

    while (curr != NULL) {
      prev = curr;
      curr = curr->next;

      if (fr != NULL)
        fr(prev->data);

      free(prev);
    }

    list->list = NULL;
    list->length = 0;
  }
}
