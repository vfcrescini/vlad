/*
 * parser.y
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

%{
#include <stdlib.h>
#include <ident.h>
#include <identlist.h>
#include <expression.h>

#ifdef DEBUG
#include <stdio.h>
#endif

extern expression_type initial_exp;

extern int yyerror(char *error);
extern int yywarn(char *warning);

int yylex(void);

int add_identifier(char ident[], unsigned short type);
int initialise(void);
int destroy(void);

expression_type initial_exp;
%}

%union {
  char identifier[128];
  expression_type exp;
  atom_type atm;
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

%type <exp> ground_exp
%type <exp> comp_exp
%type <atm> logical_atom
%type <atm> ground_atom_exp
%type <atm> ground_boolean_atom
%type <atm> ground_holds_atom
%type <atm> ground_memb_atom
%type <atm> ground_subst_atom
%type <atm> comp_memb_atom
%type <atm> comp_atom_exp
%type <atm> comp_boolean_atom
%type <atm> comp_holds_atom
%type <atm> comp_memb_atom
%type <atm> comp_subst_atom
%type <atm> comp_memb_atom

%start program

%%

program :
  init body destroy
  ;

init :
  {
    if (initialise() != 0) {
      yyerror("internal error");
      destroy();
      return -1;
    }
  }
  ;

destroy :
  {
    if (destroy() != 0) {
      yyerror("internal error");
      return -1;
    }
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
  | ident_stmt ident_stmt_list
  ;

other_stmt_list :
  other_stmt
  | other_stmt other_stmt_list
  ;

ident_stmt :
  EPI_SYM_IDENT ident_declaration EPI_SYM_SEMICOLON {
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
    if (add_identifier($1, EPI_IDENT_SUBJECT) != 0) {
      yyerror("internal error");
      destroy();
      return -1;
    }
  }
  | EPI_SYM_IDENTIFIER EPI_SYM_COMMA sub_ident_list {
    if (add_identifier($1, EPI_IDENT_SUBJECT) != 0) {
      yyerror("internal error");
      destroy();
      return -1;
    }
  }
  ;

obj_ident_list :
  EPI_SYM_IDENTIFIER {
    if (add_identifier($1, EPI_IDENT_OBJECT) != 0) {
      yyerror("internal error");
      destroy();
      return -1;
    }
  }
  | EPI_SYM_IDENTIFIER EPI_SYM_COMMA obj_ident_list {
    if (add_identifier($1, EPI_IDENT_OBJECT) != 0) {
      yyerror("internal error");
      destroy();
      return -1;
    }
  }
  ;

acc_ident_list :
  EPI_SYM_IDENTIFIER {
    if (add_identifier($1, EPI_IDENT_ACCESS) != 0) {
      yyerror("internal error");
      destroy();
      return -1;
    }
  }
  | EPI_SYM_IDENTIFIER EPI_SYM_COMMA acc_ident_list {
    if (add_identifier($1, EPI_IDENT_ACCESS) != 0) {
      yyerror("internal error");
      destroy();
      return -1;
    }
  }
  ;

sub_grp_ident_list :
  EPI_SYM_IDENTIFIER {
    if (add_identifier($1, EPI_IDENT_SUBJECT | EPI_IDENT_GROUP) != 0) {
      yyerror("internal error");
      destroy();
      return -1;
    }
  }
  | EPI_SYM_IDENTIFIER EPI_SYM_COMMA sub_grp_ident_list {
    if (add_identifier($1, EPI_IDENT_SUBJECT | EPI_IDENT_GROUP) != 0) {
      yyerror("internal error");
      destroy();
      return -1;
    }
  }
  ;

obj_grp_ident_list :
  EPI_SYM_IDENTIFIER {
    if (add_identifier($1, EPI_IDENT_OBJECT | EPI_IDENT_GROUP) != 0) {
      yyerror("internal error");
      destroy();
      return -1;
    }
  }
  | EPI_SYM_IDENTIFIER EPI_SYM_COMMA obj_grp_ident_list {
    if (add_identifier($1, EPI_IDENT_OBJECT | EPI_IDENT_GROUP) != 0) {
      yyerror("internal error");
      destroy();
      return -1;
    }
  }
  ;

acc_grp_ident_list :
  EPI_SYM_IDENTIFIER {
    if (add_identifier($1, EPI_IDENT_ACCESS | EPI_IDENT_GROUP) != 0) {
      yyerror("internal error");
      destroy();
      return -1;
    }
  }
  | EPI_SYM_IDENTIFIER EPI_SYM_COMMA acc_grp_ident_list {
    if (add_identifier($1, EPI_IDENT_ACCESS | EPI_IDENT_GROUP) != 0) {
      yyerror("internal error");
      destroy();
      return -1;
    }
  }
  ;

initial_stmt : 
  EPI_SYM_INITIALLY ground_exp EPI_SYM_SEMICOLON {
    unsigned int i;
    unsigned int len;
    atom_type *tmp_atom = NULL;

    /* now we have to go thrugh these expressions and see if they're aleady
     * in the initial state expression */

    if (expression_length($2, &len) != 0) {
      yyerror("internal error");
      destroy();
      return -1;
    }
    for (i = 0; i < len; i++) {
      if (expression_get($2, i, &tmp_atom) != 0) {
        yyerror("internal error");
        destroy();
        return -1;
      }
      if (expression_find(initial_exp, *tmp_atom) == 0) {
        yywarn("atom already declared");
        continue;
      }
      if (expression_add(&initial_exp, *tmp_atom) != 0) {
        yyerror("internal error");
        destroy();
        return -1;
      }

#ifdef DEBUG
      if (EPI_ATOM_IS_CONST(tmp_atom->type))
        printf("adding constant %s into the initial state\n", tmp_atom->truth == 0 ? "true" : "false");
      else if (EPI_ATOM_IS_HOLDS(tmp_atom->type))
        printf("adding %sholds(%s, %s, %s) into the initial state\n", 
               tmp_atom->truth == 0 ? "" : "not ",
               tmp_atom->atom.holds.subject->name,
               tmp_atom->atom.holds.access->name,
               tmp_atom->atom.holds.object->name);
      else if (EPI_ATOM_IS_MEMB(tmp_atom->type))
        printf("adding %smemb(%s, %s) into the initial state\n", 
               tmp_atom->truth == 0 ? "" : "not ",
               tmp_atom->atom.memb.element->name,
               tmp_atom->atom.memb.group->name);  
      else if (EPI_ATOM_IS_SUBST(tmp_atom->type))
        printf("adding %ssubst(%s, %s) into the initial state\n", 
               tmp_atom->truth == 0 ? "" : "not ",
               tmp_atom->atom.subst.group1->name,
               tmp_atom->atom.subst.group2->name);  
#endif
    }

    if (expression_purge(&$2) != 0) {
      yyerror("internal error");
      destroy();
      return -1;
    }
  }
  ;

