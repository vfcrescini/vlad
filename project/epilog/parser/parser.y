/*
 * parser.y
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

%{
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ident.h>
#include <gnd_exp.h>
#include <comp_exp.h>
#include <stringlist.h>
#include <symtab.h>
#include <identlist.h>
#include <transtab.h>
#include <translist.h>

extern int yyerror(char *error);
extern int yywarn(char *warning);

int yylex(void);

int add_identifier(char ident[], unsigned short type);
void exit_error(char *message);
void initialise(void);
void destroy(void);
#ifdef DEBUG
int dump_strlist(stringlist_type list);
int dump_gnd_atom(gnd_atom_type atom);
int dump_gnd_exp(gnd_exp_type exp);
int dump_comp_atom(comp_atom_type atom);
int dump_comp_exp(comp_exp_type exp);
#endif

extern FILE *yyout;
FILE *yyerr;

gnd_exp_type initial_exp;
%}

%union {
  char identifier[128];
  name_type name;
  gnd_atom_type gatm;
  comp_atom_type catm;
  gnd_exp_type gexp;
  comp_exp_type cexp;
  transref_type transref;
  translist_type translist;
  stringlist_type strlist;
  identlist_type ident;
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
%token <identifier> EPI_SYM_IDENTIFIER

%type <gexp> is_clause;
%type <gexp> ground_exp
%type <cexp> comp_exp
%type <gatm> logical_atom
%type <gatm> ground_atom_exp
%type <gatm> ground_boolean_atom
%type <gatm> ground_holds_atom
%type <gatm> ground_memb_atom
%type <gatm> ground_subst_atom
%type <catm> comp_memb_atom
%type <catm> comp_atom_exp
%type <catm> comp_boolean_atom
%type <catm> comp_holds_atom
%type <catm> comp_memb_atom
%type <catm> comp_subst_atom
%type <catm> comp_memb_atom
%type <strlist> trans_var_def;
%type <strlist> trans_var_list;
%type <transref> trans_ref_def;
%type <translist> trans_ref_list;
%type <translist> after_clause;
%type <ident> trans_ref_ident_list;

%start program

%%

program :
  init body destroy
  ;

init :
  {
    initialise();
  }
  ;

destroy :
  {
#ifdef DEBUG
    fprintf(yyerr, "initial state:\n");
    dump_gnd_exp(initial_exp);
#endif

    destroy();
  }
  ;

body :
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
  | ident_stmt_list ident_stmt
  ;

other_stmt_list :
  other_stmt
  | other_stmt_list other_stmt
  ;

ident_stmt :
  EPI_SYM_IDENT ident_declaration EPI_SYM_SEMICOLON {
  }
  ;

other_stmt :
  initial_stmt {
  }
  | trans_stmt {
  }
  | policy_stmt {
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
  EPI_SYM_SUBTYPE sub_ident_list {
  }
  ;

obj_ident_decl :
  EPI_SYM_OBJTYPE obj_ident_list {
  }
  ;

acc_ident_decl :
  EPI_SYM_ACCTYPE acc_ident_list {
  }
  ;

sub_grp_ident_decl :
  EPI_SYM_SUBGRPTYPE sub_grp_ident_list {
  }
  ;

obj_grp_ident_decl :
  EPI_SYM_OBJGRPTYPE obj_grp_ident_list {
  }
  ;

acc_grp_ident_decl :
  EPI_SYM_ACCGRPTYPE acc_grp_ident_list {
  }
  ;

sub_ident_list :
  EPI_SYM_IDENTIFIER {
    if (add_identifier($1, EPI_IDENT_SUBJECT) != 0)
      exit_error("internal error");
  }
  | sub_ident_list EPI_SYM_COMMA EPI_SYM_IDENTIFIER {
    if (add_identifier($3, EPI_IDENT_SUBJECT) != 0)
      exit_error("internal error");
  }
  ;

obj_ident_list :
  EPI_SYM_IDENTIFIER {
    if (add_identifier($1, EPI_IDENT_OBJECT) != 0)
      exit_error("internal error");
  }
  | obj_ident_list EPI_SYM_COMMA EPI_SYM_IDENTIFIER {
    if (add_identifier($3, EPI_IDENT_OBJECT) != 0)
      exit_error("internal error");
  }
  ;

acc_ident_list :
  EPI_SYM_IDENTIFIER {
    if (add_identifier($1, EPI_IDENT_ACCESS) != 0)
      exit_error("internal error");
  }
  | acc_ident_list EPI_SYM_COMMA EPI_SYM_IDENTIFIER {
    if (add_identifier($3, EPI_IDENT_ACCESS) != 0)
      exit_error("internal error");
  }
  ;

sub_grp_ident_list :
  EPI_SYM_IDENTIFIER {
    if (add_identifier($1, EPI_IDENT_SUBJECT | EPI_IDENT_GROUP) != 0) 
      exit_error("internal error");
  }
  | sub_grp_ident_list EPI_SYM_COMMA EPI_SYM_IDENTIFIER {
    if (add_identifier($3, EPI_IDENT_SUBJECT | EPI_IDENT_GROUP) != 0) 
      exit_error("internal error");
  }
  ;

obj_grp_ident_list :
  EPI_SYM_IDENTIFIER {
    if (add_identifier($1, EPI_IDENT_OBJECT | EPI_IDENT_GROUP) != 0)
      exit_error("internal error");
  }
  | obj_grp_ident_list EPI_SYM_COMMA EPI_SYM_IDENTIFIER {
    if (add_identifier($3, EPI_IDENT_OBJECT | EPI_IDENT_GROUP) != 0)
      exit_error("internal error");
  }
  ;

acc_grp_ident_list :
  EPI_SYM_IDENTIFIER {
    if (add_identifier($1, EPI_IDENT_ACCESS | EPI_IDENT_GROUP) != 0)
      exit_error("internal error");
  }
  | acc_grp_ident_list EPI_SYM_COMMA EPI_SYM_IDENTIFIER {
    if (add_identifier($3, EPI_IDENT_ACCESS | EPI_IDENT_GROUP) != 0)
      exit_error("internal error");
  }
  ;

initial_stmt : 
  EPI_SYM_INITIALLY ground_exp EPI_SYM_SEMICOLON {
    unsigned int i;
    gnd_atom_type *tmp_atom = NULL;

    /* now we have to go through these expressions and see if they're aleady
     * in the initial state expression */

    for (i = 0; i < gnd_exp_length($2); i++) {
      if (gnd_exp_get($2, i, &tmp_atom) != 0) 
        exit_error("internal error");

      if (gnd_exp_find(initial_exp, *tmp_atom) == 0) {
        yywarn("atom already in the initial state");
        continue;
      }

      if (gnd_exp_add(&initial_exp, *tmp_atom) != 0) 
        exit_error("internal error");

#ifdef DEBUG
      fprintf(yyerr, "adding atom to initial state:\n");
      dump_gnd_atom(*tmp_atom);
#endif
    }

    gnd_exp_purge(&$2);
  }
  ;

