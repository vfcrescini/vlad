/*
 * consttab.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_CONSTTAB_H
#define __VLAD_CONSTTAB_H

#include <list.h>
#include <expression.h>

/* consttab list is simply a list of expression/condition pairs */

class constraint : public list_item
{
  public :
    constraint(expression *e, expression *c, expression *n);
    ~constraint();
    bool cmp(list_item *item);
  private :
    expression *exp;
    expression *cond;
    expression *ncond;
} ;

class consttab : public list
{
  public :
    consttab(const char *n);
    ~consttab();
    int add(expression *e, expression *c, expression *n);
} ;

#endif
