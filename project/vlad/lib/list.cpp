/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004, 2005 University of Western Sydney
 * by Vino Fernando Crescini <jcrescin@cit.uws.edu.au>
 *
 * PolicyUpdater is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * PolicyUpdater is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PolicyUpdater; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <cstdlib>
#include <cstring>

#include <vlad/vlad.h>
#include <vlad/mem.h>
#include <vlad/list.h>

vlad_list_item::vlad_list_item()
{
}

vlad_list_item::~vlad_list_item()
{
}

vlad_list_trav::vlad_list_trav()
{
}

vlad_list_trav::~vlad_list_trav()
{
}

/* default is to create a list in which all elements are unique */
vlad_list::vlad_list()
{
  m_uniq = true;
  m_length = 0;
  m_head = NULL;
  m_tail = NULL;
}

/* a_uniq specifies whether items are unique or not */
vlad_list::vlad_list(bool a_uniq)
{
  m_uniq = a_uniq;
  m_length = 0;
  m_head = NULL;
  m_tail = NULL;
}

/* dtor */
vlad_list::~vlad_list()
{
  purge(true);
}

/* compares 2 lists */
bool vlad_list::cmp(vlad_list *a_list)
{
  unsigned int i;
  vlad_list_item *item1;
  vlad_list_item *item2;

  if (a_list == NULL || a_list->length() != m_length)
    return false;

  for (i = 0; i < m_length; i++) {
    /*
     * the following 2 statements will only fail if we give them
     * out of bounds values or null ptrs none of which should occur
     */
    a_list->get(i, &item1);
    get(i, &item2);

    /*
     * if we are given a list of a different type, vlad_list_item::cmp() should
     * detect it and return false.
     */
    if (!item1->cmp(item2))
      return false;
  }

  return true;
}

/* returns the length of the list */
unsigned int vlad_list::length()
{
  return m_length;
}

/* traverse the list, calling vlad_list_trav::trav() for each node */
int vlad_list::traverse(vlad_list_trav *a_trav)
{
  vlad_list_node *node = m_head;
  int retval = VLAD_OK;

  if (a_trav == NULL)
    return VLAD_NULLPTR;

  while (node != NULL && retval == VLAD_OK) {
    retval = trav(dynamic_cast<vlad_list_item *>(node->data), a_trav);

    if (retval != VLAD_OK)
      return retval;

    node = node->next;
  }

  return retval;
}

/* called by traverse() */
int vlad_list::trav(vlad_list_item *a_item, vlad_list_trav *a_trav)
{
  /* default is to do nothing */
  return VLAD_OK;
}

/* add pointer to list, assumes memory has been allocated to it */
int vlad_list::add(vlad_list_item *a_data)
{
  vlad_list_node *new_node;
  int retval;

  if (a_data == NULL)
    return VLAD_NULLPTR;

  /* if the m_uniq flag is set, ensure that data is not already in */
  if (m_uniq) {
    if ((retval = find(a_data)) == VLAD_OK)
      return VLAD_DUPLICATE;
    else if (retval != VLAD_NOTFOUND)
      return retval;
  }

  if ((new_node = VLAD_MEM_ADT_MALLOC(vlad_list_node, 1)) == NULL)
    return VLAD_MALLOCFAILED;

  if (m_tail == NULL)
    m_head = new_node;
  else
    m_tail->next = new_node;

  new_node->data = a_data;
  new_node->next = NULL;
  m_tail = new_node;
  m_length++;

  return VLAD_OK;
}

/* deletes index'th data, f = true to free mem or false to not free it */
int vlad_list::del(unsigned int a_index, bool a_free)
{
  vlad_list_node *prev;
  vlad_list_node *curr;
  unsigned int i;

  if (m_length <= 0 || a_index >= m_length)
    return VLAD_OUTOFBOUNDS;

  prev = NULL;
  curr = m_head;

  for (i = 0; i < a_index; i++) {
    prev = curr;
    curr = curr->next;
  }

  if (prev == NULL) {
    if (curr == m_tail)
      m_tail = NULL;
    m_head = m_head->next;
  }
  else {
    if (curr == m_tail)
      m_tail = prev;
    prev->next = curr->next;
  }

  if (a_free)
    VLAD_MEM_DELETE(curr->data);

  VLAD_MEM_FREE(curr);

  m_length--;

  return VLAD_OK;
}

