/*
 * scanner.l
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

%option nounput
%option always-interactive
%s COMMENT

%{
#include <stdlib.h>
#include <string.h>

#include <expression.h>

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
true               true
false              false
holds              holds
member             memb
subset             subst
initially          initially
transform          trans
causes             causes
if                 if
is                 is
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

<INITIAL>{open_parent} return EPI_SYM_OPEN_PARENT;

<INITIAL>{close_parent} return EPI_SYM_CLOSE_PARENT;

<INITIAL>{comma} return EPI_SYM_COMMA;

<INITIAL>{semicolon} return EPI_SYM_SEMICOLON;

<INITIAL>{and} return EPI_SYM_AND;

<INITIAL>{not} return EPI_SYM_NOT;

<INITIAL>{true} return EPI_SYM_TRUE;

<INITIAL>{false} return EPI_SYM_FALSE;

<INITIAL>{holds} return EPI_SYM_HOLDS;

<INITIAL>{member} return EPI_SYM_MEMB;

<INITIAL>{subset} return EPI_SYM_SUBST;

<INITIAL>{initially} return EPI_SYM_INITIALLY;

<INITIAL>{transform} return EPI_SYM_TRANS;

<INITIAL>{causes} return EPI_SYM_CAUSES;

<INITIAL>{if} return EPI_SYM_IF;

<INITIAL>{is} return EPI_SYM_IS;

<INITIAL>{after} return EPI_SYM_AFTER;

<INITIAL>{ident} return EPI_SYM_IDENT;

<INITIAL>{sub_grp_type} return EPI_SYM_SUBGRPTYPE;

<INITIAL>{obj_grp_type} return EPI_SYM_OBJGRPTYPE;

<INITIAL>{acc_grp_type} return EPI_SYM_ACCGRPTYPE;

<INITIAL>{sub_type} return EPI_SYM_SUBTYPE;

<INITIAL>{obj_type} return EPI_SYM_OBJTYPE;

<INITIAL>{acc_type} return EPI_SYM_ACCTYPE;

<INITIAL>{identifier} {
  if (yytext == NULL) {
    strcpy(yylval.identifier, "");
    return EPI_SYM_ERROR;
  }

  strcpy(yylval.identifier, yytext);

  return EPI_SYM_IDENTIFIER;
}

<INITIAL>{begin_comment} {
  BEGIN COMMENT;
}

<INITIAL>{end_comment} {
  return EPI_SYM_ERROR;
}

<COMMENT>{end_comment} {
  BEGIN INITIAL;
}

<COMMENT><<EOF>> {
  return EPI_SYM_ERROR;
}

{whitespace} ;

{newline} {
  line_no++;
}

<COMMENT>{everything} ;

<INITIAL>{everything} {
  return EPI_SYM_ERROR;
}

%%
