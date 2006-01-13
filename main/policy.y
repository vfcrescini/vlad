/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004, 2005, 2006 University of Western Sydney
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

#include <tribe/rel.h>

#include <vlad/vlad.h>
#include <vlad/mem.h>
#include <vlad/identifier.h>
#include <vlad/fact.h>
#include <vlad/expression.h>
#include <vlad/rel.h>
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

/* functions from scanner */
int policyinit(FILE *a_in, FILE *a_out);
int policyerror(char *a_error);
int policylex();

/* available functions */
int policy_init(FILE *a_in, FILE *a_out, FILE *a_err, vlad_polbase *a_pbase);
int policy_parse();

/* convenience functions */
int add_entity(const char *a_name, unsigned char a_type);
int add_interval(const char *a_name,
                 bool a_ep,
                 unsigned int a_ep1,
                 unsigned int a_ep2);

#ifdef YYBYACC
int policyparse();
#endif

%}

%union {
  unsigned int terminal;
  unsigned int number;
  char identifier[VLAD_MAXLEN_IDENT];
  unsigned int endpoints[2];
  vlad_fact *fct;
  vlad_expression *exp;
  vlad_rel *rel;
  vlad_varlist *vlist;
  vlad_rlist *rlist;
}

%token <terminal> VLAD_SYM_EOF
%token <terminal> VLAD_SYM_ERROR
%token <terminal> VLAD_SYM_OPEN_PARENT
%token <terminal> VLAD_SYM_CLOSE_PARENT
%token <terminal> VLAD_SYM_OPEN_BRACKET
%token <terminal> VLAD_SYM_CLOSE_BRACKET
%token <terminal> VLAD_SYM_COMMA
%token <terminal> VLAD_SYM_SEMICOLON
%token <terminal> VLAD_SYM_AND
%token <terminal> VLAD_SYM_NOT
%token <terminal> VLAD_SYM_HOLDS
%token <terminal> VLAD_SYM_MEMB
%token <terminal> VLAD_SYM_SUBST
%token <terminal> VLAD_SYM_ENTITY
%token <terminal> VLAD_SYM_INTERVAL
%token <terminal> VLAD_SYM_INITIALLY
%token <terminal> VLAD_SYM_RELATION
%token <terminal> VLAD_SYM_IMPLIED
%token <terminal> VLAD_SYM_BY
%token <terminal> VLAD_SYM_WITH
%token <terminal> VLAD_SYM_ABSENCE
%token <terminal> VLAD_SYM_ALWAYS
%token <terminal> VLAD_SYM_CAUSES
%token <terminal> VLAD_SYM_IF
%token <terminal> VLAD_SYM_WHERE
%token <terminal> VLAD_SYM_SUBSINTYPE
%token <terminal> VLAD_SYM_OBJSINTYPE
%token <terminal> VLAD_SYM_ACCSINTYPE
%token <terminal> VLAD_SYM_SUBGRPTYPE
%token <terminal> VLAD_SYM_OBJGRPTYPE
%token <terminal> VLAD_SYM_ACCGRPTYPE
%token <terminal> VLAD_SYM_EQL
%token <terminal> VLAD_SYM_BEF
%token <terminal> VLAD_SYM_DUR
%token <terminal> VLAD_SYM_OVR
%token <terminal> VLAD_SYM_MET
%token <terminal> VLAD_SYM_STA
%token <terminal> VLAD_SYM_FIN
%token <number> VLAD_SYM_NUMBER
%token <identifier> VLAD_SYM_IDENTIFIER
%type <endpoints> interval_endpoint_decl
%type <fct> fact
%type <fct> boolean_fact
%type <fct> holds_fact
%type <fct> subst_fact
%type <fct> memb_fact
%type <exp> expression
%type <exp> implied_clause
%type <exp> with_clause
%type <exp> if_clause
%type <rel> relation_atom
%type <rel> rel_eql_atom
%type <rel> rel_bef_atom
%type <rel> rel_dur_atom
%type <rel> rel_ovr_atom
%type <rel> rel_met_atom
%type <rel> rel_sta_atom
%type <rel> rel_fin_atom
%type <rlist> relation_list
%type <rlist> where_clause
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
  identifier_section initial_section relation_section constraint_section update_section {
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

identifier_section : {
    int retval;
    /* after the entity section, we must close the symbol table */
    if ((retval = pbase->close_symtab()) != VLAD_OK) {
      errorcode = retval;
      policyerror("unable to close symtab");
      return retval;
    }
  }
  | identifier_stmt_list {
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

relation_section :
  relation_stmt_list {
    int retval;
    if ((retval = pbase->close_tctab()) != VLAD_OK) {
      errorcode = retval;
      policyerror("unable to close relation table");
      return retval;
    }
  }
  | {
    int retval;
    if ((retval = pbase->close_tctab()) != VLAD_OK) {
      errorcode = retval;
      policyerror("unable to close relation table");
      return retval;
    }
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

identifier_stmt_list :
  identifier_stmt
  | identifier_stmt_list identifier_stmt
  ;

initial_stmt_list :
  initial_stmt
  | initial_stmt_list initial_stmt
  ;

relation_stmt_list :
  relation_stmt
  | relation_stmt_list relation_stmt
  ;

constraint_stmt_list :
  constraint_stmt
  | constraint_stmt_list constraint_stmt
  ;

update_stmt_list :
  update_stmt
  | update_stmt_list update_stmt
  ;

identifier_stmt :
  VLAD_SYM_ENTITY entity_declaration VLAD_SYM_SEMICOLON {
  }
  | VLAD_SYM_INTERVAL interval_declaration VLAD_SYM_SEMICOLON {
  }
  ;

entity_declaration :
  sub_sin_entity_decl
  | obj_sin_entity_decl
  | acc_sin_entity_decl
  | sub_grp_entity_decl
  | obj_grp_entity_decl
  | acc_grp_entity_decl
  ;

interval_declaration :
  interval_decl {
  }
  | interval_declaration VLAD_SYM_COMMA interval_decl {
  }
  ;

sub_sin_entity_decl :
  VLAD_SYM_SUBSINTYPE sub_sin_entity_list {
  }
  ;

obj_sin_entity_decl :
  VLAD_SYM_OBJSINTYPE obj_sin_entity_list {
  }
  ;

acc_sin_entity_decl :
  VLAD_SYM_ACCSINTYPE acc_sin_entity_list {
  }
  ;

sub_grp_entity_decl :
  VLAD_SYM_SUBGRPTYPE sub_grp_entity_list {
  }
  ;

obj_grp_entity_decl :
  VLAD_SYM_OBJGRPTYPE obj_grp_entity_list {
  }
  ;

acc_grp_entity_decl :
  VLAD_SYM_ACCGRPTYPE acc_grp_entity_list {
  }
  ;

sub_sin_entity_list :
  VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_entity($1, VLAD_IDENT_ENT_SUB_SIN)) != VLAD_OK)
      return retval;
  }
  | sub_sin_entity_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_entity($3, VLAD_IDENT_ENT_SUB_SIN)) != VLAD_OK)
      return retval;
  }
  ;

