/*
 * parser.y
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

%{
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stringlist.h>
#include <ident.h>
#include <identlist.h>
#include <atom.h>
#include <expression.h>
#include <compatom.h>
#include <compexp.h>


extern int yyerror(char *error);
extern int yywarn(char *warning);

int yylex(void);

int add_identifier(char ident[], unsigned short type);
void exit_error(char *message);
int initialise(void);
int destroy(void);

expression_type initial_exp;
FILE *yyerr;
%}

%union {
  char identifier[128];
  atom_type groundatm;
  expression_type groundexp;
  compatom_type compatm;
  compexp_type compexp;
  stringlist_type strlist;
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

%type <groundexp> ground_exp
%type <compexp> comp_exp
%type <groundatm> logical_atom
%type <groundatm> ground_atom_exp
%type <groundatm> ground_boolean_atom
%type <groundatm> ground_holds_atom
%type <groundatm> ground_memb_atom
%type <groundatm> ground_subst_atom
%type <compatm> comp_memb_atom
%type <compatm> comp_atom_exp
%type <compatm> comp_boolean_atom
%type <compatm> comp_holds_atom
%type <compatm> comp_memb_atom
%type <compatm> comp_subst_atom
%type <compatm> comp_memb_atom
%type <strlist> trans_var_def;
%type <strlist> trans_var_list;

%start program

%%

program :
  init body destroy
  ;

init :
  {
    if (initialise() != 0) 
      exit_error("internal error");
  }
  ;

destroy :
  {

#ifdef DEBUG
    unsigned int i;
    unsigned int len;
    atom_type *tmp_atom = NULL;

    expression_length(initial_exp, &len);

    for (i = 0; i < len; i++) {
      expression_get(initial_exp, i, &tmp_atom);

      if (EPI_ATOM_IS_CONST(*tmp_atom))
        fprintf(yyerr, "initial state: constant %s\n", tmp_atom->truth == 0 ? "true" : "false");
      else if (EPI_ATOM_IS_HOLDS(*tmp_atom))
        fprintf(yyerr, "initial state: %sholds(%s, %s, %s)\n",
               tmp_atom->truth == 0 ? "" : "not ",
               tmp_atom->atom.holds.subject->name,
               tmp_atom->atom.holds.access->name,
               tmp_atom->atom.holds.object->name);
      else if (EPI_ATOM_IS_MEMB(*tmp_atom))
        fprintf(yyerr, "initial state: %smemb(%s, %s)\n", 
               tmp_atom->truth == 0 ? "" : "not ",
               tmp_atom->atom.memb.element->name,
               tmp_atom->atom.memb.group->name);  
      else if (EPI_ATOM_IS_SUBST(*tmp_atom))
        fprintf(yyerr, "initial state: %ssubst(%s, %s)\n", 
               tmp_atom->truth == 0 ? "" : "not ",
               tmp_atom->atom.subst.group1->name,
               tmp_atom->atom.subst.group2->name);     
    }
#endif

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
    if (add_identifier($1, EPI_IDENT_SUBJECT) != 0)
      exit_error("internal error");
  }
  | EPI_SYM_IDENTIFIER EPI_SYM_COMMA sub_ident_list {
    if (add_identifier($1, EPI_IDENT_SUBJECT) != 0)
      exit_error("internal error");
  }
  ;

obj_ident_list :
  EPI_SYM_IDENTIFIER {
    if (add_identifier($1, EPI_IDENT_OBJECT) != 0)
      exit_error("internal error");
  }
  | EPI_SYM_IDENTIFIER EPI_SYM_COMMA obj_ident_list {
    if (add_identifier($1, EPI_IDENT_OBJECT) != 0)
      exit_error("internal error");
  }
  ;

acc_ident_list :
  EPI_SYM_IDENTIFIER {
    if (add_identifier($1, EPI_IDENT_ACCESS) != 0)
      exit_error("internal error");
  }
  | EPI_SYM_IDENTIFIER EPI_SYM_COMMA acc_ident_list {
    if (add_identifier($1, EPI_IDENT_ACCESS) != 0)
      exit_error("internal error");
  }
  ;

sub_grp_ident_list :
  EPI_SYM_IDENTIFIER {
    if (add_identifier($1, EPI_IDENT_SUBJECT | EPI_IDENT_GROUP) != 0) 
      exit_error("internal error");
  }
  | EPI_SYM_IDENTIFIER EPI_SYM_COMMA sub_grp_ident_list {
    if (add_identifier($1, EPI_IDENT_SUBJECT | EPI_IDENT_GROUP) != 0) 
      exit_error("internal error");
  }
  ;

obj_grp_ident_list :
  EPI_SYM_IDENTIFIER {
    if (add_identifier($1, EPI_IDENT_OBJECT | EPI_IDENT_GROUP) != 0)
      exit_error("internal error");
  }
  | EPI_SYM_IDENTIFIER EPI_SYM_COMMA obj_grp_ident_list {
    if (add_identifier($1, EPI_IDENT_OBJECT | EPI_IDENT_GROUP) != 0)
      exit_error("internal error");
  }
  ;

acc_grp_ident_list :
  EPI_SYM_IDENTIFIER {
    if (add_identifier($1, EPI_IDENT_ACCESS | EPI_IDENT_GROUP) != 0)
      exit_error("internal error");
  }
  | EPI_SYM_IDENTIFIER EPI_SYM_COMMA acc_grp_ident_list {
    if (add_identifier($1, EPI_IDENT_ACCESS | EPI_IDENT_GROUP) != 0)
      exit_error("internal error");
  }
  ;

initial_stmt : 
  EPI_SYM_INITIALLY ground_exp EPI_SYM_SEMICOLON {
    unsigned int i;
    unsigned int len;
    atom_type *tmp_atom = NULL;

    /* now we have to go through these expressions and see if they're aleady
     * in the initial state expression */

    if (expression_length($2, &len) != 0)
      exit_error("internal error");

    for (i = 0; i < len; i++) {
      if (expression_get($2, i, &tmp_atom) != 0) 
        exit_error("internal error");

      if (expression_find(initial_exp, *tmp_atom) == 0) {
        yywarn("atom already declared");
        continue;
      }

      if (expression_add(&initial_exp, *tmp_atom) != 0) 
        exit_error("internal error");

#ifdef DEBUG
      if (EPI_ATOM_IS_CONST(*tmp_atom))
        fprintf(yyerr, "adding constant %s into the initial state\n", tmp_atom->truth == 0 ? "true" : "false");
      else if (EPI_ATOM_IS_HOLDS(*tmp_atom))
        fprintf(yyerr, "adding %sholds(%s, %s, %s) into the initial state\n", 
               tmp_atom->truth == 0 ? "" : "not ",
               tmp_atom->atom.holds.subject->name,
               tmp_atom->atom.holds.access->name,
               tmp_atom->atom.holds.object->name);
      else if (EPI_ATOM_IS_MEMB(*tmp_atom))
        fprintf(yyerr, "adding %smemb(%s, %s) into the initial state\n", 
               tmp_atom->truth == 0 ? "" : "not ",
               tmp_atom->atom.memb.element->name,
               tmp_atom->atom.memb.group->name);  
      else if (EPI_ATOM_IS_SUBST(*tmp_atom))
        fprintf(yyerr, "adding %ssubst(%s, %s) into the initial state\n", 
               tmp_atom->truth == 0 ? "" : "not ",
               tmp_atom->atom.subst.group1->name,
               tmp_atom->atom.subst.group2->name);  
#endif
    }

    if (expression_purge(&$2) != 0)
      exit_error("internal error");
  }
  ;

