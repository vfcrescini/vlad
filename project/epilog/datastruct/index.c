/*
 * index.c
 * Vino Crescini
 */

#include <stdlib.h>
#include "index.h"

int index_init(index_type *index)
{
  if (!index)
    return -1;
    
  *index = NULL;
  return 0;
}

int index_add(index_type *index, unsigned int num)
{
  index_type new_node;
  
  if (!index)
    return -1;

  if (!(new_node = (index_type) malloc(sizeof(index_node))))
    return -1;

  new_node->num = num;
  new_node->next = *index;
  *index = new_node;

  return 0;
}

int index_get(index_type *index, unsigned int *num)
{
  index_type old_node;
  
  if (!index || !(*index) || !num)
    return -1;

  *num = (*index)->num;
  old_node = *index;
  *index = (*index)->next;

  free(old_node);

  return 0;
}
