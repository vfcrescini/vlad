/*
 * parser.y
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

%{
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <new>

#include <config.h>
#include <vlad.h>
#include <kb.h>

extern FILE *yyout;
FILE *yyerr;

kb kbase;

extern int yyerror(char *error);
extern int yywarn(char *warning);

int yylex(void);

%}

%union {
  char identifier[128];
  unsigned int terminal;
  unsigned int gnd_atom;
  numberlist *gnd_exp;
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
%type <gnd_atom> ground_atom 
%type <gnd_atom> ground_boolean_atom 
%type <gnd_atom> ground_holds_atom 
%type <gnd_atom> ground_subst_atom 
%type <gnd_atom> ground_memb_atom 
%type <gnd_atom> logical_atom 
%type <gnd_exp> ground_exp

%start program

%%

program :
  init body destroy
  ;

init : {
    int retval;

    if ((retval = kbase.init()) != VLAD_OK) {
      fprintf(stderr, "failed to initialise knowledge base: %d\n", retval);
      return retval;
    }
  }
  ;

destroy :
  {
  }
  ;

body :
  ident_section initial_section constraint_section trans_section query_section
  ;

ident_section : {
    int retval;
    /* after the ident section, we must close the symbol table */
    if ((retval = kbase.close()) != VLAD_OK) {
      fprintf(stderr, "internal error: %d\n", retval);
      return retval;
    }
  }
  | ident_stmt_list {
    int retval;
    /* after the ident section, we must close the symbol table */
    if ((retval = kbase.close()) != VLAD_OK) {
      fprintf(stderr, "internal error: %d\n", retval);
      return retval;
    }
  }
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
    switch (kbase.add_ident($1, VLAD_IDENT_SUBJECT)) {
    case VLAD_OK :
#ifdef DEBUG
      fprintf(yyerr, "declared subject identifier %s\n", $1);
#endif
      break;
    case VLAD_DUPLICATE :
      fprintf(yyerr, "identifier %s already declared\n", $1);
      return VLAD_DUPLICATE;
    case VLAD_MALLOCFAILED :
      fprintf(yyerr, "memory overflow\n");
      return VLAD_MALLOCFAILED;
    default :
      fprintf(yyerr, "internal error: %d\n", VLAD_FAILURE);
      return VLAD_FAILURE;
    }
  }
  | sub_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    switch (kbase.add_ident($3, VLAD_IDENT_SUBJECT)) {
    case VLAD_OK :
#ifdef DEBUG
      fprintf(yyerr, "declared subject identifier %s\n", $3);
#endif
      break;
    case VLAD_DUPLICATE :
      fprintf(yyerr, "identifier %s already declared\n", $3);
      return VLAD_DUPLICATE;
    case VLAD_MALLOCFAILED :
      fprintf(yyerr, "memory overflow\n");
      return VLAD_MALLOCFAILED;
    default :
      fprintf(yyerr, "internal error: %d\n", VLAD_FAILURE);
      return VLAD_FAILURE;
    }
  }
  ;