trans_stmt : 
  EPI_SYM_TRANS EPI_SYM_IDENTIFIER trans_var_def EPI_SYM_CAUSES comp_exp EPI_SYM_IF comp_exp EPI_SYM_SEMICOLON {
    unsigned int len = 0;
    unsigned int i;
    char *temp_string = NULL;

    if (stringlist_length($3, &len) != 0) 
      exit_error("internal error");

#ifdef DEBUG
    fprintf(yyerr, "transformation: %s\n", $2);
#endif

    for (i = 0; i < len; i++) {
      if (stringlist_get($3, i, &temp_string) != 0)
        exit_error("internal error");

#ifdef DEBUG
      fprintf(yyerr, "variable: %s\n", temp_string);
#endif
    }

    if (stringlist_purge(&$3) != 0)
      exit_error("internal error");
  }
  ;

policy_stmt : 
  is_clause after_clause EPI_SYM_SEMICOLON 
  | is_clause EPI_SYM_SEMICOLON
  ;

trans_var_def : 
  EPI_SYM_OPEN_PARENT EPI_SYM_CLOSE_PARENT {
    if (stringlist_init(&$$) != 0)
      exit_error("internal error");
  }
  | EPI_SYM_OPEN_PARENT trans_var_list EPI_SYM_CLOSE_PARENT {
    $$ = $2;
  }
  ;

