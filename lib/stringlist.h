/*
 * stringlist.h
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <vlad/list.h>

#ifndef __VLAD_STRINGLIST_H
#define __VLAD_STRINGLIST_H

class string : public list_item
{
  public :
    string();
    ~string();
    /* compare item with this string */
    bool cmp(list_item *a_item);
    int init(const char *a_str);
    char *get();
#ifdef VLAD_DEBUG
    /* assuming s has enough memory allocation */
    void print(char *a_str);
#endif
  private :
    char *m_string;
} ;

class stringlist : public list
{
  public :
    /* n is the optional name for the list */
    stringlist();
    ~stringlist();
    /* add a string in the list */
    int add(const char *a_str);
    /* get the index of the string */
    int get(const char *a_str, unsigned int *a_index);
    /* get the ith string in the list */
    int get(unsigned int a_index, char **a_str);
    /* return true if string is in the list */
    int find(const char *a_str);
#ifdef VLAD_DEBUG
    /* assumimg s has enough memory allocation */
    void print(char *a_str);
#endif
} ;

#endif
