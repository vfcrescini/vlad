/*
 * list.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include "vlad.h"
#include "list.h"

list::list()
{
  len = 0;
  head = NULL;
  fr = NULL;
  cmp = NULL;
  initialised = false;
}

list::~list()
{
  purge(true);
}

unsigned int list::length()
{
  return len;
}

/* registers the free and compare function pointers */
int list::init(void (*f)(void *), bool (*c)(void *, void *))
{
  if (f == NULL || c == NULL)
    return VLAD_NULLPTR;
  
  fr = f;
  cmp = c;
  initialised = true;

  return VLAD_OK;
}

/* add pointer to list, assumes memory has been allocated to it */
int list::add(void *data)
{
  list_node *new_node;

  if (!initialised)
    return VLAD_UNINITIALISED;
  
  if (data == NULL)
    return VLAD_NULLPTR;

  if ((new_node = VLAD_ADT_MALLOC(list_node)) == NULL)
    return VLAD_MALLOCFAILED;

  new_node->data = data;
  new_node->next = head;
  head = new_node;
  len++;
 
  return VLAD_OK;
}

/* deletes index'th data, f = true to free mem or false to not free it */
int list::del_i(unsigned int index, bool f)
{  
  list_node *prev;
  list_node *curr;
  unsigned int i; 

  if (!initialised)
    return VLAD_UNINITIALISED;
  
  if (len <= 0 || index >= len)
    return VLAD_OUTOFBOUNDS;

  prev = NULL;
  curr = head;
   
  for (i = 0; i < len - index - 1; i++) {
    prev = curr;
    curr = curr->next;
  }

  if (prev == NULL)
    head = head->next;
  else
    prev->next = curr->next;

  if (f)
    fr(curr->data);

  free(curr);

  len--;

  return VLAD_OK;
}

/* deletes all the nodes that matches data, f = true to free mem */
int list::del_d(void *data, bool f)
{

  list_node *prev;
  list_node *curr;
  int found = 0;

  if (!initialised)
    return VLAD_UNINITIALISED;
  
  if (data == NULL)
    return VLAD_NULLPTR;

  prev = NULL;
  curr = head;

  while (curr != NULL) {
    if (cmp(curr->data, data) == 0) {

      found = 1;

      if (prev == NULL)
        head = head->next;
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
int list::get_i(unsigned int index, void **data)
{
  unsigned int i;
  list_node *curr;

  if (!initialised)
    return VLAD_UNINITIALISED;  
  
  if (data == NULL)
    return VLAD_NULLPTR;

  if (len <= 0 || index >= len)
   return VLAD_OUTOFBOUNDS;

  curr = head;

  for (i = 0; i < len - index - 1; i++)
    curr = curr->next;

  *data = curr->data;

  return VLAD_OK;
}

/* returns 0 if data is in the list */
int list::find(void *data)
{
  list_node *curr;

  if (!initialised)
    return VLAD_UNINITIALISED;

  if (data == NULL)
    return VLAD_NULLPTR;
  
  curr = head;

  while (curr != NULL) {
    if (cmp(curr->data, data) == 0) 
      return VLAD_OK;

    curr = curr->next;
  }

  return VLAD_NOTFOUND;
}	

/* destroys the list f = true to free mem */
void list::purge(bool f)
{
  list_node *curr;
  list_node *prev;
  
  if (len > 0 && initialised) {
    curr = head;
  
    while (curr != NULL) {
      prev = curr;
      curr = curr->next;

      if (f)
        fr(prev->data);

      free(prev);
    }

    head = NULL;
    len = 0;
  }
}