trans_var_list : 
  EPI_SYM_IDENTIFIER {

    if (identlist_find($1) == 0) 
      exit_error("variable name already declared as an identiifer");

    if (stringlist_init(&$$) != 0)
      exit_error("internal error");

    if (stringlist_add(&$$, $1) != 0) 
      exit_error("internal error");
  }
  | EPI_SYM_IDENTIFIER EPI_SYM_COMMA trans_var_list {

    if (identlist_find($1) == 0)
      exit_error("variable name already declared as an identiifer");

    if (stringlist_add(&$3, $1) != 0)
      exit_error("internal error");

    $$ = $3;
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
    if (expression_init(&$$) != 0) 
      exit_error("internal error");

    if (expression_find($$, $1) == 0)
      yywarn("atom already declared");
    else if (expression_add(&$$, $1) != 0)
      exit_error("internal error");
  }
  | ground_boolean_atom logical_op ground_exp {
    if (expression_find($3, $1) == 0)
      yywarn("atom already declared");
    else if (expression_add(&$3, $1) != 0) 
      exit_error("internal error");

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
        identlist_get($7, &object) != 0) 
      exit_error("undeclared identifier");

    if (!EPI_IDENT_IS_SUBJECT(*subject)) 
      exit_error("first parameter of holds must be a subject");
 
    if (!EPI_IDENT_IS_ACCESS(*access)) 
      exit_error("second parameter of holds must be an access-right");

    if (!EPI_IDENT_IS_OBJECT(*object))
      exit_error("third parameter of holds must be an object");

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
        identlist_get($5, &group2) != 0) 
      exit_error("undeclared identifier");

    if (!EPI_IDENT_IS_GROUP(*group1)) 
      exit_error("parameters of subst must be groups");
 
    if (group1->type != group2->type)
      exit_error("parameters of subst are of different types");

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
        identlist_get($5, &group) != 0) 
      exit_error("undeclared identifier");

    if (EPI_IDENT_IS_GROUP(*element)) 
      exit_error("first parameter of memb must not be a group");
 
    if (!EPI_IDENT_IS_GROUP(*group)) 
      exit_error("second parameter of memb must be a group");

    if (EPI_IDENT_BASETYPE(*element) != EPI_IDENT_BASETYPE(*group))
      exit_error("parameters of memb are of different types");

    $$.type = EPI_ATOM_MEMB;
    $$.truth = epi_true;
    $$.atom.memb.element = element;
    $$.atom.memb.group = group;
  }
  ;

