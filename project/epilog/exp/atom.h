/*
 * atom.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_ATOM_H
#define __EPI_ATOM_H

#include <ident.h>

typedef enum
{
  epi_true = 0,
  epi_false = 1
} truth_type;

typedef struct 
{
  ident_type *subject;
  ident_type *access;
  ident_type *object;
} holds_type;

typedef struct
{
  ident_type *element;
  ident_type *group;
} memb_type;

typedef struct
{
  ident_type *group1;
  ident_type *group2;
} subst_type;

typedef struct
{
  unsigned short int type;
  truth_type truth;
  union {
    holds_type holds;
    memb_type memb; 
    subst_type subst;
  } atom;
} atom_type;

/* creates a pointer to an atom of type const */
int atom_create_const(atom_type **atom, truth_type truth);

/* creates a pointer to an atom of type holds */
int atom_create_holds(atom_type **atom, 
                      ident_type *sub, 
                      ident_type *acc,
                      ident_type *obj,
                      truth_type truth);

/* creates a pointer to an atom of type memb */
int atom_create_memb(atom_type **atom,
                     ident_type *element,
                     ident_type *group,
                     truth_type truth);

/* creates a pointer to an atom of type subst */
int atom_create_subst(atom_type **atom,
                      ident_type *group1,
                      ident_type *group2,
                      truth_type truth);

/* destroys atom structure */
int atom_destroy(atom_type *atom);

/* returns 0 if the contents of atom1 and atom2 are identical */
int atom_compare(atom_type atom1, atom_type atom2);

/* first 3 bits indicate the type */
#define EPI_ATOM_CONST       1
#define EPI_ATOM_HOLDS       2
#define EPI_ATOM_MEMB        3
#define EPI_ATOM_SUBST       4

/* some convenience macros */
#define EPI_ATOM_IS_CONST(X) (((X) & 7) == EPI_ATOM_CONST)
#define EPI_ATOM_IS_HOLDS(X) (((X) & 7) == EPI_ATOM_HOLDS)
#define EPI_ATOM_IS_MEMB(X)  (((X) & 7) == EPI_ATOM_MEMB)
#define EPI_ATOM_IS_SUBST(X) (((X) & 7) == EPI_ATOM_SUBST)

#endif
