/*
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

%option noyymore
%option noreject
%option noyywrap
%option always-interactive

%{
#include "y.tab.h"
#include "symtab.h"

#ifdef YY_FLEX_LEX_COMPAT
#undef YY_USES_REJECT
#undef YY_NEED_STRLEN
#endif

#define YY_NO_UNPUT

extern int yylval;
%}

underscore         _
open_parent        \(
close_parent       \)
comma              ,
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
lower_case         [a-z]
upper_case         [A-Z]
digit              [0-9]
alpha              {lower_case}|{upper_case}
alpha_num          {alpha}|{digit}
alpha_num_uscr     {alpha_num}|{underscore}
alpha_num_uscr_str ({alpha_num_uscr})+ 
s_subject_ident    s|s{alpha_num_uscr_str}
s_access_ident     a|a{alpha_num_uscr_str}
s_object_ident     o|o{alpha_num_uscr_str}
g_subject_ident    S|S{alpha_num_uscr_str}
g_access_ident     A|A{alpha_num_uscr_str}
g_object_ident     O|O{alpha_num_uscr_str}
v_ident            v|V|v{alpha_num_uscr_str}|V{alpha_num_uscr_str}
t_ident            t|t{alpha_num_uscr_str}|T{alpha_num_uscr_str}
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
{s_subject_ident} {
  symtab_add(yytext, &yylval);
  return EPI_SYM_S_SUB_IDENT;
}
{s_access_ident} {
  symtab_add(yytext, &yylval);
  return EPI_SYM_S_ACC_IDENT;
}
{s_object_ident} {
  symtab_add(yytext, &yylval);
  return  EPI_SYM_S_OBJ_IDENT;
}
{g_subject_ident} {
  symtab_add(yytext, &yylval);
  return EPI_SYM_G_SUB_IDENT;
}
{g_access_ident} {
  symtab_add(yytext, &yylval);
  return EPI_SYM_G_ACC_IDENT;
}
{g_object_ident} {
  symtab_add(yytext, &yylval);
  return EPI_SYM_G_OBJ_IDENT;
}
{v_ident} {
  symtab_add(yytext, &yylval);
  return EPI_SYM_VAR_IDENT;
}
{t_ident} {
  symtab_add(yytext, &yylval);
  return EPI_SYM_TRANS_IDENT;
}
{whitespace}

%%
