// error.cc -- debugging and error macros
// Copyright (C) 1999-2000 Tommi Syrjänen <Tommi.Syrjanen@hut.fi>
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//  
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//  

#include "../config.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#include "error.h"
#include "global.h"
#include "debug.h"
#include "list.h"
#include "symbol.h"
#include "term.h"
#include "predicate.h"

int indent(int amount, char *buf)
{
  int i;
  for (i = 0; i < amount; i++) {
    buf[i] = ' ';
  }
  return amount;
}

int get_word(char *source, char* target)
{
  int res = 0;

  while (*source && !isspace(*source)) {
    *target++ = *source++;
    res++;
  }
  *target = '\0';
  return res;
}

void format_message(char *msg, int indent_amount, int indent_first_line,
		    int width)
{
  int pos = 0;
  int column = 0;
  int word_length = 0;
  char *p;
  static char work_buf[BUFFER_LENGTH] = { 0 };
  static char word_buf[BUFFER_LENGTH] = { 0 };

  if (indent_amount > 8) {
    indent_amount = 8;
  }
  
  if (indent_first_line) {
    pos += indent(indent_amount, work_buf);
    column += indent_amount;
  } 
  p = msg;

  while (*p) {
    word_length = get_word(p, word_buf);

    if (word_length > 0) {
      if (column + word_length <= width) {
	column += word_length +1;
	pos += sprintf(&work_buf[pos], "%s ", word_buf);
      } else { // newline
	work_buf[pos++] = '\n';
	pos += indent(indent_amount, &work_buf[pos]);
	column = indent_amount + word_length +1;
	pos += sprintf(&work_buf[pos], "%s ", word_buf);
      }
      p += word_length;
    } else {
      p++;
    }
  }
  work_buf[pos++] = '\n';
  work_buf[pos] = '\0';

  strcpy(msg, work_buf);
}


void error(int level, char *fmt, ...)  
{
  int pos = 0;
  char *p = 0;
  int indent_level = 0;
  static char buf[BUFFER_LENGTH] = { 0 };

  va_list(ap);
  va_start(ap, fmt);
  
  pos += vsprintf(buf, fmt, ap);
  va_end(ap);
  if (pos >= BUFFER_LENGTH) {
    fprintf(stderr, "buffer overflow\n");
      abort(); // buffer owerflow, don't do anything
  }

  buf[pos] = '\0';

  // if the error string is really long, suppose that it is
  // preformatted
  if (pos > 1024) {
    fprintf(stderr, buf);
  } else {
    int found_colons = 0;
    p = buf;
    
    while (*p && found_colons < 2) {
      indent_level++;
      if (*p == ':')
	found_colons++;
      p++;
    }
    if (found_colons != 2) {
      indent_level = 4;
    }
    
    if (level >= SYS_ERR) {
      perror(buf);
    } else {
      format_message(buf, indent_level+1, false, 76);
      fprintf(stderr, buf);
    }

  }
  if (level > USR_ERR) {
    print_warnings();
    exit(E_ERROR);
  } else
    sys_data.num_errors++;
}

struct warn_label {
  char *label;
  int value;
};
  
static warn_label labels[] =
{
  { "a", WARN_ALL },
  { "all", WARN_ALL },
  { "ar", WARN_ARITY },
  { "arity", WARN_ARITY },
  { "error", WARN_ERROR },
  { "er", WARN_ERROR },
  { "extended", WARN_EXTENDED },
  { "ex", WARN_EXTENDED } ,
  { "l" ,WARN_LIBRARY },
  { "library", WARN_LIBRARY },
  { "si", WARN_SIMILAR },
  { "similar", WARN_SIMILAR },
  { "sy", WARN_SYNTAX },
  { "syntax", WARN_SYNTAX },
  { "typo", WARN_TYPO },
  { "t", WARN_TYPO },
  { "u", WARN_UNSAT },
  { "unsat", WARN_UNSAT },
  { "w", WARN_WEIGHT },
  { "weight", WARN_WEIGHT },
  { "*", WARN_NONE }
};


int get_warn_from_string(char *st)
{
  int i = 0;

  while (strcmp(labels[i].label,st) && (*labels[i].label != '*')) { 
    i++;
  }
  return labels[i].value;
}

int enabled_warning(int cause)
{
  if (cause & sys_data.warnings)
    return 1;
  else
    return 0;
}

