/*
 * expression.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_EXPRESSION_H
#define __VLAD_EXPRESSION_H

#include <list.h>
#include <atom.h>

/* a list of atoms. no checking. */

class expression : public list
{
  public :
    expression(const char *n);
    ~expression();
    /* add pre-malloc'ed atom */
    int add(atom *a);
    /* return true if atom is in the expression */
    int find(atom a);
#ifdef DEBUG
    /* assumimg s has enough memory allocation */
    void print(char *s);
#endif
} ;

#endif
