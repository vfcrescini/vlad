/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004, 2005 University of Western Sydney
 * by Vino Fernando Crescini <jcrescin@cit.uws.edu.au>
 *
 * PolicyUpdater is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * PolicyUpdater is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PolicyUpdater; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __VLAD_ATOM_H
#define __VLAD_ATOM_H

#include <vlad/list.h>
#include <vlad/stringlist.h>

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
    /* get values from atom */
    int get(char **n1, char **n2, char **n3, unsigned char *ty, bool *tr);
    int get_holds(char **s, char **a, char **o);
    int get_member(char **e, char **g);
    int get_subset(char **g1, char **g2);
    /*initialise atoms */
    int init(char *n1, char *n2, char *n3, unsigned char ty, bool tr);
    int init_holds(const char *s, const char *a, const char *o, bool t);
    int init_member(const char *e, const char *g, bool t);
    int init_subset(const char *g1, const char *g2, bool t);
    /* create a new instance of this atom */
    int copy(atom **a);
    /* replaces vars in vlist with idents in ilist. new atom a */
    int replace(stringlist *vlist, stringlist *ilist, atom **a);
    void negate();
#ifdef VLAD_DEBUG
    /* assuming s has enough memory allocation */
    void print(char *s);
#endif
  private :
    int reset();
    unsigned char type;
    bool truth;
    union {
      holds_atom holds;
      member_atom member;
      subset_atom subset;
    } ;
    bool initialised;
} ;

#endif
