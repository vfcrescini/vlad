/*
 * parser.y
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

%{
#include <stdlib.h>
/*
#include <symtab.h>
#include <gnd_atom.h>
#include <gnd_atomlist.h>
*/

#ifdef DEBUG
#include <stdio.h>
#endif

int yylex();
extern int yyerror(char *error);
%}

%union {
  char *identifier;
  unsigned int terminal;
}

%token <terminal> EPI_SYM_EOF
%token <terminal> EPI_SYM_ERROR
%token <terminal> EPI_SYM_OPEN_PARENT
%token <terminal> EPI_SYM_CLOSE_PARENT
%token <terminal> EPI_SYM_COMMA
%token <terminal> EPI_SYM_SEMICOLON
%token <terminal> EPI_SYM_AND
%token <terminal> EPI_SYM_NOT
%token <terminal> EPI_SYM_TRUE
%token <terminal> EPI_SYM_FALSE
%token <terminal> EPI_SYM_HOLDS
%token <terminal> EPI_SYM_MEMB
%token <terminal> EPI_SYM_SUBST
%token <terminal> EPI_SYM_INITIALLY
%token <terminal> EPI_SYM_TRANS
%token <terminal> EPI_SYM_CAUSES
%token <terminal> EPI_SYM_IF
%token <terminal> EPI_SYM_IS
%token <terminal> EPI_SYM_AFTER
%token <terminal> EPI_SYM_SUBTYPE
%token <terminal> EPI_SYM_OBJTYPE
%token <terminal> EPI_SYM_ACCTYPE
%token <terminal> EPI_SYM_SUBGRPTYPE
%token <terminal> EPI_SYM_OBJGRPTYPE
%token <terminal> EPI_SYM_ACCGRPTYPE
%token <terminal> EPI_SYM_IDENT
%token <terminal> EPI_SYM_IDENTIFIER

%start program

%%

program : 
  first_part
  | last_part
  |
  ;

first_part :
  ident_stmt_list
  | ident_stmt_list last_part
  ;

last_part :
  other_stmt_list;

ident_stmt_list :
  ident_stmt
  | ident_stmt ident_stmt_list
  ;

other_stmt_list :
  other_stmt
  | other_stmt other_stmt_list
  ;

ident_stmt :
  EPI_SYM_IDENT ident_declaration EPI_SYM_SEMICOLON
  {
  }
  ;

other_stmt :
  initial_stmt 
  | trans_stmt 
  | policy_stmt
  ;

ident_declaration :
  sub_ident_decl
  | obj_ident_decl
  | acc_ident_decl
  | sub_grp_ident_decl
  | obj_grp_ident_decl
  | acc_grp_ident_decl
  ;

sub_ident_decl :
  EPI_SYM_SUBTYPE sub_ident_list
  {
  }
  ;

obj_ident_decl :
  EPI_SYM_OBJTYPE obj_ident_list
  {
  }
  ;

acc_ident_decl :
  EPI_SYM_ACCTYPE acc_ident_list
  {
  }
  ;

sub_grp_ident_decl :
  EPI_SYM_SUBGRPTYPE sub_grp_ident_list
  {
  }
  ;

obj_grp_ident_decl :
  EPI_SYM_OBJGRPTYPE obj_grp_ident_list
  {
  }
  ;

acc_grp_ident_decl :
  EPI_SYM_ACCGRPTYPE acc_grp_ident_list
  {
  }
  ;

sub_ident_list :
  EPI_SYM_IDENTIFIER 
  {
  }
  | EPI_SYM_IDENTIFIER EPI_SYM_COMMA sub_ident_list
  {
  }
  ;

obj_ident_list :
  EPI_SYM_IDENTIFIER 
  {
  }
  | EPI_SYM_IDENTIFIER EPI_SYM_COMMA obj_ident_list
  {
  }
  ;

acc_ident_list :
  EPI_SYM_IDENTIFIER
  {
  }
  | EPI_SYM_IDENTIFIER EPI_SYM_COMMA acc_ident_list
  {
  }
  ;

