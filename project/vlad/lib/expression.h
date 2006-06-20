/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004, 2005 University of Western Sydney
 * by Vino Fernando Crescini <jcrescin@scm.uws.edu.au>
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
#ifdef VLAD_DEBUG
    /* assumimg s has enough memory allocation */
    void print(char *s);
#endif
} ;

#endif
