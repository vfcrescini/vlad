/*
 * parser.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_PARSER_H
#define __VLAD_PARSER_H

#include <cstdio>
#include <vlad/kb.h>

/* parsers must first be initialised to be used */
int program_init(FILE *a_in, FILE *a_out, FILE *a_err, kb *a_kb);
int query_init(FILE *a_in, FILE *a_out, FILE *a_err, kb *a_kb, unsigned char a_m);

/* it is important that these functions are used instead of the externally
 * available yyparse() functions */
int program_parse();
int query_parse();

#endif
