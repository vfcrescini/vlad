/*
 * atom.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_ATOM_H
#define __VLAD_ATOM_H

#include <identifier.h>

#define VLAD_ATOM_NULL   0
#define VLAD_ATOM_HOLDS  1
#define VLAD_ATOM_MEMBER 2
#define VLAD_ATOM_SUBSET 3

#define VLAD_ATOM_IS_TYPE_VALID(X) (((X) > 0) && ((X) < 4))
#define VLAD_ATOM_IS_HOLDS(X)      ((X) == VLAD_ATOM_HOLDS)
#define VLAD_ATOM_IS_MEMBER(X)     ((X) == VLAD_ATOM_MEMBER)
#define VLAD_ATOM_IS_SUBSET(X)     ((X) == VLAD_ATOM_SUBSET)

class holds_atom
{
  public :
    holds_atom();
    holds_atom(identifier *s, identifier *a, identifier *o);
    ~holds_atom();
    bool verify();
    void get(identifier **s, identifier **a, identifier **o);
  private :
    identifier *sub;
    identifier *acc;
    identifier *obj;
} ;

class member_atom
{
  public :
    member_atom();
    member_atom(identifier *e, identifier *g);
    ~member_atom();
    bool verify();
    void get(identifier **e, identifier **g);
  private :
    identifier *elt;
    identifier *grp;
} ;

class subset_atom
{
  public :
    subset_atom();
    subset_atom(identifier *g1, identifier *g2);
    ~subset_atom();
    bool verify();
    void get(identifier **g1, identifier **g2);
  private :
    identifier *grp1;
    identifier *grp2;
} ;

class atom : public list_item
{
  public :
    atom();
    ~atom();
    int init_holds(identifier *s, identifier *a, identifier *o, bool t);
    int init_member(identifier *e, identifier *g, bool t);
    int init_subset(identifier *g1, identifier *g2, bool t);
    unsigned char get_type();
    int get(identifier **i1, identifier **i2, identifier **i3, bool *tr, unsigned char *ty);
    /* verify the integrity of this atom */
    bool verify();
    /* implementation of cmp() in list_item class */
    bool cmp(list_item *item);
  private :
    unsigned char type;
    bool truth;
    union {
      holds_atom *holds;
      member_atom *member;
      subset_atom *subset;
    } ;
} ;

#endif
