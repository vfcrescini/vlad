/*
 * transtab.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_TRANSTAB_H
#define __VLAD_TRANSTAB_H

#include <list.h>
#include <expression.h>
#include <stringlist.h>

class transdef : public list_item
{
  public :
    transdef(const char *n, stringlist *v, expression *pr, expression *po);
    ~transdef();
    bool cmp(list_item *item);
    char *get_name();
    stringlist *get_vlist();
    expression *get_precond();
    expression *get_postcond();
  private :
    char *name;
    stringlist *vlist;
    expression *precond;
    expression *postcond;
} ;

class transtab : public list
{
  public :
    transtab();
    ~transtab();
    int add(const char *n, stringlist *v, expression *pr, expression *po);
    int get(const char *n, transdef **t);
    /* replace variables with identifiers in ilist, then get pr and pp */
    int get(unsigned int i, stringlist *ilist, expression **pr, expression **po);
} ; 

#endif
