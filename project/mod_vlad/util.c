/*
 * util.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include "apr_file_io.h"

#include "util.h"

/* a version of yyinput that uses apache apr */
int modvlad_apache_yyinput(void *a_stream, char *a_buf, int a_max)
{
  apr_size_t size = (apr_size_t) a_max;
  apr_file_t *file = (apr_file_t *) a_stream;

  apr_file_read(file, (void *) a_buf, &size);

  return size;
}

/* a version of yyinput that uses libc fread */
int modvlad_default_yyinput(void *a_stream, char *a_buf, int a_maxsize)
{
  int size;
  FILE *tmp = (FILE *) a_stream;

  /* if an error is encountered, assume EOF */
  if (((size = fread(a_buf, 1, a_maxsize, tmp)) == 0) && ferror(tmp)) {
    *a_buf = EOF;
    return 1;
  }

  return size;
}
