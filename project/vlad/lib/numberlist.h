/*
 * numberlist.h
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_NUMBERLIST_H
#define __VLAD_NUMBERLIST_H

#include <vlad/list.h>

/* a number */
class number : public list_item
{
  public :
    number();
    ~number();
    /* compare item with this number */
    bool cmp(list_item *a_item);
    int init(unsigned int a_num);
    unsigned int get();
  private :
    unsigned int m_number;
} ;

/* a list of numbers */
class numberlist : public list
{
  public :
    numberlist();
    ~numberlist();
    /* add a number in the list */
    int add(unsigned int a_num);
    /* get the i'th number in the list */
    int get(unsigned int a_index, unsigned int *a_num);
    /* empty the list */
    void purge();
} ;

#endif
