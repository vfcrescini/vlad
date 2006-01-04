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

/* masks */

#define VLAD_IDENT_MASK_NUL          0   /* 0000 0000 */
#define VLAD_IDENT_MASK_ENT          1   /* 0000 0001 */
#define VLAD_IDENT_MASK_INT          2   /* 0000 0010 */

#define VLAD_IDENT_MASK_VAR          4   /* 0000 0100 */

#define VLAD_IDENT_MASK_ENT_SUB      8   /* 0000 1000 */
#define VLAD_IDENT_MASK_ENT_ACC      16  /* 0001 0000 */
#define VLAD_IDENT_MASK_ENT_OBJ      32  /* 0010 0000 */

#define VLAD_IDENT_MASK_ENT_SIN      64  /* 0100 0000 */
#define VLAD_IDENT_MASK_ENT_GRP      128 /* 1000 0000 */
#define VLAD_IDENT_MASK_ENT_BOT      192 /* 1100 0000 */

#define VLAD_IDENT_MASK_TYPE         3   /* 0000 0011 */
#define VLAD_IDENT_MASK_ENT_BASE     56  /* 0011 1000 */
#define VLAD_IDENT_MASK_ENT_QUAN     192 /* 1100 0000 */

/* identifier types */

#define VLAD_IDENT_NUL               VLAD_IDENT_MASK_NUL
#define VLAD_IDENT_ENT               VLAD_IDENT_MASK_ENT
#define VLAD_IDENT_INT               VLAD_IDENT_MASK_INT

/* entity identifier bases */

#define VLAD_IDENT_ENT_SUB           VLAD_IDENT_MASK_ENT_SUB
#define VLAD_IDENT_ENT_ACC           VLAD_IDENT_MASK_ENT_ACC
#define VLAD_IDENT_ENT_OBJ           VLAD_IDENT_MASK_ENT_OBJ

/* entity types */

#define VLAD_IDENT_ENT_SUB_SIN       \
  (VLAD_IDENT_MASK_ENT | VLAD_IDENT_MASK_ENT_SUB | VLAD_IDENT_MASK_ENT_SIN)
#define VLAD_IDENT_ENT_ACC_SIN       \
  (VLAD_IDENT_MASK_ENT | VLAD_IDENT_MASK_ENT_ACC | VLAD_IDENT_MASK_ENT_SIN)
#define VLAD_IDENT_ENT_OBJ_SIN       \
  (VLAD_IDENT_MASK_ENT | VLAD_IDENT_MASK_ENT_OBJ | VLAD_IDENT_MASK_ENT_SIN)
#define VLAD_IDENT_ENT_SUB_GRP       \
  (VLAD_IDENT_MASK_ENT | VLAD_IDENT_MASK_ENT_SUB | VLAD_IDENT_MASK_ENT_GRP)
#define VLAD_IDENT_ENT_ACC_GRP       \
  (VLAD_IDENT_MASK_ENT | VLAD_IDENT_MASK_ENT_ACC | VLAD_IDENT_MASK_ENT_GRP)
#define VLAD_IDENT_ENT_OBJ_GRP       \
  (VLAD_IDENT_MASK_ENT | VLAD_IDENT_MASK_ENT_OBJ | VLAD_IDENT_MASK_ENT_GRP)

/* interval type */

#define VLAD_IDENT_INT               VLAD_IDENT_MASK_INT

/* variable types */

#define VLAD_IDENT_VAR_SUB           \
  (VLAD_IDENT_MASK_ENT | VLAD_IDENT_MASK_VAR | VLAD_IDENT_MASK_ENT_SUB | VLAD_IDENT_MASK_ENT_SIN | VLAD_IDENT_MASK_ENT_GRP)
#define VLAD_IDENT_VAR_ACC           \
  (VLAD_IDENT_MASK_ENT | VLAD_IDENT_MASK_VAR | VLAD_IDENT_MASK_ENT_ACC | VLAD_IDENT_MASK_ENT_SIN | VLAD_IDENT_MASK_ENT_GRP)
#define VLAD_IDENT_VAR_OBJ           \
  (VLAD_IDENT_MASK_ENT | VLAD_IDENT_MASK_VAR | VLAD_IDENT_MASK_ENT_OBJ | VLAD_IDENT_MASK_ENT_SIN | VLAD_IDENT_MASK_ENT_GRP)

