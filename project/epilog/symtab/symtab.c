/*
 * symtab.c
 * Vino Crescini
 */

#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "indexedlist.h"

int symtab_cmp(void *ptr1, void *ptr2);
int symtab_free(void *ptr);

static indexedlist_type symbol_table;

/* cast 2 pointers to string and call strcmp */
int symtab_cmp(void *ptr1, void *ptr2)
{
  if (ptr1 == NULL && ptr2 == NULL)
    return 0;

  if (ptr1 == NULL)
    return -1;

  if (ptr2 == NULL)
    return 1;

  return strcmp((char *) ptr1, (char *) ptr2);
}

/* NULL-checking version of free */
int symtab_free(void *ptr)
{
  if (ptr == NULL)
    return -1;

  free(ptr);

  return 0;
}

/* initialise */
int symtab_init()
{
  return indexedlist_init(&symbol_table);
}

/* add. we have to manually allocate memory before storing the pointer */ 
int symtab_add(char *symbol, unsigned int *index)
{
  char *new_sym;

  if (symbol == NULL || index == NULL)
    return -1;

  /* make sure the symbol's not already in the table */
  if (indexedlist_get_index(symbol_table, symbol, index, symtab_cmp) == 0)
    return 0;

  /* allocate space for the string */
  if ((new_sym = (char *) malloc(sizeof(char) * (strlen(symbol) + 1))) == NULL)
    return -1;

  strcpy(new_sym, symbol);

  return indexedlist_add(&symbol_table, (void *) new_sym, index);
}

/* delete an entry with an index */
int symtab_del_index(unsigned int index)
{
  return indexedlist_del_index_f(&symbol_table, index, symtab_free);
}

/* same as above, but with a symbol instead of an index */
int symtab_del_sym(char *symbol)
{
  return indexedlist_del_data_f(&symbol_table, 
                                (void *) symbol,
                                symtab_cmp, 
                                symtab_free);
}

/* make a copy of the data. left to the calling function to free it */
int symtab_get(char **symbol, unsigned int index)
{
  char *tmp_sym = NULL;

  if (symbol == NULL)
    return -1;

  if (indexedlist_get_data(symbol_table, (void **) &tmp_sym, index) < 0)
    return -1;

  if (tmp_sym == NULL)
    return -1;

  if ((*symbol = (char *) malloc(sizeof(char) * (strlen(tmp_sym) + 1))) == NULL)
    return -1;

  strcpy(*symbol, tmp_sym);

  return 0;
}
