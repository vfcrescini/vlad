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
    transref();
    ~transref();
    bool cmp(list_item *item);
    int init(const char *n, stringlist *il);
    int get(char **n, stringlist **il);
#ifdef DEBUG
    void print(char *s);
#endif
  private :
    char *name;
    stringlist *ilist;
    bool initialised;
} ;

class sequence : public list
{
   public :
     sequence();
     ~sequence();
     /* add pre-malloc'ed transref */
     int add(transref *t);
     /* add pre-malloc'ed name and ilist */
     int add(const char *n, stringlist *il);
     /* get i'th name and ilist */
     int get(unsigned int i, char **n, stringlist **il);
#ifdef DEBUG
    void print(char *s);
#endif
} ;

#endif
