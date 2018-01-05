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

#ifdef DEBUG
unsigned int cnt_init = 0;
unsigned int cnt_const = 0;
unsigned int cnt_trans = 0;
unsigned int cnt_query = 0;
#endif

extern int yyerror(char *error);
extern int yywarn(char *warning);

int add_identifier(const char *n, unsigned char t);

int yylex(void);

%}

%union {
  char identifier[128];
  unsigned int terminal;
  atom *atm;
  expression *exp;
  stringlist *vlist;
  transref *tref;
  sequence *tseq;
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
%type <atm> atom 
%type <atm> boolean_atom 
%type <atm> holds_atom 
%type <atm> subst_atom 
%type <atm> memb_atom 
%type <atm> logical_atom 
%type <exp> expression
%type <exp> with_clause
%type <exp> if_clause
%type <vlist> trans_var_list
%type <vlist> trans_var_def
%type <vlist> trans_ref_ident_args
%type <vlist> trans_ref_ident_list
%type <tref> trans_ref_def
%type <tseq> trans_ref_list
%type <tseq> after_clause

%start program

%%

program :
  init body destroy
  ;

init : {
    int retval;

    if ((retval = kbase.init()) != VLAD_OK) {
      fprintf(yyerr, "failed to initialise knowledge base: %d\n", retval);
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
    if ((retval = kbase.close_symtab()) != VLAD_OK) {
      fprintf(yyerr, "internal error: %d\n", retval);
      return retval;
    }
  }
  | ident_stmt_list {
    int retval;
    /* after the ident section, we must close the symbol table */
    if ((retval = kbase.close_symtab()) != VLAD_OK) {
      fprintf(yyerr, "internal error: %d\n", retval);
      return retval;
    }
  }
  ;

initial_section : {
    int retval;
    /* after the initial section, we must close the init table */
    if ((retval = kbase.close_inittab()) != VLAD_OK) {
      fprintf(yyerr, "internal error: %d\n", retval);
      return retval;
    }
  }
  | initial_stmt_list {
    int retval;
    /* after the initial section, we must close the init table */
    if ((retval = kbase.close_inittab()) != VLAD_OK) {
      fprintf(yyerr, "internal error: %d\n", retval);
      return retval;
    }
  }
  ;

constraint_section : {
    int retval;
    /* after the constraint section, we must close the constraint table */
    if ((retval = kbase.close_consttab()) != VLAD_OK) {
      fprintf(yyerr, "internal error: %d\n", retval);
      return retval;
    }
  }
  | constraint_stmt_list {
    int retval;
    /* after the constraint section, we must close the constraint table */
    if ((retval = kbase.close_consttab()) != VLAD_OK) {
      fprintf(yyerr, "internal error: %d\n", retval);
      return retval;
    }
  }
  ;

trans_section : {
     int retval;
    /* after the transformation section, we must close the trans table */
    if ((retval = kbase.close_transtab()) != VLAD_OK) {
      fprintf(yyerr, "internal error: %d\n", retval);
      return retval;
    } 
  }
  | trans_stmt_list {
    int retval;
    /* after the transformation section, we must close the trans table */
    if ((retval = kbase.close_transtab()) != VLAD_OK) {
      fprintf(yyerr, "internal error: %d\n", retval);
      return retval;
    }
  }
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
    int retval;
    if ((retval = add_identifier($1, VLAD_IDENT_SUBJECT)) != VLAD_OK)
      return retval;
  }
  | sub_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_identifier($3, VLAD_IDENT_SUBJECT)) != VLAD_OK)
      return retval;
  }
  ;

obj_ident_list :
  VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_identifier($1, VLAD_IDENT_OBJECT)) != VLAD_OK)
      return retval;
  }
  | obj_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_identifier($3, VLAD_IDENT_OBJECT)) != VLAD_OK)
      return retval;
  }
  ;

acc_ident_list :
  VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_identifier($1, VLAD_IDENT_ACCESS)) != VLAD_OK)
      return retval;
  }
  | acc_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_identifier($3, VLAD_IDENT_ACCESS)) != VLAD_OK)
      return retval;
  }
  ;

