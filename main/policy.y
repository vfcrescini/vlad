/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004, 2005 University of Western Sydney
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

/* vars defined from the scanner */
extern unsigned int policylinenum;

/* local vars */
static FILE *fin = NULL;
static FILE *fout = NULL;
static FILE *ferr = NULL;
static vlad_polbase *pbase = NULL;
static int errorcode = VLAD_FAILURE;
static bool initialised = false;

#ifdef VLAD_DEBUG
static unsigned int cnt_init = 0;
static unsigned int cnt_const = 0;
static unsigned int cnt_update = 0;
#endif

/* functions from scanner */
int policyinit(FILE *a_in, FILE *a_out);
int policyerror(char *a_error);
int policylex();

/* available functions */
int policy_init(FILE *a_in, FILE *a_out, FILE *a_err, vlad_polbase *a_pbase);
int policy_parse();

/* convenience functions */
int add_identifier(const char *a_name, unsigned char a_type);

#ifdef YYBYACC
int policyparse();
#endif

%}

%union {
  unsigned int terminal;
  char identifier[VLAD_MAXLEN_IDENT];
  vlad_fact *fct;
  vlad_expression *exp;
  vlad_varlist *vlist;
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
%token <terminal> VLAD_SYM_SUBSINTYPE
%token <terminal> VLAD_SYM_OBJSINTYPE
%token <terminal> VLAD_SYM_ACCSINTYPE
%token <terminal> VLAD_SYM_SUBGRPTYPE
%token <terminal> VLAD_SYM_OBJGRPTYPE
%token <terminal> VLAD_SYM_ACCGRPTYPE
%token <terminal> VLAD_SYM_IDENT
%token <identifier> VLAD_SYM_IDENTIFIER
%type <fct> fact
%type <fct> boolean_fact
%type <fct> holds_fact
%type <fct> subst_fact
%type <fct> memb_fact
%type <exp> expression
%type <exp> implied_clause
%type <exp> with_clause
%type <exp> if_clause
%type <vlist> update_var_list
%type <vlist> update_var_def

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
  ident_section initial_section constraint_section update_section {
    int retval;
    /* after the body, we must close the polbase */
    if ((retval = pbase->close_polbase()) != VLAD_OK) {
      errorcode = retval;
      policyerror("unable to close policy base");
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
    if ((retval = pbase->close_symtab()) != VLAD_OK) {
      errorcode = retval;
      policyerror("unable to close symtab");
      return retval;
    }
  }
  | ident_stmt_list {
    int retval;
    /* after the ident section, we must close the symbol table */
    if ((retval = pbase->close_symtab()) != VLAD_OK) {
      errorcode = retval;
      policyerror("unable to close symtab");
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

update_section : {
  }
  | update_stmt_list {
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

update_stmt_list :
  update_stmt
  | update_stmt_list update_stmt
  ;

ident_stmt :
  VLAD_SYM_IDENT ident_declaration VLAD_SYM_SEMICOLON {
  }
  ;

ident_declaration :
  sub_sin_ident_decl
  | obj_sin_ident_decl
  | acc_sin_ident_decl
  | sub_grp_ident_decl
  | obj_grp_ident_decl
  | acc_grp_ident_decl
  ;

sub_sin_ident_decl :
  VLAD_SYM_SUBSINTYPE sub_sin_ident_list {
  }
  ;

obj_sin_ident_decl :
  VLAD_SYM_OBJSINTYPE obj_sin_ident_list {
  }
  ;

acc_sin_ident_decl :
  VLAD_SYM_ACCSINTYPE acc_sin_ident_list {
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

sub_sin_ident_list :
  VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_identifier($1, VLAD_IDENT_SUB_SIN)) != VLAD_OK)
      return retval;
  }
  | sub_sin_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_identifier($3, VLAD_IDENT_SUB_SIN)) != VLAD_OK)
      return retval;
  }
  ;

obj_sin_ident_list :
  VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_identifier($1, VLAD_IDENT_OBJ_SIN)) != VLAD_OK)
      return retval;
  }
  | obj_sin_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_identifier($3, VLAD_IDENT_OBJ_SIN)) != VLAD_OK)
      return retval;
  }
  ;

acc_sin_ident_list :
  VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_identifier($1, VLAD_IDENT_ACC_SIN)) != VLAD_OK)
      return retval;
  }
  | acc_sin_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_identifier($3, VLAD_IDENT_ACC_SIN)) != VLAD_OK)
      return retval;
  }
  ;

