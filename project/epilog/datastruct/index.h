/*
 * index.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_INDEX_H
#define __EPI_INDEX_H

typedef struct index_node
{
  unsigned int num;
  struct index_node *next;
} index_node;

typedef struct index_node *index_type;

int index_init(index_type *index);
int index_add(index_type *index, unsigned int num);
int index_get(index_type *index, unsigned int *num);

#endif
