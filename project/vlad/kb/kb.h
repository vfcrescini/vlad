/*
 * kb.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_KB_H
#define __VLAD_KB_H

#include <symtab.h>

/* atom type */
#define VLAD_ATOM_CONST  0
#define VLAD_ATOM_HOLDS  1
#define VLAD_ATOM_MEMBER 2
#define VLAD_ATOM_SUBSET 3

/* convenience macros */
#define VLAD_ATOM_TYPE_VALID(X) (((X) >= 0) && ((X) < 4))
#define VLAD_ATOM_IS_HOLDS(X)   ((X) == VLAD_ATOM_HOLDS)
#define VLAD_ATOM_IS_MEMBER(X)  ((X) == VLAD_ATOM_MEMBER)
#define VLAD_ATOM_IS_SUBSET(X)  ((X) == VLAD_ATOM_SUBSET)

class kb {
  public :
    kb();
    ~kb();
    /* (re)init kb */
    int init();
    /* register an identifier in the kb */
    int add_ident(const char *n, unsigned char t);
    /* gives an atom id based on the identifiers already given */
    int get_atom(const char *n1,
                 const char *n2,
                 const char *n3,
                 unsigned char t,
                 unsigned int *a);
  private :
    symtab *stable;
} ;

#endif
