/*
 * smwrap.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_SMWRAP_H
#define __VLAD_SMWRAP_H

/* class smwrap for the smodels library */

#include <vlad/numberlist.h>
#include <smodels.h>
#include <api.h>
#include <atomrule.h>

class smwrap
{
  public :
    smwrap();
    ~smwrap();
    int init();
    /* after this no more calls to add_atom() are allowed */
    int close_atom();
    /* after this no more calls to add_rule_*() are allowed */
    int close_rule();
    /* register an atom */
    int add_atom(unsigned int a_atom);
    /* add an axiom (always true or always false) */
    int add_axiom(bool a_tr, unsigned int a_count, ...);
    /* add rule: variable argument list */
    int add_rule(unsigned int a_pcount, unsigned int a_ncount, unsigned int a_head, ...);
    /* add rule: numberlist */
    int add_rule(unsigned int a_head, numberlist *a_pbody, numberlist *a_nbody);
    /* return true, false or unknown */
    int ask(numberlist *a_exp,
            unsigned int (*a_negate)(unsigned int, unsigned int),
            unsigned int a_parm,
            unsigned char *a_res);
  private :
    Smodels *pr_smod;
    Api *pr_api;
    int pr_stage;
} ;

#endif