comp_exp :
  comp_boolean_atom {
    var_type *temp_var = NULL;
    unsigned int varlist_len = 0;
    unsigned int i;

    if (expression_init(&($$.exp)) != 0 ||
        expression_add(&($$.exp), $1.atom) != 0 ||
        varlist_init(&($$.varlist)) != 0 ||
        varlist_length($1.varlist, &varlist_len) != 0) 
      exit_error("internal error");

    /* go through the atom's varlist and add them to the
     * expression's varlist */
    for (i = 0; i < varlist_len; i++) {
      if (varlist_get_index($1.varlist, i, &temp_var) != 0 ||
          temp_var == NULL)
        exit_error("internal error");

      if (varlist_find($$.varlist, temp_var->name) != 0) {
        if (varlist_add(&($$.varlist), temp_var->name, temp_var->ident) != 0)
          exit_error("internal error");
      }
    }
    if (varlist_purge(&($1.varlist)) != 0)
      exit_error("internal error");
  }
  | comp_boolean_atom logical_op comp_exp {
    var_type *temp_var = NULL;
    unsigned int varlist_len = 0;
    unsigned int i;

    if (expression_add(&($3.exp), $1.atom) != 0 ||
        varlist_length($1.varlist, &varlist_len) != 0)
      exit_error("internal error");

    /* go through the atom's varlist and add them to the
     * expression's varlist */
    for (i = 0; i < varlist_len; i++) {
      if (varlist_get_index($1.varlist, i, &temp_var) != 0 ||
          temp_var == NULL) 
        exit_error("internal error");

      if (varlist_find($3.varlist, temp_var->name) != 0) {
        if (varlist_add(&($3.varlist), temp_var->name, temp_var->ident) != 0) 
          exit_error("internal error");
      }
    }

    if (varlist_purge(&($1.varlist)) != 0)
      exit_error("internal error");

    $$ = $3;
  }
  ;

comp_boolean_atom :
  comp_atom_exp {
    $$ = $1;
  }
  | EPI_SYM_NOT comp_atom_exp {
    $$ = $2;
    $$.atom.truth = epi_false;
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
    $$.atom = $1;
  }
  ;

comp_holds_atom :
  EPI_SYM_HOLDS EPI_SYM_OPEN_PARENT EPI_SYM_IDENTIFIER EPI_SYM_COMMA EPI_SYM_IDENTIFIER EPI_SYM_COMMA EPI_SYM_IDENTIFIER EPI_SYM_CLOSE_PARENT {
    ident_type *subject = NULL;
    ident_type *access = NULL;
    ident_type *object = NULL;

    if (varlist_init(&($$.varlist)) != 0)
      exit_error("internal error");

    $$.atom.type = EPI_ATOM_HOLDS;
    $$.atom.truth = epi_true;

    if (identlist_get($3, &subject) != 0)
      subject = NULL;
     
    if (identlist_get($5, &access) != 0)
      access = NULL;

    if (identlist_get($7, &object) != 0)
      object = NULL;

    if (subject != NULL) {
      /* subject is an identifier */
      if (!EPI_IDENT_IS_SUBJECT(*subject))
        exit_error("first parameter of holds must be a subject");
    }
    else {
      /* subject is a variable */
      if (ident_create(&subject, $3, 0) != 0)
        exit_error("internal error");

      if (varlist_add(&($$.varlist), $3, subject) != 0)
        exit_error("internal error");
    }

    if (access != NULL) {
      /* access is an identifier */
      if (!EPI_IDENT_IS_ACCESS(*access))
        exit_error("second parameter of holds must be an access-right");
    }
    else {
      /* access is a variable */
      if (ident_create(&access, $5, 0) != 0) 
        exit_error("internal error");

      if (varlist_add(&($$.varlist), $5, access) != 0)
        exit_error("internal error");
    }

    if (object != NULL) {
      /* object is an identifier */
      if (!EPI_IDENT_IS_OBJECT(*object))
        exit_error("third parameter of holds must be an object");
    }
    else {
      /* object is a variable */
      if (ident_create(&object, $7, 0) != 0) 
        exit_error("internal error");

      if (varlist_add(&($$.varlist), $7, object) != 0) 
        exit_error("internal error");
    }

    $$.atom.atom.holds.subject = subject;
    $$.atom.atom.holds.access = access;
    $$.atom.atom.holds.object = object;
  }
  ;

