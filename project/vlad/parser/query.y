/*
 * query.y
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
extern int querylineno;

/* local vars */
static FILE *fin = NULL;
static FILE *fout = NULL;
static FILE *ferr = NULL;
static kb *kbase = NULL;
static unsigned char mode = VLAD_MODE_GENERATE;
static int errorcode = VLAD_FAILURE;
static bool initialised = false;

#ifdef DEBUG
static unsigned int cnt_query = 0;
#endif

/* functions from scanner */
int queryinit(FILE *a_in, FILE *a_out);
int queryerror(char *error);
int querylex();

/* available functions */
int query_init(FILE *a_in, FILE *a_out, FILE *a_err, kb *a_kb, unsigned char a_m);
int query_parse();
%}

%union {
  char identifier[VLAD_MAXLEN_IDENT];
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
%token <terminal> VLAD_SYM_HOLDS
%token <terminal> VLAD_SYM_MEMB
%token <terminal> VLAD_SYM_SUBST
%token <terminal> VLAD_SYM_QUERY
%token <terminal> VLAD_SYM_AFTER
%token <identifier> VLAD_SYM_IDENTIFIER
%type <atm> atom
%type <atm> boolean_atom
%type <atm> holds_atom
%type <atm> subst_atom
%type <atm> memb_atom
%type <exp> expression
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

init :
  {
  }
  ;

body :
  query_section
  ;

destroy :
  {
  }
  ;

query_section :
  | query_stmt_list
  ;

query_stmt_list :
  query_stmt
  | query_stmt_list query_stmt
  ;

query_stmt :
  VLAD_SYM_QUERY expression after_clause VLAD_SYM_SEMICOLON {
    int retval;
#ifdef DEBUG
    char q[VLAD_MAXLEN_STR];
    char r[VLAD_MAXLEN_STR];
#endif

  switch(mode) {
    case VLAD_MODE_GENERATE : {
      if ((retval = kbase->generate_nlp($2, $3, fout)) != VLAD_OK) {
        errorcode = retval;
        queryerror("could not generate nlp");
        return retval;
      }
      break;
    }
#ifdef SMODELS
    case VLAD_MODE_EVALUATE : {
      unsigned char res;
      switch(retval = kbase->evaluate_query($2, $3, &res)) {
        case VLAD_OK :
          fprintf(fout, "%s\n", VLAD_RESULT_STRING(res));
          break;
        case VLAD_NOMODEL :
          errorcode = retval;
          queryerror("could not evaluate query: conflict encountered");
          return VLAD_NOMODEL;
        default :
          errorcode = retval;
          queryerror("could not evaluate query: unexpected error");
          return retval;
      }
      break;
    }
#endif
    default :
      errorcode = VLAD_FAILURE;
      queryerror("invalid mode");
      return VLAD_FAILURE;
  }

#ifdef DEBUG
    $2->print(q);
    if ($3 != NULL)
      $3->print(r);
    else
      strcpy(r, "none");

    fprintf(ferr, "query[%d]:\n", cnt_query++);
    fprintf(ferr, "  expression: %s\n", q);
    fprintf(ferr, "  trans seq:  %s\n", r);
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
      errorcode = VLAD_MALLOCFAILED;
      queryerror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->add($1)) != VLAD_OK) {
      errorcode = retval;
      queryerror("could not add transformation reference");
      return retval;
    }
  }
  | trans_ref_list VLAD_SYM_COMMA trans_ref_def {
    int retval;

    if ((retval = $$->add($3)) != VLAD_OK) {
      errorcode = retval;
      queryerror("could not add transformation reference");
      return retval;
    }
  }
  ;

trans_ref_def :
  VLAD_SYM_IDENTIFIER VLAD_SYM_OPEN_PARENT trans_ref_ident_args VLAD_SYM_CLOSE_PARENT {
    int retval;
    char *name;

    /* first allocate memory for the name */
    if ((name = VLAD_STRING_MALLOC($1)) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      queryerror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    strcpy(name, $1);

    /* then add the entire thing into a transref */
    if (($$ = VLAD_NEW(transref())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      queryerror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->init(name, $3)) != VLAD_OK) {
      errorcode = retval;
      queryerror("could not initialise transformation reference");
      return retval;
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
      errorcode = VLAD_MALLOCFAILED;
      queryerror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->add($1)) != VLAD_OK) {
      errorcode = retval;
      queryerror("could not add identifier to transformation reference");
      return retval;
    }
  }
  | trans_ref_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    int retval;

    if ((retval = $$->add($3)) != VLAD_OK) {
      errorcode = retval;
      queryerror("could not add identifier to transformation reference");
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
      queryerror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->add($1)) != VLAD_OK) {
      errorcode = retval;
      queryerror("could not add atom to expression");
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
        queryerror("could not add atom to expression");
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
      queryerror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->init_holds($3, $5, $7, true)) != VLAD_OK) {
      errorcode = retval;
      queryerror("could not initialise holds atom");
      return retval;
    }
  }
  ;

subst_atom :
  VLAD_SYM_SUBST VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;

    if (($$ = VLAD_NEW(atom())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      queryerror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->init_subset($3, $5, true)) != VLAD_OK) {
      errorcode = retval;
      queryerror("could not initialise subset atom");
      return retval;
    }
  }
  ;

memb_atom :
  VLAD_SYM_MEMB VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;

    if (($$ = VLAD_NEW(atom())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      queryerror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->init_member($3, $5, true)) != VLAD_OK) {
      errorcode = retval;
      queryerror("could not initialise member atom");
      return retval;
    }
  }
  ;

%%

int queryerror(char *error)
{
  fprintf(ferr, "line %d (error %d) %s\n", querylineno, errorcode, error);

  return 0;
}

int query_init(FILE *a_in, FILE *a_out, FILE *a_err, kb *a_kb, unsigned char a_m)
{
  int retval;

  if (a_in == NULL || a_out == NULL || a_err == NULL || a_kb == NULL)
    return VLAD_NULLPTR;

  if (a_m != VLAD_MODE_GENERATE && a_m != VLAD_MODE_EVALUATE)
    return VLAD_INVALIDINPUT;

  if ((retval = queryinit(a_in, a_out)) != VLAD_OK)
    return retval;

  fin = a_in;
  fout = a_out;
  ferr = a_err;
  kbase = a_kb;
  mode = a_m;
  initialised = true;

  return VLAD_OK;
}

int query_parse()
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  return queryparse();
}