sub_grp_ident_list :
  VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_identifier($1, VLAD_IDENT_SUB_GRP)) != VLAD_OK)
      return retval;
  }
  | sub_grp_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_identifier($3, VLAD_IDENT_SUB_GRP)) != VLAD_OK)
      return retval;
  }
  ;

obj_grp_ident_list :
  VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_identifier($1, VLAD_IDENT_OBJ_GRP)) != VLAD_OK)
      return retval;
  }
  | obj_grp_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_identifier($3, VLAD_IDENT_OBJ_GRP)) != VLAD_OK)
      return retval;
  }
  ;

acc_grp_ident_list :
  VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_identifier($1, VLAD_IDENT_ACC_GRP)) != VLAD_OK)
      return retval;
  }
  | acc_grp_ident_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_identifier($3, VLAD_IDENT_ACC_GRP)) != VLAD_OK)
      return retval;
  }
  ;

initial_stmt :
  VLAD_SYM_INITIALLY expression VLAD_SYM_SEMICOLON {
    int retval;
    unsigned int i;
    vlad_fact *f;
#ifdef VLAD_DEBUG
    char s[VLAD_MAXLEN_STR];
#endif

    /*
     * we must, unfortunetly, go through the expression and add them one
     * at a time to ensure uniqueness and integrity.
     */

    for (i = 0; i < VLAD_LIST_LENGTH($2); i++) {
      if ((retval = $2->get(i, &f)) != VLAD_OK) {
        errorcode = retval;
        policyerror("invalid fact");
        return retval;
      }

      if ((retval = pbase->add_inittab(f)) != VLAD_OK) {
        errorcode = retval;
        policyerror("could not add fact into initial state table");
        return retval;
      }

#ifdef VLAD_DEBUG
      f->print(s);
      fprintf(ferr, "initial state[%d]: %s\n", cnt_init++, s);
#endif

    }

    /* when everything's been registered into the polbase, delete the expression */
    VLAD_MEM_DELETE($2);
  }
  ;

constraint_stmt : VLAD_SYM_ALWAYS expression implied_clause with_clause VLAD_SYM_SEMICOLON {
    int retval;
#ifdef VLAD_DEBUG
    char e[VLAD_MAXLEN_STR];
    char c[VLAD_MAXLEN_STR];
    char n[VLAD_MAXLEN_STR];
#endif
    if ((retval = pbase->add_consttab($2, $3, $4)) != VLAD_OK) {
      errorcode = retval;
      policyerror("could not add constraint into constraint table");
      return retval;
    }

#ifdef VLAD_DEBUG
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
    VLAD_MEM_DELETE($2);
    if ($3 != NULL)
      VLAD_MEM_DELETE($3);
    if ($4 != NULL)
      VLAD_MEM_DELETE($4);
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

update_stmt :
  VLAD_SYM_IDENTIFIER update_var_def VLAD_SYM_CAUSES expression if_clause VLAD_SYM_SEMICOLON {
    int retval;
#ifdef VLAD_DEBUG
    char v[VLAD_MAXLEN_STR];
    char pr[VLAD_MAXLEN_STR];
    char po[VLAD_MAXLEN_STR];
#endif

    if ((retval = pbase->add_updatetab($1, $2, $5, $4)) != VLAD_OK) {
      errorcode = retval;
      policyerror("could not add update into update table");
      return retval;
    }

#ifdef VLAD_DEBUG
    if ($2 != NULL)
      $2->print(v);
    else
      strcpy(v, "none");

    if ($5 != NULL)
      $5->print(pr);
    else
      strcpy(pr, "none");

    $4->print(po);

    fprintf(ferr, "update[%d]:\n", cnt_update++);
    fprintf(ferr, "  name:        %s\n", $1);
    fprintf(ferr, "  varlist:    %s\n", v);
    fprintf(ferr, "  precond:    %s\n", pr);
    fprintf(ferr, "  postcond:   %s\n", po);
#endif

    /* cleanup */
    if ($2 != NULL)
      VLAD_MEM_DELETE($2);
    if ($5 != NULL)
      VLAD_MEM_DELETE($5);
    VLAD_MEM_DELETE($4);
  }
  ;

if_clause : {
    $$ = NULL;
  }
  | VLAD_SYM_IF expression {
    $$ = $2;
  }
  ;

update_var_def :
  VLAD_SYM_OPEN_PARENT VLAD_SYM_CLOSE_PARENT {
    $$ = NULL;
  }
  | VLAD_SYM_OPEN_PARENT update_var_list VLAD_SYM_CLOSE_PARENT {
    $$ = $2;
  }
  ;

update_var_list :
  VLAD_SYM_IDENTIFIER {
    int retval;

    if (($$ = VLAD_MEM_NEW(vlad_varlist())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      policyerror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->add($1)) != VLAD_OK) {
      errorcode = retval;
      policyerror("could not add variable into var list");
      return retval;
    }
  }
  | update_var_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    int retval;

    if ((retval = $$->add($3)) != VLAD_OK) {
      errorcode = retval;
      policyerror("could not add variable into var list");
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
      policyerror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->add($1)) != VLAD_OK) {
      errorcode = retval;
      policyerror("could not add fact into expression");
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
        policyerror("invalid fact");
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

    if (($$ = VLAD_MEM_NEW(vlad_fact())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      policyerror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->init_holds($3, $5, $7, true)) != VLAD_OK) {
      errorcode = retval;
      policyerror("could not initialise holds fact");
      return retval;
    }
  }
  ;

