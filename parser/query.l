/*
 * query.l
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
#include "query.tab.h"

unsigned int querylineno = 1;

int queryinit(FILE *a_in, FILE *a_out);
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
after              after
identifier         [a-zA-Z]([a-zA-Z0-9_]){0,127}
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

<INITIAL>{after} return VLAD_SYM_AFTER;

<INITIAL>{identifier} {
  if (querytext == NULL) {
    strcpy(querylval.identifier, "");
    return VLAD_SYM_ERROR;
  }

  strcpy(querylval.identifier, querytext);

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
  querylineno++;
}

<COMMENT>{everything} ;

<INITIAL>{everything} {
  return VLAD_SYM_ERROR;
}

%%

int queryinit(FILE *a_in, FILE *a_out)
{
  if (a_in == NULL || a_out == NULL)
    return VLAD_NULLPTR;

  queryin = a_in;
  queryout = a_out;

  yyrestart(a_in);
  BEGIN INITIAL;

  return VLAD_OK;
}
