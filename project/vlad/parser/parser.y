/*
 * parser.y
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

%{
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern int yyerror(char *error);
extern int yywarn(char *warning);

int yylex(void);

extern FILE *yyout;
FILE *yyerr;
%}

%union {
  char identifier[128];
  unsigned int terminal;
}

%token <terminal> VLAD_SYM_EOF
%token <terminal> VLAD_SYM_ERROR
%token <terminal> VLAD_SYM_OPEN_PARENT
%token <terminal> VLAD_SYM_CLOSE_PARENT
%token <terminal> VLAD_SYM_COMMA
%token <terminal> VLAD_SYM_SEMICOLON
%token <terminal> VLAD_SYM_AND
%token <terminal> VLAD_SYM_NOT
%token <terminal> VLAD_SYM_TRUE
%token <terminal> VLAD_SYM_FALSE
%token <terminal> VLAD_SYM_HOLDS
%token <terminal> VLAD_SYM_MEMB
%token <terminal> VLAD_SYM_SUBST
%token <terminal> VLAD_SYM_INITIALLY
%token <terminal> VLAD_SYM_IMPLIES
%token <terminal> VLAD_SYM_WITH
%token <terminal> VLAD_SYM_ABSENCE
%token <terminal> VLAD_SYM_ALWAYS
%token <terminal> VLAD_SYM_TRANS
%token <terminal> VLAD_SYM_CAUSES
%token <terminal> VLAD_SYM_IF
%token <terminal> VLAD_SYM_IS
%token <terminal> VLAD_SYM_AFTER
%token <terminal> VLAD_SYM_SUBTYPE
%token <terminal> VLAD_SYM_OBJTYPE
%token <terminal> VLAD_SYM_ACCTYPE
%token <terminal> VLAD_SYM_SUBGRPTYPE
%token <terminal> VLAD_SYM_OBJGRPTYPE
%token <terminal> VLAD_SYM_ACCGRPTYPE
%token <terminal> VLAD_SYM_IDENT
%token <identifier> VLAD_SYM_IDENTIFIER

%start program

%%

program :
  init body destroy
  ;

init :
  {
  }
  ;

destroy :
  {
  }
  ;

body :
  ident_section initial_section constraint_section trans_section query_section
  ;

ident_section : 
  | ident_stmt_list
  ;

initial_section :
  | initial_stmt_list
  ;

constraint_section :
  | constraint_stmt_list
  ;

trans_section :
  | trans_stmt_list
  ;

query_section :
  | query_stmt_list
  ;

ident_stmt_list :
  ident_stmt
  | ident_stmt_list ident_stmt
  ;

initial_stmt_list :
  initial_stmt
  | initial_stmt_list initial_stmt
  ;

constraint_stmt_list :
  constraint_stmt
  | constraint_stmt_list constraint_stmt
  ;

trans_stmt_list :
  trans_stmt
  | trans_stmt_list trans_stmt
  ;

query_stmt_list :
  query_stmt
  | query_stmt_list query_stmt
  ;

ident_stmt :
  VLAD_SYM_IDENT ident_declaration VLAD_SYM_SEMICOLON {
  }
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
  VLAD_SYM_SUBTYPE sub_ident_list {
  }
  ;

obj_ident_decl :
  VLAD_SYM_OBJTYPE obj_ident_list {
  }
  ;

acc_ident_decl :
  VLAD_SYM_ACCTYPE acc_ident_list {
  }
  ;

sub_grp_ident_decl :
  VLAD_SYM_SUBGRPTYPE sub_grp_ident_list {
  }
  ;

obj_grp_ident_decl :
  VLAD_SYM_OBJGRPTYPE obj_grp_ident_list {
  }
  ;

acc_grp_ident_decl :
  VLAD_SYM_ACCGRPTYPE acc_grp_ident_list {
  }
  ;

sub_ident_list :
  VLAD_SYM_IDENTIFIER {
  }
  | sub_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
  }
  ;

obj_ident_list :
  VLAD_SYM_IDENTIFIER {
  }
  | obj_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
  }
  ;

acc_ident_list :
  VLAD_SYM_IDENTIFIER {
  }
  | acc_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
  }
  ;

sub_grp_ident_list :
  VLAD_SYM_IDENTIFIER {
  }
  | sub_grp_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
  }
  ;

obj_grp_ident_list :
  VLAD_SYM_IDENTIFIER {
  }
  | obj_grp_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
  }
  ;

acc_grp_ident_list :
  VLAD_SYM_IDENTIFIER {
  }
  | acc_grp_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
  }
  ;

initial_stmt : 
  VLAD_SYM_INITIALLY ground_exp VLAD_SYM_SEMICOLON {
  }
  ;

constraint_stmt :
  implies_stmt
  | always_stmt
  ;

implies_stmt :
  ground_exp VLAD_SYM_IMPLIES ground_exp with_clause VLAD_SYM_SEMICOLON {
  }
  ;

with_clause :
  | VLAD_SYM_WITH VLAD_SYM_ABSENCE ground_exp {
  }
  ;

always_stmt :
  VLAD_SYM_ALWAYS ground_exp VLAD_SYM_SEMICOLON {
  }
  ;

trans_stmt : 
  VLAD_SYM_TRANS VLAD_SYM_IDENTIFIER trans_var_def VLAD_SYM_CAUSES comp_exp VLAD_SYM_IF comp_exp VLAD_SYM_SEMICOLON {
  }
  ;

query_stmt : 
  is_clause after_clause VLAD_SYM_SEMICOLON {
  }
  | is_clause VLAD_SYM_SEMICOLON {
  }
  ;

trans_var_def : 
  VLAD_SYM_OPEN_PARENT VLAD_SYM_CLOSE_PARENT {
  }
  | VLAD_SYM_OPEN_PARENT trans_var_list VLAD_SYM_CLOSE_PARENT {
  }
  ;

trans_var_list : 
  VLAD_SYM_IDENTIFIER {
  }
  | trans_var_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
  }
  ;

is_clause : 
  VLAD_SYM_IS ground_exp {
  }
  ;

after_clause : 
  VLAD_SYM_AFTER trans_ref_list {
  }
  ;

trans_ref_list : 
  trans_ref_def {
  }
  | trans_ref_list VLAD_SYM_COMMA trans_ref_def {
  }
  ;

trans_ref_def : 
  VLAD_SYM_IDENTIFIER VLAD_SYM_OPEN_PARENT trans_ref_ident_list VLAD_SYM_CLOSE_PARENT {
  }
  ;

trans_ref_ident_list : 
  VLAD_SYM_IDENTIFIER {
  }
  | trans_ref_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
  }
  ;

logical_op :
  VLAD_SYM_AND {
  }
  ;

ground_exp : 
  ground_boolean_atom { 
  }
  | ground_exp logical_op ground_boolean_atom {
  }
  ;

ground_boolean_atom :
  ground_atom_exp {
  }
  | VLAD_SYM_NOT ground_atom_exp {
  }
  ;

ground_atom_exp :
  ground_holds_atom {
  }
  | ground_subst_atom {
  }
  | ground_memb_atom {
  }
  | logical_atom {
  }
  ;

ground_holds_atom :
  VLAD_SYM_HOLDS VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
  }
  ;

ground_subst_atom :
  VLAD_SYM_SUBST VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
  }
  ;
ground_memb_atom :
  VLAD_SYM_MEMB VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
  }
  ;

comp_exp :
  comp_boolean_atom {
  }
  | comp_exp logical_op comp_boolean_atom {
  }
  ;

comp_boolean_atom :
  comp_atom_exp {
  }
  | VLAD_SYM_NOT comp_atom_exp {
  }
  ;

comp_atom_exp :
  comp_holds_atom {
  }
  | comp_subst_atom {
  }
  | comp_memb_atom {
  }
  | logical_atom {
  }
  ;

comp_holds_atom :
  VLAD_SYM_HOLDS VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
  }
  ;

comp_subst_atom :
  VLAD_SYM_SUBST VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
  }
  ;

comp_memb_atom :
  VLAD_SYM_MEMB VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
  }
  ;

logical_atom : 
  VLAD_SYM_TRUE {
  }
  | VLAD_SYM_FALSE {
  }
  ;

%%
