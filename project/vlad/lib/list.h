/*
 * list.h
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_LIST_H
#define __VLAD_LIST_H

/* abstract class to be used as the items in the list */

class list_item
{
  public :
    list_item();
    virtual ~list_item();
    virtual bool cmp(list_item *a_item) = 0;
} ;

/*
 * simple list works like a queue, except the api allows the data to be
 * accessed given its ordinal index (the order in which it was added to the
 * list).
 */

typedef struct list_node
{
  list_item *data;
  struct list_node *next;
} list_node;

class list
{
  public :
    list();
    list(bool a_uniq);
    virtual ~list();
    bool cmp(list *a_list);
    unsigned int length();
  private :
    unsigned int m_length;
    bool m_uniq;
    list_node *m_head;
    list_node *m_tail;
  protected :
    /* add pointer to list, assumes memory has been allocated to it */
    int add(list_item *a_data);
    /* deletes index'th data, free = true to free mem or false to not free it */
    int del(unsigned int a_index, bool a_free);
    /* deletes all the nodes that matches data, free = true to free mem */
    int del(list_item *a_data, bool a_free);
    /* gives an array of indices of the data given */
    int get(list_item *a_item, unsigned int **a_array, unsigned int *a_size);
    /* gives a reference to the index'th data */
    int get(unsigned int a_index, list_item **a_data);
    /* gives a reference to all the nodes that matches item. array + size */
    int get(list_item *a_item, list_item ***a_data, unsigned int *a_size);
    /* returns 0 if data is in the list */
    int find(list_item *a_data);
    /* destroys the list, f = true to free mem */
    void purge(bool a_free);
} ;

#endif
