/*
 * compatom.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_COMPATOM_H
#define __EPI_COMPATOM_H

#include "atom.h"
#include "varlist.h"

/* a structure for a non-ground atom. holds a ground-atom plus
 * a list of variables (if any) */

typedef struct
{
  atom_type atom;
  varlist_type varlist;
} compatom_type;

#endif