sub_grp_ident_list :
  VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_identifier($1, VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP)) != VLAD_OK)
      return retval;
  }
  | sub_grp_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_identifier($3, VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP)) != VLAD_OK)
      return retval;
  }
  ;

obj_grp_ident_list :
  VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_identifier($1, VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP)) != VLAD_OK)
      return retval;
  }
  | obj_grp_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_identifier($3, VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP)) != VLAD_OK)
      return retval;
  }
  ;

acc_grp_ident_list :
  VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_identifier($1, VLAD_IDENT_ACCESS | VLAD_IDENT_GROUP)) != VLAD_OK)
      return retval;
  }
  | acc_grp_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_identifier($3, VLAD_IDENT_ACCESS | VLAD_IDENT_GROUP)) != VLAD_OK)
      return retval;
  }
  ;

initial_stmt : 
  VLAD_SYM_INITIALLY expression VLAD_SYM_SEMICOLON {
    int retval;
    unsigned int i;
    atom *a;
#ifdef DEBUG
    char s[128];
#endif

    /*
     * we must, unfortunetly, go through the expression and add them one
     * at a time to ensure uniqueness and integrity.
     */

    for (i = 0; i < $2->length(); i++) {
      if ((retval = $2->get(i, &a)) != VLAD_OK) {
        fprintf(yyerr, "internal error: %d\n", retval);
        return retval;
      }

      if ((retval = kbase.add_inittab(a)) != VLAD_OK) {
        fprintf(yyerr, "internal error: %d\n", retval);
        return retval;
      }

#ifdef DEBUG
      a->print(s);
      fprintf(yyerr, "initial state[%d]: %s\n", cnt_init++, s);
#endif

    }

    /* when everything's been registered into the kb, delete the expression */
    delete $2;
  }
  ;

constraint_stmt :
  implies_stmt
  | always_stmt
  ;

implies_stmt :
  expression VLAD_SYM_IMPLIES expression with_clause VLAD_SYM_SEMICOLON {
    int retval;
#ifdef DEBUG
    char e[1024];
    char c[1024];
    char n[1024];
#endif
    if ((retval = kbase.add_consttab($3, $1, $4)) != VLAD_OK) {
      fprintf(yyerr, "internal error: %d\n", retval);
      return retval;
    }

#ifdef DEBUG
    $3->print(e);
    $1->print(c);
    if ($4 != NULL)
      $4->print(n);
    else
      strcpy(n, "none");

    fprintf(yyerr, "constraint[%d]:\n", cnt_const++);
    fprintf(yyerr, "  expression: %s\n", e);
    fprintf(yyerr, "  condition:   %s\n", c);
    fprintf(yyerr, "  absence:     %s\n", n);
#endif

    /* cleanup */
    delete $1;
    delete $3;
    if ($4 != NULL)
      delete $4;
  }
  ;

with_clause : {
    $$ = NULL;
  }
  | VLAD_SYM_WITH VLAD_SYM_ABSENCE expression {
    $$ = $3;
  }
  ;

always_stmt :
  VLAD_SYM_ALWAYS expression VLAD_SYM_SEMICOLON {
    int retval;
#ifdef DEBUG
    char e[1024];
#endif

    if ((retval = kbase.add_consttab($2, NULL, NULL)) != VLAD_OK) {
      fprintf(yyerr, "internal error: %d\n", retval);
      return retval;
    }

#ifdef DEBUG
    $2->print(e);

    fprintf(yyerr, "constraint[%d]:\n", cnt_const++);
    fprintf(yyerr, "  expression: %s\n", e);
    fprintf(yyerr, "  condition:   none\n");
    fprintf(yyerr, "  absence:     none\n");
#endif
    delete $2;
  }
  ;

