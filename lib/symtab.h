/*
 * symtab.h
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_SYMTAB_H
#define __VLAD_SYMTAB_H

#include <vlad/stringlist.h>

class symtab
{
  public :
    symtab();
    ~symtab();
    /* initialise */
    int init();
    /* add symbol in symbol table */
    int add(const char *a_s, unsigned char a_t);
    /* get the index and type of the identifier based on the name */
    int get(const char *a_s, unsigned int *a_i, unsigned char *a_t);
    /* get the ith identifier of type t */
    int get(unsigned int a_i, unsigned char a_t, char **a_s);
    /* get an array of identifiers that matches the given type */
    int get(unsigned char a_t, char ***a_a, unsigned int *a_s);
    /* return the number of identifiers that are of type t */
    unsigned int length(unsigned char a_t);
    /* return 0 if symbol is in the table */
    int find(const char *a_s);
    /* return 0 if symbol of type t is in the table */
    int find(const char *a_s, unsigned char a_t);
    /* give the type of the given identifier */
    int type(const char *a_s, unsigned char *a_t);
  private :
    /* m_list: 0 = ss, 1 = as, 2 = os, 3 = sg, 4 = ag, 5 = og */
    stringlist *m_lists[VLAD_IDENT_LAST + 1];
    bool m_init;
} ;

#endif
