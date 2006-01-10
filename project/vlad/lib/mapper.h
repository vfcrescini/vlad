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

#ifndef __VLAD_MAPPER_H
#define __VLAD_MAPPER_H

#include <vlad/mem.h>
#include <vlad/symtab.h>
#include <vlad/fact.h>

/* a class that maps facts to integers */

class vlad_mapper : virtual public vlad_mem
{
  public :

    vlad_mapper();
    ~vlad_mapper();

    /* (re)init mapper */
    int init(vlad_symtab *a_stab);

    /* get the total numbers */
    unsigned int get_totals(unsigned int a_type);

    /* given the state, truth value and indices, map a holds fact into an id */
    unsigned int compute_holds(unsigned int a_sub,
                               unsigned int a_acc,
                               unsigned int a_obj,
                               unsigned int a_state,
                               bool a_truth);

    /* given the state, truth value and indices, map a memb fact into an id */
    unsigned int compute_memb(unsigned char a_type,
                              unsigned int a_elt,
                              unsigned int a_grp,
                              unsigned int a_state,
                              bool a_truth);

    /* given the state, truth value and indices, map a subst fact into an id */
    unsigned int compute_subst(unsigned char a_type,
                               unsigned int a_grp1,
                               unsigned int a_grp2,
                               unsigned a_state,
                               bool a_truth);

    /* used by the other compute_*() functions */
    unsigned int compute_fact(unsigned int a_fact,
                              unsigned int a_state,
                              bool a_truth);

    /* gives a unique id based on the fact given */
    int encode_fact(vlad_fact *a_fact,
                    unsigned int a_state,
                    unsigned int *a_id);

    /* gives the fact based on the id given */
    int decode_fact(unsigned int a_id,
                    unsigned int *a_state,
                    vlad_fact **a_fact);

    /* returns the id of the negation of the given fact id */
    unsigned int negate_fact(unsigned int a_fact);

  private :

    bool m_init;
    vlad_symtab *m_stab;

    /* gives a unique id based on the holds entities given */
    int encode_holds(const char *a_sub,
                     const char *a_acc,
                     const char *a_obj,
                     unsigned int a_state,
                     bool a_truth,
                     unsigned int *a_id);

    /* gives a unique id based on the member entities given */
    int encode_memb(const char *a_elt,
                    const char *a_grp,
                    unsigned int a_state,
                    bool a_truth,
                    unsigned int *a_id);

    /* gives a unique id based on the subset entities given */
    int encode_subst(const char *a_grp1,
                     const char *a_grp2,
                     unsigned int a_state,
                     bool a_truth,
                     unsigned int *a_id);

    /* gives entities based on the holds id given */
    int decode_holds(unsigned int a_id,
                     char **a_sub,
                     char **a_acc,
                     char **a_obj);

    /* gives entities based on the member id given */
    int decode_memb(unsigned int a_id, char **a_elt, char **a_grp);

    /* gives entities based on the subset id given */
    int decode_subst(unsigned int a_id, char **a_grp1, char **a_grp2);
} ;

#endif
