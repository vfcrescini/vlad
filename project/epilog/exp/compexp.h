/*
 * compexp.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_COMPEXP_H
#define __EPI_COMPEXP_H

#include "expression.h"
#include "varlist.h"

typedef struct
{
  expression_type exp;
  varlist_type varlist;
} compexp_type;

#endif
