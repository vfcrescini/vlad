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

int add_identifier(const char *n, unsigned char t);

#ifdef DEBUG
int print_atom(unsigned int a, char *s);
int print_exp(numberlist l, char *s);
#endif

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
%type <gnd_exp> with_clause

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
    if ((retval = kbase.close_symtab()) != VLAD_OK) {
      fprintf(stderr, "internal error: %d\n", retval);
      return retval;
    }
  }
  | ident_stmt_list {
    int retval;
    /* after the ident section, we must close the symbol table */
    if ((retval = kbase.close_symtab()) != VLAD_OK) {
      fprintf(stderr, "internal error: %d\n", retval);
      return retval;
    }
  }
  ;

initial_section : {
    int retval;
    /* after the initial section, we must close the init table */
    if ((retval = kbase.close_inittab()) != VLAD_OK) {
      fprintf(stderr, "internal error: %d\n", retval);
      return retval;
    }
  }
  | initial_stmt_list {
    int retval;
    /* after the initial section, we must close the init table */
    if ((retval = kbase.close_inittab()) != VLAD_OK) {
      fprintf(stderr, "internal error: %d\n", retval);
      return retval;
    }
  }
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
  VLAD_SYM_INITIALLY ground_exp VLAD_SYM_SEMICOLON {
    unsigned int i;
    unsigned int tmp;
    int retval;
#ifdef DEBUG
    char s[128];
#endif

    /*
     * we must, unfortunetly, go through the expression and add them one
     * at a time to ensure uniqueness and integrity.
     */

    for (i = 0; i < $2->length(); i++) {
      if ((retval = $2->getn(i, &tmp)) != VLAD_OK) {
        fprintf(stderr, "internal error: %d\n", retval);
        return retval;
      }

      if ((retval != kbase.add_inittab(tmp)) != VLAD_OK) {
        fprintf(stderr, "internal error: %d\n", retval);
        return retval;
      }

#ifdef DEBUG
      if ((retval = print_atom(tmp, s)) != VLAD_OK) {
        fprintf(stderr, "internal error: %d\n", retval);
        return retval;
      }

      fprintf(stderr, "added [%d] %s to initial state\n", tmp, s);
#endif
    }

    /* after adding, clean up */
    delete $2;
  }
  ;

constraint_stmt :
  implies_stmt
  | always_stmt
  ;

implies_stmt :
  ground_exp VLAD_SYM_IMPLIES ground_exp with_clause VLAD_SYM_SEMICOLON {
    int retval;
#ifdef DEBUG
    char e[1024];
    char c[1024];
    char n[1024];
#endif

    if ((retval = kbase.add_consttab($3, $1, $4)) != VLAD_OK)
      return retval;

#ifdef DEBUG
    if ((retval = print_exp(*$3, e)) != VLAD_OK)
      return retval;
    if ((retval = print_exp(*$1, c)) != VLAD_OK)
      return retval;
    if ((retval = print_exp(*$4, n)) != VLAD_OK)
      return retval;

    fprintf(stderr, "added constraint:\n");
    fprintf(stderr, "  expression: %s\n", e);
    fprintf(stderr, "  condition: %s\n", c);
    fprintf(stderr, "  absence: %s\n", n);
    
#endif
  }
  ;

