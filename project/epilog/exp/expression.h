/*
 * expression.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_EXPRESSION_H
#define __EPI_EXPRESSION_H

#include <epilog.h>

/* first 3 bits indicate the type */
#define EPI_ATOM_CONST       1
#define EPI_ATOM_HOLDS       2
#define EPI_ATOM_MEMB        3
#define EPI_ATOM_SUBST       4

/* some convenience macros */
#define EPI_ATOM_IS_CONST(X)      (((X).type & 7) == EPI_ATOM_CONST)
#define EPI_ATOM_IS_HOLDS(X)      (((X).type & 7) == EPI_ATOM_HOLDS)
#define EPI_ATOM_IS_MEMB(X)       (((X).type & 7) == EPI_ATOM_MEMB)
#define EPI_ATOM_IS_SUBST(X)      (((X).type & 7) == EPI_ATOM_SUBST)
#define EPI_ATOM_NEGATE(X)        ((X).truth = ((X).truth == EPI_TRUE) ? EPI_FALSE : EPI_TRUE)
#define EPI_ATOM_HOLDS_SUBJECT(X) ((X).atom.holds.subject)
#define EPI_ATOM_HOLDS_ACCESS(X)  ((X).atom.holds.access)
#define EPI_ATOM_HOLDS_OBJECT(X)  ((X).atom.holds.object)
#define EPI_ATOM_MEMB_ELEMENT(X)  ((X).atom.memb.element)
#define EPI_ATOM_MEMB_GROUP(X)    ((X).atom.memb.group)
#define EPI_ATOM_SUBST_GROUP1(X)  ((X).atom.subst.group1)
#define EPI_ATOM_SUBST_GROUP2(X)  ((X).atom.subst.group2)

#endif
