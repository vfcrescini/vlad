/*
 * parser.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __VLAD_PARSER_H
#define __VLAD_PARSER_H

/* vars that can be re-assigned */
extern FILE *yyin;
extern FILE *yyout;
extern FILE *yyerr;

/* available function */
extern int yyparse(void);

/* mode flag */
extern unsigned char mode;

#endif
