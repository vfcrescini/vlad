/*
 * epilog.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdio.h>
#include <parser.h>
#include <identlist.h>
#include <expression.h>

int main()
{
  expression_type initial_exp;

  yyin = stdin;
  yyout = stdout;

  identlist_init();
  expression_init(&initial_exp);
  
  yyparse();

  identlist_purge();

  return 0;
}

int yyerror(char *error)
{
  fprintf(stderr, "line %d: %s\n", line_no, error);

  return 0;
}