sub_grp_ident_list :
  EPI_SYM_IDENTIFIER 
  {
  }
  | EPI_SYM_IDENTIFIER EPI_SYM_COMMA sub_grp_ident_list
  {
  }
  ;

obj_grp_ident_list :
  EPI_SYM_IDENTIFIER
  {
  }
  | EPI_SYM_IDENTIFIER EPI_SYM_COMMA obj_grp_ident_list
  {
  }
  ;

acc_grp_ident_list :
  EPI_SYM_IDENTIFIER 
  {
  }
  | EPI_SYM_IDENTIFIER EPI_SYM_COMMA acc_grp_ident_list
  {
  }
  ;

initial_stmt : 
  EPI_SYM_INITIALLY expression EPI_SYM_SEMICOLON
  {
  }
  ;

trans_stmt : 
  EPI_SYM_TRANS EPI_SYM_IDENTIFIER trans_var_def EPI_SYM_CAUSES expression EPI_SYM_IF expression EPI_SYM_SEMICOLON
  {
  }
  ;

policy_stmt : 
  is_clause after_clause EPI_SYM_SEMICOLON 
  | is_clause EPI_SYM_SEMICOLON
  ;

trans_var_def : 
  EPI_SYM_OPEN_PARENT EPI_SYM_CLOSE_PARENT 
  {
  }
  | EPI_SYM_OPEN_PARENT trans_var_list EPI_SYM_CLOSE_PARENT
  {
  }
  ;

trans_var_list : 
  EPI_SYM_IDENTIFIER 
  {
  }
  | EPI_SYM_IDENTIFIER EPI_SYM_COMMA trans_var_list
  {
  }
  ;

is_clause : 
  EPI_SYM_IS expression
  {
  }
  ;

after_clause : 
  EPI_SYM_AFTER trans_ref_list
  {
  }
  ;

trans_ref_list : 
  trans_ref_def
  {
  }
  | trans_ref_def EPI_SYM_COMMA trans_ref_list
  {
  }
  ;

trans_ref_def : 
  EPI_SYM_IDENTIFIER EPI_SYM_OPEN_PARENT trans_ref_ident_list EPI_SYM_CLOSE_PARENT
  {
  }
  ;

trans_ref_ident_list : 
  EPI_SYM_IDENTIFIER
  {
  }
  | EPI_SYM_IDENTIFIER EPI_SYM_COMMA trans_ref_ident_list
  {
  }
  ;

logical_op :
  EPI_SYM_AND
  {
  }
  ;

expression : 
  boolean_exp
  {
  }
  | boolean_exp logical_op expression
  {
  }
  ;

boolean_exp :
  atom_exp
  {
  }
  | EPI_SYM_NOT atom_exp
  {
  }
  ;

atom_exp :
  holds_exp
  {
  }
  | subst_exp
  {
  }
  | memb_expr
  {
  }
  | logical_const
  {
  }
  ;

holds_exp :
  EPI_SYM_HOLDS EPI_SYM_OPEN_PARENT EPI_SYM_IDENTIFIER EPI_SYM_COMMA EPI_SYM_IDENTIFIER EPI_SYM_COMMA EPI_SYM_IDENTIFIER EPI_SYM_CLOSE_PARENT
  {
  }
  ;

subst_exp :
  EPI_SYM_SUBST EPI_SYM_OPEN_PARENT EPI_SYM_IDENTIFIER EPI_SYM_COMMA EPI_SYM_IDENTIFIER EPI_SYM_CLOSE_PARENT
  {
  }
  ;

memb_expr :
  EPI_SYM_MEMB EPI_SYM_OPEN_PARENT EPI_SYM_IDENTIFIER EPI_SYM_COMMA EPI_SYM_IDENTIFIER EPI_SYM_CLOSE_PARENT
  {
  }
  ;

logical_const : 
  EPI_SYM_TRUE 
  {
  }
  | EPI_SYM_FALSE
  {
  }
  ;

%%
