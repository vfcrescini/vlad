/*
 * list.h
 * generic linked-list implementation
 * Vino Crescini
 *
 */

#ifndef __LIST_H
#define __LIST_H

typedef struct list_node {
  struct list_node *next;
  unsigned int index;
  void *data;
} list_node;

typedef struct {
  unsigned int size;
  list_node *list;
} list_type;

void list_init(list_type *list);
int list_size(list_type list);
int list_get(list_type list, unsigned int index, void **data);
int list_add(list_type *list, void *data, unsigned int *index);
int list_remove(list_type *list, unsigned int index);

#endif