trans_stmt : 
  EPI_SYM_TRANS EPI_SYM_IDENTIFIER trans_var_def EPI_SYM_CAUSES comp_exp EPI_SYM_IF comp_exp EPI_SYM_SEMICOLON {
    transdef_type tmp_transdef;

    if (transtab_find($2) == 0)
      exit_error("trans identifier already declared");

    if (transdef_compose(&tmp_transdef, $2, $3, $7, $5) != 0)
      exit_error("internal_error");

    if (transtab_add(tmp_transdef) != 0)
      exit_error("internal error");

#ifdef DEBUG
    fprintf(yyerr, "transformation declaration\n");
    fprintf(yyerr, "  name:\n");
    fprintf(yyerr, "    %s\n", $2);
    fprintf(yyerr, "  variables:\n");
    dump_strlist($3);
    fprintf(yyerr, "  preconditions:\n");
    dump_comp_exp($7);
    fprintf(yyerr, "  postconditions:\n");
    dump_comp_exp($5);
#endif
  }
  ;

policy_stmt : 
  is_clause after_clause EPI_SYM_SEMICOLON {
    transref_type *tmp_trans;
    unsigned short int result;
    gnd_exp_type curr;
    gnd_exp_type prev;
    unsigned int i;

    if (gnd_exp_copy(initial_exp, &prev))
      exit_error("internal error1");

    for (i = 0; i < translist_length($2); i++) {
      if (translist_get($2, i, &tmp_trans) != 0)
        exit_error("internal error3");
      if (transtab_transform(prev, *tmp_trans, &curr) != 0)
        exit_error("internal error4");
      gnd_exp_purge(&prev);

      prev = curr;
    }

    if (gnd_exp_eval($1, curr, &result) != 0)
      exit_error("internal error");

    switch (result) {
      case EPI_RESULT_TRUE :
        fprintf(yyout, "true\n");
        break;
      case EPI_RESULT_FALSE :
        fprintf(yyout, "false\n");
        break;
      default :
        fprintf(yyout, "?\n");
    }

    gnd_exp_purge(&curr);
    translist_purge(&$2);
    gnd_exp_purge(&$1);
  }
  | is_clause EPI_SYM_SEMICOLON {
    unsigned short int result;

    if (gnd_exp_eval($1, initial_exp, &result) != 0)
      exit_error("internal error");

    switch (result) {
      case EPI_RESULT_TRUE :
        fprintf(yyout, "true\n");
        break;
      case EPI_RESULT_FALSE :
        fprintf(yyout, "false\n");
        break;
      default :
        fprintf(yyout, "?\n");
    }    

    gnd_exp_purge(&$1);
  }
  ;

