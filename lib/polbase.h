/*
 * polbase.h
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_POLBASE_H
#define __VLAD_POLBASE_H

#include <vlad/vlad.h>
#include <vlad/symtab.h>

class polbase {
  public :
    polbase();
    ~polbase();
    /* (re)init the policy base */
    int init();
    /* close symbol table */
    int close_symtab();
  private :
    /* m_stage: 0 = uninit, 1 = init, 2 = symtab closed */
    unsigned char m_stage;
    /* m_tot_atoms: 0 = holds, 1 = access, 2 = object */
    unsigned int m_tot_atoms[VLAD_ATOM_LAST + 1];
    /* the symbol table */
    symtab *stable;
    /* gives a unique id based on the holds entities given */
    int encode_holds(const char *a_sub, const char *a_acc, const char *a_obj, unsigned int *a_id);
    /* gives a unique id based on the member entities given */
    int encode_memb(const char *a_elt, const char *a_grp, unsigned int *a_id);
    /* gives a unique id based on the subset entities given */
    int encode_subst(const char *a_grp1, const char *a_grp2, unsigned int *a_id);
    /* gives entities based on the holds id given */
    int decode_holds(unsigned int a_id, char **a_sub, char **a_acc, char **a_obj);
    /* gives entities based on the member id given */
    int decode_memb(unsigned int a_id, char **a_elt, char **a_grp);
    /* gives entities based on the subset id given */
    int decode_subst(unsigned int a_id, char **a_grp1, char **a_grp2);
} ;

#endif
