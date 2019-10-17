/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004, 2005, 2006 University of Western Sydney
 * by Vino Fernando Crescini <jcrescin@scm.uws.edu.au>
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

#include <vlad/vlad.h>
#include <vlad/mem.h>
#include <vlad/identifier.h>
#include <vlad/polbase.h>

#ifdef VLAD_TIMER
  #include <sys/time.h>
#endif

/* vars defined from the scanner */
extern unsigned int agentlinenum;

/* local vars */
static FILE *fin = NULL;
static FILE *fout = NULL;
static FILE *ferr = NULL;
static vlad_polbase *pbase = NULL;
static unsigned char mode = VLAD_MODE_GENERATE;
static int errorcode = VLAD_FAILURE;
static bool initialised = false;
#ifdef VLAD_TIMER
static bool timer = 0;
#endif

/* functions from scanner */
int agentinit(FILE *a_in, FILE *a_out);
int agenterror(const char *a_error);
int agentlex();

/* available functions */
#ifdef VLAD_TIMER
int agent_init(FILE *a_in,
               FILE *a_out,
               FILE *a_err,
               vlad_polbase *a_pbase,
               unsigned char a_m,
               bool a_t);
#else
int agent_init(FILE *a_in,
               FILE *a_out,
               FILE *a_err,
               vlad_polbase *a_pbase,
               unsigned char a_m);
#endif
int agent_parse();

#ifdef YYBYACC
int agentparse();
#endif

#ifdef VLAD_TIMER
  #define VLAD_TIME_DIFF(X,Y) \
    (((Y).tv_sec + ((Y).tv_usec / 1000000.0)) - ((X).tv_sec + ((X).tv_usec / 1000000.0)))
#endif
%}

%union {
  unsigned int terminal;
  unsigned int number;
  char identifier[VLAD_MAXLEN_IDENT];
  vlad_fact *fct;
  vlad_expression *exp;
  vlad_stringlist *vlist;
  vlad_updateref *uref;
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
#ifdef VLAD_TIMER
    struct timeval tv1;
    struct timeval tv2;
#endif

#ifdef VLAD_TIMER
    if (timer)
      gettimeofday(&tv1, NULL);
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

#ifdef VLAD_TIMER
    if (timer) {
      gettimeofday(&tv2, NULL);
      fprintf(ferr, "%f\n", VLAD_TIME_DIFF(tv1, tv2));
    }
#endif

    /* cleanup */
    VLAD_MEM_DELETE($2);
  }
  ;

compute_stmt : VLAD_SYM_COMPUTE VLAD_SYM_SEMICOLON {
    int retval;
#ifdef VLAD_TIMER
    struct timeval tv1;
    struct timeval tv2;
#endif

#ifdef VLAD_TIMER
    if (timer)
      gettimeofday(&tv1, NULL);
#endif

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

#ifdef VLAD_TIMER
    if (timer) {
      gettimeofday(&tv2, NULL);
      fprintf(ferr, "%f\n", VLAD_TIME_DIFF(tv1, tv2));
    }
#endif

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
    if ((name = VLAD_MEM_STR_MALLOC($1)) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      agenterror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    strcpy(name, $1);

    /* then add the entire thing into a updateref */
    if (($$ = VLAD_MEM_NEW(vlad_updateref())) == NULL) {
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

    if (($$ = VLAD_MEM_NEW(vlad_stringlist())) == NULL) {
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

    if (($$ = VLAD_MEM_NEW(vlad_expression())) == NULL) {
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
  VLAD_SYM_HOLDS VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;

    if (($$ = VLAD_MEM_NEW(vlad_fact())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      agenterror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->init_holds($3, $5, $7, $9, true)) != VLAD_OK) {
      errorcode = retval;
      agenterror("could not initialise holds fact");
      return retval;
    }
  }
  ;

subst_fact :
  VLAD_SYM_SUBST VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;

    if (($$ = VLAD_MEM_NEW(vlad_fact())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      agenterror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->init_subset($3, $5, $7, true)) != VLAD_OK) {
      errorcode = retval;
      agenterror("could not initialise subset fact");
      return retval;
    }
  }
  ;

memb_fact :
  VLAD_SYM_MEMB VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;

    if (($$ = VLAD_MEM_NEW(vlad_fact())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      agenterror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->init_member($3, $5, $7, true)) != VLAD_OK) {
      errorcode = retval;
      agenterror("could not initialise member fact");
      return retval;
    }
  }
  ;

%%

int agenterror(const char *a_error)
{
  fprintf(ferr, "line %d (error %d) %s\n", agentlinenum, errorcode, a_error);

  return 0;
}

#ifdef VLAD_TIMER
int agent_init(FILE *a_in,
               FILE *a_out,
               FILE *a_err,
               vlad_polbase *a_pbase,
               unsigned char a_m,
               bool a_t)
#else
int agent_init(FILE *a_in,
               FILE *a_out,
               FILE *a_err,
               vlad_polbase *a_pbase,
               unsigned char a_m)
#endif
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
#ifdef VLAD_TIMER
  timer = a_t;
#endif

  return VLAD_OK;
}

int agent_parse()
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  return agentparse();
}
