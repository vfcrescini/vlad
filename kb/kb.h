/*
 * kb.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_KB_H
#define __VLAD_KB_H

#include <symtab.h>
#include <numberlist.h>

/* atom type */
#define VLAD_ATOM_CONST  0
#define VLAD_ATOM_HOLDS  1
#define VLAD_ATOM_MEMBER 2
#define VLAD_ATOM_SUBSET 3

/* convenience macros */
#define VLAD_ATOM_TYPE_VALID(X) (((X) >= 0) && ((X) < 4))
#define VLAD_ATOM_IS_CONST(X)   ((X) == VLAD_ATOM_CONST)
#define VLAD_ATOM_IS_HOLDS(X)   ((X) == VLAD_ATOM_HOLDS)
#define VLAD_ATOM_IS_MEMBER(X)  ((X) == VLAD_ATOM_MEMBER)
#define VLAD_ATOM_IS_SUBSET(X)  ((X) == VLAD_ATOM_SUBSET)

class kb {
  public :
    kb();
    ~kb();
    /* (re)init kb */
    int init();
    /* after this is called, no further calls to add_ident() is allowed */
    int close();
    /* register an identifier in the kb */
    int add_ident(const char *n, unsigned char t);
    /* gives an atom id based on the identifiers already given */
    int encode_atom(const char *n1,
                    const char *n2,
                    const char *n3,
                    unsigned char ty,
                    bool tr,
                    unsigned int *a);
    /* returns the negation of the given atom */
    int negate_atom(unsigned int in, unsigned int *out);
  private :
    symtab *stable;
    unsigned int s_len;
    unsigned int a_len;
    unsigned int o_len;
    unsigned int sg_len;
    unsigned int ag_len;
    unsigned int og_len;
    unsigned int h_tot;
    unsigned int m_tot;
    unsigned int s_tot;
    unsigned int pos_tot;
    unsigned int neg_tot;
    bool initialised;
    bool closed;
} ;

#endif
