/*
 * program.y
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

%{
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <new>

#include <config.h>
#include <vlad/vlad.h>
#include <vlad/kb.h>

/* vars defined from the scanner */
extern int programlineno;

/* local vars */
static FILE *fin = NULL;
static FILE *fout = NULL;
static FILE *ferr = NULL;
static kb *kbase = NULL;
static int errorcode = VLAD_FAILURE;
static bool initialised = false;

#ifdef DEBUG
static unsigned int cnt_init = 0;
static unsigned int cnt_const = 0;
static unsigned int cnt_trans = 0;
#endif

/* functions from scanner */
int programinit(FILE *a_in, FILE *a_out);
int programerror(char *error);
int programlex();

/* available functions */
int program_init(FILE *a_in, FILE *a_out, FILE *a_err, kb *a_kb);
int prgoram_parse();

/* convenience functions */
int add_identifier(const char *n, unsigned char t);

#ifdef YYBYACC
int programparse();
#endif

%}

%union {
  unsigned int terminal;
  char identifier[VLAD_MAXLEN_IDENT];
  atom *atm;
  expression *exp;
  stringlist *vlist;
}

%token <terminal> VLAD_SYM_EOF
%token <terminal> VLAD_SYM_ERROR
%token <terminal> VLAD_SYM_OPEN_PARENT
%token <terminal> VLAD_SYM_CLOSE_PARENT
%token <terminal> VLAD_SYM_COMMA
%token <terminal> VLAD_SYM_SEMICOLON
%token <terminal> VLAD_SYM_AND
%token <terminal> VLAD_SYM_NOT
%token <terminal> VLAD_SYM_HOLDS
%token <terminal> VLAD_SYM_MEMB
%token <terminal> VLAD_SYM_SUBST
%token <terminal> VLAD_SYM_INITIALLY
%token <terminal> VLAD_SYM_IMPLIED
%token <terminal> VLAD_SYM_BY
%token <terminal> VLAD_SYM_WITH
%token <terminal> VLAD_SYM_ABSENCE
%token <terminal> VLAD_SYM_ALWAYS
%token <terminal> VLAD_SYM_CAUSES
%token <terminal> VLAD_SYM_IF
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
%type <exp> expression
%type <exp> implied_clause
%type <exp> with_clause
%type <exp> if_clause
%type <vlist> trans_var_list
%type <vlist> trans_var_def

%start program

%%

program :
  init body destroy
  ;

init :
  {
  }
  ;

body :
  ident_section initial_section constraint_section trans_section {
    int retval;
    /* after the body, we must close the kb */
    if ((retval = kbase->close_kb()) != VLAD_OK) {
      errorcode = retval;
      programerror("unable to close kb");
      return retval;
    }
  }
  ;

destroy :
  {
  }
  ;

ident_section : {
    int retval;
    /* after the ident section, we must close the symbol table */
    if ((retval = kbase->close_symtab()) != VLAD_OK) {
      errorcode = retval;
      programerror("unable to close symtab");
      return retval;
    }
  }
  | ident_stmt_list {
    int retval;
    /* after the ident section, we must close the symbol table */
    if ((retval = kbase->close_symtab()) != VLAD_OK) {
      errorcode = retval;
      programerror("unable to close symtab");
      return retval;
    }
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
    char s[VLAD_MAXLEN_STR];
#endif

    /*
     * we must, unfortunetly, go through the expression and add them one
     * at a time to ensure uniqueness and integrity.
     */

    for (i = 0; i < VLAD_LIST_LENGTH($2); i++) {
      if ((retval = $2->get(i, &a)) != VLAD_OK) {
        errorcode = retval;
        programerror("invalid atom");
        return retval;
      }

      if ((retval = kbase->add_inittab(a)) != VLAD_OK) {
        errorcode = retval;
        programerror("could not add atom into initial state table");
        return retval;
      }

#ifdef DEBUG
      a->print(s);
      fprintf(ferr, "initial state[%d]: %s\n", cnt_init++, s);
#endif

    }

    /* when everything's been registered into the kb, delete the expression */
    delete $2;
  }
  ;

constraint_stmt : VLAD_SYM_ALWAYS expression implied_clause with_clause VLAD_SYM_SEMICOLON {
    int retval;
#ifdef DEBUG
    char e[VLAD_MAXLEN_STR];
    char c[VLAD_MAXLEN_STR];
    char n[VLAD_MAXLEN_STR];
#endif
    if ((retval = kbase->add_consttab($2, $3, $4)) != VLAD_OK) {
      errorcode = retval;
      programerror("could not add constraint into constraint table");
      return retval;
    }

#ifdef DEBUG
    $2->print(e);
    if ($3 != NULL)
      $3->print(c);
    else
      strcpy(c, "none");
    if ($4 != NULL)
      $4->print(n);
    else
      strcpy(n, "none");

    fprintf(ferr, "constraint[%d]:\n", cnt_const++);
    fprintf(ferr, "  expression: %s\n", e);
    fprintf(ferr, "  condition:   %s\n", c);
    fprintf(ferr, "  absence:     %s\n", n);
#endif

    /* cleanup */
    delete $2;
    if ($3 != NULL)
      delete $3;
    if ($4 != NULL)
      delete $4;
  }
  ;

implied_clause : {
    $$ = NULL;
  }
  | VLAD_SYM_IMPLIED VLAD_SYM_BY expression {
    $$ = $3;
  }
  ;

