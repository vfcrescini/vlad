/*
 * vlad.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <getopt.h>

#include <config.h>
#include <vlad.h>
#include <parser.h>
#include <list.h>
#include <identifier.h>
#include <symtab.h>

int main(int argc, char *argv[])
{
  int option;

  opterr = 0;

  while ((option = getopt(argc, argv, "vh")) != -1) {
    switch(option) {
      case 'v' :
        fprintf(stdout, "vLad %s\n", VERSION);
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

#if 1
  yyparse();
#else
  symtab *x;
  identifier *y;

  x = new symtab();
  x->add("1", VLAD_IDENT_SUBJECT);
  x->add("2", VLAD_IDENT_SUBJECT);
  x->add("3", VLAD_IDENT_SUBJECT);
  x->add("4", VLAD_IDENT_SUBJECT);
  x->add("5", VLAD_IDENT_SUBJECT);
  x->add("5", VLAD_IDENT_SUBJECT);

  fprintf(stderr, "%d\n\n", x->length());

  x->get("5", &y);
  fprintf(stderr, "%s\n", y->get_name());
  x->get("4", &y);
  fprintf(stderr, "%s\n", y->get_name());
  x->get("3", &y);
  fprintf(stderr, "%s\n", y->get_name());
  x->get("2", &y);
  fprintf(stderr, "%s\n", y->get_name());
  x->get("1", &y);
  fprintf(stderr, "%s\n\n", y->get_name());

  x->del("3");
  
  x->get("5", &y);
  fprintf(stderr, "%s\n", y->get_name());
  x->get("4", &y);
  fprintf(stderr, "%s\n", y->get_name());
  x->get("3", &y);
  fprintf(stderr, "%s\n", y->get_name());
  x->get("2", &y);
  fprintf(stderr, "%s\n", y->get_name());
  x->get("1", &y);
  fprintf(stderr, "%s\n\n", y->get_name());
#endif

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
