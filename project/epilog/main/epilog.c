/*
 * epilog.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdio.h>
#include <symtab.h>
#include <parser.h>

int main()
{
  yyin = stdin;
  yyout = stdout;

  symtab_init();
  yyparse();

  return 0;
}

int yyerror(char *error)
{
  fprintf(stderr, "line %d: %s\n", line_no, error);

  return 0;
}
