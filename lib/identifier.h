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

#ifndef __VLAD_IDENTIFIER_H
#define __VLAD_IDENTIFIER_H

#include <vlad/vlad.h>

/* maximum length of an identifier */
#define VLAD_MAXLEN_IDENT        128

/* identifier types */

#define VLAD_IDENT_VAR           0 /* 0000 */
#define VLAD_IDENT_SUB_SIN       1 /* 0001 */
#define VLAD_IDENT_ACC_SIN       2 /* 0010 */
#define VLAD_IDENT_OBJ_SIN       3 /* 0011 */
#define VLAD_IDENT_SUB_GRP       5 /* 0101 */
#define VLAD_IDENT_ACC_GRP       6 /* 0110 */
#define VLAD_IDENT_OBJ_GRP       7 /* 0111 */

#define VLAD_IDENT_GRP           4 /* 0100 */
#define VLAD_IDENT_MASK          7 /* 0111 */

#define VLAD_IDENT_TOTAL         6

/* convenience macros */

#define VLAD_IDENT_BASETYPE(X)   \
  ((X) & VLAD_IDENT_MASK & ~VLAD_IDENT_GRP)

#define VLAD_IDENT_IS_VALID(X) ( \
  ((X) == VLAD_IDENT_SUB_SIN) || \
  ((X) == VLAD_IDENT_ACC_SIN) || \
  ((X) == VLAD_IDENT_OBJ_SIN) || \
  ((X) == VLAD_IDENT_SUB_GRP) || \
  ((X) == VLAD_IDENT_ACC_GRP) || \
  ((X) == VLAD_IDENT_OBJ_GRP)    \
)

#define VLAD_IDENT_IS_SUBJECT(X) \
  (((X) & VLAD_IDENT_MASK & ~VLAD_IDENT_GRP) == VLAD_IDENT_SUB_SIN)

#define VLAD_IDENT_IS_ACCESS(X)  \
  (((X) & VLAD_IDENT_MASK & ~VLAD_IDENT_GRP) == VLAD_IDENT_ACC_SIN)

#define VLAD_IDENT_IS_OBJECT(X)  \
  (((X) & VLAD_IDENT_MASK & ~VLAD_IDENT_GRP) == VLAD_IDENT_OBJ_SIN)

#define VLAD_IDENT_IS_GROUP(X)   ((X) & VLAD_IDENT_GRP)

/* anything that starts with a small letter is an entity identifier */
#define VLAD_IDENT_IS_ENT(X)     ((X) && ((X)[0] >= 'a') && ((X)[0] <= 'z'))

/* anything that starts with a capital letter is a variable */
#define VLAD_IDENT_IS_VAR(X)     ((X) && ((X)[0] >= 'A') && ((X)[0] <= 'Z'))

#endif
