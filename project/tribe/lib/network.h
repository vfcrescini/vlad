#ifndef __TBE_NETWORK_H
#define __TBE_NETWORK_H

#include <tribe/tribe.h>
#include <tribe/list.h>

/* the network */
typedef struct {
  unsigned int interval;
  tbe_list *rlist;
} tbe_net_node;

typedef tbe_list tbe_net;

/* initialise a new net */
int tbe_net_init(tbe_net *a_net);
/* destroy the given net */
void tbe_net_purge(tbe_net *a_net);
/* add a new interval into the network */
int tbe_net_add_int(tbe_net *a_net, unsigned int a_int);
/* add a new relation to an existing interval, also propagate the relation */
int tbe_net_add_rel(tbe_net *a_net,
                    unsigned int a_int1,
                    unsigned int a_int2,
                    unsigned int a_relset);
/* returns the rel set between the given two intervals in the given network */
unsigned int tbe_net_rel(tbe_net a_net,
                         unsigned int a_int1,
                         unsigned int a_int2);
/* print the network as it is stored physically */
void tbe_net_dump1(tbe_net a_net, FILE *a_stream);

/* print the network as it is conceptually */
void tbe_net_dump2(tbe_net a_net, FILE *a_stream);

#endif