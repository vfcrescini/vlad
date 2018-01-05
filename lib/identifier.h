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

#include <vlad/vlad.h>
#include <vlad/mem.h>

/* maximum length of an identifier */

#define VLAD_MAXLEN_IDENT            128

/* base types */

#define VLAD_IDENT_SUB               1   /* 0000 0001 */
#define VLAD_IDENT_ACC               2   /* 0000 0010 */
#define VLAD_IDENT_OBJ               3   /* 0000 0011 */

/* masks */

#define VLAD_IDENT_BASE_MASK         3   /* 0000 0011 */
#define VLAD_IDENT_SIN_MASK          4   /* 0000 0100 */
#define VLAD_IDENT_GRP_MASK          8   /* 0000 1000 */
#define VLAD_IDENT_VAR_MASK          16  /* 0001 0000 */

/* identifier types */

#define VLAD_IDENT_NUL               0

#define VLAD_IDENT_ENT_SUB_SIN       \
  (VLAD_IDENT_SUB | VLAD_IDENT_SIN_MASK)
#define VLAD_IDENT_ENT_ACC_SIN       \
  (VLAD_IDENT_ACC | VLAD_IDENT_SIN_MASK)
#define VLAD_IDENT_ENT_OBJ_SIN       \
  (VLAD_IDENT_OBJ | VLAD_IDENT_SIN_MASK)
#define VLAD_IDENT_ENT_SUB_GRP       \
  (VLAD_IDENT_SUB | VLAD_IDENT_GRP_MASK)
#define VLAD_IDENT_ENT_ACC_GRP       \
  (VLAD_IDENT_ACC | VLAD_IDENT_GRP_MASK)
#define VLAD_IDENT_ENT_OBJ_GRP       \
  (VLAD_IDENT_OBJ | VLAD_IDENT_GRP_MASK)

#define VLAD_IDENT_VAR_SUB           \
  (VLAD_IDENT_SUB | VLAD_IDENT_SIN_MASK | VLAD_IDENT_GRP_MASK | VLAD_IDENT_VAR_MASK)
#define VLAD_IDENT_VAR_ACC           \
  (VLAD_IDENT_ACC | VLAD_IDENT_SIN_MASK | VLAD_IDENT_GRP_MASK | VLAD_IDENT_VAR_MASK)
#define VLAD_IDENT_VAR_OBJ           \
  (VLAD_IDENT_OBJ | VLAD_IDENT_SIN_MASK | VLAD_IDENT_GRP_MASK | VLAD_IDENT_VAR_MASK)
#define VLAD_IDENT_VAR_SUB_SIN       \
  (VLAD_IDENT_SUB | VLAD_IDENT_SIN_MASK | VLAD_IDENT_VAR_MASK)
#define VLAD_IDENT_VAR_ACC_SIN       \
  (VLAD_IDENT_ACC | VLAD_IDENT_SIN_MASK | VLAD_IDENT_VAR_MASK)
#define VLAD_IDENT_VAR_OBJ_SIN       \
  (VLAD_IDENT_OBJ | VLAD_IDENT_SIN_MASK | VLAD_IDENT_VAR_MASK)
#define VLAD_IDENT_VAR_SUB_GRP       \
  (VLAD_IDENT_SUB | VLAD_IDENT_GRP_MASK | VLAD_IDENT_VAR_MASK)
#define VLAD_IDENT_VAR_ACC_GRP       \
  (VLAD_IDENT_ACC | VLAD_IDENT_GRP_MASK | VLAD_IDENT_VAR_MASK)
#define VLAD_IDENT_VAR_OBJ_GRP       \
  (VLAD_IDENT_OBJ | VLAD_IDENT_GRP_MASK | VLAD_IDENT_VAR_MASK)

/* total number of types */

#define VLAD_IDENT_ENT_TOTAL         6
#define VLAD_IDENT_VAR_TOTAL         9

/* additional macros */

#define VLAD_IDENT_TYPE_BASETYPE(X)  ((X) & VLAD_IDENT_BASE_MASK)

#define VLAD_IDENT_TYPE_IS_SUB(X)    \
  (((X) & VLAD_IDENT_BASE_MASK) == VLAD_IDENT_SUB)

#define VLAD_IDENT_TYPE_IS_ACC(X)    \
  (((X) & VLAD_IDENT_BASE_MASK) == VLAD_IDENT_ACC)

#define VLAD_IDENT_TYPE_IS_OBJ(X)    \
  (((X) & VLAD_IDENT_BASE_MASK) == VLAD_IDENT_OBJ)

#define VLAD_IDENT_TYPE_IS_SIN(X)    ((X) & VLAD_IDENT_SIN_MASK)
#define VLAD_IDENT_TYPE_IS_GRP(X)    ((X) & VLAD_IDENT_GRP_MASK)

/* a helper class containing static functions to validate identifiers */

class vlad_identifier : virtual public vlad_mem
{
  public :

    vlad_identifier();
    ~vlad_identifier();

    /* returns VLAD_OK if the given identifier is a valid entity */
    static int validate_ent_ident(const char *a_ident);

    /* returns VLAD_OK if the given identifier is a valid variable */
    static int validate_var_ident(const char *a_ident);

    /* returns VLAD_OK if the given type is a valid entity type */
    static int validate_ent_type(unsigned char a_type);

    /* returns VLAD_OK if the given type is a valid variable type */
    static int validate_var_type(unsigned char a_type);

    /* returns the type of the given variable */
    static unsigned char get_var_type(const char *a_var);

    /* returns VLAD_OK if the entity type is compatible with the var */
    static int check_compat(const char *a_var, unsigned char a_type);
} ;

#endif
