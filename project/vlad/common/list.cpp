/*
 * list.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include "vlad.h"
#include "list.h"
#include <stdlib.h>

vlad_list::vlad_list()
{
  len = 0;
  list = NULL;
  fr = NULL;
  cmp = NULL;
  initialised = false;
}

vlad_list::~vlad_list()
{
  purge(true);
}

unsigned int vlad_list::length()
{
  return len;
}

/* registers the free and compare function pointers */
int vlad_list::init(void (*f)(void *), bool (*c)(void *, void *))
{
  if (f == NULL || c == NULL)
    return VLAD_NULLPTR;
  
  fr = f;
  cmp = c;
  initialised = true;

  return VLAD_OK;
}

/* add pointer to list, assumes memory has been allocated to it */
int vlad_list::add(void *data)
{
  vlad_list_node *new_node;

  if (!initialised)
    return VLAD_UNINITIALISED;
  
  if (data == NULL)
    return VLAD_NULLPTR;

  if ((new_node = VLAD_ADT_MALLOC(vlad_list_node)) == NULL)
    return VLAD_MALLOCFAILED;

  new_node->data = data;
  new_node->next = list;
  list = new_node;
  len++;
 
  return VLAD_OK;
}

/* deletes index'th data, f = true to free mem or false to not free it */
int vlad_list::del_i(unsigned int index, bool f)
{  
  vlad_list_node *prev;
  vlad_list_node *curr;
  unsigned int i; 

  if (!initialised)
    return VLAD_UNINITIALISED;
  
  if (len <= 0 || index >= len)
    return VLAD_OUTOFBOUNDS;

  prev = NULL;
  curr = list;
   
  for (i = 0; i < len - index - 1; i++) {
    prev = curr;
    curr = curr->next;
  }

  if (prev == NULL)
    list = list->next;
  else
    prev->next = curr->next;

  if (f)
    fr(curr->data);

  free(curr);

  len--;

  return VLAD_OK;
}

/* deletes all the nodes that matches data, f = true to free mem */
int vlad_list::del_d(void *data, bool f)
{

  vlad_list_node *prev;
  vlad_list_node *curr;
  int found = 0;

  if (!initialised)
    return VLAD_UNINITIALISED;
  
  if (data == NULL)
    return VLAD_NULLPTR;

  prev = NULL;
  curr = list;

  while (curr != NULL) {
    if (cmp(curr->data, data) == 0) {

      found = 1;

      if (prev == NULL)
        list = list->next;
      else
        prev->next = curr->next;

      if (f)
        fr(curr->data);

      free(curr);

      len--;
    }
    prev = curr;
    curr = curr->next;
  }

  return (found ? VLAD_OK : VLAD_NOTFOUND);
}

/* gives a reference to the index'th data */
int vlad_list::get_i(unsigned int index, void **data)
{
  unsigned int i;
  vlad_list_node *curr;

  if (!initialised)
    return VLAD_UNINITIALISED;  
  
  if (data == NULL)
    return VLAD_NULLPTR;

  if (len <= 0 || index >= len)
   return VLAD_OUTOFBOUNDS;

  curr = list;

  for (i = 0; i < len - index - 1; i++)
    curr = curr->next;

  *data = curr->data;

  return VLAD_OK;
}

/* returns 0 if data is in the list */
int vlad_list::find(void *data)
{
  vlad_list_node *curr;

  if (!initialised)
    return VLAD_UNINITIALISED;

  if (data == NULL)
    return VLAD_NULLPTR;
  
  curr = list;

  while (curr != NULL) {
    if (cmp(curr->data, data) == 0) 
      return VLAD_OK;

    curr = curr->next;
  }

  return VLAD_NOTFOUND;
}	

/* destroys the list, f = true to free mem */
void vlad_list::purge(bool f)
{
  vlad_list_node *curr;
  vlad_list_node *prev;
  
  if (len > 0 && initialised) {
    curr = list;
  
    while (curr != NULL) {
      prev = curr;
      curr = curr->next;

      if (f)
        fr(prev->data);

      free(prev);
    }

    list = NULL;
    len = 0;
  }
}
