/*
 * proc.c
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdlib.h>
#include <string.h>

#include "apr_pools.h"
#include "apr_strings.h"
#include "apr_poll.h"

#include <vlad/vlad.h>
#include <vlad/wrapper.h>

#include "util.h"
#include "proc.h"

/* format the strings with the delims and terms then write to fd */
static int sendfd(apr_file_t *a_fd,
                  apr_proc_mutex_t *a_mx,
                  apr_array_header_t *a_array);

/* receive from fd. tokenize and store to array */
static int receivefd(apr_file_t *a_fd,
                     apr_proc_mutex_t *a_mx,
                     apr_array_header_t *a_array);

/* process client request */
static int processreq(apr_pool_t *a_p,
                      void *a_kb,
                      apr_array_header_t *a_req,
                      apr_array_header_t *a_rep);

/* format the strings with the delims and terms then write to fd */
static int sendfd(apr_file_t *a_fd,
           apr_proc_mutex_t *a_mx,
           apr_array_header_t *a_array)
{
  char str[MODVLAD_MAXMSG_LEN];
  char delimiter[] = { MODVLAD_DELIMITER, '\0' };
  char terminator[] = { MODVLAD_TERMINATOR, '\0' };
  unsigned int i;
  int size;

  if (!a_fd || !a_array)
    return -1;

  /* init */
  memset(str, 0, MODVLAD_MAXMSG_LEN);

  /* first we compose the string */
  for (i = 0; i < a_array->nelts; i++) {
    char *tmpstr = ((char **)a_array->elts)[i];
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
static int receivefd(apr_file_t *a_fd,
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

/* process client request */
static int processreq(apr_pool_t *a_p,
                      void *a_kb,
                      apr_array_header_t *a_req,
                      apr_array_header_t *a_rep)
{
  const char *id = NULL;
  const char *cmd = NULL;

  if (!a_p || !a_kb || !a_req || !a_rep)
    return -1;

  if (a_req->nelts < 2)
    return -1;

  id = ((const char **)a_req->elts)[0];
  cmd = ((const char **)a_req->elts)[1];

  /* send out id */
  *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, id);

  if (!strcmp(cmd, "Q")) {
    /* query */
    void *exp = NULL;
    unsigned char res;

    exp = modvlad_create_query(a_p,
                               ((const char **)a_req->elts)[2],
                               ((const char **)a_req->elts)[3],
                               ((const char **)a_req->elts)[4]);

    vlad_kb_query_evaluate(a_kb, exp, &res);

    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "QR");
    if (res == VLAD_RESULT_TRUE) 
      *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "Y");
    else if (res == VLAD_RESULT_FALSE) 
      *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "N");
    else 
      *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "?");

    /* cleanup */
    vlad_exp_destroy(exp);
  }
  else if (!strcmp(cmd, "TT")) {
    /* total number of transformations */
    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "TTR");
    *(const char **) apr_array_push(a_rep) = apr_psprintf(a_rep->pool,
                                                          "%d",
                                                          vlad_kb_length_transtab(a_kb));
  }
  else if (!strcmp(cmd, "TG")) {
    /* get transformation */
    char *name = NULL;
    char *idx = NULL;
    void *list = NULL;
    void *prexp = NULL;
    void *poexp = NULL;

    idx = ((char **)a_req->elts)[2];

    vlad_kb_get_transtab(a_kb, atoi(idx), &name, &list, &prexp, &poexp);

    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "TGR");
    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, name);
    *(const char **) apr_array_push(a_rep) = apr_psprintf(a_rep->pool,
                                                          "%d",
                                                          vlad_list_length(list));
  }
  else if (!strcmp(cmd, "IT")) {
    /* total number of indentfiers */
    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "ITR");
    *(const char **) apr_array_push(a_rep) = apr_psprintf(a_rep->pool,
                                                          "%d",
                                                          vlad_kb_length_symtab(a_kb));
  }
  else if (!strcmp(cmd, "IG")) {
    /* get identifier */
    char *name = NULL;
    char *idx = NULL;

    idx = ((char **)a_req->elts)[2];

    vlad_kb_get_symtab(a_kb, atoi(idx), &name);

    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "IGR");
    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, name);
  }
  else if (!strcmp(cmd, "ST")) {
    /* total number in sequence */
    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "STR");
    *(const char **) apr_array_push(a_rep) = apr_psprintf(a_rep->pool,
                                                          "%d",
                                                          vlad_kb_length_seqtab(a_kb));
  }
  else if (!strcmp(cmd, "SG")) {
    /* get from sequence */
    char *name = NULL;
    char *idx = NULL;
    void *list = NULL;

    idx = ((char **)a_req->elts)[2];

    vlad_kb_get_seqtab(a_kb, atoi(idx), &name, &list);

    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "SGR");
    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, name);
  }
  else if (!strcmp(cmd, "SA")) {
    /* add to sequence */
    char *name = NULL;
    char *num = NULL;
    void *slist = NULL;
    void *tref = NULL;
    unsigned int i;

    name = ((char **)a_req->elts)[2];
    num = ((char **)a_req->elts)[3];

    vlad_strlist_create(&slist);
    vlad_tref_create(&tref);

    for (i = 0; i < atoi(num); i++) {
      char *parm = NULL;
      parm = ((char **)a_req->elts)[4 + i];
      vlad_strlist_add(slist, parm);
    }

    vlad_tref_init(tref, name, slist);
    vlad_kb_add_seqtab(a_kb, tref);
    vlad_kb_compute_evaluate(a_kb);

    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "SAR");
    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "0");
  }
  else if (!strcmp(cmd, "SD")) {
    /* delete from sequence */
    char *idx = NULL;

    idx = ((char **)a_req->elts)[2];

    vlad_kb_del_seqtab(a_kb, atoi(idx));
    vlad_kb_compute_evaluate(a_kb);

    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "SDR");
    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "0");
  }
  else {
    /* error */
  }

  return 0;
}

