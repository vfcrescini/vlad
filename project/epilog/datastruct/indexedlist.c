/*
 * indexedlist.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include "indexedlist.h"

/* init */
int indexedlist_init(indexedlist_type *list)
{
  if (list == NULL)
    return -1;
    
  list->list = NULL;
  list->length = 0;
  index_init(&(list->unused_index));
  
  return 0; 
}

/* add the data in. we leave it to the calling function to allocate
 * memory for the data. no duplicate checking */
int indexedlist_add(indexedlist_type *list, 
                    void *data, 
                    unsigned int *index)
{
  indexedlist_node *new_node;

  if (list == NULL || data == NULL || index == NULL)
    return -1;

  if ((new_node = (indexedlist_node *) malloc(sizeof(indexedlist_node))) == NULL)
   return -1;

  /* just keep a reference of the data, no malloc */
  new_node->data = data;
  new_node->next = list->list;
  list->list = new_node;

  if (index_get(&(list->unused_index), &(new_node->index)) < 0)
    new_node->index = (list->length)++;

  *index = new_node->index;

  return 0;
}

/* given the index, the correspoding data will be returned. */
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

/* returns 0 if the index was found, -1 otherwise. must provide
 * cmp function 
 */
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

/* 
 * delete a node that matches the contents of data must provide a cmp function
 * that returns 0 if the contents match. 
 */
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

      /* we leave it to the calling function to free the data pointer */
      free(curr);

      (list->length)--;

      return 0;
    }
    prev = curr;
    curr = curr->next;
  }  

  return -1;
}

/* as above, but with an index */
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

      /* remember this index for use the next time we add */
      if (index_add(&(list->unused_index), curr->index) < 0)
        return -1;

      free(curr);

      (list->length)--;

      return 0;
    }
    prev = curr;
    curr = curr->next;
  }  

  return -1;
}

/* same as above, but with a function to free the pointer */
int indexedlist_del_data_f(indexedlist_type *list,
                           void *data, 
                           int (*cmp)(void *, void *),
                           int (*f)(void *))
{
  indexedlist_node *prev;
  indexedlist_node *curr;

  if (list == NULL || 
      list->length <= 0 || 
      data == NULL || 
      cmp == NULL || 
      f == NULL)
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

      f(curr->data);
      free(curr);

      (list->length)--;

      return 0;
    }
    prev = curr;
    curr = curr->next;
  }  

  return -1;
}

/* same as above, but with a function to free the pointer */
int indexedlist_del_index_f(indexedlist_type *list, 
                            unsigned int index,
                            int (*f)(void *))
{
  indexedlist_node *prev;
  indexedlist_node *curr;

  if (list == NULL || list->length <= 0 || f == NULL)
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

      f(curr->data);
      free(curr);

      (list->length)--;

      return 0;
    }
    prev = curr;
    curr = curr->next;
  }  

  return -1;
}