trans_stmt : 
  VLAD_SYM_IDENTIFIER trans_var_def VLAD_SYM_CAUSES expression if_clause VLAD_SYM_SEMICOLON {
    int retval;
#ifdef DEBUG
    char v[1024];
    char pr[1024];
    char po[1024];
#endif

    if ((retval = kbase.add_transtab($1, $2, $5, $4)) != VLAD_OK) {
      fprintf(yyerr, "internal error: %d\n", retval);
      return retval;
    }

#ifdef DEBUG
    if ($2 != NULL)
      $2->print(v);
    else
      strcpy(v, "none");

    if ($5 != NULL)
      $5->print(pr);
    else
      strcpy(pr, "none");

    $4->print(po);

    fprintf(yyerr, "transformation[%d]:\n", cnt_trans++);
    fprintf(yyerr, "  name:        %s\n", $1);
    fprintf(yyerr, "  varlist:    %s\n", v);
    fprintf(yyerr, "  precond:    %s\n", pr);
    fprintf(yyerr, "  postcond:   %s\n", po);
#endif

    /* cleanup */
    if ($2 != NULL)
      delete $2;
    if ($5 != NULL)
      delete $5;
    delete $4;
  }
  ;

if_clause : {
    $$ = NULL;
  }
  | VLAD_SYM_IF expression {
    $$ = $2;
  }
  ;

trans_var_def : 
  VLAD_SYM_OPEN_PARENT VLAD_SYM_CLOSE_PARENT {
    $$ = NULL;
  }
  | VLAD_SYM_OPEN_PARENT trans_var_list VLAD_SYM_CLOSE_PARENT {
    $$ = $2;
  }
  ;

trans_var_list : 
  VLAD_SYM_IDENTIFIER {
    int retval;

    if (($$ = VLAD_NEW(stringlist())) == NULL) {
      fprintf(yyerr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->add($1)) != VLAD_OK) {
      fprintf(yyerr, "interal error: %d\n", retval);
      return retval;
    }
  }
  | trans_var_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    int retval;

    if ((retval = $$->add($3)) != VLAD_OK) {
      fprintf(yyerr, "interal error: %d\n", retval);
      return retval;
    }
  }
  ;

query_stmt : 
  VLAD_SYM_IS expression after_clause VLAD_SYM_SEMICOLON {
    int retval;
#ifdef DEBUG
    char q[1024];
    char r[10240];
#endif

    if ((retval = kbase.generate_nlp($2, $3, yyout)) != VLAD_OK) {
      fprintf(yyerr, "internal error: %d\n", retval);
      return retval;
    }

#ifdef DEBUG
    $2->print(q);
    if ($3 != NULL)
      $3->print(r);
    else
      strcpy(r, "none");

    fprintf(yyerr, "query[%d]:\n", cnt_query++);
    fprintf(yyerr, "  expression: %s\n", q);
    fprintf(yyerr, "  trans seq:  %s\n", r);
#endif

    /* cleanup */
    delete $2;
    if ($3 != NULL)
      delete $3;
  }
  ;

after_clause : {
    $$ = NULL;
  }
  | VLAD_SYM_AFTER trans_ref_list {
    $$ = $2;
  }
  ;

trans_ref_list : 
  trans_ref_def {
    int retval;

    if (($$ = VLAD_NEW(sequence())) == NULL) {
      fprintf(yyerr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->add($1)) != VLAD_OK) {
      fprintf(yyerr, "interal error: %d\n", retval);
      return retval;
    }
  }
  | trans_ref_list VLAD_SYM_COMMA trans_ref_def {
    int retval;

    if ((retval = $$->add($3)) != VLAD_OK) {
      fprintf(yyerr, "interal error: %d\n", retval);
      return retval;
    }
  }
  ;

