/*
 *  This file is part of PolicyUpdater.
 *
 *  Copyright (C) 2003, 2004, 2005 University of Western Sydney
 *  by Vino Fernando Crescini <jcrescin@cit.uws.edu.au>
 *
 *  PolicyUpdater is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  PolicyUpdater is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with PolicyUpdater; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __VLAD_TNET_H
#define __VLAD_TNET_H

#include <tribe/network.h>

#include <vlad/stringlist.h>
#include <vlad/varlist.h>
#include <vlad/symtab.h>
#include <vlad/rel.h>
#include <vlad/rlist.h>

class vlad_tnet : virtual public vlad_mem
{
  public :

    vlad_tnet();
    ~vlad_tnet();

    /* (re)initialise temporal network */
    int init();

    /* add a new interval (a_int must already be in the symtab */
    int add_interval(const char *a_int, vlad_symtab *a_stab);

    /* bind the given (existing) interval with the given endpoints */
    int add_endpoints(const char *a_int,
                      unsigned int a_ep1,
                      unsigned int a_ep2,
                      vlad_symtab *a_stab);

    /* add the given temporal constraints */
    int add_constraints(vlad_rlist *a_rlist, vlad_symtab *a_stab);

    /* gives the relation set that exists between the given 2 intervals */
    int get_relset(const char *a_int1,
                   const char *a_int2,
                   vlad_symtab *a_stab,
                   unsigned int *a_rs);

   /* checks whether the given tuple satisfies the given constraints */
   int check_tuple(vlad_stringlist *a_tuple,
                   vlad_varlist *a_vlist,
                   vlad_rlist *a_rlist,
                   vlad_symtab *a_stab);

  private :

    tbe_net m_net;
    bool m_init;
} ;

#endif
