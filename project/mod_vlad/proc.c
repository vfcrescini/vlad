/*
 * proc.c
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <string.h>

#include "apr_pools.h"
#include "apr_strings.h"

#include "proc.h"

/* format the strings with the delims and terms then write to fd */
int sendfd(apr_file_t *a_fd,
           apr_proc_mutex_t *a_mx,
           apr_array_header_t *a_array);
/* receive from fd. tokenize and store to array */
int receivefd(apr_file_t *a_fd,
              apr_proc_mutex_t *a_mx,
              apr_array_header_t *a_array);

/* format the strings with the delims and terms then write to fd */
int sendfd(apr_file_t *a_fd,
           apr_proc_mutex_t *a_mx,
           apr_array_header_t *a_array)
{
  char str[MODVLAD_MAXMSG_LEN];
  char delimiter[] = { MODVLAD_DELIMITER };
  char terminator[] = { MODVLAD_TERMINATOR };
  unsigned int i;
  int size;

  if (!a_fd || !a_array)
    return -1;

  /* init */
  memset(str, 0, MODVLAD_MAXMSG_LEN);

  /* first we compose the string */
  for (i = 0; i < a_array->nelts; i++) {
    const char *tmpstr = ((const char **)a_array->elts)[i];
    int tmplen = strlen(str) +
                 strlen(tmpstr) +
                 strlen(delimiter) +
                 strlen(terminator);

    /* make sure we are within the limit */
    if (tmplen >= MODVLAD_MAXMSG_LEN)
      return -1;

    if (i != 0)
      strcat(str, delimiter);

    strcat(str, tmpstr);
  }

  strcat(str, terminator);

  /* now write it to fd */

  size = strlen(str);

  if (a_mx)
    apr_proc_mutex_lock(a_mx);

  apr_file_write(a_fd, str, &size);

  if (a_mx)
    apr_proc_mutex_unlock(a_mx);

  return (size == strlen(str));
}

/* receive from fd. tokenize and store to array */
int receivefd(apr_file_t *a_fd,
              apr_proc_mutex_t *a_mx,
              apr_array_header_t *a_array)
{
  char str[MODVLAD_MAXMSG_LEN];
  char tmp[MODVLAD_MAXMSG_LEN];
  unsigned int i;
  unsigned int j;
  int size;

  if (!a_fd || !a_array)
    return -1;

  /* init */
  memset(str, 0, MODVLAD_MAXMSG_LEN);
  memset(tmp, 0, MODVLAD_MAXMSG_LEN);

  /* read from fd and store into the buffer */
  size = MODVLAD_MAXMSG_LEN;

  if (a_mx)
    apr_proc_mutex_lock(a_mx);

  apr_file_read(a_fd, str, &size);

  if (a_mx)
    apr_proc_mutex_unlock(a_mx);

  if (size != strlen(str))
    return -1;

  /* now go through the buffer and tokenize to the array */
  for (i = 0, j = 0; i < strlen(str); i++) {
    if (str[i] == MODVLAD_DELIMITER) {
      tmp[j] = '\0';
      *(const char **) apr_array_push(a_array) = apr_pstrdup(a_array->pool, tmp);
      memset(tmp, 0, MODVLAD_MAXMSG_LEN);
      j = 0;
    }
    else if (str[i] == MODVLAD_TERMINATOR) {
      tmp[j] = '\0';
      *(const char **) apr_array_push(a_array) = apr_pstrdup(a_array->pool, tmp);
      memset(tmp, 0, MODVLAD_MAXMSG_LEN);
      j = 0;
      break;
    }
    else
      tmp[j++] = str[i];
  }

  return 0;
}

/* server init: create and init kb */
int modvlad_server_init(apr_pool_t *a_p,
                        void **a_kb,
                        const char *a_docroot,
                        const char *a_ufile,
                        const char *a_pfile)
{
  if (!a_p || !a_kb || !a_docroot || !a_ufile || !a_pfile) {
    
  }

  return 0;
}
                                                                              
/* start listening on the fd */
void modvlad_server_listen(apr_pool_t *a_p,
                           void *a_kb,
                           apr_file_t *a_fdin,
                           apr_file_t *a_fdout)
{

}