trans_ref_def : 
  VLAD_SYM_IDENTIFIER VLAD_SYM_OPEN_PARENT trans_ref_ident_args VLAD_SYM_CLOSE_PARENT {
    char *name;

    /* first allocate memory for the name */
    if ((name = VLAD_STRING_MALLOC($1)) == NULL) {
      fprintf(yyerr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    }

    strcpy(name, $1);
 
    /* then add the entire thing into a transref */
    if (($$ = VLAD_NEW(transref(name, $3))) == NULL) {
      fprintf(yyerr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    }   
  }
  ;

trans_ref_ident_args : {
    $$ = NULL;
  }
  | trans_ref_ident_list {
    $$ = $1;
  }
  ;

trans_ref_ident_list : 
  VLAD_SYM_IDENTIFIER {
    int retval;

    if (($$ = VLAD_NEW(stringlist())) == NULL) {
      fprintf(yyerr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->add($1)) != VLAD_OK) {
      fprintf(yyerr, "interal error: %d\n", retval);
      return retval;
    }
  }
  | trans_ref_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    int retval;

    if ((retval = $$->add($3)) != VLAD_OK) {
      fprintf(yyerr, "interal error: %d\n", retval);
      return retval;
    }
  }
  ;

logical_op :
  VLAD_SYM_AND {
  }
  ;

expression : 
  boolean_atom { 
    int retval;

    if (($$ = VLAD_NEW(expression())) == NULL) {
      fprintf(yyerr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->add($1)) != VLAD_OK)
      return retval;
  }
  | expression logical_op boolean_atom {
    int retval;
    switch ((retval = $$->add($3))) {
      case VLAD_OK :
      case VLAD_DUPLICATE :
        /* we simply ignore duplicates */
        break;
      default :
        return retval;
    }
  }
  ;

boolean_atom :
  atom {
    $$ = $1;
  }
  | VLAD_SYM_NOT atom {
    $$ = $2;
    $$->negate();
  }
  ;

atom :
  holds_atom {
    $$ = $1;
  }
  | subst_atom {
    $$ = $1;
  }
  | memb_atom {
    $$ = $1;
  }
  | logical_atom {
    $$ = $1;
  }
  ;

holds_atom :
  VLAD_SYM_HOLDS VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;

    if (($$ = VLAD_NEW(atom())) == NULL) {
      fprintf(yyerr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->init_holds($3, $5, $7, true)) != VLAD_OK)
      return retval;
  }
  ;

subst_atom :
  VLAD_SYM_SUBST VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;

    if (($$ = VLAD_NEW(atom())) == NULL) {
      fprintf(yyerr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->init_subset($3, $5, true)) != VLAD_OK)
      return retval;
  }
  ;

memb_atom :
  VLAD_SYM_MEMB VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;

    if (($$ = VLAD_NEW(atom())) == NULL) {
      fprintf(yyerr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->init_member($3, $5, true)) != VLAD_OK)
      return retval;
  }
  ;

logical_atom : 
  VLAD_SYM_TRUE {
    int retval;

    if (($$ = VLAD_NEW(atom())) == NULL) {
      fprintf(yyerr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->init_const(true, true)) != VLAD_OK)
      return retval;
  }
  | VLAD_SYM_FALSE {
    int retval;

    if (($$ = VLAD_NEW(atom())) == NULL) {
      fprintf(yyerr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->init_const(false, true)) != VLAD_OK)
      return retval;
  }
  ;
%%

int add_identifier(const char *n, unsigned char t)
{
  if (!VLAD_IDENT_IS_VALID(t) || t == 0) {
    fprintf(yyerr, "internal error: %d\n", VLAD_INVALIDINPUT);
    return VLAD_INVALIDINPUT;
  }

  switch (kbase.add_symtab(n, t)) {
    case VLAD_OK :
#ifdef DEBUG
      switch (t) {
        case VLAD_IDENT_SUBJECT :
          fprintf(yyerr, "declared identifier subject: %s\n", n);
          break;
        case VLAD_IDENT_ACCESS :
          fprintf(yyerr, "declared identifier access: %s\n", n);
          break;
        case VLAD_IDENT_OBJECT :
          fprintf(yyerr, "declared identifier object: %s\n", n);
          break;
        case VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP :
           fprintf(yyerr, "declared identifier subject-group: %s\n", n);
          break;
        case VLAD_IDENT_ACCESS | VLAD_IDENT_GROUP :
           fprintf(yyerr, "declared identifier access-group: %s\n", n);
          break;
        case VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP :
           fprintf(yyerr, "declared identiifer object-group: %s\n", n);
          break;
      }
#endif
      break;
    case VLAD_DUPLICATE :
      fprintf(yyerr, "already declared: identifier %s\n", n);
      return VLAD_DUPLICATE;
    case VLAD_MALLOCFAILED :
      fprintf(yyerr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    default :
      fprintf(yyerr, "internal error: %d\n", VLAD_FAILURE);
      return VLAD_FAILURE;
  }

  return VLAD_OK;
}
