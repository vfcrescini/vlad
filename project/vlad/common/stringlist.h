/*
 * stringlist.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
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
    bool cmp(list_item *item);
    int init(const char *s);
    char *get();
#ifdef DEBUG
    /* assuming s has enough memory allocation */
    void print(char *s);
#endif
  private :
    char *str;
} ;

class stringlist : public list
{
  public :
    /* n is the optional name for the list */
    stringlist();
    ~stringlist();
    /* add a string in the list */
    int add(const char *s);
    /* get the index of the string */
    int get(const char *s, unsigned int *i);
    /* get the ith string in the list */
    int get(unsigned int i, char **s);
    /* return true if string is in the list */
    int find(const char *s);
#ifdef DEBUG
    /* assumimg s has enough memory allocation */
    void print(char *s);
#endif
} ;

#endif
