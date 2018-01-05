/*
 * operation.l
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

%option nounput
%option noyywrap
%option always-interactive
%s COMMENT

%{
#include <cstdlib>
#include <cstring>

#include <config.h>
#include <vlad/vlad.h>
#include <vlad/kb.h>
#include "operation.tab.h"

unsigned int operationlineno = 1;

int operationinit(FILE *a_in, FILE *a_out);
%}

begin_comment      \/\*
end_comment        \*\/
underscore         _
open_parent        \(
close_parent       \)
comma              \,
semicolon          ;
and                &&
not                !
holds              holds
member             memb
subset             subst
query              query
compute            compute
sequence           seq
add                add
delete             del
list               list
digit              [0-9]
number             ({digit})+
alpha              [a-zA-Z]
alphanum           {alpha}|{digit}|{underscore}
identifier         {alpha}({alphanum}){0,127}
whitespace         ([\t ])+
newline            \n
everything         .

%%

<INITIAL>{open_parent} return VLAD_SYM_OPEN_PARENT;

<INITIAL>{close_parent} return VLAD_SYM_CLOSE_PARENT;

<INITIAL>{comma} return VLAD_SYM_COMMA;

<INITIAL>{semicolon} return VLAD_SYM_SEMICOLON;

<INITIAL>{and} return VLAD_SYM_AND;

<INITIAL>{not} return VLAD_SYM_NOT;

<INITIAL>{holds} return VLAD_SYM_HOLDS;

<INITIAL>{member} return VLAD_SYM_MEMB;

<INITIAL>{subset} return VLAD_SYM_SUBST;

<INITIAL>{query} return VLAD_SYM_QUERY;

<INITIAL>{compute} return VLAD_SYM_COMPUTE;

<INITIAL>{sequence} return VLAD_SYM_SEQUENCE;

<INITIAL>{add} return VLAD_SYM_ADD;

<INITIAL>{delete} return VLAD_SYM_DELETE;

<INITIAL>{list} return VLAD_SYM_LIST;

<INITIAL>{number} {
  if (operationtext == NULL) {
    operationlval.number = 0;
    return VLAD_SYM_ERROR;
  }

  operationlval.number = atoi(operationtext);

  return VLAD_SYM_NUMBER;
}

<INITIAL>{identifier} {
  if (operationtext == NULL) {
    strcpy(operationlval.identifier, "");
    return VLAD_SYM_ERROR;
  }

  strcpy(operationlval.identifier, operationtext);

  return VLAD_SYM_IDENTIFIER;
}

<INITIAL>{begin_comment} {
  BEGIN COMMENT;
}

<INITIAL>{end_comment} {
  return VLAD_SYM_ERROR;
}

<COMMENT>{end_comment} {
  BEGIN INITIAL;
}

<COMMENT><<EOF>> {
  return VLAD_SYM_ERROR;
}

{whitespace} ;

{newline} {
  operationlineno++;
}

<COMMENT>{everything} ;

<INITIAL>{everything} {
  return VLAD_SYM_ERROR;
}

%%

int operationinit(FILE *a_in, FILE *a_out)
{
  if (a_in == NULL || a_out == NULL)
    return VLAD_NULLPTR;

  operationin = a_in;
  operationout = a_out;

  yyrestart(a_in);
  BEGIN INITIAL;

  return VLAD_OK;
}
