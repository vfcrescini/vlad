/*
 * wrapper.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_WRAPPER_H
#define __VLAD_WRAPPER_H

/* class wrapper for the smodels library */

#include <vlad/numberlist.h>
#include <smodels.h>
#include <api.h>
#include <atomrule.h>

class wrapper
{
  public :
    wrapper();
    ~wrapper();
    int init();
    /* after this no more calls to add_atom() are allowed */
    int close_atom();
    /* after this no more calls to add_rule_*() are allowed */
    int close_rule();
    /* register an atom */
    int add_atom(unsigned int a);
    /* add a single axiom (always true or always false) */
    int add_axiom(unsigned int a, bool t);
    /* add multiple axiom (always true or always false) */
    int add_axiom(numberlist *a, bool t);
    /* add single rule (single head, pos body and neg body */
    int add_rule(unsigned int h, unsigned int pb, unsigned int nb);
    /* add multiple rule (single head, pos body list and neg body list */
    int add_rule(unsigned int h, numberlist *pb, numberlist *nb);
    /* return true, false or unknown */
    int ask(unsigned int a, char *r);
  private :
    Smodels *pr_smod;
    Api *pr_api;
    int stage;
} ;

#endif
