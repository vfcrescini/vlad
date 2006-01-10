/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004, 2005, 2006 University of Western Sydney
 * by Vino Fernando Crescini <jcrescin@cit.uws.edu.au>
 *
 * PolicyUpdater is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * PolicyUpdater is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PolicyUpdater; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <getopt.h>

#include <vlad/vlad.h>
#include <vlad/mem.h>
#include <vlad/polbase.h>
#include "parser.h"

void print_usage(FILE *a_fs);

int main(int argc, char *argv[])
{
  int retval;
  int curr_opt;
  FILE *policyin = NULL;
  FILE *agentin = NULL;
  vlad_polbase *pbase = NULL;
  unsigned char mode = VLAD_MODE_GENERATE;
  char arglist[] = {
#ifdef VLAD_SMODELS
    'e',
#endif
#ifdef VLAD_TIMER
    't',
#endif
    'v',
    'h',
  };
#ifdef VLAD_TIMER
  bool timer = false;
#endif

  opterr = 0;

  while ((curr_opt = getopt(argc, argv, arglist)) != -1) {
    switch(curr_opt) {
      case 'v' :
        fprintf(stdout, "vLad %s\n", VLAD_VERSION);
        fprintf(stdout, "Copyright (C) 2003, 2004, 2005, 2006 University of Western Sydney\n");
        fprintf(stdout, "by Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>\n");
        return VLAD_OK;
      case 'h' :
        print_usage(stdout);
        return VLAD_OK;;
#ifdef VLAD_SMODELS
      case 'e' :
	mode = VLAD_MODE_EVALUATE;
	break;
#endif
#ifdef VLAD_TIMER
      case 't' :
        timer = true;
        break;
#endif
      default :
        print_usage(stdout);
        return VLAD_FAILURE;
    }
  }

  /* get policy file details */
  if (argv[optind] == NULL || !strcmp(argv[optind], "-")) {
    print_usage(stdout);
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
  if ((pbase = VLAD_MEM_NEW(vlad_polbase)) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = pbase->init()) != VLAD_OK) {
    fprintf(stderr, "cannot initialise policy base: %d\n", retval);
    return retval;
  }

  /* first initialise the parsers */
  if ((retval = policy_init(policyin, stdout, stderr, pbase)) != VLAD_OK)
    return retval;
#ifdef VLAD_TIMER
  if ((retval = agent_init(agentin, stdout, stderr, pbase, mode, timer)) != VLAD_OK)
#else
  if ((retval = agent_init(agentin, stdout, stderr, pbase, mode)) != VLAD_OK)
#endif
    return retval;

  /* then parse */
  if ((retval = policy_parse()) != VLAD_OK)
    return retval;
  if ((retval = agent_parse()) != VLAD_OK)
    return retval;

  /* cleanup */
  fclose(policyin);
  fclose(agentin);
  VLAD_MEM_DELETE(pbase);

  return VLAD_OK;
}

void print_usage(FILE *a_fs)
{
  if (a_fs == NULL)
    return;

  fprintf(a_fs, "usage: vlad [options] policy-filename [agent-filename]\n");
  fprintf(a_fs, "  -v display version number\n");
  fprintf(a_fs, "  -h display this message\n");
#ifdef VLAD_SMODELS
  fprintf(a_fs, "  -e if configured with --with-smodels, evaluate queries\n");
#endif
#ifdef VLAD_TIMER
  fprintf(a_fs, "  -t if configured with --enable-timer, display computation time\n");
#endif
}
