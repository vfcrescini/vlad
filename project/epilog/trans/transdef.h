/*
 * transdef.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_TRANSDEF_H
#define __EPI_TRANSDEF_H

#include <stringlist.h>
#include <expression.h>

typedef struct 
{
  char *name;
  stringlist_type varlist;
  expression_type precond;
  expression_type postcond;
} transdef_type;

/* creates a newly-allocated transdef; copies and allocates name but only the
 * reference for varlist, precond and postcond */
int transdef_create(transdef_type **transdef, 
                    char *name, 
                    stringlist_type varlist,
                    expression_type precond, 
                    expression_type postcond);

/* same as above, but does not allocate new mem for the transdef_type struct */
int transdef_compose(transdef_type *transdef, 
                     char *name, 
                     stringlist_type varlist,
                     expression_type precond, 
                     expression_type postcond);

/* creates a copy of transdef1 */
int transdef_copy(transdef_type **transdef2, transdef_type transdef1);

/* frees this transdef and the name */
int transdef_destroy(transdef_type *transdef);

/* frees this transdef and all its members */
int transdef_destroy_all(transdef_type *transdef);

#endif
