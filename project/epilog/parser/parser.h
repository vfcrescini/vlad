/*
 * parser.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __EPI_PARSER_H
#define __EPI_PARSER_H

/* vars that can be re-assigned */
extern FILE *yyin;
extern FILE *yyout;
extern FILE *yyerr;

/* vars that can be referenced */
extern unsigned int line_no;

/* available function */
extern int yyparse(void);

/* function to be called with error */
int yyerror(char *error);
int yywarn(char *error);

#endif
