/*
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

%{
#include <stdlib.h>
#include <symtab.h>
#include <gnd_atom.h>
#include <gnd_atomlist.h>

#ifdef DEBUG
#include <stdio.h>
#endif

int yylex();
extern int yyerror(char *error);
%}

%union {
  gnd_atom_type gnd_atom;
  unsigned int terminal;
}

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
%token <terminal> EPI_SYM_S_SUB_IDENT
%token <terminal> EPI_SYM_S_ACC_IDENT
%token <terminal> EPI_SYM_S_OBJ_IDENT
%token <terminal> EPI_SYM_G_SUB_IDENT
%token <terminal> EPI_SYM_G_ACC_IDENT
%token <terminal> EPI_SYM_G_OBJ_IDENT
%token <terminal> EPI_SYM_VAR_IDENT
%token <terminal> EPI_SYM_TRANS_IDENT

%type <gnd_atom> ground_subst
%type <gnd_atom> ground_memb

%type <gnd_atom> ground_subject_subst 
%type <gnd_atom> ground_access_subst
%type <gnd_atom> ground_object_subst 
%type <gnd_atom> ground_subject_memb 
%type <gnd_atom> ground_access_memb 
%type <gnd_atom> ground_object_memb 

%start program

%%

program : 
  statement_list
  ;

statement_list : 
  statement 
  | statement statement_list
  ;

statement : 
  initial_stmt 
  | trans_stmt 
  | policy_stmt
  ;

initial_stmt : 
  EPI_SYM_INITIALLY ground_exp EPI_SYM_SEMICOLON
  {
  }
  ;

trans_stmt : 
  EPI_SYM_TRANS EPI_SYM_TRANS_IDENT trans_var_def EPI_SYM_CAUSES comp_exp EPI_SYM_IF comp_exp EPI_SYM_SEMICOLON
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
  EPI_SYM_VAR_IDENT 
  {
  }
  | EPI_SYM_VAR_IDENT EPI_SYM_COMMA trans_var_list
  {
  }
  ;

is_clause : 
  EPI_SYM_IS ground_exp
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
  EPI_SYM_TRANS_IDENT EPI_SYM_OPEN_PARENT sub_acc_obj_list EPI_SYM_CLOSE_PARENT
  {
  }
  ;

sub_acc_obj_list : 
  sub_acc_obj_ident
  {
  }
  | sub_acc_obj_ident EPI_SYM_COMMA sub_acc_obj_list
  {
  }
  ;

sub_acc_obj_ident : 
  ground_sub_ident
  | ground_acc_ident
  | ground_obj_ident
  ;

logical_op :
  EPI_SYM_AND
  {
  }
  ;

comp_exp :
  comp_boolean_exp logical_op comp_exp 
  | comp_boolean_exp
  ;

comp_boolean_exp :
  EPI_SYM_NOT comp_atom
  {
  }
  | comp_atom
  {
  }
  ;

comp_atom :
  comp_holds
  | comp_subst
  | comp_memb
  | logical_const
  ;

comp_holds :
  EPI_SYM_HOLDS EPI_SYM_OPEN_PARENT comp_sub_ident EPI_SYM_COMMA comp_acc_ident EPI_SYM_COMMA comp_obj_ident EPI_SYM_CLOSE_PARENT
  {
  }
  ;

comp_subst :
  EPI_SYM_SUBST EPI_SYM_OPEN_PARENT comp_group_var_ident EPI_SYM_COMMA comp_group_var_ident EPI_SYM_CLOSE_PARENT
  {
  }
  ;

comp_memb :
  EPI_SYM_MEMB EPI_SYM_OPEN_PARENT comp_single_var_ident EPI_SYM_COMMA comp_group_var_ident EPI_SYM_CLOSE_PARENT
  {
  }
  ;

ground_exp : 
  ground_boolean_exp logical_op ground_exp
  {
  }
  | ground_boolean_exp
  {
  }
  ;

ground_boolean_exp :
  EPI_SYM_NOT ground_atom
  {
  }
  | ground_atom
  {
  }
  ;

ground_atom :
  ground_holds
  {
  }
  | ground_subst
  {
  }
  | ground_memb
  {
  }
  | logical_const
  {
  }
  ;

ground_holds :
  EPI_SYM_HOLDS EPI_SYM_OPEN_PARENT ground_sub_ident EPI_SYM_COMMA ground_acc_ident EPI_SYM_COMMA ground_obj_ident EPI_SYM_CLOSE_PARENT
  {
  }
  ;

ground_subst :
  ground_subject_subst
  {
    $$ = $1;
    $$.type = atom_subst;
  }
  | ground_access_subst
  {
    $$ = $1;
    $$.type = atom_subst;
  }
  | ground_object_subst
  {
    $$ = $1;
    $$.type = atom_subst;
  }
  ;

ground_memb :
  ground_subject_memb
  {
    $$ = $1;
    $$.type = atom_memb;
  }
  | ground_access_memb
  {
    $$ = $1;
    $$.type = atom_memb;
  }
  | ground_object_memb
  {
    $$ = $1;
    $$.type = atom_memb;
  }
  ;

ground_subject_subst :
  EPI_SYM_SUBST EPI_SYM_OPEN_PARENT EPI_SYM_G_SUB_IDENT EPI_SYM_COMMA EPI_SYM_G_SUB_IDENT EPI_SYM_CLOSE_PARENT
  {
    $$.atom.subst.group1 = $3;
    $$.atom.subst.group2 = $5;
  }
  ;

ground_access_subst :
  EPI_SYM_SUBST EPI_SYM_OPEN_PARENT EPI_SYM_G_ACC_IDENT EPI_SYM_COMMA EPI_SYM_G_ACC_IDENT EPI_SYM_CLOSE_PARENT
  {
    $$.atom.subst.group1 = $3;
    $$.atom.subst.group2 = $5;
  }
  ;

ground_object_subst :
  EPI_SYM_SUBST EPI_SYM_OPEN_PARENT EPI_SYM_G_OBJ_IDENT EPI_SYM_COMMA EPI_SYM_G_OBJ_IDENT EPI_SYM_CLOSE_PARENT
  {
    $$.atom.subst.group1 = $3;
    $$.atom.subst.group2 = $5;
  }
  ;

ground_subject_memb :
  EPI_SYM_MEMB EPI_SYM_OPEN_PARENT EPI_SYM_S_SUB_IDENT EPI_SYM_COMMA EPI_SYM_G_SUB_IDENT EPI_SYM_CLOSE_PARENT
  {
    $$.atom.memb.member = $3;
    $$.atom.memb.group = $5;
  }
  ;

ground_access_memb :
  EPI_SYM_MEMB EPI_SYM_OPEN_PARENT EPI_SYM_S_ACC_IDENT EPI_SYM_COMMA EPI_SYM_G_ACC_IDENT EPI_SYM_CLOSE_PARENT
  {
    $$.atom.memb.member = $3;
    $$.atom.memb.group = $5;
  }
  ;

ground_object_memb :
  EPI_SYM_MEMB EPI_SYM_OPEN_PARENT EPI_SYM_S_OBJ_IDENT EPI_SYM_COMMA EPI_SYM_G_OBJ_IDENT EPI_SYM_CLOSE_PARENT 
  {
    $$.atom.memb.member = $3;
    $$.atom.memb.group = $5;
  }
  ;

comp_sub_ident :
  ground_sub_ident
  {
  }
  | EPI_SYM_VAR_IDENT
  {
  }
  ;

comp_acc_ident :
  ground_acc_ident
  {
  }
  | EPI_SYM_VAR_IDENT
  {
  }
  ;

comp_obj_ident :
  ground_obj_ident
  {
  }
  | EPI_SYM_VAR_IDENT
  {
  }
  ;

comp_single_var_ident :
  EPI_SYM_S_SUB_IDENT
  {
  }
  | EPI_SYM_S_ACC_IDENT
  {
  }
  | EPI_SYM_S_OBJ_IDENT
  {
  }
  | EPI_SYM_VAR_IDENT
  {
  }
  ;

comp_group_var_ident :
  EPI_SYM_G_SUB_IDENT
  {
  }
  | EPI_SYM_G_ACC_IDENT
  {
  }
  | EPI_SYM_G_OBJ_IDENT
  {
  }
  | EPI_SYM_VAR_IDENT
  {
  }
  ;

ground_sub_ident : 
  EPI_SYM_S_SUB_IDENT
  {
  }
  | EPI_SYM_G_SUB_IDENT
  {
  }
  ;

ground_acc_ident :
  EPI_SYM_S_ACC_IDENT 
  {
  }
  | EPI_SYM_G_ACC_IDENT
  {
  }
  ;

ground_obj_ident : 
  EPI_SYM_S_OBJ_IDENT 
  {
  }
  | EPI_SYM_G_OBJ_IDENT
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
