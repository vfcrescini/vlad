/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004, 2005, 2006 University of Western Sydney
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
#include <vlad/symtab.h>
#include <vlad/updatetab.h>
#include <vlad/expression.h>
#include <vlad/stringlist.h>

class vlad_updateref : public vlad_list_item
{
  public :

    vlad_updateref();
    ~vlad_updateref();

    /* compares two update references */
    bool cmp(vlad_list_item *a_item);

    /* initialise with this update ref */
    int init(const char *a_name, vlad_stringlist *a_list);

    /* get a reference of this update ref */
    int get(char **a_name, vlad_stringlist **a_list);

    /* verify that entities are in symtab and update is in updatetab */
    int verify(vlad_symtab *a_stab, vlad_updatetab *a_utab);

#ifdef VLAD_DEBUG
    void print(char *a_str);
#endif

  private :

    char *m_name;
    vlad_stringlist *m_list;
    bool m_init;
} ;

class vlad_seqtab : public vlad_list
{
   public :

     vlad_seqtab();
     ~vlad_seqtab();

     /* add pre-malloc'ed updateref */
     int add(vlad_updateref *a_uref);

     /* add pre-malloc'ed name and ilist */
     int add(const char *a_name, vlad_stringlist *a_list);

     /* delete i'th item */
     int del(unsigned int a_index);

     /* get i'th name and ilist */
     int get(unsigned int a_index, char **a_name, vlad_stringlist **a_list);

#ifdef VLAD_DEBUG
    void print(char *a_str);
#endif
} ;

#endif
