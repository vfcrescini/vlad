/*
 * kb.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_KB_H
#define __VLAD_KB_H

#include <atom.h>
#include <expression.h>
#include <stringlist.h>
#include <symtab.h>
#include <consttab.h>
#include <transtab.h>

class kb {
  public :
    kb();
    ~kb();
    /* (re)init kb */
    int init();
    /* after this is called, no further calls to add_symtab() is allowed */
    int close_symtab();
    /* after this is called, no further calls to add_inittab() is allowed */
    int close_inittab();
    /* after this is called, no further calls to add_consttab() is allowed */
    int close_consttab();
    /* after this is called, no further calls to add_transtab() is allowed */
    int close_transtab();
    /* register an identifier in the kb */
    int add_symtab(const char *n, unsigned char t);
    /* add an atom into the initial state table */
    int add_inittab(atom *a);
    /* add a constrant into the constraints table */
    int add_consttab(expression *e, expression *c, expression *n);
    /* add a transformation declaration in the trans table */
    int add_transtab(const char *n,
                     stringlist *v,
                     expression *pr,
                     expression *po);
  private :
    symtab *stable;
    expression *itable;
    consttab *ctable;
    transtab *ttable;
    unsigned char stage;
    /* make sure atom a is valid */
    int kb::verify_atom(atom *a, stringlist *v);
    /* 
     * verifies that s, a and o are in the symtab and that
     * they are of the right  type, or listed in v if v is non-null 
     */
     int verify_atom_holds(const char *s,
                           const char *a,
                           const char *o,
                           stringlist *v);
    /* 
     * verifies that e and g are in the symtab and that they are of the right 
     * type, or listed in v if v is non-null
     */
    int verify_atom_member(const char *e, const char *g, stringlist *v);

   /* 
    * verifies that g1 and g2 are in the symtab and that they are of the right 
    * type, or listed in v if v is non-null
    */ 
    int verify_atom_subset(const char *g1, const char *g2, stringlist *v);

} ;

#endif