trans_stmt : 
  EPI_SYM_TRANS EPI_SYM_IDENTIFIER trans_var_def EPI_SYM_CAUSES comp_exp EPI_SYM_IF comp_exp EPI_SYM_SEMICOLON {
  }
  ;

policy_stmt : 
  is_clause after_clause EPI_SYM_SEMICOLON 
  | is_clause EPI_SYM_SEMICOLON
  ;

trans_var_def : 
  EPI_SYM_OPEN_PARENT EPI_SYM_CLOSE_PARENT {
  }
  | EPI_SYM_OPEN_PARENT trans_var_list EPI_SYM_CLOSE_PARENT {
  }
  ;

trans_var_list : 
  EPI_SYM_IDENTIFIER {
  }
  | EPI_SYM_IDENTIFIER EPI_SYM_COMMA trans_var_list {
  }
  ;

is_clause : 
  EPI_SYM_IS ground_exp {
  }
  ;

after_clause : 
  EPI_SYM_AFTER trans_ref_list {
  }
  ;

trans_ref_list : 
  trans_ref_def {
  }
  | trans_ref_def EPI_SYM_COMMA trans_ref_list {
  }
  ;

trans_ref_def : 
  EPI_SYM_IDENTIFIER EPI_SYM_OPEN_PARENT trans_ref_ident_list EPI_SYM_CLOSE_PARENT {
  }
  ;

