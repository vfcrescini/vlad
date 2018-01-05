/*
 * kb.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_KB_H
#define __VLAD_KB_H

#include <cstdio>

#include <vlad/atom.h>
#include <vlad/expression.h>
#include <vlad/stringlist.h>
#include <vlad/sequence.h>
#include <vlad/symtab.h>
#include <vlad/consttab.h>
#include <vlad/transtab.h>

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
    /* generate a human-readable general logic program and dump output to f */
    int generate_nlp(expression *e, sequence *s, FILE *f);
#ifdef SMODELS
    /* use wrapper class to evaluate a query */
    int evaluate_query(expression *e, sequence *s, unsigned char *r);
#endif
  private :
    symtab *stable;
    expression *itable;
    consttab *ctable;
    transtab *ttable;
    unsigned int s_len;
    unsigned int a_len;
    unsigned int o_len;
    unsigned int sg_len;
    unsigned int ag_len;
    unsigned int og_len;
    unsigned int h_tot;
    unsigned int m_tot;
    unsigned int s_tot;
    unsigned int pos_tot;
    unsigned char stage;
    /* make sure atom a is valid */
    int verify_atom(atom *a, stringlist *v);
    /* make sure expression e is valid */
    int verify_expression(expression *e);
    /* make sure transref is valid */
    int verify_transref(char *n, stringlist *il);
    /* make sure sequence s is valid */
    int verify_sequence(sequence *s);
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
    /* functions for decoding atoms */
    int decode_holds(char **s, char **a, char **o, unsigned int n);
    int decode_member(char **e, char **g, unsigned int n);
    int decode_subset(char **g1, char **g2, unsigned int n);
    /* gives an atom id based on the identifiers already given */
    int encode_atom(atom *a, unsigned int s, unsigned int *n);
    /* returns the atom details given the id */
    int decode_atom(atom **a, unsigned int *s, unsigned int n);
    /* returns an atom id based on the info given */
    unsigned int compute_atom(unsigned int a_st, bool a_tr, unsigned int a_atm);
    /* returns a holds atom id based on the info given */
    unsigned int compute_holds(unsigned int a_st, bool a_tr, unsigned int a_sub, unsigned int a_acc, unsigned int a_obj);
    /* returns a member atom id based on the info given */
    unsigned int compute_member(unsigned int a_st, bool a_tr, char a_ty, unsigned int a_elt, unsigned int a_grp);
    /* returns a subset atom id based on the info given */
    unsigned int compute_subset(unsigned int a_st, bool a_tr, char a_ty, unsigned int a_grp1, unsigned int a_grp2);
} ;
#endif
