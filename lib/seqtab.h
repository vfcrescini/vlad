/*
 * seqtab.h
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
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
    bool cmp(list_item *a_item);
    int init(const char *a_name, stringlist *a_list);
    int get(char **a_name, stringlist **a_list);
#ifdef VLAD_DEBUG
    void print(char *a_str);
#endif
  private :
    char *m_name;
    stringlist *m_list;
    bool m_init;
} ;

class seqtab : public list
{
   public :
     seqtab();
     ~seqtab();
     /* add pre-malloc'ed transref */
     int add(transref *a_tref);
     /* add pre-malloc'ed name and ilist */
     int add(const char *a_name, stringlist *a_list);
     /* delete i'th item */
     int del(unsigned int a_index);
     /* get i'th name and ilist */
     int get(unsigned int a_index, char **a_name, stringlist **a_list);
#ifdef VLAD_DEBUG
    void print(char *a_str);
#endif
} ;

#endif