trans_var_def : 
  EPI_SYM_OPEN_PARENT EPI_SYM_CLOSE_PARENT {
    stringlist_init(&$$);
  }
  | EPI_SYM_OPEN_PARENT trans_var_list EPI_SYM_CLOSE_PARENT {
    $$ = $2;
  }
  ;

trans_var_list : 
  EPI_SYM_IDENTIFIER {
    if (symtab_find($1) == 0)
      exit_error("variable name already declared as an identiifer");

    stringlist_init(&$$);

    if (stringlist_add(&$$, $1) != 0) 
      exit_error("internal error");
  }
  | trans_var_list EPI_SYM_COMMA EPI_SYM_IDENTIFIER {
    if (symtab_find($3) == 0)
      exit_error("variable name already declared as an identiifer");

    if (stringlist_add(&$1, $3) != 0)
      exit_error("internal error");

    $$ = $1;
  }
  ;

is_clause : 
  EPI_SYM_IS ground_exp {
    $$ = $2;
  }
  ;

after_clause : 
  EPI_SYM_AFTER trans_ref_list {
    $$ = $2;
  }
  ;

trans_ref_list : 
  trans_ref_def {
    translist_init(&$$);

    if (translist_add(&$$, $1) != 0)
      exit_error("internal error");
  }
  | trans_ref_list EPI_SYM_COMMA trans_ref_def {
    if (translist_add(&$1, $3) != 0)
      exit_error("internal error");
    $$ = $1;
  }
  ;

trans_ref_def : 
  EPI_SYM_IDENTIFIER EPI_SYM_OPEN_PARENT trans_ref_ident_list EPI_SYM_CLOSE_PARENT {
    if (transtab_find($1) != 0)
      exit_error("transformation not declared");
    if (transref_compose(&$$, $1, $3) != 0)
      exit_error("internal error");
  }
  ;

trans_ref_ident_list : 
  EPI_SYM_IDENTIFIER {
    ident_type *tmp_ident = NULL;

    identlist_init(&$$);

    if (symtab_get($1, &tmp_ident) != 0)
      exit_error("identifier not declared");
    if (identlist_add(&$$, tmp_ident) != 0)
      exit_error("internal error");
  }
  | trans_ref_ident_list EPI_SYM_COMMA EPI_SYM_IDENTIFIER {
    ident_type *tmp_ident = NULL;

    if (symtab_get($3, &tmp_ident) != 0)
      exit_error("identifier not declared");
    if (identlist_add(&$1, tmp_ident) != 0)
      exit_error("internal error");
    $$ = $1;
  }
  ;

logical_op :
  EPI_SYM_AND {
  }
  ;