obj_sin_entity_list :
  VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_entity($1, VLAD_IDENT_ENT_OBJ_SIN)) != VLAD_OK)
      return retval;
  }
  | obj_sin_entity_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_entity($3, VLAD_IDENT_ENT_OBJ_SIN)) != VLAD_OK)
      return retval;
  }
  ;

acc_sin_entity_list :
  VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_entity($1, VLAD_IDENT_ENT_ACC_SIN)) != VLAD_OK)
      return retval;
  }
  | acc_sin_entity_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_entity($3, VLAD_IDENT_ENT_ACC_SIN)) != VLAD_OK)
      return retval;
  }
  ;

sub_grp_entity_list :
  VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_entity($1, VLAD_IDENT_ENT_SUB_GRP)) != VLAD_OK)
      return retval;
  }
  | sub_grp_entity_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_entity($3, VLAD_IDENT_ENT_SUB_GRP)) != VLAD_OK)
      return retval;
  }
  ;

obj_grp_entity_list :
  VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_entity($1, VLAD_IDENT_ENT_OBJ_GRP)) != VLAD_OK)
      return retval;
  }
  | obj_grp_entity_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_entity($3, VLAD_IDENT_ENT_OBJ_GRP)) != VLAD_OK)
      return retval;
  }
  ;

