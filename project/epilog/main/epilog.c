/*
 * epilog.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdio.h>
#include <symtab.h>

extern int yyparse();

int main()
{
  symtab_init();
  yyparse();

  return 0;
}
