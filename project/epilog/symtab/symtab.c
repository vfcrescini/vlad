/*
 * symtab.c
 * Vino Crescini
 */

#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "indexedlist.h"

int symtab_cmp(void *one, void *two);

static indexedlist_type symbol_table;

int symtab_cmp(void *one, void *two)
{
  if (one == NULL && two == NULL)
    return 0;

  if (one == NULL)
    return -1;

  if (two == NULL)
    return 1;

  return strcmp((char *) one, (char *) two);
}

int symtab_init()
{
  return indexedlist_init(&symbol_table);
}
 
int symtab_add(char *symbol, unsigned int *index)
{
  char *new_sym;

  if (symbol == NULL || index == NULL)
    return -1;

  if ((new_sym = (char *) malloc(sizeof(char) * (strlen(symbol) + 1))) == NULL)
    return -1;

  strcpy(new_sym, symbol);

  return indexedlist_add(&symbol_table, (void *) new_sym, index, symtab_cmp);
}

int symtab_del_index(unsigned int index)
{
  char *symbol = NULL;
  
  if (indexedlist_get_data(symbol_table, (void **) &symbol, index) < 0)
    return -1;

  if (symbol != NULL)
    free(symbol);

  return indexedlist_del_index(&symbol_table, index);
}

int symtab_del_sym(char *symbol)
{
  unsigned int index;
 
  if (symbol == NULL)
   return -1;

  if (indexedlist_get_index(symbol_table, (void *) symbol, &index, symtab_cmp) < 0)
    return -1;

  return symtab_del_index(index);
}

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
