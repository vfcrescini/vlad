/*
 * epilog.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_EPILOG_H
#define __EPI_EPILOG_H

/* version */

#define EPI_VERSION "0.9.5"

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
/* unexpected null pointer */
#define EPI_NULLPTR        -1
/* call to malloc failed */
#define EPI_MALLOCFAILED   -2
/* index given is out of bounds */
#define EPI_OUTOFBOUNDS    -3
/* find functions: search failed */
#define EPI_NOTFOUND       -4
/* add functions: element already exists in an all-unique list */
#define EPI_DUPLICATE      -5
/* add functions: negation of an atom is already implied */
#define EPI_NEGIMPLIED     -6
/* general failure error code */
#define EPI_FAILURE       -10

#endif
