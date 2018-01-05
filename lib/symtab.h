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
    int init();
    /* add symbol in symbol table */
    int add(const char *s, unsigned char t);
    /* get the i'th identifier */
    int get(unsigned int i, char **s);
    /* get the index and type of the identifier based on the name */
    int get(const char *s, unsigned int *i, unsigned char *t);
    /* get the ith identifier of type t */
    int get(unsigned int i, unsigned char t, char **s);
    /* get an array of identifiers that matches the given type */
    int get(unsigned char t, char ***a, unsigned int *s);
    /* return the total number of indentifers */
    unsigned int length();
    /* return the number of identifiers that are of type t */
    unsigned int length(unsigned char t);
    /* return 0 if symbol is in the table */
    int find(const char *s);
    /* return 0 if symbol of type t is in the table */
    int find(const char *s, unsigned char t);
    /* give the type of the given identifier */
    int type(const char *s, unsigned char *t);
  private :
    stringlist *sub_list;
    stringlist *acc_list;
    stringlist *obj_list;
    stringlist *sub_grp_list;
    stringlist *acc_grp_list;
    stringlist *obj_grp_list;
    bool initialised;
} ;

#endif