trans_ref_ident_list : 
  EPI_SYM_IDENTIFIER {
  }
  | EPI_SYM_IDENTIFIER EPI_SYM_COMMA trans_ref_ident_list {
  }
  ;

logical_op :
  EPI_SYM_AND {
  }
  ;

ground_exp : 
  ground_boolean_atom { 
    if (expression_init(&$$) != 0) {
      yyerror("internal error");
      destroy();
      return -1;
    }
    if (expression_add(&$$, $1) != 0) {
      yyerror("internal error");
      destroy();
      return -1;
    }
  }
  | ground_boolean_atom logical_op ground_exp { 
    if (expression_add(&$3, $1) != 0) {
      yyerror("internal error");
      destroy();
      return -1;
    }
    $$ = $3;
  }
  ;

ground_boolean_atom :
  ground_atom_exp {
    $$ = $1;
  }
  | EPI_SYM_NOT ground_atom_exp {
    $$ = $2;
    $$.truth = epi_false;
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

    if (identlist_get($3, &subject) != 0 || 
        identlist_get($5, &access) != 0 ||
        identlist_get($7, &object) != 0) {
      yyerror("undeclared identifier");
      destroy();
      return -1;
    }

    if (!EPI_IDENT_IS_SUBJECT(subject->type)) {
      yyerror("first parameter of holds must be a subject");
      destroy();
      return -1;
    }
 
    if (!EPI_IDENT_IS_ACCESS(access->type)) {
      yyerror("second parameter of holds must be an access-right");
      destroy();
      return -1;
    }
    if (!EPI_IDENT_IS_OBJECT(object->type)) {
      yyerror("third parameter of holds must be an object");
      destroy();
      return -1;
    }

    $$.type = EPI_ATOM_HOLDS;
    $$.truth = epi_true;
    $$.atom.holds.subject = subject;
    $$.atom.holds.access = access;
    $$.atom.holds.object = object;
  }
  ;

ground_subst_atom :
  EPI_SYM_SUBST EPI_SYM_OPEN_PARENT EPI_SYM_IDENTIFIER EPI_SYM_COMMA EPI_SYM_IDENTIFIER EPI_SYM_CLOSE_PARENT {
    ident_type *group1 = NULL;
    ident_type *group2 = NULL;

    if (identlist_get($3, &group1) != 0 || 
        identlist_get($5, &group2) != 0) {
      yyerror("undeclared identifier");
      destroy();
      return -1;
    }

    if (!EPI_IDENT_IS_GROUP(group1->type)) {
      yyerror("parameters of subst must be groups");
      destroy();
      return -1;
    }
 
    if (group1->type != group2->type) {
      yyerror("parameters of subst are of different types");
      destroy();
      return -1;
    }

    $$.type = EPI_ATOM_SUBST;
    $$.truth = epi_true;
    $$.atom.subst.group1 = group1;
    $$.atom.subst.group2 = group2;
  }
  ;

