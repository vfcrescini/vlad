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

#ifndef __VLAD_SYMTAB_H
#define __VLAD_SYMTAB_H

#include <vlad/stringlist.h>

class symtab
{
  public :
    symtab();
    ~symtab();
    /* initialise */
    int init();
    /* add symbol in symbol table */
    int add(const char *a_s, unsigned char a_t);
    /* get the index and type of the identifier based on the name */
    int get(const char *a_s, unsigned int *a_i, unsigned char *a_t);
    /* get the ith identifier of type t */
    int get(unsigned int a_i, unsigned char a_t, char **a_s);
    /* get an array of identifiers that matches the given type */
    int get(unsigned char a_t, char ***a_a, unsigned int *a_s);
    /* return the number of identifiers that are of type t */
    unsigned int length(unsigned char a_t);
    /* return 0 if symbol is in the table */
    int find(const char *a_s);
    /* return 0 if symbol of type t is in the table */
    int find(const char *a_s, unsigned char a_t);
    /* give the type of the given identifier */
    int type(const char *a_s, unsigned char *a_t);
  private :
    /* m_list: 0 = ss, 1 = as, 2 = os, 3 = sg, 4 = ag, 5 = og */
    stringlist *m_lists[VLAD_IDENT_LAST + 1];
    bool m_init;
} ;

#endif