ground_exp : 
  ground_boolean_atom { 
    gnd_exp_init(&$$);

    if (gnd_exp_find($$, $1) == 0)
      yywarn("atom already declared");
    else if (gnd_exp_add(&$$, $1) != 0)
      exit_error("internal error");
  }
  | ground_exp logical_op ground_boolean_atom {
    if (gnd_exp_find($1, $3) == 0)
      yywarn("atom already declared");
    else if (gnd_exp_add(&$1, $3) != 0) 
      exit_error("internal error");

    $$ = $1;
  }
  ;

ground_boolean_atom :
  ground_atom_exp {
    $$ = $1;
  }
  | EPI_SYM_NOT ground_atom_exp {
    $$ = $2;
    $$.truth = EPI_FALSE;
  }
  ;

ground_atom_exp :
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
  EPI_SYM_HOLDS EPI_SYM_OPEN_PARENT EPI_SYM_IDENTIFIER EPI_SYM_COMMA EPI_SYM_IDENTIFIER EPI_SYM_COMMA EPI_SYM_IDENTIFIER EPI_SYM_CLOSE_PARENT {
    ident_type *subject = NULL;
    ident_type *access = NULL;
    ident_type *object = NULL;

    if (symtab_get($3, &subject) != 0 || 
        symtab_get($5, &access) != 0 ||
        symtab_get($7, &object) != 0) 
      exit_error("undeclared identifier");

    if (!EPI_IDENT_IS_SUBJECT(*subject)) 
      exit_error("first parameter of holds must be a subject");
 
    if (!EPI_IDENT_IS_ACCESS(*access)) 
      exit_error("second parameter of holds must be an access-right");

    if (!EPI_IDENT_IS_OBJECT(*object))
      exit_error("third parameter of holds must be an object");

    gnd_atom_create_holds(&$$, subject, access, object, EPI_TRUE);
  }
  ;

ground_subst_atom :
  EPI_SYM_SUBST EPI_SYM_OPEN_PARENT EPI_SYM_IDENTIFIER EPI_SYM_COMMA EPI_SYM_IDENTIFIER EPI_SYM_CLOSE_PARENT {
    ident_type *group1 = NULL;
    ident_type *group2 = NULL;

    if (symtab_get($3, &group1) != 0 || 
        symtab_get($5, &group2) != 0) 
      exit_error("undeclared identifier");

    if (!EPI_IDENT_IS_GROUP(*group1)) 
      exit_error("parameters of subst must be groups");
 
    if (group1->type != group2->type)
      exit_error("parameters of subst are of different types");

    gnd_atom_create_subst(&$$, group1, group2, EPI_TRUE);
  }
  ;
ground_memb_atom :
  EPI_SYM_MEMB EPI_SYM_OPEN_PARENT EPI_SYM_IDENTIFIER EPI_SYM_COMMA EPI_SYM_IDENTIFIER EPI_SYM_CLOSE_PARENT {
    ident_type *element = NULL;
    ident_type *group = NULL;

    if (symtab_get($3, &element) != 0 || 
        symtab_get($5, &group) != 0) 
      exit_error("undeclared identifier");

    if (EPI_IDENT_IS_GROUP(*element)) 
      exit_error("first parameter of memb must not be a group");
 
    if (!EPI_IDENT_IS_GROUP(*group)) 
      exit_error("second parameter of memb must be a group");

    if (EPI_IDENT_BASETYPE(*element) != EPI_IDENT_BASETYPE(*group))
      exit_error("parameters of memb are of different types");

    gnd_atom_create_memb(&$$, element, group, EPI_TRUE);
  }
  ;

comp_exp :
  comp_boolean_atom {
    comp_exp_init(&$$);

    if (comp_exp_find($$, $1) == 0)
      yywarn("atom already declared");
    else if (comp_exp_add(&$$, $1) != 0)
      exit_error("internal error");
  }
  | comp_exp logical_op comp_boolean_atom {
    if (comp_exp_find($1, $3) == 0)
      yywarn("atom already declared");
    else if (comp_exp_add(&$1, $3) != 0) 
      exit_error("internal error");

    $$ = $1;
  }
  ;

comp_boolean_atom :
  comp_atom_exp {
    $$ = $1;
  }
  | EPI_SYM_NOT comp_atom_exp {
    $$ = $2;
    $$.truth = EPI_FALSE;
  }
  ;