ground_memb_atom :
  EPI_SYM_MEMB EPI_SYM_OPEN_PARENT EPI_SYM_IDENTIFIER EPI_SYM_COMMA EPI_SYM_IDENTIFIER EPI_SYM_CLOSE_PARENT {
    ident_type *element = NULL;
    ident_type *group = NULL;

    if (identlist_get($3, &element) != 0 || 
        identlist_get($5, &group) != 0) {
      yyerror("undeclared identifier");
      destroy();
      return -1;
    }

    if (EPI_IDENT_IS_GROUP(element->type)) {
      yyerror("first parameter of memb must not be a group");
      destroy();
      return -1;
    }
 
    if (!EPI_IDENT_IS_GROUP(group->type)) {
      yyerror("second parameter of memb must be a group");
      destroy();
      return -1;
    }

    if (EPI_IDENT_BASETYPE(element->type) != EPI_IDENT_BASETYPE(group->type)) {
      yyerror("parameters of memb are of different types");
      destroy();
      return -1;
    }

    $$.type = EPI_ATOM_MEMB;
    $$.truth = epi_true;
    $$.atom.memb.element = element;
    $$.atom.memb.group = group ;
  }
  ;

comp_exp :
  comp_boolean_atom {
  }
  | comp_boolean_atom logical_op comp_exp {
  }
  ;

comp_boolean_atom :
  comp_atom_exp {
  }
  | EPI_SYM_NOT comp_atom_exp {
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
  EPI_SYM_HOLDS EPI_SYM_OPEN_PARENT EPI_SYM_IDENTIFIER EPI_SYM_COMMA EPI_SYM_IDENTIFIER EPI_SYM_COMMA EPI_SYM_IDENTIFIER EPI_SYM_CLOSE_PARENT {
  }
  ;

comp_subst_atom :
  EPI_SYM_SUBST EPI_SYM_OPEN_PARENT EPI_SYM_IDENTIFIER EPI_SYM_COMMA EPI_SYM_IDENTIFIER EPI_SYM_CLOSE_PARENT {
  }
  ;

comp_memb_atom :
  EPI_SYM_MEMB EPI_SYM_OPEN_PARENT EPI_SYM_IDENTIFIER EPI_SYM_COMMA EPI_SYM_IDENTIFIER EPI_SYM_CLOSE_PARENT {
  }
  ;

logical_atom : 
  EPI_SYM_TRUE {
    $$.truth = epi_true;
    $$.type = EPI_ATOM_CONST;
  }
  | EPI_SYM_FALSE {
    $$.truth = epi_false;
    $$.type = EPI_ATOM_CONST;
  }
  ;

%%

int add_identifier(char ident[], unsigned short type) 
{
  if (identlist_find(ident) == 0) {
    yyerror("identifier declared more than once");
    return -1;
  }
  
#ifdef DEBUG
  if (EPI_IDENT_IS_SUBJECT(type) && ! EPI_IDENT_IS_GROUP(type))
    printf("declared subject identifier %s\n", ident);
  else if (EPI_IDENT_IS_OBJECT(type) && ! EPI_IDENT_IS_GROUP(type))
    printf("declared object identifier %s\n", ident);
  else if (EPI_IDENT_IS_ACCESS(type) && ! EPI_IDENT_IS_GROUP(type))
    printf("declared access identifier %s\n", ident); 
  if (EPI_IDENT_IS_SUBJECT(type) && EPI_IDENT_IS_GROUP(type))
    printf("declared subject-group identifier %s\n", ident);
  else if (EPI_IDENT_IS_OBJECT(type) && EPI_IDENT_IS_GROUP(type))
    printf("declared object-group identifier %s\n", ident);
  else if (EPI_IDENT_IS_ACCESS(type) && EPI_IDENT_IS_GROUP(type))
    printf("declared access-group identifier %s\n", ident);   
#endif

  if (identlist_add(ident, type) != 0) {
    yyerror("internal error");
    return -1;
  }

  return 0;
}

/* initialise our global lists */
int initialise(void)
{
#ifdef DEBUG
  printf("initialising global lists\n");
#endif

  if (identlist_init() != 0)
    return -1;

  if (expression_init(&initial_exp))
    return -1;

  return 0;
}

/* destroy dynamically allocated mem */
int destroy(void)
{
#ifdef DEBUG
  printf("destroying global lists\n");
#endif

  if (identlist_purge() != 0)
    return -1;

  if (expression_purge(&initial_exp) != 0)
    return -1;

  return 0;
}
