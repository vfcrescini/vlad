/* 
 * stringlist.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_STRINGLIST_H
#define __EPI_STRINGLIST_H

#include "simplelist.h"

#define stringlist_type simplelist_type

/* initialise list */
int stringlist_init(stringlist_type *list);

/* gives the number of strings in the stringlist */
int stringlist_length(stringlist_type list, unsigned int *len);

/* return 0 if the string is in the stringlist */
int stringlist_find(stringlist_type list, char *string);

/* gives a reference to the index'th string in the stringlist */
int stringlist_get(stringlist_type list, unsigned int index, char **string);

/* add an string into the stringlist */
int stringlist_add(stringlist_type *list, char *string);

/* delete an string from the stringlist */
int stringlist_del(stringlist_type *list, char *string);

/* delete all strings from this stringlist */
int stringlist_purge(stringlist_type *list);

#endif
