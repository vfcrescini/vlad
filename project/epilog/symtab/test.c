#include <stdio.h>
#include "symtab.h"

int main()
{
  unsigned int x;
  char *sym;
  int i;
  
  symtab_init();
  symtab_add("one", &x);
  printf("%lu\n", x);
  symtab_add("two", &x);
  printf("%lu\n", x);
  symtab_add("three", &x);
  printf("%lu\n", x);
  symtab_add("four", &x);
  printf("%lu\n", x);
  symtab_add("five", &x);
  printf("%lu\n", x);

  printf("XXXX\n");

  for (i = 0; i < 5; i++) {
    symtab_get(&sym, i);
    printf("%d=%s\n", i, sym);
    free(sym);
    sym = NULL;
  }
    
  printf("XXXX\n");

  symtab_del_index(3);

  for (i = 0; i < 5; i++) {
    symtab_get(&sym, i);
    printf("%d=%s\n", i, sym);
    free(sym);
    sym = NULL;
  }

  printf("XXXX\n");

  symtab_add("quatro", &x);
  printf("%lu\n", x);

  printf("XXXX\n");

  for (i = 0; i < 5; i++) {
    symtab_get(&sym, i);
    printf("%d=%s\n", i, sym);
    free(sym);
    sym = NULL;
  }

  printf("XXXX\n");

  symtab_del_sym("three");

  printf("XXXX\n");

 for (i = 0; i < 5; i++) {
    symtab_get(&sym, i);
    printf("%d=%s\n", i, sym);
    free(sym);
    sym = NULL;
  }
  
  return 0;
}