void warn(int cause, long lineno, char *fmt, ...)
{
  static char warn_buf[BUFFER_LENGTH] = { 0 };
  int pos = 0;
  int indent_level = 0;
  if (!(cause & sys_data.warnings))
    return;

  if (lineno) {
    pos += sprintf(warn_buf, "%s: %s: ", error_file_and_line(lineno),
		   (sys_data.abort_when_warn) ? "Error" : "Warning");
  } else {
    pos += sprintf(warn_buf, "%s: ", (sys_data.abort_when_warn) ? 
		   "Error" : "Warning");
  }
  indent_level = pos;

  if (pos >= BUFFER_LENGTH) {
    fprintf(stderr, "buffer overflow");
    abort();
  }
  
  va_list(ap);
  va_start(ap, fmt);
  pos += vsprintf(&warn_buf[pos], fmt, ap);
  va_end(ap);

  if (pos >= BUFFER_LENGTH) {
    fprintf(stderr, "buffer overflow");
    abort();
  }
  warn_buf[pos] = '\0';

  format_message(warn_buf, indent_level, false, 76);
  
  warning_list->Insert(clone_string(warn_buf), lineno);
  
  if (sys_data.abort_when_warn)
    sys_data.num_errors++;
};

// for debugging, prints out all enabled warnings 
void decode_warnings(int w)
{
  fprintf(stderr, "enabled warnings: ");
  if ( w & WARN_WEIGHT) {
    fprintf(stderr, "weight, ");
  }

  if( w & WARN_ARITY ) {
    fprintf(stderr, "arity, " );
  }

  if( w & WARN_UNSAT ) {
    fprintf(stderr, "unsat, " );
  }

  if( w & WARN_SIMILAR ) {
    fprintf(stderr, "similar, " );
  }

  //  if( w & WARN_TYPO ) {
  //    fprintf(stderr, "typo, " );
  //  }

  if (w & WARN_LIBRARY) {
    fprintf(stderr, "library, " );
  }

  fprintf(stderr, "\n");
}

void print_warnings()
{
  char *st = NULL;

  while ((st = warning_list->Iterate())) {
    fprintf(stderr, "%s\n", st);
  }
  warning_list->Clear();
}

  
RuntimeError::RuntimeError(ErrorType t, long a1, long a2)
{
  type = t;
  arg1 = a1;
  arg2 = a2;
}

static const char *error_messages[] = {
  "invalid argument type",
  "comparison of incompatible types:",
  "divide by zero",
  "incorrect number of arguments",
  "argument out of range",
  "result overflow",
  "trying to find the size of the extension of a non-domain predicate"
};

const char *RuntimeError::GetErrorMessage()
{
  return error_messages[type];
}

void print_argument(long x)
{
  if (IS_NUMBER(x)) {
    fprintf(stderr, "%ld", GET_VALUE(x));
  } else {
    char *st = constant_table->LookupByValue(x);

    if (!st)
      int_error("missing constant", "");

    fprintf(stderr, "%s", st);
  }
}

void RuntimeError::Print()
{
  fprintf(stderr, "%s ", GetErrorMessage());

  switch (type) {
  case ERR_ARGUMENT:
    fprintf(stderr, "for argument '");
    print_argument(arg1);
    fprintf(stderr, "'");
    break;
  case ERR_INVALID_COMPARE:
    fprintf(stderr, "'");
    print_argument(arg1);
    fprintf(stderr, "' and '");
    print_argument(arg2);
    fprintf(stderr, "' can't be compared");
    break;
  case ERR_DIVIDE_BY_ZERO:
  case ERR_OVERFLOW:
    break;
  case ERR_NUMARGS:
    fprintf(stderr, "- excepting %ld arguments.", arg1);
    break;
  case ERR_RANGE:
    fprintf(stderr, "- for argument `%ld'", arg1);
    break;
  case ERR_NONFIXED_EXTENSION:
    fprintf(stderr, " of the predicate '%s'",
	    predicates[arg1]->Name());
    break;
  default:
    int_error("invalid error type", "");
  }
  fprintf(stderr, "\n");
}

void runtime_error(InternalFunction, ErrorType t, long a1,
		   long a2)  
{
  RuntimeError rt(t, a1, a2);

#ifdef CXX_HAS_NO_EXCEPTIONS
  rt.Print();
  fprintf(stderr, " in function '%s'\n", function_strings[fun]);
  
  exit(1);
#else
  throw rt;
#endif
}

char *error_file_and_line(long lineno)
{
  static char buf[BUFFER_LENGTH] = { 0 };
  if (lineno == 0) {
    buf[0] = '\0';
    return buf;
  }
  
  int file_number = sys_data.num_input_files -1;

  if (sys_data.num_input_files <= 1) {
    sprintf(buf, "%ld", lineno);
  } else {
    
    while (sys_data.file_start_lines[file_number] > lineno)
      file_number--;
    
    if (strlen(sys_data.input_files[file_number]) > (BUFFER_LENGTH - 10))
      error(SYS_ERR, "buffer overflow");
    
    sprintf(buf, "%s:%ld", sys_data.input_files[file_number],
	    lineno-sys_data.file_start_lines[file_number]+1);
  }
  return buf;
}
