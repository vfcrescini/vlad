/*
 * vlad.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <getopt.h>

#include <config.h>
#include <vlad/vlad.h>
#include <vlad/parser.h>

int main(int argc, char *argv[])
{
  int option;
#ifdef SMODELS
  char *arglist = "vhe";
  char *helpstring = "[-v|-h|[-e] filename]";
#else
  char *arglist = "vh";
  char *helpstring = "[-v|-h|filename]";
#endif

  mode = VLAD_MODE_GENERATE;
  opterr = 0;

  while ((option = getopt(argc, argv, arglist)) != -1) {
    switch(option) {
      case 'v' :
        fprintf(stdout, "vLad %s\n", VERSION);
        return 0;
      case 'h' :
        fprintf(stdout, "Usage: %s %s\n", argv[0], helpstring);
        return 0;
#ifdef SMODELS
      case 'e' :
	mode = VLAD_MODE_EVALUATE;
	break;
#endif
      default :
        fprintf(stderr, "Usage: %s %s\n", argv[0], helpstring);
        return -1;
    }
  }

  if (argv[optind] == NULL || !strcmp(argv[optind], "-"))
    yyin = stdin;
  else {
    if ((yyin = fopen(argv[optind], "r")) == NULL) {
      fprintf(stderr, "Cannot open %s for reading\n", argv[optind]);
      return -1;
    }
  }

  yyout = stdout;
  yyerr = stderr;

  yyparse();

  return 0;
}
