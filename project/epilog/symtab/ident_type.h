/*
 * ident_type.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_IDENT_TYPE_H
#define __EPI_IDENT_TYPE_H

/* identifier type values indicated by the first 3 bits */
#define EPI_IDENT_SUBJECT     1
#define EPI_IDENT_ACCESS      2 
#define EPI_IDENT_OBJECT      3
#define EPI_IDENT_VAR         4
#define EPI_IDENT_TRANS       5

/* identifier group bit indicated by the 4th bit */
#define EPI_IDENT_GROUP       8

/* some convenience macros */
#define EPI_IDENT_IS_SUBJECT(X) (((X) & 7) == EPI_IDENT_SUBJECT)
#define EPI_IDENT_IS_ACCESS(X)  (((X) & 7) == EPI_IDENT_ACCESS)
#define EPI_IDENT_IS_OBJECT(X)  (((X) & 7) == EPI_IDENT_OBJECT)
#define EPI_IDENT_IS_VAR(X)     (((X) & 7) == EPI_IDENT_VAR)
#define EPI_IDENT_IS_TRANS(X)   (((X) & 7) == EPI_IDENT_TRANS)
#define EPI_IDENT_IS_GROUP(X)   ((X) & EPI_IDENT_GROUP)

#endif