comp_subst_atom :
  EPI_SYM_SUBST EPI_SYM_OPEN_PARENT EPI_SYM_IDENTIFIER EPI_SYM_COMMA EPI_SYM_IDENTIFIER EPI_SYM_CLOSE_PARENT {
    ident_type *group1 = NULL;
    ident_type *group2 = NULL;

    if (varlist_init(&($$.varlist)) != 0)
      exit_error("internal error");

    $$.atom.type = EPI_ATOM_SUBST;
    $$.atom.truth = epi_true;

    if (identlist_get($3, &group1) != 0)
      group1 = NULL;
     
    if (identlist_get($5, &group2) != 0)
      group2 = NULL;

    if (group1 != NULL && group2 != NULL) {
      if (group1->type != group2->type)
        exit_error("parameters of subst are of different types");
    }

    if (group1 != NULL) {
      /* group1 is an identifier */
      if (!EPI_IDENT_IS_GROUP(*group1))
        exit_error("first parameter of subst must be a group");
    }
    else {
      /* group1 is a variable */
      if (ident_create(&group1, $5, 0) != 0) 
        exit_error("internal error");

      if (varlist_add(&($$.varlist), $5, group1) != 0)
        exit_error("internal error");
    }

    if (group2 != NULL) {
      /* group2 is an identifier */
      if (!EPI_IDENT_IS_GROUP(*group2))
        exit_error("first parameter of subst must be a group");
    }
    else {
      /* group2 is a variable */
      if (ident_create(&group2, $5, 0) != 0) 
        exit_error("internal error");

      if (varlist_add(&($$.varlist), $5, group2) != 0) 
        exit_error("internal error");
    }

    $$.atom.atom.subst.group1 = group1;
    $$.atom.atom.subst.group2 = group2;
  }
  ;

comp_memb_atom :
  EPI_SYM_MEMB EPI_SYM_OPEN_PARENT EPI_SYM_IDENTIFIER EPI_SYM_COMMA EPI_SYM_IDENTIFIER EPI_SYM_CLOSE_PARENT {
    ident_type *element = NULL;
    ident_type *group = NULL;

    if (varlist_init(&($$.varlist)) != 0) 
      exit_error("internal error");

    $$.atom.type = EPI_ATOM_MEMB;
    $$.atom.truth = epi_true;

    if (identlist_get($3, &element) != 0)
      element = NULL;
     
    if (identlist_get($5, &group) != 0)
      group = NULL;

    if (element != NULL && group != NULL) {
      if (EPI_IDENT_BASETYPE(*element) != EPI_IDENT_BASETYPE(*group))
        exit_error("parameters of memb are of different types");
    }

    if (element != NULL) {
      /* element is an identifier */
      if (EPI_IDENT_IS_GROUP(*element))
        exit_error("first parameter of memb must not be a group");
    }
    else {
      /* element is a variable */
      if (ident_create(&element, $3, 0) != 0) 
        exit_error("internal error");

      if (varlist_add(&($$.varlist), $3, element) != 0)
        exit_error("internal error");
    }

    if (group != NULL) {
      /* group is an identifier */
      if (!EPI_IDENT_IS_GROUP(*group)) 
        exit_error("second parameter of memb must be a group");
    }
    else {
      /* group is a variable */
      if (ident_create(&group, $5, 0) != 0) 
        exit_error("internal error");

      if (varlist_add(&($$.varlist), $5, group) != 0) 
        exit_error("internal error");
    }

    $$.atom.atom.memb.element = element;
    $$.atom.atom.memb.group = group;
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
  fprintf(yyerr, "initialising global lists\n");
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
  fprintf(yyerr, "destroying global lists\n");
#endif

  if (identlist_purge() != 0)
    return -1;

  if (expression_purge(&initial_exp) != 0)
    return -1;

  return 0;
}

void exit_error(char *message)
{
  yyerror(message);
  destroy();
  exit(-1);
}