obj_ident_list :
  VLAD_SYM_IDENTIFIER {
    switch (kbase.add_ident($1, VLAD_IDENT_OBJECT)) {
    case VLAD_OK :
#ifdef DEBUG
      fprintf(yyerr, "declared object identifier %s\n", $1);
#endif
      break;
    case VLAD_DUPLICATE :
      fprintf(yyerr, "identifier %s already declared\n", $1);
      return VLAD_DUPLICATE;
    case VLAD_MALLOCFAILED :
      fprintf(yyerr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    default :
      fprintf(yyerr, "internal error: %d\n", VLAD_FAILURE);
      return VLAD_FAILURE;
    }
  }
  | obj_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    switch (kbase.add_ident($3, VLAD_IDENT_OBJECT)) {
    case VLAD_OK :
#ifdef DEBUG
      fprintf(yyerr, "declared object identifier %s\n", $3);
#endif
      break;
    case VLAD_DUPLICATE :
      fprintf(yyerr, "identifier %s already declared\n", $3);
      return VLAD_DUPLICATE;
    case VLAD_MALLOCFAILED :
      fprintf(yyerr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    default :
      fprintf(yyerr, "internal error: %d\n", VLAD_FAILURE);
      return VLAD_FAILURE;
    }
  }
  ;

acc_ident_list :
  VLAD_SYM_IDENTIFIER {
    switch (kbase.add_ident($1, VLAD_IDENT_ACCESS)) {
    case VLAD_OK :
#ifdef DEBUG
      fprintf(yyerr, "declared access identifier %s\n", $1);
#endif
      break;
    case VLAD_DUPLICATE :
      fprintf(yyerr, "identifier %s already declared\n", $1);
      return VLAD_DUPLICATE;
    case VLAD_MALLOCFAILED :
      fprintf(yyerr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    default :
      fprintf(yyerr, "internal error: %d\n", VLAD_FAILURE);
      return VLAD_FAILURE;
    }
  }
  | acc_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    switch (kbase.add_ident($3, VLAD_IDENT_ACCESS)) {
    case VLAD_OK :
#ifdef DEBUG
      fprintf(yyerr, "declared access identifier %s\n", $3);
#endif
      break;
    case VLAD_DUPLICATE :
      fprintf(yyerr, "identifier %s already declared\n", $3);
      return VLAD_DUPLICATE;
    case VLAD_MALLOCFAILED :
      fprintf(yyerr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    default :
      fprintf(yyerr, "internal error: %d\n", VLAD_FAILURE);
      return VLAD_FAILURE;
    }
  }
  ;

sub_grp_ident_list :
  VLAD_SYM_IDENTIFIER {
    switch (kbase.add_ident($1, VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP)) {
    case VLAD_OK :
#ifdef DEBUG
      fprintf(yyerr, "declared subject-group identifier %s\n", $1);
#endif
      break;
    case VLAD_DUPLICATE :
      fprintf(yyerr, "identifier %s already declared\n", $1);
      return VLAD_DUPLICATE;
    case VLAD_MALLOCFAILED :
      fprintf(yyerr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    default :
      fprintf(yyerr, "internal error: %d\n", VLAD_FAILURE);
      return VLAD_FAILURE;
    }
  }
  | sub_grp_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
  switch (kbase.add_ident($3, VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP)) {
    case VLAD_OK :
#ifdef DEBUG
      fprintf(yyerr, "declared subject-group identifier %s\n", $3);
#endif
      break;
    case VLAD_DUPLICATE :
      fprintf(yyerr, "identifier %s already declared\n", $3);
      return VLAD_DUPLICATE;
    case VLAD_MALLOCFAILED :
      fprintf(yyerr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    default :
      fprintf(yyerr, "internal error: %d\n", VLAD_FAILURE);
      return VLAD_FAILURE;
    }
  }
  ;

obj_grp_ident_list :
  VLAD_SYM_IDENTIFIER {
  switch (kbase.add_ident($1, VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP)) {
    case VLAD_OK :
#ifdef DEBUG
      fprintf(yyerr, "declared object-group identifier %s\n", $1);
#endif
      break;
    case VLAD_DUPLICATE :
      fprintf(yyerr, "identifier %s already declared\n", $1);
      return VLAD_DUPLICATE;
    case VLAD_MALLOCFAILED :
      fprintf(yyerr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    default :
      fprintf(yyerr, "internal error: %d\n", VLAD_FAILURE);
      return VLAD_FAILURE;
    }
  }
  | obj_grp_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
  switch (kbase.add_ident($3, VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP)) {
    case VLAD_OK :
#ifdef DEBUG
      fprintf(yyerr, "declared object-group identifier %s\n", $3);
#endif
      break;
    case VLAD_DUPLICATE :
      fprintf(yyerr, "identifier %s already declared\n", $3);
      return VLAD_DUPLICATE;
    case VLAD_MALLOCFAILED :
      fprintf(yyerr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    default :
      fprintf(yyerr, "internal error: %d\n", VLAD_FAILURE);
      return VLAD_FAILURE;
    }
  }
  ;

acc_grp_ident_list :
  VLAD_SYM_IDENTIFIER {
  switch (kbase.add_ident($1, VLAD_IDENT_ACCESS | VLAD_IDENT_GROUP)) {
    case VLAD_OK :
#ifdef DEBUG
      fprintf(yyerr, "declared access-group identifier %s\n", $1);
#endif
      break;
    case VLAD_DUPLICATE :
      fprintf(yyerr, "identifier %s already declared\n", $1);
      return VLAD_DUPLICATE;
    case VLAD_MALLOCFAILED :
      fprintf(yyerr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    default :
      fprintf(yyerr, "internal error: %d\n", VLAD_FAILURE);
      return VLAD_FAILURE;
    }
  }
  | acc_grp_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
  switch (kbase.add_ident($3, VLAD_IDENT_ACCESS | VLAD_IDENT_GROUP)) {
    case VLAD_OK :
#ifdef DEBUG
      fprintf(yyerr, "declared access-group identifier %s\n", $3);
#endif
      break;
    case VLAD_DUPLICATE :
      fprintf(yyerr, "identifier %s already declared\n", $3);
      return VLAD_DUPLICATE;
    case VLAD_MALLOCFAILED :
      fprintf(yyerr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    default :
      fprintf(yyerr, "internal error: %d\n", VLAD_FAILURE);
      return VLAD_FAILURE;
    }
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
  VLAD_SYM_IDENTIFIER trans_var_def VLAD_SYM_CAUSES comp_exp if_clause VLAD_SYM_SEMICOLON {
  }
  ;

if_clause :
  | VLAD_SYM_IF comp_exp {
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

query_stmt : 
  VLAD_SYM_IS ground_exp after_clause VLAD_SYM_SEMICOLON {
  }
  ;

after_clause : 
  | VLAD_SYM_AFTER trans_ref_list {
  }
  ;

trans_ref_list : 
  trans_ref_def {
  }
  | trans_ref_list VLAD_SYM_COMMA trans_ref_def {
  }
  ;

trans_ref_def : 
  VLAD_SYM_IDENTIFIER VLAD_SYM_OPEN_PARENT trans_ref_ident_args VLAD_SYM_CLOSE_PARENT {
  }
  ;

trans_ref_ident_args :
  | trans_ref_ident_list {
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
    int retval;
    if (($$ = VLAD_NEW(numberlist("ground exp"))) == NULL) {
      fprintf(stderr, "memory overflow\n");
      return VLAD_MALLOCFAILED;
    }
    if ((retval = $$->add($1)) != VLAD_OK)
      return retval;
  }
  | ground_exp logical_op ground_boolean_atom {
    int retval;
    if ((retval = $$->add($3)) != VLAD_OK)
      return retval; 
  }
  ;

ground_boolean_atom :
  ground_atom {
#ifdef DEBUG
    char *n1;
    char *n2;
    char *n3;
    unsigned char ty;
    bool tr;
    int retval;
#endif

    $$ = $1;

#ifdef DEBUG
    if ((retval = kbase.decode_atom(&n1, &n2, &n3, &ty, &tr, $1)) != VLAD_OK) {
      fprintf(stderr, "internal error: %d", retval);
      return retval;
    }

    switch(ty) {
      case VLAD_ATOM_CONST :
        printf("%5d %s\n", $1, n1);
        break;
      case VLAD_ATOM_HOLDS :
        printf("%5d holds(%s,%s,%s)\n", $1, n1, n2, n3);
        break;
      case VLAD_ATOM_MEMBER :
        printf("%5d memb(%s,%s)\n", $1, n1, n2);
        break;
      case VLAD_ATOM_SUBSET :
        printf("%5d subst(%s,%s)\n", $1, n1, n2);
        break;
    }
#endif
  }
  | VLAD_SYM_NOT ground_atom {
#ifdef DEBUG
    char *n1;
    char *n2;
    char *n3;
    unsigned char ty;
    bool tr;
#endif
    int retval;

    if ((retval = kbase.negate_atom($2, &$$)) != VLAD_OK) {
      fprintf(stderr, "internal error: %d", retval);
      return retval;
    }

#ifdef DEBUG
    if ((retval = kbase.decode_atom(&n1, &n2, &n3, &ty, &tr, $$)) != VLAD_OK) {
      fprintf(stderr, "internal error: %d", retval);
      return retval;
    }

    switch(ty) {
      case VLAD_ATOM_CONST :
        printf("%5d %s\n", $$, n1);
        break;
      case VLAD_ATOM_HOLDS :
        printf("%5d holds(%s,%s,%s)\n", $$, n1, n2, n3);
        break;
      case VLAD_ATOM_MEMBER :
        printf("%5d memb(%s,%s)\n", $$, n1, n2);
        break;
      case VLAD_ATOM_SUBSET :
        printf("%5d subst(%s,%s)\n", $$, n1, n2);
        break;
    }
#endif
  }
  ;

ground_atom :
  ground_holds_atom {
    $$ = $1;
  }
  | ground_subst_atom {
    $$ = $1;
  }
  | ground_memb_atom {
    $$ = $1;
  }
  | logical_atom {
    $$ = $1;
  }
  ;

ground_holds_atom :
  VLAD_SYM_HOLDS VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;
    if ((retval = kbase.encode_atom($3, $5, $7, VLAD_ATOM_HOLDS, true, &$$)) != VLAD_OK) {
      fprintf(stderr, "internal error: %d", retval);
      return retval;
    }
  }
  ;

ground_subst_atom :
  VLAD_SYM_SUBST VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;
    if ((retval = kbase.encode_atom($3, $5, NULL, VLAD_ATOM_SUBSET, true, &$$)) != VLAD_OK) {
      fprintf(stderr, "internal error: %d", retval);
      return retval;
    }
  }
  ;
ground_memb_atom :
  VLAD_SYM_MEMB VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;
    if ((retval = kbase.encode_atom($3, $5, NULL, VLAD_ATOM_MEMBER, true, &$$)) != VLAD_OK) {
      fprintf(stderr, "internal error: %d", retval);
      return retval;
    }
  }
  ;

comp_exp :
  comp_boolean_atom {
  }
  | comp_exp logical_op comp_boolean_atom {
  }
  ;

comp_boolean_atom :
  comp_atom {
  }
  | VLAD_SYM_NOT comp_atom {
  }
  ;

comp_atom :
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
    int retval;
    if ((retval = kbase.encode_atom("true", NULL, NULL, VLAD_ATOM_CONST, true, &$$)) != VLAD_OK) {
      fprintf(stderr, "internal error: %d", retval);
      return retval;
    }
  }
  | VLAD_SYM_FALSE {
    int retval;
    if ((retval = kbase.encode_atom("false", NULL, NULL, VLAD_ATOM_CONST, true, &$$)) != VLAD_OK) {
      fprintf(stderr, "internal error: %d", retval);
      return retval;
    }
  }
  ;
%%
