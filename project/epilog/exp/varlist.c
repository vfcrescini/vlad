/*
 * varlist.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <string.h>
#include "varlist.h"

int varlist_compare(void *p1, void *p2);
int varlist_destroy(void *p);

/* initialise list */
int varlist_init(varlist_type *list)
{
  return simplelist_init(list);
}

/* gives the length of the list */
int varlist_length(varlist_type list, unsigned int *len)
{
  if (len == NULL)
    return -1;

  *len = list.length;

  return 0; 
}

/* add variable entry into list. names are copied, but identifiers are 
 * assumed to have been dynamically allocated */
int varlist_add(varlist_type *list, char *name, ident_type *ident)
{
  var_type *new_var = NULL;

  if (list == NULL || name == NULL || ident == NULL)
    return -1;

  if ((new_var = (var_type *) malloc(sizeof(var_type))) == NULL)
    return -1;

  if ((new_var->name = (char *) malloc(sizeof(char) * strlen(name))) == NULL)
    return -1;

  strcpy(new_var->name, name);
  new_var->ident = ident;

  return simplelist_add(list, (void *) new_var);
}

/* remove a variable from the list, given the name. (name is freed, identifier
 * is not) */
int varlist_del(varlist_type *list, char *name)
{
  var_type temp_var;

  if (list == NULL || name == NULL)
    return -1;

  temp_var.name = NULL;
  temp_var.ident = NULL;

  return simplelist_del_data(list, 
                             (void *) &temp_var,
                             varlist_compare,
                             varlist_destroy);
}

/* returns 0 if the variable whose name is "name" is in the list */
int varlist_find(varlist_type list, char *name)
{
  var_type temp_var;

  if (name == NULL)
    return -1;

  temp_var.name = name;
  temp_var.ident = NULL;

  return simplelist_find_data(list, (void *) &temp_var, varlist_compare);
}

/* gives a reference to the identifer associated with the variable "name" */
int varlist_get_data(varlist_type list, char *name, ident_type **ident)
{
  var_type temp_var1;
  var_type *temp_var2 = NULL;

  if (name == NULL || ident == NULL)
    return -1;

  temp_var1.name = name;
  temp_var1.ident = NULL;

  if (simplelist_get_data(list, (void *) &temp_var1, (void **) &temp_var2, varlist_compare) != 0)
    return -1;

  if (temp_var2 == NULL)
    return -1;

  *ident = temp_var2->ident;

  return 0;
}

/* same as above, but with an index, also gives a reference to the entire
 * var_type structure */
int varlist_get_index(varlist_type list, unsigned int index, var_type **var)
{
  if (var == NULL)
    return -1;

  return simplelist_get_index(list, index, (void **) var);
}

/* destroy this varlist */
int varlist_purge(varlist_type *list)
{
  unsigned int len;
  unsigned int i;

  if (list == NULL)
    return -1;

  if (simplelist_length(*list, &len) != 0)
    return -1;

  for (i = 0; i < len; i++)
    if (simplelist_del_index(list, 0, varlist_destroy) != 0)
      return -1;

  return 0;
}

/* returns 0 if the NAMES of the variables pointed to by p1 and p2 are equal */
int varlist_compare(void *p1, void *p2)
{
  if (p1 == NULL && p2 == NULL)
    return -1;

  return strcmp(((var_type *) p1)->name, ((var_type *) p2)->name);
}

/* free memory (names, but never the ident) */
int varlist_destroy(void *p)
{
  if (p == NULL)
    return -1;

  if (((var_type *) p)->name != NULL)
    free(((var_type *) p)->name);

  free((var_type *) p);
  p = NULL;

  return 0;
}
