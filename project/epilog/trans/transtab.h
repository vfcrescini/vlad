/*
 * transtab.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_TRANSTAB_H
#define __EPI_TRANSTAB_H

#include <simplelist.h>
#include "transdef.h"

/* initialise list */
int transtab_init(void);

/* add transformation into table */
int transtab_add(transdef_type trans);

/* gives the length of the list */
int transtab_length(unsigned int *len);

/* gives a reference to a transdef_type structure */
int transtab_get(char *name, transdef_type **trans);

/* returns 0 if the transformation name is in the list */
int transtab_find(char *name);

/* empty the list */
int transtab_purge(void);

#endif
