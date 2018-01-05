/*
 * symtab.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_SYMTAB_H
#define __VLAD_SYMTAB_H

#include <identifier.h>

class symtab : public list
{
  public :
    symtab();
    ~symtab();
    /* add symbol in symbol table */
    int add(const char *n, unsigned char t);
    /* delete the symbol n from the table */
    int del(const char *n);
    /* get the identifier object associated with the given name */
    int get(const char *n, identifier **i);
    /* get an array of identifiers of a particular type */
    int get(unsigned char t, identifier ***a, unsigned int *s);
    /* return true if symbol is in the table */
    int find(const char *n);
} ;

#endif
