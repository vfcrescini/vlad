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

#ifndef __VLAD_TRANSTAB_H
#define __VLAD_TRANSTAB_H

#include <vlad/list.h>
#include <vlad/expression.h>
#include <vlad/stringlist.h>

class transdef : public list_item
{
  public :
    transdef();
    ~transdef();
    bool cmp(list_item *item);
    int get(char **n, stringlist **v, expression **pr, expression **po);
    int init(const char *n, stringlist *v, expression *pr, expression *po);
  private :
    char *name;
    stringlist *vlist;
    expression *precond;
    expression *postcond;
    bool initialised;
} ;

class transtab : public list
{
  public :
    transtab();
    ~transtab();
    int add(const char *n, stringlist *v, expression *pr, expression *po);
    /* get trans by name */
    int get(const char *n, stringlist **v, expression **pr, expression **po);
    /* get trans by index */
    int get(unsigned int i,
            char **n,
            stringlist **v,
            expression **pr,
            expression **po);
    /* replace variables with identifiers in ilist, then get pr and pp */
    int replace(const char *n, stringlist *ilist, expression **pr, expression **po);
} ;

#endif
