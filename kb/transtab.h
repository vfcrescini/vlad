/*
 * transtab.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_TRANSTAB_H
#define __VLAD_TRANSTAB_H

#include <vlad/list.h>
#include <vlad/expression.h>
#include <vlad/stringlist.h>

class transdef : public list_item
{
  public :
    transdef();
    ~transdef();
    bool cmp(list_item *item);
    int get(char **n, stringlist **v, expression **pr, expression **po);
    int init(const char *n, stringlist *v, expression *pr, expression *po);
  private :
    char *name;
    stringlist *vlist;
    expression *precond;
    expression *postcond;
    bool initialised;
} ;

class transtab : public list
{
  public :
    transtab();
    ~transtab();
    int add(const char *n, stringlist *v, expression *pr, expression *po);
    /* get trans by name */
    int get(const char *n, stringlist **v, expression **pr, expression **po);
    /* replace variables with identifiers in ilist, then get pr and pp */
    int replace(const char *n, stringlist *ilist, expression **pr, expression **po);
} ; 

#endif