subst_fact :
  VLAD_SYM_SUBST VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;

    if (($$ = VLAD_MEM_NEW(vlad_fact())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      policyerror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->init_subset($3, $5, true)) != VLAD_OK) {
      errorcode = retval;
      policyerror("could not initialise subset fact");
      return retval;
    }
  }
  ;

memb_fact :
  VLAD_SYM_MEMB VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;

    if (($$ = VLAD_MEM_NEW(vlad_fact())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      policyerror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->init_member($3, $5, true)) != VLAD_OK) {
      errorcode = retval;
      policyerror("could not initialise member fact");
      return retval;
    }
  }
  ;

%%

int add_identifier(const char *a_name, unsigned char a_type)
{
  if (!VLAD_IDENT_IS_VALID(a_type)) {
    errorcode = VLAD_INVALIDINPUT;
    policyerror("invalid identifier");
    return VLAD_INVALIDINPUT;
  }

  switch (pbase->add_symtab(a_name, a_type)) {
    case VLAD_OK :
#ifdef VLAD_DEBUG
      switch (a_type) {
        case VLAD_IDENT_SUB_SIN :
          fprintf(ferr, "declared identifier subject: %s\n", a_name);
          break;
        case VLAD_IDENT_ACC_SIN :
          fprintf(ferr, "declared identifier access: %s\n", a_name);
          break;
        case VLAD_IDENT_OBJ_SIN :
          fprintf(ferr, "declared identifier object: %s\n", a_name);
          break;
        case VLAD_IDENT_SUB_GRP :
           fprintf(ferr, "declared identifier subject-group: %s\n", a_name);
          break;
        case VLAD_IDENT_ACC_GRP :
           fprintf(ferr, "declared identifier access-group: %s\n", a_name);
          break;
        case VLAD_IDENT_OBJ_GRP :
           fprintf(ferr, "declared identiifer object-group: %s\n", a_name);
          break;
      }
#endif
      break;
    case VLAD_DUPLICATE :
      errorcode = VLAD_DUPLICATE;
      policyerror("identifier already declared");
      return VLAD_DUPLICATE;
    case VLAD_MALLOCFAILED :
      errorcode = VLAD_MALLOCFAILED;
      policyerror("memory overflow");
      return VLAD_MALLOCFAILED;
    default :
      errorcode = VLAD_FAILURE;
      policyerror("cannot add identifier to symtab: unexpected error");
      return VLAD_FAILURE;
  }

  return VLAD_OK;
}

int policyerror(char *a_error)
{
  fprintf(ferr, "line %d (error %d) %s\n", policylinenum, errorcode, a_error);

  return 0;
}

int policy_init(FILE *a_in, FILE *a_out, FILE *a_err, vlad_polbase *a_pbase)
{
  int retval;

  if (a_in == NULL || a_out == NULL || a_err == NULL || a_pbase == NULL)
    return VLAD_NULLPTR;

  if ((retval = policyinit(a_in, a_out)) != VLAD_OK)
    return retval;

  fin = a_in;
  fout = a_out;
  ferr = a_err;
  pbase = a_pbase;
  initialised = true;

  return VLAD_OK;
}

int policy_parse()
{
  if (!initialised)
    return VLAD_UNINITIALISED;

  return policyparse();
}
