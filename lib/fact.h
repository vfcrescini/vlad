/*
 * fact.h
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_FACT_H
#define __VLAD_FACT_H

#include <vlad/list.h>
#include <vlad/stringlist.h>

/* structure to hold facts. no checking done here, values are just stored */

typedef struct
{
  char *subject;
  char *access;
  char *object;
} holds_atom;

typedef struct
{
  char *element;
  char *group;
} member_atom;

typedef struct
{
  char *group1;
  char *group2;
} subset_atom;

class fact : public list_item
{
  public :
    fact();
    ~fact();
    bool cmp(list_item *a_item);
    /* get attributes from fact */
    int get(char **a_ent1,
            char **a_ent2,
            char **a_ent3,
            unsigned char *a_type,
            bool *a_truth);
    int get_holds(char **a_sub, char **a_acc, char **a_obj);
    int get_member(char **a_elt, char **a_grp);
    int get_subset(char **a_grp1, char **a_grp2);
    /*initialise facts */
    int init(const char *a_ent1,
             const char *a_ent2,
             const char *a_ent3,
             unsigned char a_type,
             bool a_truth);
    int init_holds(const char *a_sub,
                   const char *a_acc,
                   const char *a_obj,
                   bool a_truth);
    int init_member(const char *a_elt,
                    const char *a_grp,
                    bool a_truth);
    int init_subset(const char *a_grp1,
                    const char *a_grp2,
                    bool a_truth);
    /* create a new instance of this fact */
    int copy(fact **a_fact);
    /* replaces instances of var with ident, gives a new fact */
    int replace(const char *a_var, const char *a_ident, fact **a_fact);
    /* replaces vars in vlist with entities in ilist. gives a new fact */
    int replace(stringlist *a_vlist, stringlist *a_ilist, fact **a_fact);
    /* reverses the truth value */
    void negate();
    /* gives the type of the fact */
    int type(unsigned char *a_type);
    /* gives the truth value of the fact */
    int truth(bool *a_truth);
#ifdef VLAD_DEBUG
    /* assuming a_str has enough memory allocation */
    void print(char *a_str);
#endif
  private :
    int reset();
    unsigned char m_type;
    bool m_truth;
    union {
      holds_atom m_holds;
      member_atom m_member;
      subset_atom m_subset;
    } ;
    bool m_init;
} ;

#endif
