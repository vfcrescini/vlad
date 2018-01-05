/*
 * vlad.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>

#include <getopt.h>

#include <config.h>
#include <vlad/vlad.h>
#include <vlad/parser.h>

int main(int argc, char *argv[])
{
  int retval;
#ifdef SMODELS
  char *arglist = "vhe";
  char *helpstring = "-v|-h|[-e] program-filename [query-filename]";
#else
  char *arglist = "vh";
  char *helpstring = "-v|-h|program-filename [query-filename]";
#endif
  int option;

  opterr = 0;
  mode = VLAD_MODE_GENERATE;

  while ((option = getopt(argc, argv, arglist)) != -1) {
    switch(option) {
      case 'v' :
        fprintf(stdout, "vLad %s\n", VERSION);
        return VLAD_OK;
      case 'h' :
        fprintf(stdout, "usage: %s %s\n", argv[0], helpstring);
        return VLAD_OK;;
#ifdef SMODELS
      case 'e' :
	mode = VLAD_MODE_EVALUATE;
	break;
#endif
      default :
        fprintf(stderr, "usage: %s %s\n", argv[0], helpstring);
        return VLAD_FAILURE;
    }
  }

  /* get program file details */
  if (argv[optind] == NULL || !strcmp(argv[optind], "-")) {
    fprintf(stderr, "usage: %s %s\n", argv[0], helpstring);
    return VLAD_FAILURE;
  }

  if ((programin = fopen(argv[optind], "r")) == NULL) {
    fprintf(stderr, "Unable to open for reading: %s\n", argv[optind]);
    return VLAD_OPENFAILED;
  }

  /* now get query file details */
  if (optind + 1 >= argc || argv[optind + 1] == NULL || !strcmp(argv[optind + 1], "-"))
    queryin = stdin;
  else if ((queryin = fopen(argv[optind + 1], "r")) == NULL) {
    fprintf(stderr, "unable to open for reading: %s\n", argv[optind + 1]);
    return VLAD_OPENFAILED;
  }

  /* create an instance of the kb and initialise it */
  if ((kbase = VLAD_NEW(kb)) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = kbase->init()) != VLAD_OK) {
    fprintf(stderr, "cannot initialise kbase: %d\n", retval);
    return retval;
  }

  /* set both out streams to stdout */
  programout = stdout;
  queryout = stdout;

  /* first parse the program */
  if ((retval = programparse()) != VLAD_OK)
    return retval;

  /* then the queries */
  if ((retval = queryparse()) != VLAD_OK)
    return retval;

  /* cleanup */
  fclose(programin);
  fclose(queryin);
  delete kbase;

  return VLAD_OK;
}
