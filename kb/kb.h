/*
 * kb.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_KB_H
#define __VLAD_KB_H

#include <cstdio>

#include <atom.h>
#include <expression.h>
#include <stringlist.h>
#include <numberlist.h>
#include <sequence.h>
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
    /* generate a human-readable general logic program and dump output to f */
    int generate_nlp(expression *e, sequence *s, FILE *f);
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
    /* make sure transref is valid */
    int verify_transref(char *n, stringlist *il);
    /* functions for encoding atoms */
    int encode_holds(const char *s, const char *a, const char *o, unsigned int *n);
    int encode_member(const char *e, const char *g, unsigned int *n);
    int encode_subset(const char *g1, const char *g2, unsigned int *n);
    /* functions for decoding atoms */
    int decode_holds(char **s, char **a, char **o, unsigned int n);
    int decode_member(char **e, char **g, unsigned int n);
    int decode_subset(char **g1, char **g2, unsigned int n);
    /* gives an atom id based on the identifiers already given */
    int encode_atom(atom *a, unsigned int s, unsigned int *n);
    /* returns the atom details given the id */
    int decode_atom(atom **a, unsigned int *s, unsigned int n);
    /* gives a list of encoded atom id's that represent inheritance rules */
    int generate_inheritance(unsigned int state_tot,
                             numberlist **l1,
                             numberlist **l2,
                             numberlist **l3);
    /* gives a list of encoded atom id's that represent transitivity rules */
    int generate_transitivity(unsigned int state_tot,
                              numberlist **l1,
                              numberlist **l2,
                              numberlist **l3);
    /* gives a list of encoded atom id's that represent complementary rules */
    int generate_complementary(unsigned int state_tot,
                               numberlist **l1,
                               numberlist **l2);
    /* gives a list of encoded atom id's that represent intertial rules */
    int generate_inertial(unsigned int state_tot,
                          numberlist **l1,
                          numberlist **l2,
                          numberlist **l3);

    /* gives a list of encoded atom id's that represent initial state atoms */
    int generate_initialstate(numberlist **l1);

    /* gives a list of encoded atom id's that represent constraint rules */
    int generate_constraints(unsigned int state_tot,
                             numberlistlist **l1,
                             numberlistlist **l2,
                             numberlistlist **l3);
    /* gives a list of encoded atom id's that represent transformation rules */
    int generate_transformation(sequence *seq,
                                numberlistlist **l1,
                                numberlistlist **l2);
} ;
#endif
