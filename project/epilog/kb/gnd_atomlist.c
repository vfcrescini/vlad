/*
 * gnd_atomlist.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include "gnd_atomlist.h"

int gnd_atomlist_cmp(void *ptr1, void *ptr2);
int gnd_atomlist_free(void *ptr);

int gnd_atomlist_init(gnd_atomlist_type *list)
{
  return indexedlist_init((indexedlist_type *) list);
}

int gnd_atomlist_add(gnd_atomlist_type *list, gnd_atom_type atom)
{
  gnd_atom_type *new_atom;
  unsigned int index;

  if (list == NULL)
    return -1;

  if ((new_atom = (gnd_atom_type *) malloc(sizeof(gnd_atom_type))) == NULL)
    return -1;

  *new_atom = atom;

  return indexedlist_add((indexedlist_type *) list, (void *) new_atom, &index);
}

int gnd_atomlist_remove(gnd_atomlist_type *list, gnd_atom_type atom)
{
  if (list == NULL)
    return -1;

  return indexedlist_del_data_f((indexedlist_type *) list, 
                                &atom,
                                gnd_atomlist_cmp,
                                gnd_atomlist_free);
}

int gnd_atomlist_isin(gnd_atomlist_type list, gnd_atom_type atom)
{
  unsigned int index;

  if (indexedlist_get_index((indexedlist_type) list, 
                            (void *) &atom,
                            &index,
                            gnd_atomlist_cmp) < 0)

    return -1;

  return index >= 0;
}

/* return 0 if equal, -1 otherwise */
int gnd_atomlist_cmp(void *ptr1, void *ptr2)
{
  if (ptr1 == NULL && ptr2 == NULL)
    return 0;

  if (ptr1 == NULL || ptr2 == NULL)
    return -1;

  return gnd_atom_cmp(*(gnd_atom_type *) ptr1, *(gnd_atom_type *) ptr2);
}

/* just call free, there are no dynamically allocated mem inside */
int gnd_atomlist_free(void *ptr)
{
  if (ptr == NULL)
    return -1;

  free(ptr);

  return 0;
}
