/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004 University of Western Sydney
 * by Vino Fernando Crescini <jcrescin@cit.uws.edu.au>
 *
 * PolicyUpdater is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * PolicyUpdater is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PolicyUpdater; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

%{
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <new>

#include <vlad/vlad.h>
#include <vlad/polbase.h>

/* vars defined from the scanner */
extern int agentlineno;

/* local vars */
static FILE *fin = NULL;
static FILE *fout = NULL;
static FILE *ferr = NULL;
static polbase *pbase = NULL;
static unsigned char mode = VLAD_MODE_GENERATE;
static int errorcode = VLAD_FAILURE;
static bool initialised = false;

#ifdef VLAD_DEBUG
static unsigned int cnt_agent = 0;
#endif

/* functions from scanner */
int agentinit(FILE *a_in, FILE *a_out);
int agenterror(char *a_error);
int agentlex();

/* available functions */
int agent_init(FILE *a_in,
                   FILE *a_out,
                   FILE *a_err,
                   polbase *a_pbase,
                   unsigned char a_m);
int agent_parse();

#ifdef YYBYACC
int agentparse();
#endif
%}

%union {
  unsigned int terminal;
  char identifier[VLAD_MAXLEN_IDENT];
  unsigned int number;
  fact *fct;
  expression *exp;
  stringlist *vlist;
  updateref *uref;
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
%token <terminal> VLAD_SYM_COMPUTE
%token <terminal> VLAD_SYM_SEQUENCE
%token <terminal> VLAD_SYM_ADD
%token <terminal> VLAD_SYM_DELETE
%token <terminal> VLAD_SYM_LIST
%token <number> VLAD_SYM_NUMBER
%token <identifier> VLAD_SYM_IDENTIFIER
%type <fct> fact
%type <fct> boolean_fact
%type <fct> holds_fact
%type <fct> subst_fact
%type <fct> memb_fact
%type <exp> expression
%type <vlist> update_ref_ident_args
%type <vlist> update_ref_ident_list
%type <uref> update_ref_def

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
  statement
  | body statement
  ;

destroy :
  {
  }
  ;

statement :
  query_stmt
  | compute_stmt
  | sequence_stmt
  ;

query_stmt :
  VLAD_SYM_QUERY expression VLAD_SYM_SEMICOLON {
    int retval;
#ifdef VLAD_SMODELS
    unsigned char res;
#endif
#ifdef VLAD_DEBUG
    char q[VLAD_MAXLEN_STR];
#endif

  switch(mode) {
    case VLAD_MODE_GENERATE : {
      switch(retval = pbase->query_generate($2, fout)) {
        case VLAD_OK :
          break;
        case VLAD_INVALIDOP :
          errorcode = retval;
          agenterror("must use compute before query");
          return retval;
        default :
          errorcode = retval;
          agenterror("could not evaluate query: unexpected error");
          return retval;
      }
      break;
    }
#ifdef VLAD_SMODELS
    case VLAD_MODE_EVALUATE : {
      switch(retval = pbase->query_evaluate($2, &res)) {
        case VLAD_OK :
          fprintf(fout, "%s\n", VLAD_RESULT_STRING(res));
          break;
        case VLAD_INVALIDOP :
          errorcode = retval;
          agenterror("must use compute before query");
          return retval;
        default :
          errorcode = retval;
          agenterror("could not evaluate query: unexpected error");
          return retval;
      }
      break;
    }
#endif
    default :
      errorcode = VLAD_FAILURE;
      agenterror("invalid mode");
      return VLAD_FAILURE;
  }

#ifdef VLAD_DEBUG
    $2->print(q);

    fprintf(ferr, "query[%d]:\n", cnt_agent++);
    fprintf(ferr, "  expression: %s\n", q);
#endif

    /* cleanup */
    delete $2;
  }
  ;

compute_stmt : VLAD_SYM_COMPUTE VLAD_SYM_SEMICOLON {
    int retval;

    switch(mode) {
      case VLAD_MODE_GENERATE :
        switch(retval = pbase->compute_generate(fout)) {
          case VLAD_OK :
            break;
          default :
            errorcode = retval;
            agenterror("could not compute: unexpected error");
            return retval;
        }
        break;
#ifdef VLAD_SMODELS
      case VLAD_MODE_EVALUATE :
        switch(retval = pbase->compute_evaluate()) {
          case VLAD_OK :
            break;
          case VLAD_NOMODEL :
            errorcode = retval;
            agenterror("could not compute: conflict encountered");
            return VLAD_NOMODEL;
          default :
            errorcode = retval;
            agenterror("could not compute: unexpected error");
            return retval;
        }
        break;
#endif
      default :
        errorcode = VLAD_FAILURE;
        agenterror("invalid mode");
        return VLAD_FAILURE;
    }
  }
  ;

sequence_stmt :
  VLAD_SYM_SEQUENCE sequence_cmd_clause VLAD_SYM_SEMICOLON {
  }
  ;

sequence_cmd_clause :
  sequence_add_clause
  | sequence_delete_clause
  | sequence_list_clause
  ;

sequence_add_clause :
  VLAD_SYM_ADD update_ref_def {
    int retval;

    if ((retval = pbase->add_seqtab($2)) != VLAD_OK) {
      errorcode = retval;
      agenterror("could not add sequence to sequence table");
      return retval;
    }
  }
  ;

sequence_delete_clause :
  VLAD_SYM_DELETE VLAD_SYM_NUMBER {
    int retval;

    if ((retval = pbase->del_seqtab($2)) != VLAD_OK) {
      errorcode = retval;
      agenterror("could not delete sequence from sequence table");
      return retval;
    }
  }
  ;

sequence_list_clause :
  VLAD_SYM_LIST {
    int retval;

    if ((retval = pbase->list_seqtab(fout)) != VLAD_OK) {
      errorcode = retval;
      agenterror("could not list sequence from sequence table");
      return retval;
    }
  }
  ;

update_ref_def :
  VLAD_SYM_IDENTIFIER VLAD_SYM_OPEN_PARENT update_ref_ident_args VLAD_SYM_CLOSE_PARENT {
    int retval;
    char *name;

    /* first allocate memory for the name */
    if ((name = VLAD_STRING_MALLOC($1)) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      agenterror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    strcpy(name, $1);

    /* then add the entire thing into a updateref */
    if (($$ = VLAD_NEW(updateref())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      agenterror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->init(name, $3)) != VLAD_OK) {
      errorcode = retval;
      agenterror("could not initialise update reference");
      return retval;
    }
  }
  ;

update_ref_ident_args : {
    $$ = NULL;
  }
  | update_ref_ident_list {
    $$ = $1;
  }
  ;

update_ref_ident_list :
  VLAD_SYM_IDENTIFIER {
    int retval;

    if (($$ = VLAD_NEW(stringlist())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      agenterror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->add($1)) != VLAD_OK) {
      errorcode = retval;
      agenterror("could not add identifier to update reference");
      return retval;
    }
  }
  | update_ref_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    int retval;

    if ((retval = $$->add($3)) != VLAD_OK) {
      errorcode = retval;
      agenterror("could not add identifier to update reference");
      return retval;
    }
  }
  ;

logical_op :
  VLAD_SYM_AND {
  }
  ;

expression :
  boolean_fact {
    int retval;

    if (($$ = VLAD_NEW(expression())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      agenterror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->add($1)) != VLAD_OK) {
      errorcode = retval;
      agenterror("could not add fact to expression");
      return retval;
    }
  }
  | expression logical_op boolean_fact {
    int retval;
    switch ((retval = $$->add($3))) {
      case VLAD_OK :
      case VLAD_DUPLICATE :
        /* we simply ignore duplicates */
        break;
      default :
        errorcode = retval;
        agenterror("could not add fact to expression");
        return retval;
    }
  }
  ;

boolean_fact :
  fact {
    $$ = $1;
  }
  | VLAD_SYM_NOT fact {
    $$ = $2;
    $$->negate();
  }
  ;

fact :
  holds_fact {
    $$ = $1;
  }
  | subst_fact {
    $$ = $1;
  }
  | memb_fact {
    $$ = $1;
  }
  ;

holds_fact :
  VLAD_SYM_HOLDS VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;

    if (($$ = VLAD_NEW(fact())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      agenterror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->init_holds($3, $5, $7, true)) != VLAD_OK) {
      errorcode = retval;
      agenterror("could not initialise holds fact");
      return retval;
    }
  }
  ;

