/*
 * seqtab.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_SEQTAB_H
#define __VLAD_SEQTAB_H

#include <vlad/list.h>
#include <vlad/expression.h>
#include <vlad/stringlist.h>

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

class seqtab : public list
{
   public :
     seqtab();
     ~seqtab();
     /* add pre-malloc'ed transref */
     int add(transref *t);
     /* add pre-malloc'ed name and ilist */
     int add(const char *n, stringlist *il);
     /* delete i'th item */
     int del(unsigned int i);
     /* get i'th name and ilist */
     int get(unsigned int i, char **n, stringlist **il);
#ifdef DEBUG
    void print(char *s);
#endif
} ;

#endif
