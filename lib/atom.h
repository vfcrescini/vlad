/*
 *  This file is part of PolicyUpdater.
 * 
 *  Copyright (C) 2003, 2004, 2005 University of Western Sydney
 *  by Vino Fernando Crescini <jcrescin@scm.uws.edu.au>
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

#ifndef __VLAD_ATOM_H
#define __VLAD_ATOM_H

#include <vlad/vlad.h>

/* atom type */
#define VLAD_ATOM_HOLDS          0
#define VLAD_ATOM_MEMBER         1
#define VLAD_ATOM_SUBSET         2

#define VLAD_ATOM_TOTAL          3

/* convenience macros */
#define VLAD_ATOM_TYPE_VALID(X)  \
  (((X) >= VLAD_ATOM_HOLDS && (X) <= VLAD_ATOM_SUBSET)
#define VLAD_ATOM_IS_HOLDS(X)    ((X) == VLAD_ATOM_HOLDS)
#define VLAD_ATOM_IS_MEMBER(X)   ((X) == VLAD_ATOM_MEMBER)
#define VLAD_ATOM_IS_SUBSET(X)   ((X) == VLAD_ATOM_SUBSET)

#endif