comp_atom_exp :
  comp_holds_atom {
    $$ = $1;
  }
  | comp_subst_atom {
    $$ = $1;
  }
  | comp_memb_atom {
    $$ = $1;
  }
  | logical_atom {
    comp_atom_create_const(&$$, $1.truth);
  }
  ;

comp_holds_atom :
  EPI_SYM_HOLDS EPI_SYM_OPEN_PARENT EPI_SYM_IDENTIFIER EPI_SYM_COMMA EPI_SYM_IDENTIFIER EPI_SYM_COMMA EPI_SYM_IDENTIFIER EPI_SYM_CLOSE_PARENT {
    ident_type *subject = NULL;
    ident_type *access = NULL;
    ident_type *object = NULL;

    $$.type = EPI_ATOM_HOLDS;
    $$.truth = EPI_TRUE;

    if (symtab_get($3, &subject) != 0)
      subject = NULL;
     
    if (symtab_get($5, &access) != 0)
      access = NULL;

    if (symtab_get($7, &object) != 0)
      object = NULL;

    if (subject != NULL) {
      /* subject is an identifier */
      if (!EPI_IDENT_IS_SUBJECT(*subject))
        exit_error("first parameter of holds must be a subject");
      if (name_create_ident(subject, &(EPI_ATOM_HOLDS_SUBJECT($$))) != 0)
        exit_error("internal error");
    }
    else {
      /* subject is a variable */
      if (name_create_var($3, &(EPI_ATOM_HOLDS_SUBJECT($$))) != 0)
        exit_error("internal error");
    }

    if (access != NULL) {
      /* access is an identifier */
      if (!EPI_IDENT_IS_ACCESS(*access))
        exit_error("second parameter of holds must be an access-right");
      if (name_create_ident(access, &(EPI_ATOM_HOLDS_ACCESS($$))) != 0)
        exit_error("internal error");
    }
    else {
      /* access is a variable */
      if (name_create_var($5, &(EPI_ATOM_HOLDS_ACCESS($$))) != 0)
        exit_error("internal error");
    }

    if (object != NULL) {
      /* object is an identifier */
      if (!EPI_IDENT_IS_OBJECT(*object))
        exit_error("third parameter of holds must be an object");
      if (name_create_ident(object, &(EPI_ATOM_HOLDS_OBJECT($$))) != 0)
        exit_error("internal error");
    }
    else {
      /* object is a variable */
      if (name_create_var($7, &(EPI_ATOM_HOLDS_OBJECT($$))) != 0)
        exit_error("internal error");
    }
  }
  ;

comp_subst_atom :
  EPI_SYM_SUBST EPI_SYM_OPEN_PARENT EPI_SYM_IDENTIFIER EPI_SYM_COMMA EPI_SYM_IDENTIFIER EPI_SYM_CLOSE_PARENT {
    ident_type *group1 = NULL;
    ident_type *group2 = NULL;

    $$.type = EPI_ATOM_SUBST;
    $$.truth = EPI_TRUE;

    if (symtab_get($3, &group1) != 0)
      group1 = NULL;
     
    if (symtab_get($5, &group2) != 0)
      group2 = NULL;

    if (group1 != NULL && group2 != NULL) {
      if (group1->type != group2->type)
        exit_error("parameters of subst are of different types");
    }

    if (group1 != NULL) {
      /* group1 is an identifier */
      if (!EPI_IDENT_IS_GROUP(*group1))
        exit_error("first parameter of subst must be a group");
      if (name_create_ident(group1, &(EPI_ATOM_SUBST_GROUP1($$))) != 0)
        exit_error("internal error");
    }
    else {
      /* group1 is a variable */
      if (name_create_var($3, &(EPI_ATOM_SUBST_GROUP1($$))) != 0)
        exit_error("internal error");
    }

    if (group2 != NULL) {
      /* group2 is an identifier */
      if (!EPI_IDENT_IS_GROUP(*group2))
        exit_error("first parameter of subst must be a group");
      if (name_create_ident(group2, &(EPI_ATOM_SUBST_GROUP2($$))) != 0)
        exit_error("internal error");
    }
    else {
      /* group2 is a variable */
      if (name_create_var($5, &(EPI_ATOM_SUBST_GROUP2($$))) != 0)
        exit_error("internal error");
    }
  }
  ;

