/*
 * atom.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_ATOM_H
#define __VLAD_ATOM_H

#include <list.h>

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

/* structure to hold atoms. no checking is done here. values are just stored */

typedef struct
{
  char *subject;
  char *access;
  char *object;
} holds_atom;

typedef struct
{
  char *element;
  char *group;
} member_atom;

typedef struct
{
  char *group1;
  char *group2;
} subset_atom;

class atom : public list_item
{
  public :
    atom();
    ~atom();
    bool cmp(list_item *item);
    int init_const(bool c, bool t);
    int init_holds(const char *s, const char *a, const char *o, bool t);
    int init_member(const char *e, const char *g, bool t);
    int init_subset(const char *g1, const char *g2, bool t);
    void negate();
#ifdef DEBUG
    /* assuming s has enough memory allocation */
    void print(char *s);
#endif
  private :
    unsigned char type;
    bool truth;
    union {
      holds_atom holds;
      member_atom member;
      subset_atom subset;
      bool constant;
    } ;
} ;

#endif
