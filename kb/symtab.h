/*
 * symtab.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_SYMTAB_H
#define __VLAD_SYMTAB_H

#include <identifier.h>

class identlist : public list
{
  public :
    identlist(const char *n, unsigned char t);
    ~identlist();
    /* add an identifier in the list */
    int add(const char *n);
    /* get the identifier object associated with the given name */
    int get(const char *n, identifier **i);
    /* get the ith identifier of the list */
    int get(unsigned int i, identifier **id);
    /* return true if symbol is in the list */
    int find(const char *n);
  private :
    unsigned char type;
} ;

/* 
 * wrapper class to make it appear as if the identifiers are one list.
 * having identifiers on a separate list will ease search times.
 */

class symtab
{
  public :
    symtab();
    ~symtab();
    int init();
    /* add symbol in symbol table */
    int add(const char *n, unsigned char t);
    /* get the identifier object associated with the given name */
    int get(const char *n, identifier **i);
    /* get the ith identifier of type t */
    int get(unsigned int i, unsigned char t, identifier **id);
    /* return the number of identifiers that are of type t */
    int length(unsigned char t);
    /* return true if symbol is in the table */
    int find(const char *n);
  private :
    identlist *sub_list;
    identlist *acc_list;
    identlist *obj_list;
    identlist *sub_grp_list;
    identlist *acc_grp_list;
    identlist *obj_grp_list;
    bool initialised;
} ;

#endif
