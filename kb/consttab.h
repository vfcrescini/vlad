/*
 * consttab.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_CONSTTAB_H
#define __VLAD_CONSTTAB_H

#include <vlad/list.h>
#include <vlad/expression.h>

/* consttab list is simply a list of expression/condition pairs */

class constraint : public list_item
{
  public :
    constraint();
    ~constraint();
    bool cmp(list_item *item);
    int init(expression *e, expression *c, expression *n);
    int get(expression **e, expression **c, expression **n);
  private :
    bool initialised;
    expression *exp;
    expression *cond;
    expression *ncond;
} ;

class consttab : public list
{
  public :
    consttab();
    ~consttab();
    int add(expression *e, expression *c, expression *n);
    int get(unsigned int i, expression **e, expression **c, expression **n);
} ;

#endif
