/*
 * indexedlist.h
 * Vino Crescini
 */

#ifndef __EPI_INDEXEDLIST_H
#define __EPI_INDEXEDLIST_H

#include "index.h"

typedef struct indexedlist_node
{
  void *data;
  unsigned int index;
  struct indexedlist_node *next;
} indexedlist_node;

typedef struct indexedlist_type
{
  indexedlist_node *list;
  unsigned int length;
  index_type unused_index;
} indexedlist_type;

int indexedlist_init(indexedlist_type *list);
int indexedlist_add(indexedlist_type *list, void *data, unsigned int *index);
int indexedlist_get_data(indexedlist_type list, 
                         void **data, 
                         unsigned int index);
int indexedlist_get_index(indexedlist_type list, 
                          void *data, 
                          unsigned int *index, 
                          int (*cmp)(void *, void *));
int indexedlist_del_data(indexedlist_type *list, 
                         void *data, 
                         int (*cmp)(void *, void *));
int indexedlist_del_index(indexedlist_type *list, unsigned int index);
int indexedlist_del_data_f(indexedlist_type *list, 
                           void *data, 
                           int (*cmp)(void *, void *), 
                           int (*f)(void *));
int indexedlist_del_index_f(indexedlist_type *list, 
                            unsigned int index, 
                            int (*f)(void *));

#endif
