/*
 * consttab.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_CONSTRAINT_H
#define __VLAD_CONSTRAINT_H

#include <list.h>
#include <numberlist.h>

/* consttab list is simply a list of expression/condition pairs */

class constraint : public list_item
{
  public :
    constraint(numberlist *e, numberlist *c, numberlist *n);
    ~constraint();
    bool cmp(list_item *item);
  private :
    numberlist *exp;
    numberlist *cond;
    numberlist *ncond;
} ;

class consttab : public list
{
  public :
    consttab();
    ~consttab();
    int add(numberlist *e, numberlist *c, numberlist *n);
} ;

#endif
