/*
 * ident.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_IDENT_H
#define __EPI_IDENT_H

typedef struct {
  char *name;
  unsigned short type;
} ident_type;

/* create a new ident structure. allocate space for name */
int ident_create(ident_type **ident, char *name, unsigned short type);

/* free ident struct, including allocated space for name */
int ident_destroy(ident_type *ident);

/* return 0 if contents of ident1 and ident2 are identical */
int ident_compare(ident_type ident1, ident_type ident2);

/* identifier type values indicated by the first 2 bits */
#define EPI_IDENT_SUBJECT        1
#define EPI_IDENT_ACCESS         2 
#define EPI_IDENT_OBJECT         3

/* identifier group bit indicated by the 3th bit */
#define EPI_IDENT_GROUP          4

/* some convenience macros */
#define EPI_IDENT_IS_SUBJECT(X)      (((X).type & 3) == EPI_IDENT_SUBJECT)
#define EPI_IDENT_IS_ACCESS(X)       (((X).type & 3) == EPI_IDENT_ACCESS)
#define EPI_IDENT_IS_OBJECT(X)       (((X).type & 3) == EPI_IDENT_OBJECT)
#define EPI_IDENT_IS_GROUP(X)        ((X).type & EPI_IDENT_GROUP)
#define EPI_IDENT_BASETYPE(X)        ((X).type & 3)
#define EPI_IDENT_TYPE_IS_SUBJECT(X) (((X) & 3) == EPI_IDENT_SUBJECT)
#define EPI_IDENT_TYPE_IS_ACCESS(X)  (((X) & 3) == EPI_IDENT_ACCESS)
#define EPI_IDENT_TYPE_IS_OBJECT(X)  (((X) & 3) == EPI_IDENT_OBJECT)
#define EPI_IDENT_TYPE_IS_GROUP(X)   ((X) & EPI_IDENT_GROUP)
#define EPI_IDENT_STRING(X)          ((X)->name)

#endif
