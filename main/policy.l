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

%option nounput
%option noyywrap
%option noyylineno
%option always-interactive
%s COMMENT

%{
#include <cstdlib>
#include <cstring>

#include <vlad/vlad.h>
#include <vlad/polbase.h>
#include "policy.tab.h"

unsigned int policylinenum = 1;

int policyinit(FILE *a_in, FILE *a_out);
%}

begin_comment      \/\*
end_comment        \*\/
underscore         _
open_parent        \(
close_parent       \)
open_bracket       \[
close_bracket      \]
comma              \,
semicolon          ;
and                &&
not                !
holds              holds
member             memb
subset             subst
initially          initially
implied            implied
by                 by
with               with
absence            absence
always             always
causes             causes
if                 if
where              where
entity             entity
interval           interval
relation           relation
sub_grp_type       sub-grp
obj_grp_type       obj-grp
acc_grp_type       acc-grp
sub_sin_type       sub
obj_sin_type       obj
acc_sin_type       acc
equals             equals
before             before
during             during
overlaps           overlaps
meets              meets
starts             starts
finishes           finishes
digit              [0-9]
alpha              [a-zA-Z]
alphanum           {alpha}|{digit}|{underscore}
number             {digit}{1,127}
identifier         {alpha}({alphanum}){0,126}
whitespace         ([\t ])+
newline            \n
everything         .

%%

<INITIAL>{open_parent} return VLAD_SYM_OPEN_PARENT;

<INITIAL>{close_parent} return VLAD_SYM_CLOSE_PARENT;

<INITIAL>{open_bracket} return VLAD_SYM_OPEN_BRACKET;

<INITIAL>{close_bracket} return VLAD_SYM_CLOSE_BRACKET;

<INITIAL>{comma} return VLAD_SYM_COMMA;

<INITIAL>{semicolon} return VLAD_SYM_SEMICOLON;

<INITIAL>{and} return VLAD_SYM_AND;

<INITIAL>{not} return VLAD_SYM_NOT;

<INITIAL>{holds} return VLAD_SYM_HOLDS;

<INITIAL>{member} return VLAD_SYM_MEMB;

<INITIAL>{subset} return VLAD_SYM_SUBST;

<INITIAL>{initially} return VLAD_SYM_INITIALLY;

<INITIAL>{implied} return VLAD_SYM_IMPLIED;

<INITIAL>{by} return VLAD_SYM_BY;

<INITIAL>{with} return VLAD_SYM_WITH;

<INITIAL>{absence} return VLAD_SYM_ABSENCE;

<INITIAL>{always} return VLAD_SYM_ALWAYS;

<INITIAL>{causes} return VLAD_SYM_CAUSES;

<INITIAL>{if} return VLAD_SYM_IF;

<INITIAL>{where} return VLAD_SYM_WHERE;

<INITIAL>{entity} return VLAD_SYM_ENTITY;

<INITIAL>{interval} return VLAD_SYM_INTERVAL;

<INITIAL>{relation} return VLAD_SYM_RELATION;

<INITIAL>{sub_grp_type} return VLAD_SYM_SUBGRPTYPE;

<INITIAL>{obj_grp_type} return VLAD_SYM_OBJGRPTYPE;

<INITIAL>{acc_grp_type} return VLAD_SYM_ACCGRPTYPE;

<INITIAL>{sub_sin_type} return VLAD_SYM_SUBSINTYPE;

<INITIAL>{obj_sin_type} return VLAD_SYM_OBJSINTYPE;

<INITIAL>{acc_sin_type} return VLAD_SYM_ACCSINTYPE;

<INITIAL>{equals} return VLAD_SYM_EQL;

<INITIAL>{before} return VLAD_SYM_BEF;

<INITIAL>{during} return VLAD_SYM_DUR;

<INITIAL>{overlaps} return VLAD_SYM_OVR;

<INITIAL>{meets} return VLAD_SYM_MET;

<INITIAL>{starts} return VLAD_SYM_STA;

<INITIAL>{finishes} return VLAD_SYM_FIN;

<INITIAL>{number} {
  if (policytext == NULL) {
    policylval.number = 0;
    return VLAD_SYM_ERROR;
  }

  policylval.number = atoi(policytext);
  return VLAD_SYM_NUMBER;
}

<INITIAL>{identifier} {
  if (policytext == NULL) {
    strcpy(policylval.identifier, "");
    return VLAD_SYM_ERROR;
  }

  strcpy(policylval.identifier, policytext);

  return VLAD_SYM_IDENTIFIER;
}

<INITIAL>{begin_comment} {
  BEGIN COMMENT;
}

<INITIAL>{end_comment} {
  return VLAD_SYM_ERROR;
}

<COMMENT>{end_comment} {
  BEGIN INITIAL;
}

<COMMENT><<EOF>> {
  return VLAD_SYM_ERROR;
}

{whitespace} ;

{newline} {
  policylinenum++;
}

<COMMENT>{everything} ;

<INITIAL>{everything} {
  return VLAD_SYM_ERROR;
}

%%

int policyinit(FILE *a_in, FILE *a_out)
{
  if (a_in == NULL || a_out == NULL)
    return VLAD_NULLPTR;

  policyin = a_in;
  policyout = a_out;

  yyrestart(a_in);
  BEGIN INITIAL;

  return VLAD_OK;
}
