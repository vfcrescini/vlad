/*
 * vlad.cpp
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>

#include <getopt.h>

#include <vlad/vlad.h>
#include "parser.h"

int main(int argc, char *argv[])
{
  int retval;
#ifdef VLAD_SMODELS
  char *arglist = "vhe";
  char *helpstring = "-v|-h|[-e] program-filename [operation-filename]";
#else
  char *arglist = "vh";
  char *helpstring = "-v|-h|program-filename [operation-filename]";
#endif
  int curr_opt;
  FILE *programin = NULL;
  FILE *operationin = NULL;
  kb *kbase = NULL;
  unsigned char mode = VLAD_MODE_GENERATE;

  opterr = 0;

  while ((curr_opt = getopt(argc, argv, arglist)) != -1) {
    switch(curr_opt) {
      case 'v' :
        fprintf(stdout, "vLad %s\n", VLAD_VERSION);
        return VLAD_OK;
      case 'h' :
        fprintf(stdout, "usage: %s %s\n", argv[0], helpstring);
        return VLAD_OK;;
#ifdef VLAD_SMODELS
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

  /* now get operation file details */
  if (optind + 1 >= argc || argv[optind + 1] == NULL || !strcmp(argv[optind + 1], "-"))
    operationin = stdin;
  else if ((operationin = fopen(argv[optind + 1], "r")) == NULL) {
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

  /* first initialise the parsers */
  if ((retval = program_init(programin, stdout, stderr, kbase)) != VLAD_OK)
    return retval;
  if ((retval = operation_init(operationin, stdout, stderr, kbase, mode)) != VLAD_OK)
    return retval;

  /* then parse */
  if ((retval = program_parse()) != VLAD_OK)
    return retval;
  if ((retval = operation_parse()) != VLAD_OK)
    return retval;

  /* cleanup */
  fclose(programin);
  fclose(operationin);
  delete kbase;

  return VLAD_OK;
}
