/*
 * parser.h
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_PARSER_H
#define __VLAD_PARSER_H

#include <cstdio>
#include <vlad/polbase.h>

/* parsers must first be initialised to be used */
int policy_init(FILE *a_in, FILE *a_out, FILE *a_err, polbase *a_pbase);
int agent_init(FILE *a_in,
                   FILE *a_out,
                   FILE *a_err,
                   polbase *a_pbase,
                   unsigned char a_m);

/* it is important that these functions are used instead of the externally
 * available yyparse() functions */
int policy_parse();
int agent_parse();

#endif