/* deletes all the nodes that matches data, f = true to free mem */
int vlad_list::del(vlad_list_item *a_data, bool a_free)
{

  vlad_list_node *prev;
  vlad_list_node *curr;
  bool found = false;

  if (a_data == NULL)
    return VLAD_NULLPTR;

  prev = NULL;
  curr = m_head;

  while (curr != NULL) {
    /* if the m_uniq flag is set, stop after the first match */
    if (m_uniq && found)
      break;
	
    if (curr->data->cmp(a_data)) {
      vlad_list_node *ptmp;

      found = true;

      if (prev == NULL) {
        if (curr == m_tail)
          m_tail = NULL;
        m_head = m_head->next;
      }
      else {
        if (curr == m_tail)
          m_tail = prev;
        prev->next = curr->next;
      }

      ptmp = curr;
      curr = curr->next;

      if (a_free)
        VLAD_MEM_DELETE(ptmp->data);

      VLAD_MEM_FREE(ptmp);

      m_length--;
    }
    else {
      prev = curr;
      curr = curr->next;
    }
  }

  return (found ? VLAD_OK : VLAD_NOTFOUND);
}

/* gives an array of indices of the data given */
int vlad_list::get(vlad_list_item *a_item,
                   unsigned int **a_array,
                   unsigned int *a_size)
{
  vlad_list_node *curr;
  unsigned int count;

  if (a_size == NULL || a_array == NULL || a_item == NULL)
    return VLAD_NULLPTR;

  curr = m_head;
  count = 0;
  *a_array = NULL;
  *a_size = 0;

  while (curr != NULL) {
    /* if the m_uniq flag is set we stop when we see the first match */
    if (m_uniq && *a_size >= 1)
      break;

    if (curr->data->cmp(a_item)) {
      *a_array = VLAD_MEM_REALLOC(*a_array, unsigned int, *a_size + 1);
      if (*a_array == NULL)
        return VLAD_MALLOCFAILED;

      (*a_array)[*a_size] = count;
      (*a_size)++;
    }
    curr = curr->next;
    count++;
  }
  return (*a_size > 0) ? VLAD_OK : VLAD_NOTFOUND;
}

/* gives a reference to the index'th data */
int vlad_list::get(unsigned int a_index, vlad_list_item **a_data)
{
  unsigned int i;
  vlad_list_node *curr;

  if (a_data == NULL)
    return VLAD_NULLPTR;

  if (m_length <= 0 || a_index >= m_length)
   return VLAD_OUTOFBOUNDS;

  curr = m_head;

  for (i = 0; i < a_index; i++)
    curr = curr->next;

  *a_data = curr->data;

  return VLAD_OK;
}

/* gives a reference to all the nodes that matches item. array + size */
int vlad_list::get(vlad_list_item *a_item,
                   vlad_list_item ***a_data,
                   unsigned int *a_size)
{
  vlad_list_node *curr;

  if (a_size == NULL || a_data == NULL || a_item == NULL)
    return VLAD_NULLPTR;

  curr = m_head;
  *a_size = 0;
  *a_data = NULL;

  while (curr != NULL) {
    /* if the m_uniq flag is set we stop when we see the first match */
    if (m_uniq && *a_size >= 1)
      break;

    if (curr->data->cmp(a_item)) {
      *a_data = VLAD_MEM_REALLOC(*a_data, vlad_list_item *, *a_size + 1);
      if (*a_data == NULL)
        return VLAD_MALLOCFAILED;

      (*a_data)[*a_size] = curr->data;
      (*a_size)++;
    }
    curr = curr->next;
  }
  return (*a_size > 0) ? VLAD_OK : VLAD_NOTFOUND;
}

/* returns 0 if data is in the list */
int vlad_list::find(vlad_list_item *a_data)
{
  vlad_list_node *curr;

  if (a_data == NULL)
    return VLAD_NULLPTR;

  curr = m_head;

  while (curr != NULL) {
    if (curr->data->cmp(a_data))
      return VLAD_OK;

    curr = curr->next;
  }

  return VLAD_NOTFOUND;
}	

/* destroys the list free = true to free mem */
void vlad_list::purge(bool a_free)
{
  vlad_list_node *curr;
  vlad_list_node *prev;

  if (m_length > 0) {
    curr = m_head;

    while (curr != NULL) {
      prev = curr;
      curr = curr->next;

      if (a_free && (prev->data != NULL))
        VLAD_MEM_DELETE(prev->data);

      VLAD_MEM_FREE(prev);
    }

    m_head = NULL;
    m_tail = NULL;
    m_length = 0;
  }
}
