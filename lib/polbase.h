/*
 * polbase.h
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_POLBASE_H
#define __VLAD_POLBASE_H

#include <cstdio>

#include <vlad/vlad.h>
#include <vlad/symtab.h>
#include <vlad/expression.h>
#include <vlad/consttab.h>
#include <vlad/updatetab.h>
#include <vlad/seqtab.h>
#ifdef VLAD_SMODELS
  #include <vlad/smwrap.h>
#endif

class polbase {
  public :
    polbase();
    ~polbase();
    /* (re)init the policy base */
    int init();
    /* add an entity in the symbol table */
    int add_symtab(const char *a_name, unsigned char a_type);
    /* add a fact into the initial state table */
    int add_inittab(fact *a_fact);
    /* add a constrant into the constraints table */
    int add_consttab(expression *a_exp,
                     expression *a_cond,
                     expression *a_ncond);
    /* add an update declaration in the update table */
    int add_updatetab(const char *a_name,
                      stringlist *a_vlist,
                      expression *a_precond,
                      expression *a_postcond);
    /* add an update reference to the sequence table */
    int add_seqtab(updateref *a_uref);
    /* close symbol table */
    int close_symtab();
    /* after this is called, no further calls to add_inittab(), add_consttab()
     * or add_updatetab() is allowed */
    int close_polbase();
    /* delete an update reference from the sequence table */
    int del_seqtab(unsigned int a_index);
    /* gives an array of identifiers of the given type */
    int get_symtab(unsigned char a_type, char ***a_array, unsigned int *a_size);
    /* gives the index'th entry in the sequence table */
    int get_seqtab(unsigned int a_index, char **a_name, stringlist **a_ilist);
    /* gives the index'th entry in the update table */
    int get_updatetab(unsigned int a_index,
                      char **a_name,
                      stringlist **a_vlist,
                      expression **a_precond,
                      expression **a_postcond);
    /* returns the length of the sequence table */
    unsigned int length_seqtab();
    /* returns the length of the update table */
    unsigned int length_updatetab();
    /* checks whether name of the given type is in symtab */
    int check_symtab(const char *a_name, unsigned char a_type);
    /* enumerate the sequences in the sequence table, output to file */
    int list_seqtab(FILE *a_file);
    /* generate the rules necessary to evaluate queries */
    int compute_generate(FILE *a_file);
    /* generate the query */
    int query_generate(expression *a_exp, FILE *a_file);
#ifdef VLAD_SMODELS
    /* prepares the kb for queries */
    int compute_evaluate();
    /* use smwrap class to evaluate a query */
    int query_evaluate(expression *a_exp, unsigned char *a_res);
#endif
  private :
    /* m_stage: 0 = uninit, 1 = init, 2 = symtab closed */
    unsigned char m_stage;
    /* m_tot_atoms: 0 = holds, 1 = access, 2 = object */
    unsigned int m_tot_atoms[VLAD_ATOM_LAST + 1];
    /* total number of atoms (sum of the elements of the above array) */
    unsigned int m_tot_atom;
    /* the symbol table */
    symtab *m_stable;
    /* the initial expression table */
    expression *m_itable;
    /* the constraints table */
    consttab *m_ctable;
    /* the update table */
    updatetab *m_utable;
    /* the sequence table */
    seqtab *m_setable;
#ifdef VLAD_SMODELS
    smwrap *m_smobject;
#endif
    /* returns a holds fact id based on the info given */
    unsigned int compute_holds(unsigned int a_state,
                               bool a_truth,
                               unsigned int a_sub,
                               unsigned int a_acc,
                               unsigned int a_obj);
    /* returns a memb fact id based on the info given */
    unsigned int compute_memb(unsigned int a_state,
                              bool a_truth,
                              unsigned char a_type,
                              unsigned int a_elt,
                              unsigned int a_grp);
    /* returns a subst fact id based on the info given */
    unsigned int compute_subst(unsigned a_state,
                               bool a_truth,
                               unsigned char a_type,
                               unsigned char a_grp1,
                               unsigned char a_grp2);
    /* returns a fact id based on the info given */
    unsigned int compute_fact(unsigned int a_state,
                              bool a_truth,
                              unsigned int a_fact);
    /* gives a unique id based on the holds entities given */
    int encode_holds(const char *a_sub,
                     const char *a_acc,
                     const char *a_obj,
                     bool a_truth,
                     unsigned int a_state,
                     unsigned int *a_id);
    /* gives a unique id based on the member entities given */
    int encode_memb(const char *a_elt,
                    const char *a_grp,
                    bool a_truth,
                    unsigned int a_state,
                    unsigned int *a_id);
    /* gives a unique id based on the subset entities given */
    int encode_subst(const char *a_grp1,
                     const char *a_grp2,
                     bool a_truth,
                     unsigned int a_state,
                     unsigned int *a_id);
    /* gives a unique id based on the fact given */
    int encode_fact(fact *a_fact, unsigned int a_state, unsigned int *a_id);
    /* gives entities based on the holds id given */
    int decode_holds(unsigned int a_id,
                     char **a_sub,
                     char **a_acc,
                     char **a_obj);
    /* gives entities based on the member id given */
    int decode_memb(unsigned int a_id, char **a_elt, char **a_grp);
    /* gives entities based on the subset id given */
    int decode_subst(unsigned int a_id, char **a_grp1, char **a_grp2);
    /* gives the fact based on the id given */
    int decode_fact(fact **a_fact, unsigned int *a_state, unsigned int a_id);
    /*
     * verifies that s, a and o are in the symtab and that they are of the
     * right type, or listed in vlist if vlist is non-null
     */
    int verify_fact_holds(const char *a_sub,
                          const char *a_acc,
                          const char *a_obj,
                          stringlist *a_vlist);
    /*
     * verifies that e and g are in the symtab and that they are of the right
     * type, or listed in vlist if vlist is non-null
     */
    int verify_fact_memb(const char *a_elt,
                         const char *a_grp,
                         stringlist *a_vlist);
    /*
     * verifies that g1 and g2 are in the symtab and that they are of the right
     * type, or listed in v if v is non-null
     */
    int verify_fact_subst(const char *a_grp1,
                          const char *a_grp2,
                          stringlist *a_vlist);
    /* make sure fact is valid */
    int verify_fact(fact *a_fact, stringlist *a_vlist);
    /* make sure expression e is valid */
    int verify_expression(expression *a_exp);
    /* make sure updateref is valid */
    int verify_updateref(char *a_name, stringlist *a_ilist);
    /* returns the id of the negation of the given fact id */
    unsigned int negate_fact(unsigned int a_fact);
#ifdef VLAD_SMODELS
    /* checks whether the given fact is true, false or unknown */
    int evaluate_fact(unsigned int a_fact, unsigned char *a_res);
#endif
} ;
#endif
