/*
 * list.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_LIST_H
#define __VLAD_LIST_H

/* simple list works like a queue, except the api allows the data to be 
 * accessed given its ordinal index (the order in which it was added to the 
 * list. */

typedef struct list_node
{
  void *data;
  struct list_node *next;
} list_node;

class list
{
  public :
    list();
    ~list();
    unsigned int length();
  private :
    unsigned int len;
    list_node *head;
    bool initialised;
    void (*fr)(void *);
    bool (*cmp)(void *, void *);
  protected :
    /* registers the free and compare function pointers */
    int init(void (*f)(void *), bool (*c)(void *, void *));
    /* add pointer to list, assumes memory has been allocated to it */
    int add(void *data);
    /* deletes index'th data, f = true to free mem or false to not free it */
    int del_i(unsigned int index, bool f);
    /* deletes all the nodes that matches data, f = true to free mem */
    int del_d(void *data, bool f);
    /* gives a reference to the index'th data */
    int get_i(unsigned int index, void **data);
    /* returns 0 if data is in the list */
    int find(void *data);
    /* destroys the list, f = true to free mem */
    void purge(bool f);
} ;

#endif