comp_memb_atom :
  EPI_SYM_MEMB EPI_SYM_OPEN_PARENT EPI_SYM_IDENTIFIER EPI_SYM_COMMA EPI_SYM_IDENTIFIER EPI_SYM_CLOSE_PARENT {
    ident_type *element = NULL;
    ident_type *group = NULL;

    $$.type = EPI_ATOM_MEMB;
    $$.truth = EPI_TRUE;

    if (symtab_get($3, &element) != 0)
      element = NULL;
     
    if (symtab_get($5, &group) != 0)
      group = NULL;

    if (element != NULL && group != NULL) {
      if (EPI_IDENT_BASETYPE(*element) != EPI_IDENT_BASETYPE(*group))
        exit_error("parameters of memb are of different types");
    }

    if (element != NULL) {
      /* element is an identifier */
      if (EPI_IDENT_IS_GROUP(*element))
        exit_error("first parameter of memb must not be a group");
      if (name_create_ident(element, &(EPI_ATOM_MEMB_ELEMENT($$))) != 0)
        exit_error("internal error");
    }
    else {
      /* element is a variable */
      if (name_create_var($3, &(EPI_ATOM_MEMB_ELEMENT($$))) != 0)
        exit_error("internal error");
    }

    if (group != NULL) {
      /* group is an identifier */
      if (!EPI_IDENT_IS_GROUP(*group)) 
        exit_error("second parameter of memb must be a group");
      if (name_create_ident(group, &(EPI_ATOM_MEMB_GROUP($$))) != 0)
        exit_error("internal error");
    }
    else {
      /* group is a variable */
      if (name_create_var($5, &(EPI_ATOM_MEMB_GROUP($$))) != 0)
        exit_error("internal error");
    }
  }
  ;

logical_atom : 
  EPI_SYM_TRUE {
    gnd_atom_create_const(&$$, EPI_TRUE);
  }
  | EPI_SYM_FALSE {
    gnd_atom_create_const(&$$, EPI_FALSE);
  }
  ;

%%

int add_identifier(char ident[], unsigned short type)
{
  if (symtab_find(ident) == 0) {
    yyerror("identifier declared more than once");
    return -1;
  }
  
#ifdef DEBUG
  if (EPI_IDENT_TYPE_IS_SUBJECT(type) && ! EPI_IDENT_TYPE_IS_GROUP(type))
    fprintf(yyerr, "declared subject identifier %s\n", ident);
  else if (EPI_IDENT_TYPE_IS_OBJECT(type) && ! EPI_IDENT_TYPE_IS_GROUP(type))
    fprintf(yyerr, "declared object identifier %s\n", ident);
  else if (EPI_IDENT_TYPE_IS_ACCESS(type) && ! EPI_IDENT_TYPE_IS_GROUP(type))
    fprintf(yyerr, "declared access identifier %s\n", ident); 
  if (EPI_IDENT_TYPE_IS_SUBJECT(type) && EPI_IDENT_TYPE_IS_GROUP(type))
    fprintf(yyerr, "declared subject-group identifier %s\n", ident);
  else if (EPI_IDENT_TYPE_IS_OBJECT(type) && EPI_IDENT_TYPE_IS_GROUP(type))
    fprintf(yyerr, "declared object-group identifier %s\n", ident);
  else if (EPI_IDENT_TYPE_IS_ACCESS(type) && EPI_IDENT_TYPE_IS_GROUP(type))
    fprintf(yyerr, "declared access-group identifier %s\n", ident); 
#endif

  if (symtab_add(ident, type) != 0) {
    yyerror("internal error");
    return -1;
  }

  return 0;
}

/* initialise our global lists */
void initialise(void)
{
#ifdef DEBUG
  fprintf(yyerr, "initialising global lists\n");
#endif

  symtab_init();
  gnd_exp_init(&initial_exp);
  transtab_init();
}

/* destroy dynamically allocated mem */
void destroy(void)
{
#ifdef DEBUG
  fprintf(yyerr, "destroying global lists\n");
#endif

  symtab_purge();
  gnd_exp_purge(&initial_exp);
  transtab_purge();
}

