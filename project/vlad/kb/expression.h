/*
 * expression.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_EXPRESSION_H
#define __VLAD_EXPRESSION_H

#include <vlad/list.h>
#include <vlad/atom.h>

/* a list of atoms. no checking. */

class expression : public list
{
  public :
    expression();
    ~expression();
    /* add pre-malloc'ed atom */
    int add(atom *a);
    /* return true if atom is in the expression */
    int find(atom *a);
    /* give i'th atom */
    int get(unsigned int i, atom **a);
    /* replace vars in vlist to ident in ilist. create a new expression */
    int replace(stringlist *vlist, stringlist *ilist, expression **e);
#ifdef DEBUG
    /* assumimg s has enough memory allocation */
    void print(char *s);
#endif
} ;

#endif
