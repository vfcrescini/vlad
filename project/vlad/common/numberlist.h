/*
 * numberlist.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_NUMBERLIST_H
#define __VLAD_NUMBERLIST_H

#include <list.h>

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

class numberlist : public list
{
  public :
    numberlist();
    ~numberlist();
    /* add a number in the list */
    int add(unsigned int n);
    /* get the index of the number */
    int geti(unsigned int n, unsigned int *i);
    /* get the ith number in the list */
    int getn(unsigned int i, unsigned int *n);
    /* return true if number is in the list */
    int find(unsigned int n);
} ;

#endif
