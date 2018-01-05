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

#ifndef __VLAD_VLAD_H
#define __VLAD_VLAD_H

#include <vlad/config.h>

/* query result codes */
#define VLAD_RESULT_TRUE         0
#define VLAD_RESULT_FALSE        1
#define VLAD_RESULT_UNKNOWN      2

/* modes */
#define VLAD_MODE_GENERATE       0
#define VLAD_MODE_EVALUATE       1

/* string constants */
#define VLAD_STR_TRUE            "TRUE"
#define VLAD_STR_FALSE           "FALSE"
#define VLAD_STR_UNKNOWN         "?"
#define VLAD_STR_NOT             "NOT"
#define VLAD_STR_AND             "AND"
#define VLAD_STR_ARROW           "<-"
#define VLAD_STR_HOLDS           "holds"
#define VLAD_STR_MEMBER          "member"
#define VLAD_STR_SUBSET          "subset"
#define VLAD_STR_QUERY           "?"

/* string lengths */
#define VLAD_MAXLEN_STR          5120
#define VLAD_MAXLEN_NUM          32
#define VLAD_MAXLEN_IDENT        128

/* identifier type values indicated by the first 2 bits */
#define VLAD_IDENT_SUBJECT       1
#define VLAD_IDENT_ACCESS        2
#define VLAD_IDENT_OBJECT        3

/* identifier group bit indicated by the 3th bit */
#define VLAD_IDENT_GROUP         4

/* atom type */
#define VLAD_ATOM_HOLDS          1
#define VLAD_ATOM_MEMBER         2
#define VLAD_ATOM_SUBSET         3

/* error codes */

/* success */
#define VLAD_OK                  0
/* general failure error code */
#define VLAD_FAILURE             -1
/* unexpected null pointer */
#define VLAD_NULLPTR             -2
/* call to malloc failed */
#define VLAD_MALLOCFAILED        -3
/* index given is out of bounds */
#define VLAD_OUTOFBOUNDS         -4
/* find functions: search failed */
#define VLAD_NOTFOUND            -5
/* add functions: element already exists in an all-unique list */
#define VLAD_DUPLICATE           -6
/* add functions: negation of an atom is already implied */
#define VLAD_NEGIMPLIED          -7
/* there is something wrong with the input parameters */
#define VLAD_INVALIDINPUT        -8
/* method failed because the object was not initialised */
#define VLAD_UNINITIALISED       -9
/* no model is generated: possibly because of a contradiction */
#define VLAD_NOMODEL             -10
/* cannot open file */
#define VLAD_OPENFAILED          -11
/* operation not permitted at this time */
#define VLAD_INVALIDOP           -12

/* convenience macros */
#define VLAD_LIST_LENGTH(X)      (((X) == NULL) ? 0 : (X)->length())
#define VLAD_LIST_ITEMCMP(X,Y)   (((X) == NULL) ? ((Y) == NULL) : (((Y) == NULL) ? false : ((X)->cmp(Y))))
#define VLAD_IDENT_BASETYPE(X)   ((X) & 3)
#define VLAD_IDENT_IS_SUBJECT(X) (((X) & 3) == VLAD_IDENT_SUBJECT)
#define VLAD_IDENT_IS_ACCESS(X)  (((X) & 3) == VLAD_IDENT_ACCESS)
#define VLAD_IDENT_IS_OBJECT(X)  (((X) & 3) == VLAD_IDENT_OBJECT)
#define VLAD_IDENT_IS_GROUP(X)   ((X) & VLAD_IDENT_GROUP)
#define VLAD_IDENT_IS_VALID(X)   ((X) > 0 && (X) <= 7)
#define VLAD_ATOM_TYPE_VALID(X)  (((X) >= 0) && ((X) < 4))
#define VLAD_ATOM_IS_HOLDS(X)    ((X) == VLAD_ATOM_HOLDS)
#define VLAD_ATOM_IS_MEMBER(X)   ((X) == VLAD_ATOM_MEMBER)
#define VLAD_ATOM_IS_SUBSET(X)   ((X) == VLAD_ATOM_SUBSET)

/* malloc/new macros */
#define VLAD_ADT_MALLOC(X,Y)     ((X *) malloc(sizeof(X) * Y))
#define VLAD_STRING_MALLOC(X)    ((char *) malloc(sizeof(char) * (strlen(X) + 1)))
#ifdef __cplusplus
#define VLAD_NEW(X)              (new(std::nothrow) X)
#endif

/* int/bool to string macros */
#define VLAD_BOOL_STRING(X)      ((X) ? VLAD_STR_TRUE : VLAD_STR_FALSE)
#define VLAD_RESULT_STRING(X)    (((X) == VLAD_RESULT_TRUE) ? VLAD_STR_TRUE : (X) == VLAD_RESULT_FALSE ? VLAD_STR_FALSE : VLAD_STR_UNKNOWN)

#endif
