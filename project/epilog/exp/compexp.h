/*
 * compexp.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_COMPEXP_H
#define __EPI_COMPEXP_H

#include "expression.h"
#include "varlist.h"

/* a structure to hold non-ground expressions. holds a ground-expression
 * plus a list of variables */

typedef struct
{
  expression_type exp;
  varlist_type varlist;
} compexp_type;

#endif
