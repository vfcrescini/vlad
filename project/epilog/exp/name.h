/*
 * name.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_NAME_H
#define __EPI_NAME_H

#include <ident.h>

typedef struct {
  unsigned short int type;
  union {
    ident_type *ident;
    char *var;
  } name;
} name_type;

/* composes a variable name_type, allocates space for the string */
int name_create_var(char *string, name_type *name);

/* composes an ident name_type */
int name_create_ident(ident_type *ident, name_type *name);

/* copies a name_type */
int name_copy(name_type source, name_type *dest);

/* returns 0 if the two names are equivalent */
int name_compare(name_type one, name_type two);

/* destroy name. only var name strings are freed */
int name_destroy(name_type *name);

/* types for the type field of name_type */
#define EPI_NAME_VAR           1
#define EPI_NAME_IDENT         2

/* some convenience macros */
#define EPI_NAME_IS_VAR(X)     ((X).type == EPI_NAME_VAR)
#define EPI_NAME_IS_IDENT(X)   ((X).type == EPI_NAME_IDENT)
#define EPI_NAME_IS_SUBJECT(X) (EPI_NAME_IS_IDENT(X) && EPI_IDENT_IS_SUBJECT(*((X).name.ident)))
#define EPI_NAME_IS_ACCESS(X)  (EPI_NAME_IS_IDENT(X) && EPI_IDENT_IS_ACCESS(*((X).name.ident)))
#define EPI_NAME_IS_OBJECT(X)  (EPI_NAME_IS_IDENT(X) && EPI_IDENT_IS_OBJECT(*((X).name.ident)))
#define EPI_NAME_IS_GROUP(X)   (EPI_NAME_IS_IDENT(X) && EPI_IDENT_IS_GROUP(*((X).name.ident)))
#define EPI_NAME_BASETYPE(X)   (EPI_IDENT_BASETYPE(*((X).name.ident)))
#define EPI_NAME_STRING(X)     (EPI_NAME_IS_VAR(X) ? (X).name.var : (X).name.ident->name)

#endif
