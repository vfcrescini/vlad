/*
 * list.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstring>

#include <config.h>
#include <vlad/vlad.h>
#include <vlad/list.h>

list_item::list_item()
{
}

list_item::~list_item()
{
}

list::list()
{
  unique = true;
  len = 0;
  head = NULL;
  tail = NULL;
}

list::list(bool u)
{
  unique = u;
  len = 0;
  head = NULL;
  tail = NULL;
}

list::~list()
{
  purge(true);
}

bool list::cmp(list *l)
{
  unsigned int i;
  list_item *item1;
  list_item *item2;

  if (l == NULL || l->length() != len)
    return false;

  for (i = 0; i < len; i++) {
    /* 
     * the following 2 statements will only fail if we give them
     * out of bounds values or null ptrs none of which should occur
     */
    l->get(i, &item1);
    get(i, &item2);

    /* 
     * if we are given a list of a different type, list_item::cmp() should
     * detect it and return false.
     */
    if (!item1->cmp(item2))
      return false;
  }

  return true;
}

unsigned int list::length()
{
  return len;
}

/* add pointer to list, assumes memory has been allocated to it */
int list::add(list_item *data)
{
  list_node *new_node;
  int retval;

  if (data == NULL)
    return VLAD_NULLPTR;

  /* if the unique flag is set, ensure that data is not already in */
  if (unique) {
    if ((retval = find(data)) == VLAD_OK)
      return VLAD_DUPLICATE;
    else if (retval != VLAD_NOTFOUND)
      return retval;
  }
  
  if ((new_node = VLAD_ADT_MALLOC(list_node, 1)) == NULL)
    return VLAD_MALLOCFAILED;

  if (tail == NULL)
    head = new_node;
  else
    tail->next = new_node;

  new_node->data = data;
  new_node->next = NULL;
  tail = new_node;
  len++;
 
  return VLAD_OK;
}

/* deletes index'th data, f = true to free mem or false to not free it */
int list::del(unsigned int index, bool f)
{  
  list_node *prev;
  list_node *curr;
  unsigned int i; 

  if (len <= 0 || index >= len)
    return VLAD_OUTOFBOUNDS;

  prev = NULL;
  curr = head;
   
  for (i = 0; i < index; i++) {
    prev = curr;
    curr = curr->next;
  }

  if (prev == NULL) {
    if (curr == tail)
      tail = NULL;
    head = head->next;
  }
  else {
    if (curr == tail)
      tail = prev;
    prev->next = curr->next;
  }

  if (f)
    delete (curr->data);

  free(curr);

  len--;

  return VLAD_OK;
}

/* deletes all the nodes that matches data, f = true to free mem */
int list::del(list_item *data, bool f)
{

  list_node *prev;
  list_node *curr;
  bool found = false;

  if (data == NULL)
    return VLAD_NULLPTR;

  prev = NULL;
  curr = head;

  while (curr != NULL) {

    /* if the unique flag is set, stop after the first match */
    if (unique && found)
      break;
	
    if (curr->data->cmp(data)) {

      found = true;

      if (prev == NULL) {
        if (curr == tail)
          tail = NULL;
        head = head->next;
      }
      else {
        if (curr == tail)
          tail = prev;
        prev->next = curr->next;
      }

      if (f)
        delete (curr->data);

      free(curr);

      len--;
    }
    prev = curr;
    curr = curr->next;
  }

  return (found ? VLAD_OK : VLAD_NOTFOUND);
}

/* gives an array of indices of the data given */
int list::get(list_item *item, unsigned int **array, unsigned int *s)
{
  list_node *curr;
  unsigned int count;

  if (s == NULL || array == NULL || item == NULL)
    return VLAD_NULLPTR;

  curr = head;
  *s = 0;
  *array = NULL;
  count = 0;
  
  while (curr != NULL) {
    /* if the unique flag is set we stop when we see the first match */
    if (unique && *s >= 1)
      break;

    if (curr->data->cmp(item)) {

      if ((*array = (unsigned int *) realloc(*array, sizeof(unsigned int) * (*s + 1))) == NULL)
        return VLAD_MALLOCFAILED;

      (*array)[*s] = count;
      (*s)++;
    }
    curr = curr->next;
    count++;
  }
  return (*s > 0) ? VLAD_OK : VLAD_NOTFOUND;
}

/* gives a reference to the index'th data */
int list::get(unsigned int index, list_item **data)
{
  unsigned int i;
  list_node *curr;

  if (data == NULL)
    return VLAD_NULLPTR;

  if (len <= 0 || index >= len)
   return VLAD_OUTOFBOUNDS;

  curr = head;

  for (i = 0; i < index; i++)
    curr = curr->next;

  *data = curr->data;

  return VLAD_OK;
}

/* gives a reference to all the nodes that matches item. array + size */
int list::get(list_item *item, list_item ***data, unsigned int *s)
{
  list_node *curr;

  if (s == NULL || data == NULL || item == NULL)
    return VLAD_NULLPTR;

  curr = head;
  *s = 0;
  *data = NULL;
  
  while (curr != NULL) {
    /* if the unique flag is set we stop when we see the first match */
    if (unique && *s >= 1)
      break;

    if (curr->data->cmp(item)) {

      if ((*data = (list_item **) realloc(*data, sizeof(**data) * (*s + 1))) == NULL)
        return VLAD_MALLOCFAILED;

      (*data)[*s] = curr->data;
      (*s)++;
    }
    curr = curr->next;
  }
  return (*s > 0) ? VLAD_OK : VLAD_NOTFOUND;
}

/* returns 0 if data is in the list */
int list::find(list_item *data)
{
  list_node *curr;

  if (data == NULL)
    return VLAD_NULLPTR;
  
  curr = head;

  while (curr != NULL) {
    if (curr->data->cmp(data)) 
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
  
  if (len > 0) {
    curr = head;
  
    while (curr != NULL) {
      prev = curr;
      curr = curr->next;

      if (f && (prev->data != NULL))
        delete (prev->data);

      free(prev);
    }

    head = NULL;
    tail = NULL;
    len = 0;
  }
}
