/*
 * epilog.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdio.h>
#include <parser.h>

int main(int argc, char *argv[])
{
  yyin = stdin;
  yyout = stdout;
  yyerr = stderr;

  yyparse();

  return 0;
}

int yyerror(char *error)
{
  fprintf(yyerr, "line %d: ERROR: %s\n", line_no, error);

  return 0;
}

int yywarn(char *warn)
{
  fprintf(yyerr, "line %d: WARNING: %s\n", line_no, warn);

  return 0;
}
