/*
 * parser.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_PARSER_H
#define __EPI_PARSER_H

#include <expression.h>

/* vars that can be re-assigned */
extern FILE *yyin;
extern FILE *yyout;

/* vars that can be referenced */
extern unsigned int line_no;

/* vars that must be defined in the main func */
expression_type initial_exp;

/* available function */
extern int yyparse();

/* function to be called with error */
int yyerror(char *error);
int yywarn(char *error);

#endif
