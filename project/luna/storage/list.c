/*
 * list.c
 * generic linked-list implementation
 * Vino Crescini
 *
 */

#include <stdlib.h>
#include <limits.h>
#include "list.h"

#ifdef UINT_MAX
#define LIST_LIMIT UINT_MAX
#else
#define LIST_LIMIT 4294967295U
#endif

void list_init(list_type *list)
{
  if (list) {
    list->size = 0;
    list->list = NULL;
  }
}

int list_size(list_type list)
{
  return list.size;
}

int list_get(list_type list, unsigned int index, void **data)
{
  list_node *tmp_node = NULL;

  if (!list.size || !data)
    return 0;

  *data    = NULL;
  tmp_node = list.list;

  while (tmp_node) {
    if (tmp_node->index == index) {
      *data = tmp_node->data;
      return 1;
    }
    tmp_node = tmp_node->next;
  }

  return 0;
}

int list_add(list_type *list, void *data, unsigned int *index)
{
  list_node *new_node = NULL;
  list_node *tmp_node = NULL;
  list_node *prv_node = NULL;
  unsigned int tmp_index = 0;

  if (!list || !data || !index || list->size >= LIST_LIMIT)
    return 0;

  if (!(new_node = (list_node *) malloc(sizeof(list_node))))
    return 0;

  // find the right node
  tmp_node = list->list;
  while (tmp_node != NULL) {
    if (tmp_node->index - tmp_index > 1)
      break;
    tmp_index = tmp_node->index;
    prv_node  = tmp_node;
    tmp_node  = tmp_node->next;
  } 

  // see if the node is to be inserted at the
  // beginning or elsewhere
  if (prv_node != NULL) {
    new_node->next = prv_node->next;
    prv_node->next = new_node;
  }
  else {
    list->list     = new_node;
    new_node->next = tmp_node;
  }

  new_node->data  = data;
  new_node->index = tmp_index;
  list->size      = list->size + 1;
  *index          = new_node->index;

  return 1;
}

int list_remove(list_type *list, unsigned int index)
{
  list_node *tmp_node = NULL;
  list_node *prv_node = NULL;

  if (!list || !list->size)
    return 0;

  tmp_node = list->list;

  while (tmp_node) {
    if (tmp_node->index == index) {
      if (!prv_node)
        list->list = list->list->next;
      else
        prv_node->next = tmp_node->next;

      list->size = list->size - 1;
      free(tmp_node);

      return 1;
    }
    prv_node = tmp_node;
    tmp_node = tmp_node->next;
  }
  return 0;
}
