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

%option nounput
%option noyywrap
%option never-interactive
%s COMMENT

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "policy.tab.h"

#undef YY_INPUT
#define YY_INPUT(a_buf, a_res, a_max) \
  (a_res = custom_yyinput(custom_stream, a_buf, a_max))

/* func to register a new yyinput func */
void policy_set_yyinput(int (*a_func)(void *, char *, int), void *a_stream);

/* the default libc fread yyinput */
static int default_yyinput(void *a_stream, char *a_buf, int a_max);
/* some vars */
static int (*custom_yyinput)(void *, char *, int) = &default_yyinput;
static void *custom_stream = &policyin;
%}

begin_comment      \/\*
end_comment        \*\/
underscore         _
open_parent        \(
close_parent       \)
comma              \,
semicolon          ;
slash              \/
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
group              group
sub                sub
acc                acc
obj                obj
digit              [0-9]
number             ({digit})+
alpha              [a-zA-Z]
alphanum           {alpha}|{digit}|{underscore}|{slash}
identifier         ({alphanum}){1,128}
whitespace         ([\t ])+
newline            \n
everything         .

%%

<INITIAL>{open_parent} return MODVLAD_SYM_OPEN_PARENT;

<INITIAL>{close_parent} return MODVLAD_SYM_CLOSE_PARENT;

<INITIAL>{comma} return MODVLAD_SYM_COMMA;

<INITIAL>{semicolon} return MODVLAD_SYM_SEMICOLON;

<INITIAL>{and} return MODVLAD_SYM_AND;

<INITIAL>{not} return MODVLAD_SYM_NOT;

<INITIAL>{holds} return MODVLAD_SYM_HOLDS;

<INITIAL>{member} return MODVLAD_SYM_MEMB;

<INITIAL>{subset} return MODVLAD_SYM_SUBST;

<INITIAL>{initially} return MODVLAD_SYM_INITIALLY;

<INITIAL>{implied} return MODVLAD_SYM_IMPLIED;

<INITIAL>{by} return MODVLAD_SYM_BY;

<INITIAL>{with} return MODVLAD_SYM_WITH;

<INITIAL>{absence} return MODVLAD_SYM_ABSENCE;

<INITIAL>{always} return MODVLAD_SYM_ALWAYS;

<INITIAL>{causes} return MODVLAD_SYM_CAUSES;

<INITIAL>{if} return MODVLAD_SYM_IF;

<INITIAL>{group} return MODVLAD_SYM_GROUP;

<INITIAL>{sub} return MODVLAD_SYM_SUB;

<INITIAL>{acc} return MODVLAD_SYM_ACC;

<INITIAL>{obj} return MODVLAD_SYM_OBJ;

<INITIAL>{identifier} {
  if (policytext == NULL) {
    strcpy(policylval.ident, "");
    return MODVLAD_SYM_ERROR;
  }

  strcpy(policylval.ident, policytext);

  return MODVLAD_SYM_IDENTIFIER;
}

<INITIAL>{begin_comment} {
  BEGIN COMMENT;
}

<INITIAL>{end_comment} {
  return MODVLAD_SYM_ERROR;
}

<COMMENT>{end_comment} {
  BEGIN INITIAL;
}

<COMMENT><<EOF>> {
  return MODVLAD_SYM_ERROR;
}

{whitespace} ;

{newline} {
}

<COMMENT>{everything} ;

<INITIAL>{everything} {
  return MODVLAD_SYM_ERROR;
}

%%

/* the default libc fread yyinput */
static int default_yyinput(void *a_stream, char *a_buf, int a_max)
{
  int size;
  FILE *tmp = (FILE *) a_stream;
                                                                                                                           
  /* if an error is encountered, assume EOF */
  if (((size = fread(a_buf, 1, a_max, tmp)) == 0) && ferror(tmp)) {
    *a_buf = EOF;
    return 1;
  }
 
  return size;
}

/* func to register a new yyinput func */
void policy_set_yyinput(int (*a_func)(void *, char *, int), void *a_stream)
{
  custom_yyinput = (a_func) ? a_func : &default_yyinput;
  custom_stream = (a_stream) ? a_stream : (void *) policyin;
}
