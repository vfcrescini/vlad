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

#ifndef __VLAD_SMWRAP_H
#define __VLAD_SMWRAP_H

/* class smwrap for the smodels library */

#include <vlad/numberlist.h>
#include <smodels.h>
#include <api.h>
#include <atomrule.h>

class vlad_smwrap
{
  public :

    vlad_smwrap();
    ~vlad_smwrap();

    /* (re)initialise */
    int init();

    /* after this no more calls to add_atom() are allowed */
    int close_atom();

    /* after this no more calls to add_rule_*() are allowed */
    int close_rule();

    /* register an atom */
    int add_atom(unsigned int a_atom);

    /* add an axiom (always true or always false) */
    int add_axiom(bool a_tr, unsigned int a_count, ...);

    /* add rule: variable argument list */
    int add_rule(unsigned int a_pcount,
                 unsigned int a_ncount,
                 unsigned int a_head,
                 ...);

    /* add rule: numberlist */
    int add_rule(unsigned int a_head,
                 vlad_numberlist *a_pbody,
                 vlad_numberlist *a_nbody);

    /* give T or F depending on whether atom is in ALL models or not */
    int ask(unsigned int a_atom, bool *a_res);

  private :

    Smodels *m_smod;
    Api *m_api;
    int m_stage;
} ;

#endif
