/*
 * list.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <new.h>

#include <config.h>
#include <vlad.h>
#include <list.h>

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
#ifdef DEBUG
  fprintf(stderr, "list is unique\n");
#endif
}

list::list(bool u)
{
  unique = u;
  len = 0;
  head = NULL;
#ifdef DEBUG
  fprintf(stderr, "list is %s\n", u ? "unique" : "not-unique");
#endif
}

list::~list()
{
  purge(true);
#ifdef DEBUG
  fprintf(stderr, "list is destroyed\n");
#endif

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
    delete (curr->data);

  free(curr);

  len--;

  return VLAD_OK;
}

/* deletes all the nodes that matches data, f = true to free mem */
int list::del_d(list_item *data, bool f)
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

      if (prev == NULL)
        head = head->next;
      else
        prev->next = curr->next;

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

/* gives a reference to the index'th data */
int list::get_i(unsigned int index, list_item **data)
{
  unsigned int i;
  list_node *curr;

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

/* gives a reference to all the nodes that matches item. array + size */
int list::get_d(list_item *item, list_item ***data, unsigned int *s)
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

      if (f)
        delete (prev->data);

      free(prev);
    }

    head = NULL;
    len = 0;
  }
}
