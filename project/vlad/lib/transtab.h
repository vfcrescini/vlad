/*
 * transtab.h
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
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
    bool cmp(list_item *a_item);
    int get(char **a_name,
            stringlist **a_vlist,
            expression **a_precond,
            expression **a_postcond);
    int init(const char *a_name,
             stringlist *a_vlist,
             expression *a_precond,
             expression *a_postcond);
  private :
    char *m_name;
    stringlist *m_vlist;
    expression *m_precond;
    expression *m_postcond;
    bool m_init;
} ;

class transtab : public list
{
  public :
    transtab();
    ~transtab();
    int add(const char *a_name,
            stringlist *a_vlist,
            expression *a_precond,
            expression *a_postcond);
    /* get trans by name */
    int get(const char *a_name,
             stringlist **a_vlist,
             expression **a_precond,
             expression **a_postcond);
    /* get trans by index */
    int get(unsigned int a_index,
            char **a_name,
            stringlist **a_vlist,
            expression **a_precond,
            expression **a_postcond);
    /* replace variables with identifiers in ilist, then get pr and pp */
    int replace(const char *a_name,
                stringlist *a_ilist,
                expression **a_precond,
                expression **a_postcond);
} ;

#endif
