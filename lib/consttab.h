/*
 * consttab.h
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
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
    bool cmp(list_item *a_item);
    int init(expression *a_exp, expression *a_cond, expression *a_ncond);
    int get(expression **a_exp, expression **a_cond, expression **a_ncond);
  private :
    bool m_init;
    expression *m_exp;
    expression *m_cond;
    expression *m_ncond;
} ;

class consttab : public list
{
  public :
    consttab();
    ~consttab();
    int add(expression *a_exp, expression *a_cond, expression *a_ncond);
    int get(unsigned int a_index,
            expression **a_exp,
            expression **a_cond,
            expression **a_ncond);
} ;
#endif
