/*
 * vlad.cpp
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
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
  char *helpstring = "-v|-h|[-e] policy-filename [agent-filename]";
#else
  char *arglist = "vh";
  char *helpstring = "-v|-h|policy-filename [agent-filename]";
#endif
  int curr_opt;
  FILE *policyin = NULL;
  FILE *agentin = NULL;
  polbase *pbase = NULL;
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

  /* get policy file details */
  if (argv[optind] == NULL || !strcmp(argv[optind], "-")) {
    fprintf(stderr, "usage: %s %s\n", argv[0], helpstring);
    return VLAD_FAILURE;
  }

  if ((policyin = fopen(argv[optind], "r")) == NULL) {
    fprintf(stderr, "Unable to open for reading: %s\n", argv[optind]);
    return VLAD_OPENFAILED;
  }

  /* now get agent file details */
  if (optind + 1 >= argc || argv[optind + 1] == NULL || !strcmp(argv[optind + 1], "-"))
    agentin = stdin;
  else if ((agentin = fopen(argv[optind + 1], "r")) == NULL) {
    fprintf(stderr, "unable to open for reading: %s\n", argv[optind + 1]);
    return VLAD_OPENFAILED;
  }

  /* create an instance of the policy base and initialise it */
  if ((pbase = VLAD_NEW(polbase)) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = pbase->init()) != VLAD_OK) {
    fprintf(stderr, "cannot initialise policy base: %d\n", retval);
    return retval;
  }

  /* first initialise the parsers */
  if ((retval = policy_init(policyin, stdout, stderr, pbase)) != VLAD_OK)
    return retval;
  if ((retval = agent_init(agentin, stdout, stderr, pbase, mode)) != VLAD_OK)
    return retval;

  /* then parse */
  if ((retval = policy_parse()) != VLAD_OK)
    return retval;
  if ((retval = agent_parse()) != VLAD_OK)
    return retval;

  /* cleanup */
  fclose(policyin);
  fclose(agentin);
  delete pbase;

  return VLAD_OK;
}
