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
    /* register an atom */
    int add_atom(unsigned int a);
    /* add rule with constant head */
    int add_rule_const_head(numberlist *pbody, numberlist *nbody, bool h);
    /* add rule with constant body */
    int add_rule_const_body(numberlist *head, bool b);
    /* add rule with a single head */
    int add_rule_single_head(unsigned int head, numberlist *pbody, numberlist *nbody);
  private :
    Smodels *pr_smod;
    Api *pr_api;
    bool initialised;
} ;

#endif