subst_fact :
  VLAD_SYM_SUBST VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;

    if (($$ = VLAD_NEW(fact())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      agenterror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->init_subset($3, $5, true)) != VLAD_OK) {
      errorcode = retval;
      agenterror("could not initialise subset fact");
      return retval;
    }
  }
  ;

memb_fact :
  VLAD_SYM_MEMB VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;

    if (($$ = VLAD_NEW(fact())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      agenterror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->init_member($3, $5, true)) != VLAD_OK) {
      errorcode = retval;
      agenterror("could not initialise member fact");
      return retval;
    }
  }
  ;

%%

int agenterror(char *a_error)
{
  fprintf(ferr, "line %d (error %d) %s\n", agentlineno, errorcode, a_error);

  return 0;
}

int agent_init(FILE *a_in,
                   FILE *a_out,
                   FILE *a_err,
                   polbase *a_pbase,
                   unsigned char a_m)
{
  int retval;

  if (a_in == NULL || a_out == NULL || a_err == NULL || a_pbase == NULL)
    return VLAD_NULLPTR;

  if (a_m != VLAD_MODE_GENERATE && a_m != VLAD_MODE_EVALUATE)
    return VLAD_INVALIDINPUT;

  if ((retval = agentinit(a_in, a_out)) != VLAD_OK)
    return retval;

  fin = a_in;
  fout = a_out;
  ferr = a_err;
  pbase = a_pbase;
  mode = a_m;
  initialised = true;

  return VLAD_OK;
}

int agent_parse()
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  return agentparse();
}
