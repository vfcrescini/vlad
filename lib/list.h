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

#ifndef __VLAD_LIST_H
#define __VLAD_LIST_H

#include <vlad/mem.h>

/* abstract class to be used as the items in the list */
class vlad_list_item : public vlad_mem
{
  public :

    vlad_list_item();
    virtual ~vlad_list_item();

    /* compares 2 list items */
    virtual bool cmp(vlad_list_item *a_item) = 0;
} ;

/* class used for traversal of the list */
class vlad_list_trav : public vlad_mem
{
  public :
    vlad_list_trav();
    virtual ~vlad_list_trav();
} ;

/* simple list works like a queue, except the api allows the data to be
 * accessed given its ordinal index (the order in which it was added to the
 * list) */

typedef struct vlad_list_node
{
  vlad_list_item *data;
  struct vlad_list_node *next;
} vlad_list_node;

class vlad_list : public vlad_mem
{
  public :

    /* default is to create a list in which all elements are unique */
    vlad_list();

    /* a_uniq specifies whether items are unique or not */
    vlad_list(bool a_uniq);

    /* dtor */
    virtual ~vlad_list();

    /* compares 2 lists */
    bool cmp(vlad_list *a_list);

    /* returns the length of the list */
    unsigned int length();

    /* traverse the list, calling vlad_list_trav::trav() for each node */
    int traverse(vlad_list_trav *a_trav);

  private :

    unsigned int m_length;
    bool m_uniq;
    vlad_list_node *m_head;
    vlad_list_node *m_tail;

    /* called by traverse() */
    virtual int trav(vlad_list_item *a_item, vlad_list_trav *a_trav);

  protected :

    /* add pointer to list, assumes memory has been allocated to it */
    int add(vlad_list_item *a_data);

    /* deletes index'th data, free = true to free mem or false to not free it */
    int del(unsigned int a_index, bool a_free);

    /* deletes all the nodes that matches data, free = true to free mem */
    int del(vlad_list_item *a_data, bool a_free);

    /* gives an array of indices of the data given */
    int get(vlad_list_item *a_item,
            unsigned int **a_array,
            unsigned int *a_size);

    /* gives a reference to the index'th data */
    int get(unsigned int a_index, vlad_list_item **a_data);

    /* gives a reference to all the nodes that matches item. array + size */
    int get(vlad_list_item *a_item,
            vlad_list_item ***a_data,
            unsigned int *a_size);

    /* returns 0 if data is in the list */
    int find(vlad_list_item *a_data);

    /* destroys the list, f = true to free mem */
    void purge(bool a_free);
} ;

/* convenience macros */

#define VLAD_LIST_LENGTH(X) \
  (((X) == NULL) ? 0 : (X)->length())

#define VLAD_LIST_ITEMCMP(X,Y) \
  (((X) == NULL) ? ((Y) == NULL) : (((Y) == NULL) ? false : ((X)->cmp(Y))))

#endif
