/*
 * numberlist.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_NUMBERLIST_H
#define __VLAD_NUMBERLIST_H

#include <list.h>

/* a number */
class number : public list_item
{
  public :
    number();
    ~number();
    /* compare item with this number */
    bool cmp(list_item *item);
    int init(unsigned int n);
    unsigned int get();
  private :
    unsigned int num;
} ;

/* a list of numbers */
class numberlist : public list_item, list
{
  public :
    numberlist();
    ~numberlist();
    /* compare list with this list */
    bool cmp(list_item *item);
    /* add a number in the list */
    int add(unsigned int n);
    /* get the i'th number in the list */
    int get(unsigned int i, unsigned int *n);
} ;

/* a list of number lists */
class numberlistlist : public list
{
  public :
    numberlistlist();
    ~numberlistlist();
    /* add a number list to this list */
    int add(numberlist *l);
    /* get the i'th number list in this list */
    int get(unsigned int i, numberlist **l);
} ;

#endif
