/*
 * identifier.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_IDENTIFIER_H
#define __VLAD_IDENTIFIER_H

#include <list.h>

typedef enum
{
  subject = 0,
  access = 1,
  object = 2
} ident_type;

class identifier : public list_item
{
  public :
    identifier();
    ~identifier();
    bool cmp(list_item *item);
    int init(const char *n, ident_type t, bool g);
    const char *get_name();
    ident_type get_type();
    bool get_group();
  private :
    char *name;
    ident_type type;
    bool group;
} ;

#endif
