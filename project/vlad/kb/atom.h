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
#define VLAD_ATOM_IS_HOLDS(X)      ((X).get_type() == VLAD_ATOM_HOLDS)
#define VLAD_ATOM_IS_MEMBER(X)     ((X).get_type() == VLAD_ATOM_MEMBER)
#define VLAD_ATOM_IS_SUBSET(X)     ((X).get_type() == VLAD_ATOM_SUBSET)

class atom : public list_item
{
  public :
    atom();
    atom(unsigned char ty, bool tr);
    ~atom();
    virtual bool verify() = 0;
    virtual bool cmp(list_item *item) = 0;
    unsigned char get_type();
    bool get_truth();
  private :
    unsigned char type;
    bool truth;
} ;

class holds_atom : public atom
{
  public :
    holds_atom();
    holds_atom(identifier *s, identifier *a, identifier *o, bool t);
    ~holds_atom();
    bool verify();
    void get(identifier **s, identifier **a, identifier **o);
    bool cmp(list_item *item);
  private :
    identifier *sub;
    identifier *acc;
    identifier *obj;
} ;

class member_atom : public atom
{
  public :
    member_atom();
    member_atom(identifier *e, identifier *g, bool t);
    ~member_atom();
    bool verify();
    void get(identifier **e, identifier **g);
    bool cmp(list_item *item);
  private :
    identifier *elt;
    identifier *grp;
} ;

class subset_atom : public atom
{
  public :
    subset_atom();
    subset_atom(identifier *g1, identifier *g2, bool t);
    ~subset_atom();
    bool verify();
    void get(identifier **g1, identifier **g2);
    bool cmp(list_item *item);
  private :
    identifier *grp1;
    identifier *grp2;
} ;

#endif
