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
  private :
    char *name;
    stringlist *vtable;
    expression *precond;
    expression *postcond;
} ;

class transtab : public list
{
  public :
    transtab(const char *n);
    ~transtab();
    int add(const char *n, stringlist *v, expression *pr, expression *po);
} ; 

#endif
