%{
/*
 * Vino Crescini
 * 2002 May 30
 *
 */

#include "symbols.h"

%}

%%

\( {
  return EPI_OPENBRACKET;
}

\) {
  return EPI_CLOSEBRACKET;
}

, {
  return EPI_COMMA;
}

; {
  return EPI_SEMICOLON;
}

&& { 
  return EPI_AND;
}

\|\| { 
  return EPI_OR;
}

! {
  return EPI_NOT;
}

elt {
  return EPI_ELEMENT;
}

cont {
  return EPI_CONTAIN;
}

gholds { 
  return EPI_GHOLDS;
}

sholds {
  return EPI_SHOLDS;
}

#.*\n {
}

[a-z][a-zA-Z0-9_]* {
  printf("identifier\n");
  return EPI_IDENTIFIER;
}

[A-Z][a-zA-Z0-9_]* {
  printf("variable\n");
  return EPI_VARIABLE;
}

%%

main() 
{
  yylex();
}
