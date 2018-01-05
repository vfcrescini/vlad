/*
 * scanner.l
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

%option nounput
%option always-interactive
%s COMMENT

%{
#include <cstdlib>
#include <cstring>

#include <config.h>
#include <vlad/vlad.h>
#include <vlad/kb.h>
#include "y.tab.h"

extern YYSTYPE yylval;
unsigned int line_no = 1;
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
implies            implies
with               with
absence            absence
always             always
causes             causes
if                 if
query              query
after              after
ident              ident
sub_grp_type       sub-grp
obj_grp_type       obj-grp
acc_grp_type       acc-grp
sub_type           sub
obj_type           obj
acc_type           acc
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

<INITIAL>{initially} return VLAD_SYM_INITIALLY;

<INITIAL>{implies} return VLAD_SYM_IMPLIES;

<INITIAL>{with} return VLAD_SYM_WITH;

<INITIAL>{absence} return VLAD_SYM_ABSENCE;

<INITIAL>{always} return VLAD_SYM_ALWAYS;

<INITIAL>{causes} return VLAD_SYM_CAUSES;

<INITIAL>{if} return VLAD_SYM_IF;

<INITIAL>{query} return VLAD_SYM_QUERY;

<INITIAL>{after} return VLAD_SYM_AFTER;

<INITIAL>{ident} return VLAD_SYM_IDENT;

<INITIAL>{sub_grp_type} return VLAD_SYM_SUBGRPTYPE;

<INITIAL>{obj_grp_type} return VLAD_SYM_OBJGRPTYPE;

<INITIAL>{acc_grp_type} return VLAD_SYM_ACCGRPTYPE;

<INITIAL>{sub_type} return VLAD_SYM_SUBTYPE;

<INITIAL>{obj_type} return VLAD_SYM_OBJTYPE;

<INITIAL>{acc_type} return VLAD_SYM_ACCTYPE;

<INITIAL>{identifier} {
  if (yytext == NULL) {
    strcpy(yylval.identifier, "");
    return VLAD_SYM_ERROR;
  }

  strcpy(yylval.identifier, yytext);

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
  line_no++;
}

<COMMENT>{everything} ;

<INITIAL>{everything} {
  return VLAD_SYM_ERROR;
}

%%
