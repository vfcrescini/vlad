/*
 * querytab.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_QUERYTAB_H
#define __VLAD_QUERYTAB_H

#include <list.h>
#include <expression.h>
#include <stringlist.h>

class transref : public list_item
{
  public :
    transref(const char *n, stringlist *i);
    ~transref();
    bool cmp(list_item *item);
  private :
    char *name;
    stringlist *ilist;
} ;

class transreflist : public list
{
   public :
     transreflist(const char *n);
     ~transreflist();
     /* add pre-malloc'ed transref */
     int add(transref *t);
} ;

class query : public list_item
{
  public :
    query(expression *p, transreflist *r);
    ~query();
    bool cmp(list_item *item);
  private :
    expression *prop;
    transreflist *reflist;
} ;

class querytab : public list
{
  public :
    querytab(const char *n);
    ~querytab();
    int add(expression *p, transreflist *r);
} ;

#endif
