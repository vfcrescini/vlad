/*
 * identifier.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_IDENTIFIER_H
#define __VLAD_IDENTIFIER_H

#include <list.h>

/* identifier type values indicated by the first 2 bits */
#define VLAD_IDENT_SUBJECT            1
#define VLAD_IDENT_ACCESS             2 
#define VLAD_IDENT_OBJECT             3

/* identifier group bit indicated by the 3th bit */
#define VLAD_IDENT_GROUP              4

/* so the type cannot be more than this value */
#define VLAD_IDENT_TYPEMAX            7

/* some convenience macros */
#define VLAD_IDENT_IS_SUBJECT(X)      (((X).get_type() & 3) == VLAD_IDENT_SUBJECT)
#define VLAD_IDENT_IS_ACCESS(X)       (((X).get_type() & 3) == VLAD_IDENT_ACCESS)
#define VLAD_IDENT_IS_OBJECT(X)       (((X).get_type() & 3) == VLAD_IDENT_OBJECT)
#define VLAD_IDENT_IS_GROUP(X)        ((X).get_type() & VLAD_IDENT_GROUP)
#define VLAD_IDENT_BASETYPE(X)        ((X).get_type() & 3)
#define VLAD_IDENT_TYPE_IS_SUBJECT(X) (((X) & 3) == VLAD_IDENT_SUBJECT)
#define VLAD_IDENT_TYPE_IS_ACCESS(X)  (((X) & 3) == VLAD_IDENT_ACCESS)
#define VLAD_IDENT_TYPE_IS_OBJECT(X)  (((X) & 3) == VLAD_IDENT_OBJECT)
#define VLAD_IDENT_TYPE_IS_GROUP(X)   ((X) & VLAD_IDENT_GROUP)
#define VLAD_IDENT_STRING(X)          ((X).get_name())
#define VLAD_IDENT_EQ(X,Y)            ((X) == NULL && (Y) == NULL) ? true : (((X) != NULL && (Y) != NULL) ? ((X)->get_type() == (Y)->get_type() ? (!strcmp((X)->get_name(), (Y)->get_name()) ? true : false) : false): false)

class identifier : public list_item
{
  public :
    identifier();
    ~identifier();
    /* compare item with this identifier */
    bool cmp(list_item *item);
    int init(const char *n, unsigned char t);
    const char *get_name();
    unsigned char get_type();
  private :
    char *name;
    unsigned char type;
} ;

#endif
