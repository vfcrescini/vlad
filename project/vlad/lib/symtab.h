/*
 * symtab.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_SYMTAB_H
#define __VLAD_SYMTAB_H

#include <vlad/stringlist.h>

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
    /* get an array of identifiers that matches the given type */
    int get(unsigned char t, char ***a, unsigned int *s);
    /* return the number of identifiers that are of type t */
    unsigned int length(unsigned char t);
    /* return true if symbol is in the table */
    int find(const char *s);
    /* give the type of the given identifier */
    int type(const char *s, unsigned char *t);
  private :
    stringlist *sub_list;
    stringlist *acc_list;
    stringlist *obj_list;
    stringlist *sub_grp_list;
    stringlist *acc_grp_list;
    stringlist *obj_grp_list;
    bool initialised;
} ;

#endif
