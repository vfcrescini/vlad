/*
 * list.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_LIST_H
#define __VLAD_LIST_H

/* abstract class to be used as the items in the list */

class list_item
{
  public :
    list_item();
    virtual ~list_item();
    virtual bool cmp(list_item *item) = 0;
} ;

/* simple list works like a queue, except the api allows the data to be
 * accessed given its ordinal index (the order in which it was added to the 
 * list). */

typedef struct list_node
{
  list_item *data;
  struct list_node *next;
} list_node;

class list
{
  public :
    list();
    virtual ~list();
    unsigned int length();
  private :
    unsigned int len;
    list_node *head;
  protected :
    /* add pointer to list, assumes memory has been allocated to it */
    int add(list_item *data);
    /* deletes index'th data, f = true to free mem or false to not free it */
    int del_i(unsigned int index, bool f);
    /* deletes all the nodes that matches data, f = true to free mem */
    int del_d(list_item *data, bool f);
    /* gives a reference to the index'th data */
    int get_i(unsigned int index, list_item **data);
    /* gives a reference to the data that matches item */
    int get_d(list_item *item, list_item **data);
    /* returns 0 if data is in the list */
    int find(list_item *data);
    /* destroys the list, f = true to free mem */
    void purge(bool f);
} ;

#endif
