/*
 * transref.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_TRANSREF_H
#define __EPI_TRANSREF_H

#include <stringlist.h>
#include <identlist.h>

typedef struct 
{
  char *name;
  identlist_type identlist;
} transref_type;

/* creates a newly-allocated transref; copies and allocates name but only the
 * reference for varlist, precond and postcond */
int transref_create(transref_type **transref, 
                    char *name, 
                    identlist_type identlist);

/* same as above, but does not allocate new mem for the transref_type struct */
int transref_compose(transref_type *transref, 
                     char *name, 
                     identlist_type identlist);

/* creates a copy of transref1 */
int transref_copy(transref_type **transref2, transref_type transref1);

/* frees this transref and all its members */
int transref_destroy(transref_type *transref);

#endif