#define VLAD_IDENT_VAR_SUB_SIN       \
  (VLAD_IDENT_MASK_ENT | VLAD_IDENT_MASK_VAR | VLAD_IDENT_MASK_ENT_SUB | VLAD_IDENT_MASK_ENT_SIN)
#define VLAD_IDENT_VAR_ACC_SIN       \
  (VLAD_IDENT_MASK_ENT | VLAD_IDENT_MASK_VAR | VLAD_IDENT_MASK_ENT_ACC | VLAD_IDENT_MASK_ENT_SIN)
#define VLAD_IDENT_VAR_OBJ_SIN       \
  (VLAD_IDENT_MASK_ENT | VLAD_IDENT_MASK_VAR | VLAD_IDENT_MASK_ENT_OBJ | VLAD_IDENT_MASK_ENT_SIN)

#define VLAD_IDENT_VAR_SUB_GRP       \
  (VLAD_IDENT_MASK_ENT | VLAD_IDENT_MASK_VAR | VLAD_IDENT_MASK_ENT_SUB | VLAD_IDENT_MASK_ENT_GRP)
#define VLAD_IDENT_VAR_ACC_GRP       \
  (VLAD_IDENT_MASK_ENT | VLAD_IDENT_MASK_VAR | VLAD_IDENT_MASK_ENT_ACC | VLAD_IDENT_MASK_ENT_GRP)
#define VLAD_IDENT_VAR_OBJ_GRP       \
  (VLAD_IDENT_MASK_ENT | VLAD_IDENT_MASK_VAR | VLAD_IDENT_MASK_ENT_OBJ | VLAD_IDENT_MASK_ENT_GRP)

#define VLAD_IDENT_VAR_INT           \
  (VLAD_IDENT_MASK_INT | VLAD_IDENT_MASK_VAR)

/* additional macros */

#define VLAD_IDENT_TYPE(X)           \
  ((X) & VLAD_IDENT_MASK_TYPE)

#define VLAD_IDENT_TYPE_IS_SUB(X)    \
  (((X) & VLAD_IDENT_MASK_ENT) && ((X) & VLAD_IDENT_MASK_ENT_SUB))

#define VLAD_IDENT_TYPE_IS_ACC(X)    \
  (((X) & VLAD_IDENT_MASK_ENT) && ((X) & VLAD_IDENT_MASK_ENT_ACC))

#define VLAD_IDENT_TYPE_IS_OBJ(X)    \
  (((X) & VLAD_IDENT_MASK_ENT) && ((X) & VLAD_IDENT_MASK_ENT_OBJ))

#define VLAD_IDENT_TYPE_IS_SIN(X)    \
  (((X) & VLAD_IDENT_MASK_ENT) && ((X) & VLAD_IDENT_MASK_ENT_SIN))

#define VLAD_IDENT_TYPE_IS_GRP(X)    \
  (((X) & VLAD_IDENT_MASK_ENT) && ((X) & VLAD_IDENT_MASK_ENT_GRP))

#define VLAD_IDENT_TYPE_IS_ENT(X)    \
  ((X) & VLAD_IDENT_MASK_ENT)

#define VLAD_IDENT_TYPE_IS_INT(X)    \
  ((X) & VLAD_IDENT_MASK_INT)

#define VLAD_IDENT_TYPE_IS_VAR(X)    \
  ((X) & VLAD_IDENT_MASK_VAR)

/* a helper class containing static functions to validate identifiers */

class vlad_identifier : virtual public vlad_mem
{
  public :

    vlad_identifier();
    ~vlad_identifier();

    /* returns VLAD_OK if the given identifier is a valid non-variable */
    static int validate_nvar_ident(const char *a_ident);

    /* returns VLAD_OK if the given identifier is a valid variable */
    static int validate_var_ident(const char *a_ident);

    /* returns VLAD_OK if the given type is a valid entity type */
    static int validate_ent_type(unsigned char a_type);

    /* returns VLAD_OK if the given type is a valid interval type */
    static int validate_int_type(unsigned char a_type);

    /* returns VLAD_OK if the given type is a valid variable type */
    static int validate_var_type(unsigned char a_type);

    /* returns the type of the given variable */
    static unsigned char get_var_type(const char *a_var);

    /* returns VLAD_OK if the entity type is compatible with the var */
    static int check_compat(const char *a_var, unsigned char a_type);
} ;

#endif
