/*
 * parser.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

/* vars that can be re-assigned */
extern FILE *yyin;
extern FILE *yyout;

/* available function */
extern int yyparse();

/* function to be called with error */
int yyerror(char *error);