/* functions to send a request (and get reply) to/from the kb process */

int modvlad_client_query(apr_pool_t *a_p,
                         apr_file_t *a_fdin,
                         apr_file_t *a_fdout,
                         apr_proc_mutex_t *a_mx,
                         const char *a_sub,
                         const char *a_acc,
                         const char *a_obj,
                         unsigned char *a_res)
{
  apr_array_header_t *arr_out = NULL;
  apr_array_header_t *arr_in = NULL;

  if (!a_p || !a_fdin || !a_fdout || !a_mx || !a_sub || !a_acc || !a_obj || !a_res)
    return -1;

  arr_out = apr_array_make(a_p, 1, sizeof(char *));
  arr_in = apr_array_make(a_p, 1, sizeof(char *));

  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "69");
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "Q");
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, a_sub);
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, a_acc);
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, a_obj);

  sendfd(a_fdin, a_mx, arr_out);
  receivefd(a_fdout, a_mx, arr_in);

  if (!strcmp(((char **)arr_in->elts)[2], "Y"))
    *a_res = VLAD_RESULT_TRUE;
  else if (!strcmp(((char **)arr_in->elts)[2], "N"))
    *a_res = VLAD_RESULT_FALSE;
  else 
    *a_res = VLAD_RESULT_UNKNOWN;

  return 0;
}

int modvlad_client_trans_total(apr_pool_t *a_p,
                               apr_file_t *a_fdin,
                               apr_file_t *a_fdout,
                               apr_proc_mutex_t *a_mx,
                               unsigned int *a_tot)
{
  apr_array_header_t *arr_out = NULL;
  apr_array_header_t *arr_in = NULL;

  if (!a_p || !a_fdin || !a_fdout || !a_mx || !a_tot)
    return -1;

  arr_out = apr_array_make(a_p, 1, sizeof(char *));
  arr_in = apr_array_make(a_p, 1, sizeof(char *));

  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "69");
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "TT");

  sendfd(a_fdin, a_mx, arr_out);
  receivefd(a_fdout, a_mx, arr_in);

  *a_tot = atoi(((char **)arr_in->elts)[2]);

  return 0;
}

