/*
 * indexedlist.c
 * Vino Crescini
 */

#include <stdlib.h>
#include "indexedlist.h"

int indexedlist_init(indexedlist_type *list)
{
  if (list == NULL)
    return -1;
    
  list->list = NULL;
  list->length = 0;
  index_init(&(list->unused_index));
  
  return 0; 
}

int indexedlist_get_data(indexedlist_type list, void **data, unsigned int index)
{
  indexedlist_node *temp;
  
  if (list.length <= 0 || data == NULL)
    return -1;

  temp = list.list;

  while (temp != NULL) {
    if (temp->index == index) {
      *data = temp->data;
      return 0;
    }
    temp = temp->next;
  }
  return -1; 
}

int indexedlist_get_index(indexedlist_type list,
                          void *data, 
                          unsigned int *index,
                          int (*cmp)(void *, void *))
{
  indexedlist_node *temp;

  if (list.length <= 0 || data == NULL || index == NULL || cmp == NULL)
    return -1;

  temp = list.list;

  while (temp != NULL) {
    if (!cmp(data, temp->data)) {
      *index = temp->index;
      return 0;
    }
    temp = temp->next;
  }
  return -1; 
}

int indexedlist_add(indexedlist_type *list, 
                    void *data, 
                    unsigned int *index,
                    int (*cmp)(void *, void *))
{
  indexedlist_node *new_node;

  if (list == NULL || data == NULL || index == NULL || cmp == NULL)
    return -1;

  /* check if that data is already in */
  if (indexedlist_get_index(*list, data, index, cmp) == 0)
    return 0;

  if ((new_node = (indexedlist_node *) malloc(sizeof(indexedlist_node))) == NULL)
   return -1;

  new_node->data = data;
  new_node->next = list->list;
  list->list = new_node;

  if (index_get(&(list->unused_index), &(new_node->index)) < 0)
    new_node->index = (list->length)++;

  *index = new_node->index;

  return 0;
}

int indexedlist_del_data(indexedlist_type *list,
                         void *data, 
                         int (*cmp)(void *, void *))
{
  indexedlist_node *prev;
  indexedlist_node *curr;

  if (list == NULL || list->length <= 0 || data == NULL || cmp == NULL)
    return -1;

  prev = NULL;
  curr = list->list;

  while (curr != NULL) {
    if (!cmp(curr->data, data)) {
      if (prev == NULL)
        list->list = list->list->next;
      else
        prev->next = curr->next;

      if (index_add(&(list->unused_index), curr->index) < 0)
        return -1;

      free(curr);

      return 0;
    }
    prev = curr;
    curr = curr->next;
  }  

  return -1;
}

int indexedlist_del_index(indexedlist_type *list, unsigned int index)
{
  indexedlist_node *prev;
  indexedlist_node *curr;

  if (list == NULL || list->length <= 0)
    return -1;

  prev = NULL;
  curr = list->list;

  while (curr != NULL) {
    if (curr->index == index) {
      if (prev == NULL)
        list->list = list->list->next;
      else
        prev->next = curr->next;

      if (index_add(&(list->unused_index), curr->index) < 0)
        return -1;

      free(curr);

      return 0;
    }
    prev = curr;
    curr = curr->next;
  }  

  return -1;
}
