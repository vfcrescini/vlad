/*
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

%option nounput
%option always-interactive

%{
#include "y.tab.h"

#ifdef DEBUG
#include <stdio.h>
#endif

extern YYSTYPE yylval;
%}

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
sub_type           sub
obj_type           obj
acc_type           acc
sub_grp_type       sub-grp
obj_grp_type       obj_grp
acc_grp_type       acc_grp
ident              ident
identifier         [a-zA-Z]([a-zA-Z0-9_])*
whitespace         [\t ]+

%%

{open_parent} return EPI_SYM_OPEN_PARENT;
{close_parent} return EPI_SYM_CLOSE_PARENT;
{comma} return EPI_SYM_COMMA;
{semicolon} return EPI_SYM_SEMICOLON;
{and} return EPI_SYM_AND;
{not} return EPI_SYM_NOT;
{true} return EPI_SYM_TRUE;
{false} return EPI_SYM_FALSE;
{holds} return EPI_SYM_HOLDS;
{member} return EPI_SYM_MEMB;
{subset} return EPI_SYM_SUBST;
{initially} return EPI_SYM_INITIALLY;
{transform} return EPI_SYM_TRANS;
{causes} return EPI_SYM_CAUSES;
{if} return EPI_SYM_IF;
{is} return EPI_SYM_IS;
{after} return EPI_SYM_AFTER;
{sub_type} return EPI_SYM_SUBTYPE;
{obj_type} return EPI_SYM_OBJTYPE;
{acc_type} return EPI_SYM_ACCTYPE;
{sub_grp_type} return EPI_SYM_SUBGRPTYPE;
{obj_grp_type} return EPI_SYM_OBJGRPTYPE;
{acc_grp_type} return EPI_SYM_ACCGRPTYPE;
{ident} return EPI_SYM_IDENT;
{identifier} {
#ifdef DEBUG
  printf("identifier: %s\n", yytext);
#endif
 yylval.identifier = yytext;
 return EPI_SYM_IDENTIFIER;
}
{whitespace}

%%
