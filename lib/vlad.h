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
#define VLAD_STR_IDENTITY        "Identity"
#define VLAD_STR_INHERITANCE     "Inheritance"
#define VLAD_STR_TRANSITIVITY    "Transitivity"
#define VLAD_STR_NEGATION        "Negation"
#define VLAD_STR_INERTIAL        "Inertial"
#define VLAD_STR_INITSTATE       "Initial State"
#define VLAD_STR_CONSTRAINT      "Constraint"
#define VLAD_STR_UPDATE          "Update"
#define VLAD_STR_RULES           "Rules"
#define VLAD_STR_QUERY           "?"
#define VLAD_STR_TERMINATOR      "."
#define VLAD_STR_MEMPREFIX       "VLAD"

/* string lengths */
#define VLAD_MAXLEN_STR          5120
#define VLAD_MAXLEN_NUM          32

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

/* int/bool to string macros */
#define VLAD_BOOL_STRING(X)      ((X) ? VLAD_STR_TRUE : VLAD_STR_FALSE)
#define VLAD_RESULT_STRING(X)    (((X) == VLAD_RESULT_TRUE) ? VLAD_STR_TRUE : (X) == VLAD_RESULT_FALSE ? VLAD_STR_FALSE : VLAD_STR_UNKNOWN)

#endif
