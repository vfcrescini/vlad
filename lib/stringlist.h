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

#include <vlad/list.h>

#ifndef __VLAD_STRINGLIST_H
#define __VLAD_STRINGLIST_H

class vlad_string : public vlad_list_item
{
  public :
    vlad_string();
    ~vlad_string();
    /* compare item with this string */
    bool cmp(vlad_list_item *a_item);
    int init(const char *a_str);
    char *get();
#ifdef VLAD_DEBUG
    /* assuming s has enough memory allocation */
    void print(char *a_str);
#endif
  private :
    char *m_string;
} ;

class vlad_stringlist : public vlad_list
{
  public :
    /* n is the optional name for the list */
    vlad_stringlist();
    ~vlad_stringlist();
    /* add a string in the list */
    int add(const char *a_str);
    /* get the index of the string */
    int get(const char *a_str, unsigned int *a_index);
    /* get the ith string in the list */
    int get(unsigned int a_index, char **a_str);
    /* return true if string is in the list */
    int find(const char *a_str);
#ifdef VLAD_DEBUG
    /* assumimg s has enough memory allocation */
    void print(char *a_str);
#endif
} ;

#endif
