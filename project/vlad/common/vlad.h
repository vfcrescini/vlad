/*
 * vlad.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_VLAD_H
#define __VLAD_VLAD_H

/* query result codes */
#define VLAD_RESULT_TRUE     0
#define VLAD_RESULT_FALSE    1
#define VLAD_RESULT_UNKNOWN  2

/* modes */
#define VLAD_MODE_GENERATE   0
#define VLAD_MODE_EVALUATE   1

/* string constants */
#define VLAD_STR_TRUE        "TRUE"
#define VLAD_STR_FALSE       "FALSE"
#define VLAD_STR_UNKNOWN     "?"
#define VLAD_STR_NOT         "NOT"
#define VLAD_STR_AND         "AND"
#define VLAD_STR_ARROW       "<-"
#define VLAD_STR_HOLDS       "holds"
#define VLAD_STR_MEMBER      "member"
#define VLAD_STR_SUBSET      "subset"

/* string lengths */
#define VLAD_MAXLEN_STR      5120
#define VLAD_MAXLEN_NUM      32
#define VLAD_MAXLEN_IDENT    128

/* error codes */

/* success */
#define VLAD_OK              0
/* general failure error code */
#define VLAD_FAILURE        -1
/* unexpected null pointer */
#define VLAD_NULLPTR        -2
/* call to malloc failed */
#define VLAD_MALLOCFAILED   -3
/* index given is out of bounds */
#define VLAD_OUTOFBOUNDS    -4
/* find functions: search failed */
#define VLAD_NOTFOUND       -5
/* add functions: element already exists in an all-unique list */
#define VLAD_DUPLICATE      -6
/* add functions: negation of an atom is already implied */
#define VLAD_NEGIMPLIED     -7
/* there is something wrong with the input parameters */
#define VLAD_INVALIDINPUT   -8
/* method failed because the object was not initialised */
#define VLAD_UNINITIALISED  -9
/* no model is generated: possibly because of a contradiction */
#define VLAD_NOMODEL        -10
/* cannot open file */
#define VLAD_OPENFAILED     -11

/* malloc/new macros */
#define VLAD_ADT_MALLOC(X,Y)  ((X *) malloc(sizeof(X) * Y))
#define VLAD_STRING_MALLOC(X) ((char *) malloc(sizeof(char) * (strlen(X) + 1)))
#define VLAD_NEW(X)           (new(std::nothrow) X)

/* int/bool to string macros */
#define VLAD_BOOL_STRING(X)   ((X) ? VLAD_STR_TRUE : VLAD_STR_FALSE)
#define VLAD_RESULT_STRING(X) (((X) == VLAD_RESULT_TRUE) ? VLAD_STR_TRUE : (X) == VLAD_RESULT_FALSE ? VLAD_STR_FALSE : VLAD_STR_UNKNOWN)

#endif