acc_grp_entity_list :
  VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_entity($1, VLAD_IDENT_ENT_ACC_GRP)) != VLAD_OK)
      return retval;
  }
  | acc_grp_entity_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_entity($3, VLAD_IDENT_ENT_ACC_GRP)) != VLAD_OK)
      return retval;
  }
  ;

interval_decl :
  VLAD_SYM_IDENTIFIER interval_endpoint_decl {
    int retval;
    if ((retval = add_interval($1, true, $2[0], $2[1])) != VLAD_OK)
      return retval;
  }
  | VLAD_SYM_IDENTIFIER {
    int retval;
    if ((retval = add_interval($1, false, 0, 0)) != VLAD_OK)
      return retval;
  }
  ;

interval_endpoint_decl :
  VLAD_SYM_OPEN_BRACKET VLAD_SYM_NUMBER VLAD_SYM_COMMA VLAD_SYM_NUMBER VLAD_SYM_CLOSE_BRACKET {
    $$[0] = $2;
    $$[1] = $4;
  }
  ;

initial_stmt :
  VLAD_SYM_INITIALLY expression VLAD_SYM_SEMICOLON {
    int retval;
    unsigned int i;
    vlad_fact *f;

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
    }

    /* when everything's been registered into the polbase, delete the expression */
    VLAD_MEM_DELETE($2);
  }
  ;

relation_stmt :
  VLAD_SYM_RELATION relation_list VLAD_SYM_SEMICOLON {
    int retval;

    if ((retval = pbase->add_tctab($2)) != VLAD_OK) {
      errorcode = retval;
      policyerror("could not add relation list into the policy base");
      return errorcode;
    }

    VLAD_MEM_DELETE($2);
  }
  ;

