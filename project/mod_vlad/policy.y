/*
 * policy.y
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int policyerror(char *errmsg);
int policylex();

#ifdef YYBYACC
int policyparse();
#endif
%}

%union {
  unsigned int tok;
  char ident[128];
}

%token <tok> MODVLAD_SYM_EOF
%token <tok> MODVLAD_SYM_ERROR
%token <tok> MODVLAD_SYM_OPEN_PARENT
%token <tok> MODVLAD_SYM_CLOSE_PARENT
%token <tok> MODVLAD_SYM_COMMA
%token <tok> MODVLAD_SYM_SEMICOLON
%token <tok> MODVLAD_SYM_SLASH
%token <tok> MODVLAD_SYM_AND
%token <tok> MODVLAD_SYM_NOT
%token <tok> MODVLAD_SYM_HOLDS
%token <tok> MODVLAD_SYM_MEMB
%token <tok> MODVLAD_SYM_SUBST
%token <tok> MODVLAD_SYM_INITIALLY
%token <tok> MODVLAD_SYM_IMPLIED
%token <tok> MODVLAD_SYM_BY
%token <tok> MODVLAD_SYM_WITH
%token <tok> MODVLAD_SYM_ABSENCE
%token <tok> MODVLAD_SYM_ALWAYS
%token <tok> MODVLAD_SYM_CAUSES
%token <tok> MODVLAD_SYM_IF
%token <tok> MODVLAD_SYM_AFTER
%token <tok> MODVLAD_SYM_GROUP
%token <tok> MODVLAD_SYM_SUB
%token <tok> MODVLAD_SYM_ACC
%token <tok> MODVLAD_SYM_OBJ
%token <ident> MODVLAD_SYM_IDENTIFIER

%start program

%%

program :
  init body destroy {
  }
  ;

init : {
  }
  ;

body :
  group_section initial_section constraint_section trans_section {
  }
  ;

destroy : {
  }
  ;

group_section : {
  }
  | group_stmt_list {
  }
  ;

initial_section : {
  }
  | initial_stmt_list {
  }
  ;

constraint_section : {
  }
  | constraint_stmt_list {
  }
  ;

trans_section : {
  }
  | trans_stmt_list {
  }
  ;

group_stmt_list :
  group_stmt {
  }
  | group_stmt_list group_stmt {
  }
  ;

initial_stmt_list :
  initial_stmt {
  }
  | initial_stmt_list initial_stmt {
  }
  ;

constraint_stmt_list :
  constraint_stmt {
  }
  | constraint_stmt_list constraint_stmt {
  }
  ;

trans_stmt_list :
  trans_stmt {
  }
  | trans_stmt_list trans_stmt {
  }
  ;

group_stmt :
  MODVLAD_SYM_GROUP group_def_stmt MODVLAD_SYM_SEMICOLON {
  }
  ;

initial_stmt :
  MODVLAD_SYM_INITIALLY expression MODVLAD_SYM_SEMICOLON {
  }
  ;

constraint_stmt :
  MODVLAD_SYM_ALWAYS expression MODVLAD_SYM_IMPLIED MODVLAD_SYM_BY expression
  MODVLAD_SYM_WITH MODVLAD_SYM_ABSENCE expression MODVLAD_SYM_SEMICOLON {
  }
  ;

trans_stmt :
  MODVLAD_SYM_IDENTIFIER trans_var_def MODVLAD_SYM_CAUSES expression
  MODVLAD_SYM_IF expression MODVLAD_SYM_SEMICOLON {
  }
  ;

group_def_stmt :
  MODVLAD_SYM_SUB sub_ident_list {
  }
  | MODVLAD_SYM_ACC acc_ident_list {
  }
  | MODVLAD_SYM_OBJ obj_ident_list {
  }
  ;

trans_var_def :
  MODVLAD_SYM_OPEN_PARENT MODVLAD_SYM_CLOSE_PARENT {
  }
  |
  MODVLAD_SYM_OPEN_PARENT trans_var_list MODVLAD_SYM_CLOSE_PARENT {
  }
  ;

expression :
  boolean_atom {
  }
  | expression logical_op boolean_atom {
  }
  ;

sub_ident_list :
  MODVLAD_SYM_IDENTIFIER {
  }
  | sub_ident_list MODVLAD_SYM_COMMA MODVLAD_SYM_IDENTIFIER {
  }
  ;

acc_ident_list :
  MODVLAD_SYM_IDENTIFIER {
  }
  | acc_ident_list MODVLAD_SYM_COMMA MODVLAD_SYM_IDENTIFIER {
  }
  ;

obj_ident_list :
  MODVLAD_SYM_IDENTIFIER {
  }
  | obj_ident_list MODVLAD_SYM_COMMA MODVLAD_SYM_IDENTIFIER {
  }
  ;

trans_var_list :
  MODVLAD_SYM_IDENTIFIER {
  }
  | trans_var_list MODVLAD_SYM_COMMA MODVLAD_SYM_IDENTIFIER {
  }
  ;

logical_op :
  MODVLAD_SYM_AND {
  }
  ;

boolean_atom :
  atom {
  }
  | MODVLAD_SYM_NOT atom {
  }
  ;

atom :
  holds_atom {
  }
  | memb_atom {
  }
  | subst_atom {
  }
  ;

holds_atom :
  MODVLAD_SYM_HOLDS MODVLAD_SYM_OPEN_PARENT MODVLAD_SYM_IDENTIFIER
  MODVLAD_SYM_COMMA MODVLAD_SYM_IDENTIFIER MODVLAD_SYM_COMMA
  MODVLAD_SYM_IDENTIFIER MODVLAD_SYM_CLOSE_PARENT {
  }
  ;

memb_atom :
  MODVLAD_SYM_MEMB MODVLAD_SYM_OPEN_PARENT MODVLAD_SYM_IDENTIFIER
  MODVLAD_SYM_COMMA MODVLAD_SYM_IDENTIFIER MODVLAD_SYM_CLOSE_PARENT {
  }
  ;

subst_atom :
  MODVLAD_SYM_SUBST MODVLAD_SYM_OPEN_PARENT MODVLAD_SYM_IDENTIFIER
  MODVLAD_SYM_COMMA MODVLAD_SYM_IDENTIFIER MODVLAD_SYM_CLOSE_PARENT {
  }
  ;

%%

int policyerror(char *a_errmsg)
{
  fprintf(stderr, "ERROR: %s\n", a_errmsg);
  return 0;
}
