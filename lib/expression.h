/*
 * expression.h
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_EXPRESSION_H
#define __VLAD_EXPRESSION_H

#include <vlad/list.h>
#include <vlad/fact.h>

/* a list of facts. no checking. */

class expression : public list
{
  public :
    expression();
    ~expression();
    /* add pre-malloc'ed factg */
    int add(fact *a_fact);
    /* return true if fact is in the expression */
    int find(fact *a_fact);
    /* give i'th fact */
    int get(unsigned int a_index, fact **a_fact);
    /* replace vars in vlist to entity in ilist. create a new expression */
    int replace(stringlist *a_vlist, stringlist *a_ilist, expression **a_exp);
#ifdef VLAD_DEBUG
    /* assumimg s has enough memory allocation */
    void print(char *a_str);
#endif
} ;

#endif