int modvlad_client_trans_get(apr_pool_t *a_p,
                             apr_file_t *a_fdin,
                             apr_file_t *a_fdout,
                             apr_proc_mutex_t *a_mx,
                             unsigned int a_index,
                             const char **a_name,
                             unsigned int *a_tot)
{
  apr_array_header_t *arr_out = NULL;
  apr_array_header_t *arr_in = NULL;

  if (!a_p || !a_fdin || !a_fdout || !a_mx || !a_name)
    return -1;

  arr_out = apr_array_make(a_p, 1, sizeof(char *));
  arr_in = apr_array_make(a_p, 1, sizeof(char *));

  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "69");
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "TG");
  *(char **) apr_array_push(arr_out) = apr_psprintf(a_p, "%d", a_index);

  sendfd(a_fdin, a_mx, arr_out);
  receivefd(a_fdout, a_mx, arr_in);

  *a_name = apr_pstrdup(a_p, ((char **)arr_in->elts)[2]);
  *a_tot = atoi(apr_pstrdup(a_p, ((char **)arr_in->elts)[3]));

  return 0;
}

int modvlad_client_ident_total(apr_pool_t *a_p,
                               apr_file_t *a_fdin,
                               apr_file_t *a_fdout,
                               apr_proc_mutex_t *a_mx,
                               unsigned int *a_tot)
{
  apr_array_header_t *arr_out = NULL;
  apr_array_header_t *arr_in = NULL;

  if (!a_p || !a_fdin || !a_fdout || !a_mx || !a_tot)
    return -1;

  arr_out = apr_array_make(a_p, 1, sizeof(char *));
  arr_in = apr_array_make(a_p, 1, sizeof(char *));

  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "69");
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "IT");

  sendfd(a_fdin, a_mx, arr_out);
  receivefd(a_fdout, a_mx, arr_in);

  *a_tot = atoi(((char **)arr_in->elts)[2]);

  return 0;
}

int modvlad_client_ident_get(apr_pool_t *a_p,
                             apr_file_t *a_fdin,
                             apr_file_t *a_fdout,
                             apr_proc_mutex_t *a_mx,
                             unsigned int a_index,
                             const char **a_name)
{
  apr_array_header_t *arr_out = NULL;
  apr_array_header_t *arr_in = NULL;

  if (!a_p || !a_fdin || !a_fdout || !a_mx || !a_name)
    return -1;

  arr_out = apr_array_make(a_p, 1, sizeof(char *));
  arr_in = apr_array_make(a_p, 1, sizeof(char *));

  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "69");
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "IG");
  *(char **) apr_array_push(arr_out) = apr_psprintf(a_p, "%d", a_index);

  sendfd(a_fdin, a_mx, arr_out);
  receivefd(a_fdout, a_mx, arr_in);

  *a_name = apr_pstrdup(a_p, ((char **)arr_in->elts)[2]);

  return 0;
}

int modvlad_client_seq_total(apr_pool_t *a_p,
                             apr_file_t *a_fdin,
                             apr_file_t *a_fdout,
                             apr_proc_mutex_t *a_mx,
                             unsigned int *a_tot)
{
  apr_array_header_t *arr_out = NULL;
  apr_array_header_t *arr_in = NULL;

  if (!a_p || !a_fdin || !a_fdout || !a_mx || !a_tot)
    return -1;

  arr_out = apr_array_make(a_p, 1, sizeof(char *));
  arr_in = apr_array_make(a_p, 1, sizeof(char *));

  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "69");
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "ST");

  sendfd(a_fdin, a_mx, arr_out);
  receivefd(a_fdout, a_mx, arr_in);

  *a_tot = atoi(((char **)arr_in->elts)[2]);

  return 0;
}

int modvlad_client_seq_get(apr_pool_t *a_p,
                           apr_file_t *a_fdin,
                           apr_file_t *a_fdout,
                           apr_proc_mutex_t *a_mx,
                           unsigned int a_index,
                           const char **a_name)
{
  apr_array_header_t *arr_out = NULL;
  apr_array_header_t *arr_in = NULL;

  if (!a_p || !a_fdin || !a_fdout || !a_mx || !a_name)
    return -1;

  arr_out = apr_array_make(a_p, 1, sizeof(char *));
  arr_in = apr_array_make(a_p, 1, sizeof(char *));

  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "69");
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "SG");
  *(char **) apr_array_push(arr_out) = apr_psprintf(a_p, "%d", a_index);

  sendfd(a_fdin, a_mx, arr_out);
  receivefd(a_fdout, a_mx, arr_in);

  *a_name = apr_pstrdup(a_p, ((char **)arr_in->elts)[2]);

  return 0;
}

