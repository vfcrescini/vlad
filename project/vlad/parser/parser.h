/*
 * parser.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_PARSER_H
#define __VLAD_PARSER_H

#include <cstdio>
#include <vlad/kb.h>

/* variables to be redefined */
extern FILE *programin;
extern FILE *programout;
extern FILE *queryin;
extern FILE *queryout;

/* variables to be defined */ 
kb *kbase;
unsigned char mode;

/* available functions */
int programparse();
int queryparse();

#endif
