/*
 * symtab.h
 * Vino Crescini
 */

#ifndef __EPI_SYMTAB_H
#define __EPI_SYMTAB_H

int symtab_init();
int symtab_add(char *symbol, unsigned int *index);
int symtab_get(char **symbol, unsigned int index);
int symtab_del_index(unsigned int index);
int symtab_del_sym(char *symbol);
	       
#endif
