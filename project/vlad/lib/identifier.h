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

#ifndef __VLAD_IDENTIFIER_H
#define __VLAD_IDENTIFIER_H

#include <vlad/config.h>
#include <vlad/vlad.h>

/* maximum length of an identifier */
#define VLAD_MAXLEN_IDENT        128

/* identifier types */
#define VLAD_IDENT_SUB_SIN       0
#define VLAD_IDENT_ACC_SIN       1
#define VLAD_IDENT_OBJ_SIN       2
#define VLAD_IDENT_SUB_GRP       3
#define VLAD_IDENT_ACC_GRP       4
#define VLAD_IDENT_OBJ_GRP       5
#define VLAD_IDENT_FIRST         VLAD_IDENT_SUB_SIN
#define VLAD_IDENT_MID           VLAD_IDENT_SUB_GRP
#define VLAD_IDENT_LAST          VLAD_IDENT_OBJ_GRP

/* convenience macros */
#define VLAD_IDENT_BASETYPE(X)   \
  ((VLAD_IDENT_IS_GROUP(X)) ? ((X) - VLAD_IDENT_MID) : (X))
#define VLAD_IDENT_IS_SUBJECT(X) \
  (VLAD_IDENT_BASETYPE(X) == VLAD_IDENT_SUB_SIN)
#define VLAD_IDENT_IS_ACCESS(X)  \
  (VLAD_IDENT_BASETYPE(X) == VLAD_IDENT_ACC_SIN)
#define VLAD_IDENT_IS_OBJECT(X)  \
  (VLAD_IDENT_BASETYPE(X) == VLAD_IDENT_OBJ_SIN)
#define VLAD_IDENT_IS_GROUP(X)   ((X) >= VLAD_IDENT_MID)

/* anything that starts with a small letter is an entity identifier */
#define VLAD_IDENT_IS_IDENT(X)   \
  ((X) ? (((X)[0] >= 'a' && (X)[0] <= 'z') ? true : false) : false)

/* anything that starts with a capital letter is a variable */
#define VLAD_IDENT_IS_VAR(X)     \
  ((X) ? (((X)[0] >= 'A' && (X)[0] <= 'Z') ? true : false) : false)

#endif