void exit_error(char *message)
{
  yyerror(message);
  destroy();
  exit(-1);
}

#ifdef DEBUG
int dump_strlist(stringlist_type list)
{
  unsigned int i;
  char *tmp_string = NULL;

  for (i = 0; i < stringlist_length(list); i++) {
    stringlist_get(list, i, &tmp_string);
    fprintf(yyerr, "    %s\n", tmp_string);
  }
  return 0;
}

int dump_gnd_atom(gnd_atom_type atom)
{
  if (EPI_ATOM_IS_CONST(atom))
    fprintf(yyerr, "    constant %s\n", atom.truth == 0 ? "true" : "false");
  else if (EPI_ATOM_IS_HOLDS(atom))
    fprintf(yyerr, "    %sholds(%s, %s, %s)\n",
           atom.truth == 0 ? "" : "not ",
           EPI_IDENT_STRING(EPI_ATOM_HOLDS_SUBJECT(atom)),
           EPI_IDENT_STRING(EPI_ATOM_HOLDS_ACCESS(atom)),
           EPI_IDENT_STRING(EPI_ATOM_HOLDS_OBJECT(atom)));
  else if (EPI_ATOM_IS_MEMB(atom))
    fprintf(yyerr, "    %smemb(%s, %s)\n", 
           atom.truth == 0 ? "" : "not ",
           EPI_IDENT_STRING(EPI_ATOM_MEMB_ELEMENT(atom)),
           EPI_IDENT_STRING(EPI_ATOM_MEMB_GROUP(atom)));
  else if (EPI_ATOM_IS_SUBST(atom))
    fprintf(yyerr, "    %ssubst(%s, %s)\n", 
           atom.truth == 0 ? "" : "not ",
           EPI_IDENT_STRING(EPI_ATOM_SUBST_GROUP1(atom)),
           EPI_IDENT_STRING(EPI_ATOM_SUBST_GROUP2(atom)));

  return 0;
}

int dump_comp_atom(comp_atom_type atom)
{
  if (EPI_ATOM_IS_CONST(atom))
    fprintf(yyerr, "    constant %s\n", atom.truth == 0 ? "true" : "false");
  else if (EPI_ATOM_IS_HOLDS(atom))
    fprintf(yyerr, "    %sholds(%s, %s, %s)\n",
           atom.truth == 0 ? "" : "not ",
           EPI_NAME_STRING(EPI_ATOM_HOLDS_SUBJECT(atom)),
           EPI_NAME_STRING(EPI_ATOM_HOLDS_ACCESS(atom)),
           EPI_NAME_STRING(EPI_ATOM_HOLDS_OBJECT(atom)));
  else if (EPI_ATOM_IS_MEMB(atom))
    fprintf(yyerr, "    %smemb(%s, %s)\n", 
           atom.truth == 0 ? "" : "not ",
           EPI_NAME_STRING(EPI_ATOM_MEMB_ELEMENT(atom)),
           EPI_NAME_STRING(EPI_ATOM_MEMB_GROUP(atom)));
  else if (EPI_ATOM_IS_SUBST(atom))
    fprintf(yyerr, "    %ssubst(%s, %s)\n", 
           atom.truth == 0 ? "" : "not ",
           EPI_NAME_STRING(EPI_ATOM_SUBST_GROUP1(atom)),
           EPI_NAME_STRING(EPI_ATOM_SUBST_GROUP2(atom)));

  return 0;
}

int dump_gnd_exp(gnd_exp_type exp)
{
  unsigned int i;
  gnd_atom_type *tmp_atom = NULL;

  for (i = 0; i < gnd_exp_length(exp); i++) {
    gnd_exp_get(exp, i, &tmp_atom);
    dump_gnd_atom(*tmp_atom);
  }
  return 0;
}

int dump_comp_exp(comp_exp_type exp)
{
  unsigned int i;
  comp_atom_type *tmp_atom = NULL;

  for (i = 0; i < comp_exp_length(exp); i++) {
    comp_exp_get(exp, i, &tmp_atom);
    dump_comp_atom(*tmp_atom);
  }
  return 0;
}

#endif
