/*
 * transtab.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_TRANSTAB_H
#define __EPI_TRANSTAB_H

#include <simplelist.h>
#include <gnd_exp.h>
#include "transref.h"
#include "transdef.h"

/* initialise list */
int transtab_init(void);

/* gives the length of the list */
unsigned int transtab_length(void);

/* add transformation into table */
int transtab_add(transdef_type trans);

/* gives a reference to a transdef_type structure */
int transtab_get(char *name, transdef_type **trans);

/* returns 0 if the transformation name is in the list */
int transtab_find(char *name);

/* gives the resulting expression after the transformation is performed */
int transtab_transform(gnd_exp_type kb, transref_type tr, gnd_exp_type *ans);

/* empty the list */
void transtab_purge(void);

#endif
