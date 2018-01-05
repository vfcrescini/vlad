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

#ifndef __VLAD_VARLIST_H
#define __VLAD_VARLIST_H

#include <vlad/stringlist.h>

class vlad_varlist : public vlad_stringlist
{
  public :

    /* add a variable into the list */
    int add(const char *a_var);

    /* add the contents of the given list to this list */
    int add(vlad_varlist *a_vlist);

    /* delete all items that matches the items in the given list */
    int del(vlad_varlist *a_vlist);

    /* copy varlist */
    int copy(vlad_varlist **a_vlist);
} ;

#endif