relation_list :
  relation_atom {
    int retval;

    if (($$ = VLAD_MEM_NEW(vlad_rlist())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      policyerror("memory overflow");
      return errorcode;
    }

    if ((retval = $$->add($1)) != VLAD_OK) {
      errorcode = retval;
      policyerror("could not add relation into list");
      return errorcode;
    }

    VLAD_MEM_DELETE($1);
  }
  | relation_list VLAD_SYM_COMMA relation_atom {
    int retval;

    if ((retval = $$->add($3)) != VLAD_OK) {
      errorcode = retval;
      policyerror("could not add relation into list");
      return errorcode;
    }

    VLAD_MEM_DELETE($3);
  }
  ;

relation_atom :
  rel_eql_atom {
    $$ = $1;
  }
  | rel_bef_atom {
    $$ = $1;
  }
  | rel_dur_atom {
    $$ = $1;
  }
  | rel_ovr_atom {
    $$ = $1;
  }
  | rel_met_atom {
    $$ = $1;
  }
  | rel_sta_atom {
    $$ = $1;
  }
  | rel_fin_atom {
    $$ = $1;
  }
  ;

rel_eql_atom :
  VLAD_SYM_EQL VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;
    unsigned int rs;

    TBE_REL_SET_CLEAR(rs);
    TBE_REL_SET_ADD(rs, TBE_REL_EQL);

    if (($$ = VLAD_MEM_NEW(vlad_rel())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      policyerror("memory overflow");
      return errorcode;
    }
    if ((retval = $$->init($3, $5, rs)) != VLAD_OK) {
      errorcode = retval;
      policyerror("could not initialise relation");
      return errorcode;
    }
  }
  ;

rel_bef_atom :
  VLAD_SYM_BEF VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;
    unsigned int rs;

    TBE_REL_SET_CLEAR(rs);
    TBE_REL_SET_ADD(rs, TBE_REL_BEF);

    if (($$ = VLAD_MEM_NEW(vlad_rel())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      policyerror("memory overflow");
      return errorcode;
    }
    if ((retval = $$->init($3, $5, rs)) != VLAD_OK) {
      errorcode = retval;
      policyerror("could not initialise relation");
      return errorcode;
    }
  }
  ;

rel_dur_atom :
  VLAD_SYM_DUR VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;
    unsigned int rs;

    TBE_REL_SET_CLEAR(rs);
    TBE_REL_SET_ADD(rs, TBE_REL_DUR);

    if (($$ = VLAD_MEM_NEW(vlad_rel())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      policyerror("memory overflow");
      return errorcode;
    }
    if ((retval = $$->init($3, $5, rs)) != VLAD_OK) {
      errorcode = retval;
      policyerror("could not initialise relation");
      return errorcode;
    }
  }
  ;

rel_ovr_atom :
  VLAD_SYM_OVR VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;
    unsigned int rs;

    TBE_REL_SET_CLEAR(rs);
    TBE_REL_SET_ADD(rs, TBE_REL_OVR);

    if (($$ = VLAD_MEM_NEW(vlad_rel())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      policyerror("memory overflow");
      return errorcode;
    }
    if ((retval = $$->init($3, $5, rs)) != VLAD_OK) {
      errorcode = retval;
      policyerror("could not initialise relation");
      return errorcode;
    }
  }
  ;

rel_met_atom :
  VLAD_SYM_MET VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;
    unsigned int rs;

    TBE_REL_SET_CLEAR(rs);
    TBE_REL_SET_ADD(rs, TBE_REL_MET);

    if (($$ = VLAD_MEM_NEW(vlad_rel())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      policyerror("memory overflow");
      return errorcode;
    }
    if ((retval = $$->init($3, $5, rs)) != VLAD_OK) {
      errorcode = retval;
      policyerror("could not initialise relation");
      return errorcode;
    }
  }
  ;

rel_sta_atom :
  VLAD_SYM_STA VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;
    unsigned int rs;

    TBE_REL_SET_CLEAR(rs);
    TBE_REL_SET_ADD(rs, TBE_REL_STA);

    if (($$ = VLAD_MEM_NEW(vlad_rel())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      policyerror("memory overflow");
      return errorcode;
    }
    if ((retval = $$->init($3, $5, rs)) != VLAD_OK) {
      errorcode = retval;
      policyerror("could not initialise relation");
      return errorcode;
    }
  }
  ;

rel_fin_atom :
  VLAD_SYM_FIN VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;
    unsigned int rs;

    TBE_REL_SET_CLEAR(rs);
    TBE_REL_SET_ADD(rs, TBE_REL_FIN);

    if (($$ = VLAD_MEM_NEW(vlad_rel())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      policyerror("memory overflow");
      return errorcode;
    }
    if ((retval = $$->init($3, $5, rs)) != VLAD_OK) {
      errorcode = retval;
      policyerror("could not initialise relation");
      return errorcode;
    }
  }
  ;

constraint_stmt : VLAD_SYM_ALWAYS expression implied_clause with_clause where_clause VLAD_SYM_SEMICOLON {
    int retval;

    if ((retval = pbase->add_consttab($2, $3, $4, $5)) != VLAD_OK) {
      errorcode = retval;
      policyerror("could not add constraint into constraint table");
      return retval;
    }

    /* cleanup */
    VLAD_MEM_DELETE($2);
    if ($3 != NULL)
      VLAD_MEM_DELETE($3);
    if ($4 != NULL)
      VLAD_MEM_DELETE($4);
    if ($5 != NULL)
      VLAD_MEM_DELETE($5);
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

where_clause : {
    $$ = NULL;
  }
  | VLAD_SYM_WHERE relation_list {
    $$ = $2;
  }
  ;

update_stmt :
  VLAD_SYM_IDENTIFIER update_var_def VLAD_SYM_CAUSES expression if_clause where_clause VLAD_SYM_SEMICOLON {
    int retval;

    if ((retval = pbase->add_updatetab($1, $2, $5, $4, $6)) != VLAD_OK) {
      errorcode = retval;
      policyerror("could not add update into update table");
      return retval;
    }

    /* cleanup */
    VLAD_MEM_DELETE($4);
    if ($2 != NULL)
      VLAD_MEM_DELETE($2);
    if ($5 != NULL)
      VLAD_MEM_DELETE($5);
    if ($6 != NULL)
      VLAD_MEM_DELETE($6);
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

    /* make sure identifier is a valid variable */
    if (vlad_identifier::validate_var_ident($1) != VLAD_OK) {
      errorcode = VLAD_INVALIDINPUT;
      policyerror("invalid variable");
      return errorcode;
    }

    if (($$ = VLAD_MEM_NEW(vlad_varlist())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      policyerror("memory overflow");
      return errorcode;
    }

    if ((retval = $$->add($1)) != VLAD_OK) {
      errorcode = retval;
      policyerror("could not add variable into var list");
      return retval;
    }
  }
  | update_var_list VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER {
    int retval;

    /* make sure identifier is a valid variable */
    if (vlad_identifier::validate_var_ident($3) != VLAD_OK) {
      errorcode = VLAD_INVALIDINPUT;
      policyerror("invalid variable");
      return errorcode;
    }

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
  VLAD_SYM_HOLDS VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;

    if (($$ = VLAD_MEM_NEW(vlad_fact())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      policyerror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->init_holds($3, $5, $7, $9, true)) != VLAD_OK) {
      errorcode = retval;
      policyerror("could not initialise holds fact");
      return retval;
    }
  }
  ;

subst_fact :
  VLAD_SYM_SUBST VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;

    if (($$ = VLAD_MEM_NEW(vlad_fact())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      policyerror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->init_subset($3, $5, $7, true)) != VLAD_OK) {
      errorcode = retval;
      policyerror("could not initialise subset fact");
      return retval;
    }
  }
  ;

memb_fact :
  VLAD_SYM_MEMB VLAD_SYM_OPEN_PARENT VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_COMMA VLAD_SYM_IDENTIFIER VLAD_SYM_CLOSE_PARENT {
    int retval;

    if (($$ = VLAD_MEM_NEW(vlad_fact())) == NULL) {
      errorcode = VLAD_MALLOCFAILED;
      policyerror("memory overflow");
      return VLAD_MALLOCFAILED;
    }

    if ((retval = $$->init_member($3, $5, $7, true)) != VLAD_OK) {
      errorcode = retval;
      policyerror("could not initialise member fact");
      return retval;
    }
  }
  ;

%%

int add_entity(const char *a_name, unsigned char a_type)
{
  /* check type */
  if (vlad_identifier::validate_ent_type(a_type)) {
    errorcode = VLAD_INVALIDINPUT;
    policyerror("invalid entity identifier type");
    return VLAD_INVALIDINPUT;
  }

  /* check identifier */
  if (vlad_identifier::validate_nvar_ident(a_name)) {
    errorcode = VLAD_INVALIDINPUT;
    policyerror("invalid entity identifier");
    return VLAD_INVALIDINPUT;
  }

  switch (pbase->add_entity(a_name, a_type)) {
    case VLAD_OK :
      break;
    case VLAD_DUPLICATE :
      errorcode = VLAD_DUPLICATE;
      policyerror("identifier already used");
      return VLAD_DUPLICATE;
    case VLAD_MALLOCFAILED :
      errorcode = VLAD_MALLOCFAILED;
      policyerror("memory overflow");
      return VLAD_MALLOCFAILED;
    default :
      errorcode = VLAD_FAILURE;
      policyerror("cannot add entity identifier to symtab: unexpected error");
      return VLAD_FAILURE;
  }

  return VLAD_OK;
}

int add_interval(const char *a_name,
                 bool a_ep,
                 unsigned int a_ep1,
                 unsigned int a_ep2)
{
  int retval;

  /* check identifier */
  if (vlad_identifier::validate_nvar_ident(a_name)) {
    errorcode = VLAD_INVALIDINPUT;
    policyerror("invalid interval identifier");
    return VLAD_INVALIDINPUT;
  }

  if (a_ep) {
    if (a_ep1 >= a_ep2) {
      errorcode = VLAD_INVALIDINPUT;
      policyerror("invalid identifier endpoints");
      return VLAD_INVALIDINPUT;
    }
    retval = pbase->add_interval(a_name, a_ep1, a_ep2);
  }
  else
    retval = pbase->add_interval(a_name);

  switch (retval) {
    case VLAD_OK :
      break;
    case VLAD_DUPLICATE :
      errorcode = VLAD_DUPLICATE;
      policyerror("identifier already used");
      return VLAD_DUPLICATE;
    case VLAD_MALLOCFAILED :
      errorcode = VLAD_MALLOCFAILED;
      policyerror("memory overflow");
      return VLAD_MALLOCFAILED;
    default :
      errorcode = VLAD_FAILURE;
      policyerror("cannot add interval identifier to symtab: unexpected error");
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
