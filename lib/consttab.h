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

#ifndef __VLAD_CONSTTAB_H
#define __VLAD_CONSTTAB_H

#include <vlad/list.h>
#include <vlad/expression.h>

/* consttab list is simply a list of expression/condition pairs */

class constraint : public list_item
{
  public :
    constraint();
    ~constraint();
    bool cmp(list_item *item);
    int init(expression *e, expression *c, expression *n);
    int get(expression **e, expression **c, expression **n);
  private :
    bool initialised;
    expression *exp;
    expression *cond;
    expression *ncond;
} ;

class consttab : public list
{
  public :
    consttab();
    ~consttab();
    int add(expression *e, expression *c, expression *n);
    int get(unsigned int i, expression **e, expression **c, expression **n);
} ;

#endif
