/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004 University of Western Sydney
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

#ifndef __VLAD_SEQTAB_H
#define __VLAD_SEQTAB_H

#include <vlad/list.h>
#include <vlad/expression.h>
#include <vlad/stringlist.h>

class updateref : public list_item
{
  public :
    updateref();
    ~updateref();
    bool cmp(list_item *a_item);
    int init(const char *a_name, stringlist *a_list);
    int get(char **a_name, stringlist **a_list);
#ifdef VLAD_DEBUG
    void print(char *a_str);
#endif
  private :
    char *m_name;
    stringlist *m_list;
    bool m_init;
} ;

class seqtab : public list
{
   public :
     seqtab();
     ~seqtab();
     /* add pre-malloc'ed updateref */
     int add(updateref *a_uref);
     /* add pre-malloc'ed name and ilist */
     int add(const char *a_name, stringlist *a_list);
     /* delete i'th item */
     int del(unsigned int a_index);
     /* get i'th name and ilist */
     int get(unsigned int a_index, char **a_name, stringlist **a_list);
#ifdef VLAD_DEBUG
    void print(char *a_str);
#endif
} ;

#endif
