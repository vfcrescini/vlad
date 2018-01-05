/*
 * atomlist.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_ATOMLIST_H
#define __VLAD_ATOMLIST_H

#include <list.h>
#include <atom.h>

class atomlist : public list
{
  public :
    atomlist();
    ~atomlist();
    /* add atom into list */
    int add(atom *a);
    /* del atom from list */
    int del(atom *a);
    /* get reference of atom that matches in (may have wildcards) */
    int get(atom *in, atom ***out, unsigned int *s);
    /* few more interfaces for adding atoms */
    int add_holds(identifier *s, identifier *a, identifier *o, bool t);
    int add_member(identifier *e, identifier *g, bool t);
    int add_subset(identifier *g1, identifier *g2, bool t);
} ;

#endif
