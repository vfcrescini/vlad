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

#ifndef __VLAD_PARSER_H
#define __VLAD_PARSER_H

#include <cstdio>
#include <vlad/polbase.h>

/* parsers must first be initialised to be used */
int policy_init(FILE *a_in, FILE *a_out, FILE *a_err, vlad_polbase *a_pbase);

#ifdef VLAD_TIMER
int agent_init(FILE *a_in,
                   FILE *a_out,
                   FILE *a_err,
                   vlad_polbase *a_pbase,
                   unsigned char a_m,
                   bool a_t);
#else
int agent_init(FILE *a_in,
                   FILE *a_out,
                   FILE *a_err,
                   vlad_polbase *a_pbase,
                   unsigned char a_m);
#endif

/* it is important that these functions are used instead of the externally
 * available yyparse() functions */
int policy_parse();

int agent_parse();

#endif
