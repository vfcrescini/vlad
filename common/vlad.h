/*
 * vlad.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_VLAD_H
#define __VLAD_VLAD_H

/* version */

#define VLAD_VERSION         "0.0.3"

/* query result codes */

#define VLAD_RESULT_TRUE     0
#define VLAD_RESULT_FALSE    1
#define VLAD_RESULT_UNKNOWN  2

/* boolean codes */

#define VLAD_TRUE            0
#define VLAD_FALSE           1

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

/* convenience macro(s) */
#define VLAD_ADT_MALLOC(X)    ((X *) malloc(sizeof(X)))
#define VLAD_STRING_MALLOC(X) ((char *) malloc(sizeof(char) * (strlen(X) + 1)))

#endif
