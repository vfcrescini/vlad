/*
 * symtab.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_SYMTAB_H
#define __VLAD_SYMTAB_H

#include "identifier.h"

class symtab : public list
{
  public :
    symtab();
    ~symtab();
    /* add symbol in symbol table */
    int add(const char *n, ident_type t, bool g);
    /* delete the symbol n from the table */
    int del(const char *n);
    /* get the identifier object associated with the given name */
    int get(const char *n, identifier **i);
    /* return true if symbol is in the table */
    int find(const char *n);
} ;

#endif
