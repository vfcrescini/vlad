/*
 * epilog.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdio.h>
#include <parser.h>
#include <identlist.h>

int main()
{
  yyin = stdin;
  yyout = stdout;

  identlist_init();
  
  yyparse();

  identlist_purge();

  return 0;
}

int yyerror(char *error)
{
  fprintf(stderr, "line %d: %s\n", line_no, error);

  return 0;
}
