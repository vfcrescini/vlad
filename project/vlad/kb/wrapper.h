/*
 * wrapper.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_WRAPPER_H
#define __VLAD_WRAPPER_H

/* class wrapper for the smodels library */

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
  private :
    Smodels *pr_smod;
    Api *pr_api;
    bool initialised;
} ;

#endif
