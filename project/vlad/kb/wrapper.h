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
    /* constant head & single body */
    int add_rule_chead_sbody(bool h, unsigned int pb, unsigned int nb);
    /* constant head & multiple body */
    int add_rule_chead_mbody(bool h, numberlist *pb, numberlist *nb);
    /* single head & constant body */
    int add_rule_shead_cbody(unsigned int h, bool b);
    /* single head & single body */
    int add_rule_shead_sbody(unsigned int h, unsigned int pb, unsigned int nb);
    /* single head & multiple body */
    int add_rule_shead_mbody(unsigned int h, numberlist *pb, numberlist *nb);
    /* multiple head & constant body */
    int add_rule_mhead_cbody(numberlist *h, bool b);
    /* multiple head & single body */
    int add_rule_mhead_sbody(numberlist *h, unsigned int pb, unsigned int nb);
    /* multiple head & multiple body */
    int add_rule_mhead_mbody(numberlist *h, numberlist *pb, numberlist *nb);
  private :
    Smodels *pr_smod;
    Api *pr_api;
    int stage;
} ;

#endif