with_clause : {
    /* make an empty list */
    if (($$ = VLAD_NEW(numberlist(NULL))) == NULL) {
      fprintf(stderr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    }
  }
  | VLAD_SYM_WITH VLAD_SYM_ABSENCE ground_exp {
    $$ = $3;
  }
  ;

always_stmt :
  VLAD_SYM_ALWAYS ground_exp VLAD_SYM_SEMICOLON {
    numberlist *cond;
    numberlist *ncond;
    int retval;
#ifdef DEBUG
    char s[1024];
#endif

    /* XXX: should we add a constant here? true? false? */
    if ((cond = VLAD_NEW(numberlist(NULL))) == NULL) {
      fprintf(stderr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    }

    if ((ncond = VLAD_NEW(numberlist(NULL))) == NULL) {
      fprintf(stderr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    }

    if ((retval = kbase.add_consttab($2, cond, ncond)) != VLAD_OK) {
      fprintf(stderr, "internal error: %d\n", retval);
      return retval;
    }

#ifdef DEBUG
    if ((retval = print_exp(*$2, s)) != VLAD_OK)
      return retval;

    fprintf(stderr, "added constraint:\n");
    fprintf(stderr, "  expression: %s\n", s);
    fprintf(stderr, "  condition:\n");
    fprintf(stderr, "  absence:\n");
#endif
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
    delete $2;
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
    if (($$ = VLAD_NEW(numberlist(NULL))) == NULL) {
      fprintf(stderr, "memory overflow: %d\n", VLAD_MALLOCFAILED);
      return VLAD_MALLOCFAILED;
    }
    if ((retval = $$->add($1)) != VLAD_OK)
      return retval;
  }
  | ground_exp logical_op ground_boolean_atom {
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

ground_boolean_atom :
  ground_atom {
    $$ = $1;
  }
  | VLAD_SYM_NOT ground_atom {
    int retval;

    if ((retval = kbase.negate_atom($2, &$$)) != VLAD_OK) {
      fprintf(stderr, "internal error: %d\n", retval);
      return retval;
    }
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
      fprintf(stderr, "internal error: %d\n", retval);
      return retval;
    }
  }
  ;

ground_subst_atom :
  VLAD_SYM_SUBST VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;
    if ((retval = kbase.encode_atom($3, $5, NULL, VLAD_ATOM_SUBSET, true, &$$)) != VLAD_OK) {
      fprintf(stderr, "internal error: %d\n", retval);
      return retval;
    }
  }
  ;
ground_memb_atom :
  VLAD_SYM_MEMB VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;
    if ((retval = kbase.encode_atom($3, $5, NULL, VLAD_ATOM_MEMBER, true, &$$)) != VLAD_OK) {
      fprintf(stderr, "internal error: %d\n", retval);
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
      fprintf(stderr, "internal error: %d\n", retval);
      return retval;
    }
  }
  | VLAD_SYM_FALSE {
    int retval;
    if ((retval = kbase.encode_atom("false", NULL, NULL, VLAD_ATOM_CONST, true, &$$)) != VLAD_OK) {
      fprintf(stderr, "internal error: %d\n", retval);
      return retval;
    }
  }
  ;
%%

int add_identifier(const char *n, unsigned char t)
{
  if (!VLAD_IDENT_IS_VALID(t) || t == 0) {
    fprintf(stderr, "internal error: %d\n", VLAD_INVALIDINPUT);
    return VLAD_INVALIDINPUT;
  }

  switch (kbase.add_symtab(n, t)) {
    case VLAD_OK :
#ifdef DEBUG
      switch (t) {
        case VLAD_IDENT_SUBJECT :
          fprintf(yyerr, "declared subject identifier %s\n", n);
          break;
        case VLAD_IDENT_ACCESS :
          fprintf(yyerr, "declared access identifier %s\n", n);
          break;
        case VLAD_IDENT_OBJECT :
          fprintf(yyerr, "declared object identifier %s\n", n);
          break;
        case VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP :
           fprintf(yyerr, "declared subject-group identifier %s\n", n);
          break;
        case VLAD_IDENT_ACCESS | VLAD_IDENT_GROUP :
           fprintf(yyerr, "declared access-group identifier %s\n", n);
          break;
        case VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP :
           fprintf(yyerr, "declared object-group identifier %s\n", n);
          break;
      }
#endif
      break;
    case VLAD_DUPLICATE :
      fprintf(yyerr, "identifier %s already declared\n", n);
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

#ifdef DEBUG
int print_atom(unsigned int a, char *s)
{
  unsigned char ty;
  bool tr;
  char *n1;
  char *n2;
  char *n3;
  int retval;

  /* assume we have enough space in s */

  if ((retval = kbase.decode_atom(&n1, &n2, &n3, &ty, &tr, a)) != VLAD_OK)
    return retval;
  
  switch(ty) {
    case VLAD_ATOM_CONST :
      sprintf(s, "%s%s", tr ? "" : "!", n1);
      break;
    case VLAD_ATOM_HOLDS :
      sprintf(s, "%sholds(%s,%s,%s)", tr ? "" : "!",  n1, n2, n3);
      break;
    case VLAD_ATOM_MEMBER :
      sprintf(s, "%smemb(%s,%s)", tr ? "" : "!", n1, n2);
      break;
    case VLAD_ATOM_SUBSET :
      sprintf(s, "%ssubst(%s,%s)", tr ? "" : "!", n1, n2);
      break;
  }
  return VLAD_OK;
}

int print_exp(numberlist l, char *s)
{
  unsigned int i;
  unsigned int n;
  char tmps[128];
  int retval;

  strcpy(s, "");

  for (i = 0; i < l.length(); i++) {
    if ((retval = l.getn(i, &n)) != VLAD_OK) {
      fprintf(stderr, "internal error: %d\n", retval);
      return retval;
    }

    if ((retval = print_atom(n, tmps)) != VLAD_OK) {
      fprintf(stderr, "internal error: %d\n", retval);
      return retval;
    }

    sprintf(s, "%s %s", s, tmps);
  }

  return VLAD_OK;
}
#endif
