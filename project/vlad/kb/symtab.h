/*
 * symtab.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_SYMTAB_H
#define __VLAD_SYMTAB_H

#include <stringlist.h>

/* identifier type values indicated by the first 2 bits */
#define VLAD_IDENT_CONST         0
#define VLAD_IDENT_SUBJECT       1
#define VLAD_IDENT_ACCESS        2
#define VLAD_IDENT_OBJECT        3

/* identifier group bit indicated by the 3th bit */
#define VLAD_IDENT_GROUP         4

/* some convenience macros */
#define VLAD_IDENT_BASETYPE(X)   ((X) & 3)
#define VLAD_IDENT_IS_CONST(X)   ((X) == 0)
#define VLAD_IDENT_IS_SUBJECT(X) (((X) & 3) == VLAD_IDENT_SUBJECT)
#define VLAD_IDENT_IS_ACCESS(X)  (((X) & 3) == VLAD_IDENT_ACCESS)
#define VLAD_IDENT_IS_OBJECT(X)  (((X) & 3) == VLAD_IDENT_OBJECT)
#define VLAD_IDENT_IS_GROUP(X)   ((X) & VLAD_IDENT_GROUP)
#define VLAD_IDENT_IS_VALID(X)   ((X) >= 0 && (X) <= 7)

/* the constants true and false are automatically added to the table */
#define VLAD_CONST_TRUE          "true"
#define VLAD_CONST_FALSE         "false"

class symtab
{
  public :
    symtab();
    ~symtab();
    int init();
    /* add symbol in symbol table */
    int add(const char *s, unsigned char t);
    /* get the index and type of the identifier based on the name */
    int get(const char *s, unsigned int *i, unsigned char *t);
    /* get the ith identifier of type t */
    int get(unsigned int i, unsigned char t, char **s);
    /* return the number of identifiers that are of type t */
    unsigned int length(unsigned char t);
    /* return true if symbol is in the table */
    int find(const char *s);
  private :
    char *t_const;
    char *f_const;
    stringlist *sub_list;
    stringlist *acc_list;
    stringlist *obj_list;
    stringlist *sub_grp_list;
    stringlist *acc_grp_list;
    stringlist *obj_grp_list;
    bool initialised;
} ;

#endif