with_clause : {
    $$ = NULL;
  }
  | VLAD_SYM_WITH VLAD_SYM_ABSENCE expression {
    $$ = $3;
  }
  ;

trans_stmt :
  VLAD_SYM_IDENTIFIER trans_var_def VLAD_SYM_CAUSES expression if_clause VLAD_SYM_SEMICOLON {
    int retval;
#ifdef DEBUG
    char v[VLAD_MAXLEN_STR];
    char pr[VLAD_MAXLEN_STR];
    char po[VLAD_MAXLEN_STR];
#endif

    if ((retval = kbase->add_transtab($1, $2, $5, $4)) != VLAD_OK) {
      errorcode = retval;
      programerror("could not add transformation into trans table");
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

    fprintf(ferr, "transformation[%d]:\n", cnt_trans++);
    fprintf(ferr, "  name:        %s\n", $1);
    fprintf(ferr, "  varlist:    %s\n", v);
    fprintf(ferr, "  precond:    %s\n", pr);
    fprintf(ferr, "  postcond:   %s\n", po);
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
      errorcode = VLAD_MALLOCFAILED;
      programerror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->add($1)) != VLAD_OK) {
      errorcode = retval;
      programerror("could not add variable into var list");
      return retval;
    }
  }
  | trans_var_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    int retval;

    if ((retval = $$->add($3)) != VLAD_OK) {
      errorcode = retval;
      programerror("could not add variable into var list");
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
      errorcode = VLAD_MALLOCFAILED;
      programerror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->add($1)) != VLAD_OK) {
      errorcode = retval;
      programerror("could not add atom into expression");
      return retval;
    }
  }
  | expression logical_op boolean_atom {
    int retval;
    switch ((retval = $$->add($3))) {
      case VLAD_OK :
      case VLAD_DUPLICATE :
        /* we simply ignore duplicates */
        break;
      default :
        errorcode = retval;
        programerror("invalid atom");
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
  ;

holds_atom :
  VLAD_SYM_HOLDS VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;

    if (($$ = VLAD_NEW(atom())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      programerror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->init_holds($3, $5, $7, true)) != VLAD_OK) {
      errorcode = retval;
      programerror("could not initialise holds atom");
      return retval;
    }
  }
  ;

subst_atom :
  VLAD_SYM_SUBST VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;

    if (($$ = VLAD_NEW(atom())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      programerror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->init_subset($3, $5, true)) != VLAD_OK) {
      errorcode = retval;
      programerror("could not initialise subset atom");
      return retval;
    }
  }
  ;

memb_atom :
  VLAD_SYM_MEMB VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;

    if (($$ = VLAD_NEW(atom())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      programerror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->init_member($3, $5, true)) != VLAD_OK) {
      errorcode = retval;
      programerror("could not initialise member atom");
      return retval;
    }
  }
  ;

%%

int add_identifier(const char *n, unsigned char t)
{
  if (!VLAD_IDENT_IS_VALID(t) || t == 0) {
    errorcode = VLAD_INVALIDINPUT;
    programerror("invalid identifier");
    return VLAD_INVALIDINPUT;
  }

  switch (kbase->add_symtab(n, t)) {
    case VLAD_OK :
#ifdef DEBUG
      switch (t) {
        case VLAD_IDENT_SUBJECT :
          fprintf(ferr, "declared identifier subject: %s\n", n);
          break;
        case VLAD_IDENT_ACCESS :
          fprintf(ferr, "declared identifier access: %s\n", n);
          break;
        case VLAD_IDENT_OBJECT :
          fprintf(ferr, "declared identifier object: %s\n", n);
          break;
        case VLAD_IDENT_SUBJECT | VLAD_IDENT_GROUP :
           fprintf(ferr, "declared identifier subject-group: %s\n", n);
          break;
        case VLAD_IDENT_ACCESS | VLAD_IDENT_GROUP :
           fprintf(ferr, "declared identifier access-group: %s\n", n);
          break;
        case VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP :
           fprintf(ferr, "declared identiifer object-group: %s\n", n);
          break;
      }
#endif
      break;
    case VLAD_DUPLICATE :
      errorcode = VLAD_DUPLICATE;
      programerror("identifier already declared");
      return VLAD_DUPLICATE;
    case VLAD_MALLOCFAILED :
      errorcode = VLAD_MALLOCFAILED;
      programerror("memory overflow");
      return VLAD_MALLOCFAILED;
    default :
      errorcode = VLAD_FAILURE;
      programerror("cannot add identifier to symtab: unexpected error");
      return VLAD_FAILURE;
  }

  return VLAD_OK;
}

int programerror(char *error)
{
  fprintf(ferr, "line %d (error %d) %s\n", programlineno, errorcode, error);

  return 0;
}

int program_init(FILE *a_in, FILE *a_out, FILE *a_err, kb *a_kb)
{
  int retval;

  if (a_in == NULL || a_out == NULL || a_err == NULL || a_kb == NULL)
    return VLAD_NULLPTR;

  if ((retval = programinit(a_in, a_out)) != VLAD_OK)
    return retval;

  fin = a_in;
  fout = a_out;
  ferr = a_err;
  kbase = a_kb;
  initialised = true;

  return VLAD_OK;
}

int program_parse()
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  return programparse();
}