int modvlad_client_seq_add(apr_pool_t *a_p,
                           apr_file_t *a_fdin,
                           apr_file_t *a_fdout,
                           apr_proc_mutex_t *a_mx,
                           const char *a_name,
                           apr_array_header_t *a_parms)
{
  apr_array_header_t *arr_out = NULL;
  apr_array_header_t *arr_in = NULL;

  if (!a_p || !a_fdin || !a_fdout || !a_mx || !a_name || !a_parms)
    return -1;

  arr_out = apr_array_make(a_p, 1, sizeof(char *));
  arr_in = apr_array_make(a_p, 1, sizeof(char *));

  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "69");
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "SA");
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, a_name);
  *(char **) apr_array_push(arr_out) = apr_psprintf(a_p, "%d", a_parms->nelts);

  apr_array_cat(arr_out, a_parms);

  sendfd(a_fdin, a_mx, arr_out);
  receivefd(a_fdout, a_mx, arr_in);

  return strcmp(((char **)arr_in->elts)[2], "0");
}

int modvlad_client_seq_del(apr_pool_t *a_p,
                           apr_file_t *a_fdin,
                           apr_file_t *a_fdout,
                           apr_proc_mutex_t *a_mx,
                           unsigned int a_index)
{
  apr_array_header_t *arr_out = NULL;
  apr_array_header_t *arr_in = NULL;

  if (!a_p || !a_fdin || !a_fdout || !a_mx)
    return -1;

  arr_out = apr_array_make(a_p, 1, sizeof(char *));
  arr_in = apr_array_make(a_p, 1, sizeof(char *));

  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "69");
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "SD");
  *(char **) apr_array_push(arr_out) = apr_psprintf(a_p, "%d", a_index);

  sendfd(a_fdin, a_mx, arr_out);
  receivefd(a_fdout, a_mx, arr_in);

  return strcmp(((char **)arr_in->elts)[2], "0");
}

/* server init: create and init kb */
int modvlad_server_init(apr_pool_t *a_p,
                        void **a_kb,
                        const char *a_docroot,
                        const char *a_ufile,
                        const char *a_pfile)
{
  void *exp = NULL;
  apr_file_t *polfile = NULL;

  if (!a_p || !a_kb || !a_docroot || !a_ufile || !a_pfile)
    return -1;

  modvlad_init_kb(a_p, a_ufile, a_docroot, a_kb, &exp);

  apr_file_open(&polfile, a_pfile, APR_READ, APR_OS_DEFAULT, a_p);

  modvlad_load_kb(a_p, polfile, *a_kb, exp);

  apr_file_close(polfile);

  vlad_kb_compute_evaluate(*a_kb);

  return 0;
}

/* start listening on the fd */
void modvlad_server_listen(apr_pool_t *a_p,
                           void *a_kb,
                           apr_file_t *a_fdin,
                           apr_file_t *a_fdout)
{
  apr_pollset_t *pollset = NULL;
  const apr_pollfd_t *pollfd = NULL;
  apr_pollfd_t newfd;
  int count;

  if (!a_p || !a_kb || !a_fdin || !a_fdout)
    return;

  /* setup pollset */
  if (apr_pollset_create(&pollset, 1, a_p, 0) != APR_SUCCESS)
    return;

  newfd.p = a_p;
  newfd.desc_type = APR_POLL_FILE | APR_POLL_LASTDESC;
  newfd.reqevents = APR_POLLIN | APR_POLLPRI;
  newfd.desc.f = a_fdout;
  newfd.client_data = NULL;

  apr_pollset_add(pollset, &newfd);

  while (1) {
    apr_pollset_poll(pollset, -1, &count, &pollfd);

    /* some error occured */
    if (count != 1)
      break;

    if (pollfd->rtnevents & (APR_POLLIN | APR_POLLPRI)) {
      apr_array_header_t *reqarr = NULL;
      apr_array_header_t *reparr = NULL;
      reqarr = apr_array_make(a_p, 1, sizeof(char *));
      reparr = apr_array_make(a_p, 1, sizeof(char *));

      /* read request and write reply */
      receivefd(a_fdout, NULL, reqarr);
      processreq(a_p, a_kb, reqarr, reparr);
      sendfd(a_fdin, NULL, reparr);
    }
  }
}
