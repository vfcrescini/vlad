/*
 * expression.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_EXPRESSION_H
#define __EPI_EXPRESSION_H

typedef enum
{
  epi_true = 0,
  epi_false = 1
} truth_type;

/* first 3 bits indicate the type */
#define EPI_ATOM_CONST       1
#define EPI_ATOM_HOLDS       2
#define EPI_ATOM_MEMB        3
#define EPI_ATOM_SUBST       4

/* some convenience macros */
#define EPI_ATOM_IS_CONST(X) (((X).type & 7) == EPI_ATOM_CONST)
#define EPI_ATOM_IS_HOLDS(X) (((X).type & 7) == EPI_ATOM_HOLDS)
#define EPI_ATOM_IS_MEMB(X)  (((X).type & 7) == EPI_ATOM_MEMB)
#define EPI_ATOM_IS_SUBST(X) (((X).type & 7) == EPI_ATOM_SUBST)
#define EPI_ATOM_NEGATE(X)   ((X).truth = ((X).truth == epi_true) ? epi_false : epi_true)

#endif
