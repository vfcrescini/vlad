/*
 * gnd_atomlist.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_GND_ATOMLIST_H
#define __EPI_GND_ATOMLIST_H

#include <indexedlist.h>
#include "gnd_atom.h"

#define gnd_atomlist_type indexedlist_type

int gnd_atomlist_init(gnd_atomlist_type *list);
int gnd_atomlist_add(gnd_atomlist_type *list, gnd_atom_type atom);
int gnd_atomlist_remove(gnd_atomlist_type *list, gnd_atom_type atom);
int gnd_atomlist_isin(gnd_atomlist_type list, gnd_atom_type atom);

#endif
