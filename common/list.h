/*
 * list.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_LIST_H
#define __VLAD_LIST_H

/* abstract class to be used as the items in the list */

/* convenience macro to compare item pointers */
#define VLAD_LIST_ITEMCMP(X,Y) (((X) == NULL) ? ((Y) == NULL) : (((Y) == NULL) ? false : ((X)->cmp(Y))))

class list_item
{
  public :
    list_item();
    virtual ~list_item();
    virtual bool cmp(list_item *item) = 0;
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
    list(bool u);
    virtual ~list();
    bool cmp(list *l);
    unsigned int length();
  private :
    unsigned int len;
    bool unique;
    list_node *head;
    list_node *tail;
  protected :
    /* add pointer to list, assumes memory has been allocated to it */
    int add(list_item *data);
    /* deletes index'th data, f = true to free mem or false to not free it */
    int del(unsigned int index, bool f);
    /* deletes all the nodes that matches data, f = true to free mem */
    int del(list_item *data, bool f);
    /* gives an array of indices of the data given */
    int get(list_item *item, unsigned int **array, unsigned int *s);
    /* gives a reference to the index'th data */
    int get(unsigned int index, list_item **data);
    /* gives a reference to all the nodes that matches item. array + size */
    int get(list_item *item, list_item ***data, unsigned int *s);
    /* returns 0 if data is in the list */
    int find(list_item *data);
    /* destroys the list, f = true to free mem */
    void purge(bool f);
} ;

#endif
