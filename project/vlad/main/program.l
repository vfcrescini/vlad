/*
 * program.l
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
#include "program.tab.h"

unsigned int programlineno = 1;

int programinit(FILE *a_in, FILE *a_out);
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
initially          initially
implied            implied
by                 by
with               with
absence            absence
always             always
causes             causes
if                 if
ident              ident
sub_grp_type       sub-grp
obj_grp_type       obj-grp
acc_grp_type       acc-grp
sub_type           sub
obj_type           obj
acc_type           acc
digit              [0-9]
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

<INITIAL>{initially} return VLAD_SYM_INITIALLY;

<INITIAL>{implied} return VLAD_SYM_IMPLIED;

<INITIAL>{by} return VLAD_SYM_BY;

<INITIAL>{with} return VLAD_SYM_WITH;

<INITIAL>{absence} return VLAD_SYM_ABSENCE;

<INITIAL>{always} return VLAD_SYM_ALWAYS;

<INITIAL>{causes} return VLAD_SYM_CAUSES;

<INITIAL>{if} return VLAD_SYM_IF;

<INITIAL>{ident} return VLAD_SYM_IDENT;

<INITIAL>{sub_grp_type} return VLAD_SYM_SUBGRPTYPE;

<INITIAL>{obj_grp_type} return VLAD_SYM_OBJGRPTYPE;

<INITIAL>{acc_grp_type} return VLAD_SYM_ACCGRPTYPE;

<INITIAL>{sub_type} return VLAD_SYM_SUBTYPE;

<INITIAL>{obj_type} return VLAD_SYM_OBJTYPE;

<INITIAL>{acc_type} return VLAD_SYM_ACCTYPE;

<INITIAL>{identifier} {
  if (programtext == NULL) {
    strcpy(programlval.identifier, "");
    return VLAD_SYM_ERROR;
  }

  strcpy(programlval.identifier, programtext);

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
  programlineno++;
}

<COMMENT>{everything} ;

<INITIAL>{everything} {
  return VLAD_SYM_ERROR;
}

%%

int programinit(FILE *a_in, FILE *a_out)
{
  if (a_in == NULL || a_out == NULL)
    return VLAD_NULLPTR;

  programin = a_in;
  programout = a_out;

  yyrestart(a_in);
  BEGIN INITIAL;

  return VLAD_OK;
}
