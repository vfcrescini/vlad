/*
 * epilog.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_EPILOG_H
#define __EPI_EPILOG_H

/* version */

#define EPI_VERSION         "0.9.7"

/* query result codes */

#define EPI_RESULT_TRUE     0
#define EPI_RESULT_FALSE    1
#define EPI_RESULT_UNKNOWN  2

/* boolean codes */

#define EPI_TRUE            0
#define EPI_FALSE           1

/* error codes */

/* success */
#define EPI_OK              0
/* general failure error code */
#define EPI_FAILURE        -1
/* unexpected null pointer */
#define EPI_NULLPTR        -2
/* call to malloc failed */
#define EPI_MALLOCFAILED   -3
/* index given is out of bounds */
#define EPI_OUTOFBOUNDS    -4
/* find functions: search failed */
#define EPI_NOTFOUND       -5
/* add functions: element already exists in an all-unique list */
#define EPI_DUPLICATE      -6
/* add functions: negation of an atom is already implied */
#define EPI_NEGIMPLIED     -7
/* there is something wrong with the input parameters */
#define EPI_INVALIDINPUT   -8

/* convenience macro(s) */
#define EPI_ADT_MALLOC(X)    ((X *) malloc(sizeof(X)))
#define EPI_STRING_MALLOC(X) ((char *) malloc(sizeof(char) * (strlen(X) + 1)))

#endif
