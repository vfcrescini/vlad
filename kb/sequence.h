/*
 * sequence.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_SEQUENCE_H
#define __VLAD_SEQUENCE_H

#include <list.h>
#include <expression.h>
#include <stringlist.h>

class transref : public list_item
{
  public :
    transref(const char *n, stringlist *i);
    ~transref();
    bool cmp(list_item *item);
    char *get_name();
    stringlist *get_ilist();
#ifdef DEBUG
    void print(char *s);
#endif
  private :
    char *name;
    stringlist *ilist;
} ;

class sequence : public list
{
   public :
     sequence();
     ~sequence();
     /* add pre-malloc'ed transref */
     int add(transref *t);
     int get(unsigned int i, transref **t);
#ifdef DEBUG
    void print(char *s);
#endif
} ;

#endif
