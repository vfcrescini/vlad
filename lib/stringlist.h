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

#include <vlad/list.h>

#ifndef __VLAD_STRINGLIST_H
#define __VLAD_STRINGLIST_H

class vlad_stringlist_item : public vlad_list_item
{
  public :

    vlad_stringlist_item();
    ~vlad_stringlist_item();

    /* compare item with this string */
    bool cmp(vlad_list_item *a_item);

    /* return an exact duplicate of this item */
    vlad_list_item *copy();

    /* init with str */
    int init(const char *a_str);

    /* get value */
    char *get();

#ifdef VLAD_DEBUG
    /* assuming s has enough memory allocation */
    void print(char *a_str);
#endif

  private :

    char *m_string;
} ;

/* class for traversing */
class vlad_stringlist_trav : public vlad_list_trav
{
  public :

    virtual int trav(const char *a_str) = 0;
} ;

class vlad_stringlist : public vlad_list, public vlad_list_item
{
  public :

    vlad_stringlist();
    vlad_stringlist(bool a_uniq);
    ~vlad_stringlist();

    /* compares 2 lists */
    bool cmp(vlad_list_item *a_item);

    /* add a string in the list */
    int add(const char *a_str);

    /* add the contents of the given list to this list */
    int add(vlad_stringlist *a_slist);

    /* delete all items that matches the items in the given list */
    int del(vlad_stringlist *a_slist);

    /* get the index of the string (will only work if m_uniq is true) */
    int get(const char *a_str, unsigned int *a_index);

    /* get the ith string in the list */
    int get(unsigned int a_index, char **a_str);

    /* make a copy of this stringlist */
    int copy(vlad_stringlist **a_slist);

    /* return true if string is in the list */
    int find(const char *a_str);

#ifdef VLAD_DEBUG
    /* assumimg s has enough memory allocation */
    void print(char *a_str);
#endif

  private :

    bool m_uniq;

    /* called by vlad_list::traverse() */
    int trav(vlad_list_item *a_item, vlad_list_trav *a_trav);
} ;

#endif
