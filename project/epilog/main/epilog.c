/*
 * epilog.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <parser.h>

#define EPI_VERSION "0.9.5"

int main(int argc, char *argv[])
{
  int option;

  opterr = 0;

  while ((option = getopt(argc, argv, "vh")) != -1) {
    switch(option) {
      case 'v' :
        fprintf(stdout, "Epilog %s\n", EPI_VERSION);
        return 0;
      case 'h' :
        fprintf(stdout, "Usage: %s [-v|-h|filename]\n", argv[0]);
        return 0;
      default :
        fprintf(stderr, "Usage: %s [-v|-h|filename]\n", argv[0]);
        return -1;
    }
  }

  if (argc == 1 || (argc == 2 && !strcmp(argv[1], "-")))
    yyin = stdin;
  else if (argc >= 2) {
    if ((yyin = fopen(argv[1], "r")) == NULL) {
      fprintf(stderr, "Cannot open %s for reading\n", argv[1]);
      return -1;
    }
  }
  else {
    fprintf(stderr, "Usage: %s [-v|-h|filename]\n", argv[0]);
    return -1;
  }

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
