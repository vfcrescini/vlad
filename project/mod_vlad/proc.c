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
    return MODVLAD_NULLPTR;

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
      return MODVLAD_BUFFEROVERFLOW;

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

  return (size == strlen(str) ? MODVLAD_OK : MODVLAD_FAILURE);
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
    return MODVLAD_NULLPTR;

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
    return MODVLAD_FAILURE;

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

  return MODVLAD_OK;
}

/* process client request */
static int processreq(apr_pool_t *a_p,
                      void *a_kb,
                      apr_array_header_t *a_req,
                      apr_array_header_t *a_rep)
{
  const char *id = NULL;
  const char *cmd = NULL;

  if (!a_p || !a_req || !a_rep)
    return MODVLAD_NULLPTR;

  if (a_req->nelts < 2)
    return MODVLAD_INVALIDINPUT;

  id = ((const char **)a_req->elts)[0];
  cmd = ((const char **)a_req->elts)[1];

  /* send out id */
  *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, id);

  /* if the kb is null don't bother doing anything else */
  if (!a_kb) {
    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "ERR");
    return MODVLAD_FAILURE;
  }

  if (!strcmp(cmd, "C")) {
    /* check */
    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "CR");
  }
  else if (!strcmp(cmd, "Q")) {
    /* query */
    void *exp = NULL;
    unsigned char res;

    exp = modvlad_create_query(a_p,
                               ((const char **)a_req->elts)[2],
                               ((const char **)a_req->elts)[3],
                               ((const char **)a_req->elts)[4]);

    if (!exp) {
      *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "ERR");
      return MODVLAD_FAILURE;
    }

    if (vlad_kb_query_evaluate(a_kb, exp, &res) != VLAD_OK) {
      *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "ERR");
      return MODVLAD_FAILURE;
    }

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
    unsigned int listlen;
    unsigned int i;

    idx = ((char **)a_req->elts)[2];

    if (vlad_kb_get_transtab(a_kb, atoi(idx), &name, &list, &prexp, &poexp) != VLAD_OK) {
      *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "ERR");
      return MODVLAD_FAILURE;
    }

    listlen = vlad_list_length(list);

    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "TGR");
    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, name);
    *(const char **) apr_array_push(a_rep) = apr_psprintf(a_rep->pool, "%d", listlen);

    for (i = 0; i < listlen; i++) {
      char *tmpstr = NULL;

      vlad_strlist_get(list, i, &tmpstr);

      *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, tmpstr);
    }
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
    char *type = NULL;
    char **array = NULL;
    unsigned int size;
    unsigned int i;

    type = ((char **)a_req->elts)[2];

    if (vlad_kb_get_array_symtab(a_kb, atoi(type), &array, &size) != VLAD_OK) {
      *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "ERR");
      return MODVLAD_FAILURE;
    }

    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "IGR");
    *(const char **) apr_array_push(a_rep) = apr_psprintf(a_rep->pool, "%d", size);

    for (i = 0; i < size; i++)
      *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, array[i]);

    if (array)
      free(array);
  }
  else if (!strcmp(cmd, "IC")) {
    /* check identifier */
    if (vlad_kb_check_symtab(a_kb,
                             ((char **)a_req->elts)[2],
                             (unsigned char) atoi(((char **)a_req->elts)[3])) != VLAD_OK) {
      *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "ERR");
      return MODVLAD_FAILURE;
    }

    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "ICR");
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
    unsigned int listlen;
    unsigned int i;

    idx = ((char **)a_req->elts)[2];

    if (vlad_kb_get_seqtab(a_kb, atoi(idx), &name, &list) != VLAD_OK) {
      *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "ERR");
      return MODVLAD_FAILURE;
    }

    listlen = vlad_list_length(list);

    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "SGR");
    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, name);
    *(const char **) apr_array_push(a_rep) = apr_psprintf(a_rep->pool, "%d", listlen);

    for (i = 0; i < listlen; i++) {
      char *tmpstr = NULL;

      vlad_strlist_get(list, i, &tmpstr);

      *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, tmpstr);
    }
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

    if (atoi(num) > 0) {
      if (vlad_strlist_create(&slist) != VLAD_OK) {
        *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "ERR");
        return MODVLAD_FAILURE;
      }
    }

    if (vlad_tref_create(&tref) != VLAD_OK) {
      *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "ERR");
      return MODVLAD_FAILURE;
    }

    for (i = 0; i < atoi(num); i++) {
      char *parm = NULL;
      parm = ((char **)a_req->elts)[4 + i];

      if (vlad_strlist_add(slist, parm) != VLAD_OK) {
        *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "ERR");
        return MODVLAD_FAILURE;
      }
    }

    if (vlad_tref_init(tref, name, slist) != VLAD_OK) {
      *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "ERR");
      return MODVLAD_FAILURE;
    }

    if (vlad_kb_add_seqtab(a_kb, tref) != VLAD_OK) {
      *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "ERR");
      return MODVLAD_FAILURE;
    }

    if (vlad_kb_compute_evaluate(a_kb) != VLAD_OK) {
      *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "ERR");
      return MODVLAD_FAILURE;
    }

    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "SAR");
    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "0");
  }
  else if (!strcmp(cmd, "SD")) {
    /* delete from sequence */
    char *idx = NULL;

    idx = ((char **)a_req->elts)[2];

    if ((vlad_kb_del_seqtab(a_kb, atoi(idx)) != VLAD_OK) ||
         vlad_kb_compute_evaluate(a_kb) != VLAD_OK) {
      *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "ERR");
      return MODVLAD_FAILURE;
    }

    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "SDR");
    *(const char **) apr_array_push(a_rep) = apr_pstrdup(a_rep->pool, "0");
  }
  else
    return MODVLAD_FAILURE;

  return MODVLAD_OK;
}

/* functions to send a request (and get reply) to/from the kb process */

int modvlad_check(apr_pool_t *a_p,
                  modvlad_ipc a_ipc)
{
  apr_array_header_t *arr_out = NULL;
  apr_array_header_t *arr_in = NULL;
  unsigned int id = modvlad_idgen();

  if (!a_p || MODVLAD_IPC_CCHECK(a_ipc))
    return MODVLAD_NULLPTR;

  arr_out = apr_array_make(a_p, 1, sizeof(char *));
  arr_in = apr_array_make(a_p, 1, sizeof(char *));

  *(char **) apr_array_push(arr_out) = apr_psprintf(a_p, "%d", id);
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "C");

  sendfd(a_ipc.pipe_cli[1], a_ipc.mutex, arr_out);
  receivefd(a_ipc.pipe_cli[0], a_ipc.mutex, arr_in);

  if (id != atoi((((char **)arr_in->elts)[0])))
    return MODVLAD_OUTOFSEQ;

  if (strcmp(((char **)arr_in->elts)[1], "CR"))
    return MODVLAD_FAILURE;

  return MODVLAD_OK;
}

int modvlad_client_query(apr_pool_t *a_p,
                         modvlad_ipc a_ipc,
                         const char *a_sub,
                         const char *a_acc,
                         const char *a_obj,
                         unsigned char *a_res)
{
  apr_array_header_t *arr_out = NULL;
  apr_array_header_t *arr_in = NULL;
  unsigned int id = modvlad_idgen();

  if (!a_p || MODVLAD_IPC_CCHECK(a_ipc) || !a_sub || !a_acc || !a_obj || !a_res)
    return MODVLAD_NULLPTR;

  arr_out = apr_array_make(a_p, 1, sizeof(char *));
  arr_in = apr_array_make(a_p, 1, sizeof(char *));

  *(char **) apr_array_push(arr_out) = apr_psprintf(a_p, "%d", id);
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "Q");
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, a_sub);
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, a_acc);
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, a_obj);

  sendfd(a_ipc.pipe_cli[1], a_ipc.mutex, arr_out);
  receivefd(a_ipc.pipe_cli[0], a_ipc.mutex, arr_in);

  if (id != atoi((((char **)arr_in->elts)[0])))
    return MODVLAD_OUTOFSEQ;

  if (strcmp(((char **)arr_in->elts)[1], "QR"))
    return MODVLAD_FAILURE;

  if (!strcmp(((char **)arr_in->elts)[2], "Y"))
    *a_res = VLAD_RESULT_TRUE;
  else if (!strcmp(((char **)arr_in->elts)[2], "N"))
    *a_res = VLAD_RESULT_FALSE;
  else if (!strcmp(((char **)arr_in->elts)[2], "?"))
    *a_res = VLAD_RESULT_UNKNOWN;
  else
    return MODVLAD_FAILURE;

  return MODVLAD_OK;
}

int modvlad_client_trans_total(apr_pool_t *a_p,
                               modvlad_ipc a_ipc,
                               unsigned int *a_tot)
{
  apr_array_header_t *arr_out = NULL;
  apr_array_header_t *arr_in = NULL;
  unsigned int id = modvlad_idgen();

  if (!a_p || MODVLAD_IPC_CCHECK(a_ipc) || !a_tot)
    return MODVLAD_NULLPTR;

  arr_out = apr_array_make(a_p, 1, sizeof(char *));
  arr_in = apr_array_make(a_p, 1, sizeof(char *));

  *(char **) apr_array_push(arr_out) = apr_psprintf(a_p, "%d", id);
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "TT");

  sendfd(a_ipc.pipe_cli[1], a_ipc.mutex, arr_out);
  receivefd(a_ipc.pipe_cli[0], a_ipc.mutex, arr_in);

  if (id != atoi((((char **)arr_in->elts)[0])))
    return MODVLAD_OUTOFSEQ;

  if (strcmp(((char **)arr_in->elts)[1], "TTR"))
    return MODVLAD_FAILURE;

  *a_tot = atoi(((char **)arr_in->elts)[2]);

  return MODVLAD_OK;
}

int modvlad_client_trans_get(apr_pool_t *a_p,
                             modvlad_ipc a_ipc,
                             unsigned int a_index,
                             const char **a_name,
                             apr_array_header_t *a_parm)
{
  apr_array_header_t *arr_out = NULL;
  apr_array_header_t *arr_in = NULL;
  unsigned int id = modvlad_idgen();
  unsigned int parmlen;
  unsigned int i;

  if (!a_p || MODVLAD_IPC_CCHECK(a_ipc) || !a_name || !a_parm)
    return MODVLAD_NULLPTR;

  arr_out = apr_array_make(a_p, 1, sizeof(char *));
  arr_in = apr_array_make(a_p, 1, sizeof(char *));

  *(char **) apr_array_push(arr_out) = apr_psprintf(a_p, "%d", id);
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "TG");
  *(char **) apr_array_push(arr_out) = apr_psprintf(a_p, "%d", a_index);

  sendfd(a_ipc.pipe_cli[1], a_ipc.mutex, arr_out);
  receivefd(a_ipc.pipe_cli[0], a_ipc.mutex, arr_in);

  if (id != atoi((((char **)arr_in->elts)[0])))
    return MODVLAD_OUTOFSEQ;

  if (strcmp(((char **)arr_in->elts)[1], "TGR"))
    return MODVLAD_FAILURE;

  *a_name = apr_pstrdup(a_p, ((char **)arr_in->elts)[2]);
  parmlen = atoi(((char **)arr_in->elts)[3]);

  for (i = 0; i < parmlen; i++)
   *(char **) apr_array_push(a_parm) = apr_pstrdup(a_p, ((char **)arr_in->elts)[4 + i]);

  return MODVLAD_OK;
}

int modvlad_client_ident_total(apr_pool_t *a_p,
                               modvlad_ipc a_ipc,
                               unsigned int *a_tot)
{
  apr_array_header_t *arr_out = NULL;
  apr_array_header_t *arr_in = NULL;
  unsigned int id = modvlad_idgen();

  if (!a_p || MODVLAD_IPC_CCHECK(a_ipc) || !a_tot)
    return MODVLAD_NULLPTR;

  arr_out = apr_array_make(a_p, 1, sizeof(char *));
  arr_in = apr_array_make(a_p, 1, sizeof(char *));

  *(char **) apr_array_push(arr_out) = apr_psprintf(a_p, "%d", id);
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "IT");

  sendfd(a_ipc.pipe_cli[1], a_ipc.mutex, arr_out);
  receivefd(a_ipc.pipe_cli[0], a_ipc.mutex, arr_in);

  if (id != atoi((((char **)arr_in->elts)[0])))
    return MODVLAD_OUTOFSEQ;

  if (strcmp(((char **)arr_in->elts)[1], "ITR"))
    return MODVLAD_FAILURE;

  *a_tot = atoi(((char **)arr_in->elts)[2]);

  return MODVLAD_OK;
}

int modvlad_client_ident_get(apr_pool_t *a_p,
                             modvlad_ipc a_ipc,
                             unsigned char a_type,
                             apr_array_header_t *a_arr)
{
  apr_array_header_t *arr_out = NULL;
  apr_array_header_t *arr_in = NULL;
  unsigned int id = modvlad_idgen();
  unsigned int size;
  unsigned int i;

  if (!a_p || MODVLAD_IPC_CCHECK(a_ipc) || !a_arr)
    return MODVLAD_NULLPTR;

  arr_out = apr_array_make(a_p, 1, sizeof(char *));
  arr_in = apr_array_make(a_p, 1, sizeof(char *));

  *(char **) apr_array_push(arr_out) = apr_psprintf(a_p, "%d", id);
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "IG");
  *(char **) apr_array_push(arr_out) = apr_psprintf(a_p, "%d", a_type);

  sendfd(a_ipc.pipe_cli[1], a_ipc.mutex, arr_out);
  receivefd(a_ipc.pipe_cli[0], a_ipc.mutex, arr_in);

  if (id != atoi((((char **)arr_in->elts)[0])))
    return MODVLAD_OUTOFSEQ;

  if (strcmp(((char **)arr_in->elts)[1], "IGR"))
    return MODVLAD_FAILURE;

  size = atoi(((char **)arr_in->elts)[2]);

  for (i = 0; i < size; i++)
    *(char **) apr_array_push(a_arr) = apr_pstrdup(a_p, ((char **)arr_in->elts)[3 + i]);

  return MODVLAD_OK;
}

int modvlad_client_ident_check(apr_pool_t *a_p,
                               modvlad_ipc a_ipc,
                               const char *a_name,
                               unsigned char a_type)
{
  apr_array_header_t *arr_out = NULL;
  apr_array_header_t *arr_in = NULL;
  unsigned int id = modvlad_idgen();

  if (!a_p || MODVLAD_IPC_CCHECK(a_ipc) || !a_name)
    return MODVLAD_NULLPTR;

  arr_out = apr_array_make(a_p, 1, sizeof(char *));
  arr_in = apr_array_make(a_p, 1, sizeof(char *));

  *(char **) apr_array_push(arr_out) = apr_psprintf(a_p, "%d", id);
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "IC");
  *(char **) apr_array_push(arr_out) = apr_psprintf(a_p, "%s", a_name);
  *(char **) apr_array_push(arr_out) = apr_psprintf(a_p, "%d", a_type);

  sendfd(a_ipc.pipe_cli[1], a_ipc.mutex, arr_out);
  receivefd(a_ipc.pipe_cli[0], a_ipc.mutex, arr_in);

  if (id != atoi((((char **)arr_in->elts)[0])))
    return MODVLAD_OUTOFSEQ;

  /* if not found, ERR is set to this element */
  if (strcmp(((char **)arr_in->elts)[1], "ICR"))
    return MODVLAD_FAILURE;

  return MODVLAD_OK;
}

int modvlad_client_seq_total(apr_pool_t *a_p,
                             modvlad_ipc a_ipc,
                             unsigned int *a_tot)
{
  apr_array_header_t *arr_out = NULL;
  apr_array_header_t *arr_in = NULL;
  unsigned int id = modvlad_idgen();

  if (!a_p || MODVLAD_IPC_CCHECK(a_ipc) || !a_tot)
    return MODVLAD_NULLPTR;

  arr_out = apr_array_make(a_p, 1, sizeof(char *));
  arr_in = apr_array_make(a_p, 1, sizeof(char *));

  *(char **) apr_array_push(arr_out) = apr_psprintf(a_p, "%d", id);
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "ST");

  sendfd(a_ipc.pipe_cli[1], a_ipc.mutex, arr_out);
  receivefd(a_ipc.pipe_cli[0], a_ipc.mutex, arr_in);

  if (id != atoi((((char **)arr_in->elts)[0])))
    return MODVLAD_OUTOFSEQ;

  if (strcmp(((char **)arr_in->elts)[1], "STR"))
    return MODVLAD_FAILURE;

  *a_tot = atoi(((char **)arr_in->elts)[2]);

  return MODVLAD_OK;
}

int modvlad_client_seq_get(apr_pool_t *a_p,
                           modvlad_ipc a_ipc,
                           unsigned int a_index,
                           const char **a_name,
                           apr_array_header_t *a_parm)
{
  apr_array_header_t *arr_out = NULL;
  apr_array_header_t *arr_in = NULL;
  unsigned int id = modvlad_idgen();
  unsigned int parmlen;
  unsigned int i;

  if (!a_p || MODVLAD_IPC_CCHECK(a_ipc) || !a_name || !a_parm)
    return MODVLAD_NULLPTR;

  arr_out = apr_array_make(a_p, 1, sizeof(char *));
  arr_in = apr_array_make(a_p, 1, sizeof(char *));

  *(char **) apr_array_push(arr_out) = apr_psprintf(a_p, "%d", id);
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "SG");
  *(char **) apr_array_push(arr_out) = apr_psprintf(a_p, "%d", a_index);

  sendfd(a_ipc.pipe_cli[1], a_ipc.mutex, arr_out);
  receivefd(a_ipc.pipe_cli[0], a_ipc.mutex, arr_in);

  if (id != atoi((((char **)arr_in->elts)[0])))
    return MODVLAD_OUTOFSEQ;

  if (strcmp(((char **)arr_in->elts)[1], "SGR"))
    return MODVLAD_FAILURE;

  *a_name = apr_pstrdup(a_p, ((char **)arr_in->elts)[2]);

  parmlen = atoi(((char **)arr_in->elts)[3]);

  for (i = 0; i < parmlen; i++)
   *(char **) apr_array_push(a_parm) = apr_pstrdup(a_p, ((char **)arr_in->elts)[4 + i]);

  return MODVLAD_OK;
}

int modvlad_client_seq_add(apr_pool_t *a_p,
                           modvlad_ipc a_ipc,
                           const char *a_name,
                           apr_array_header_t *a_parms)
{
  apr_array_header_t *arr_out = NULL;
  apr_array_header_t *arr_in = NULL;
  unsigned int id = modvlad_idgen();

  if (!a_p || MODVLAD_IPC_CCHECK(a_ipc) || !a_name || !a_parms)
    return MODVLAD_NULLPTR;

  arr_out = apr_array_make(a_p, 1, sizeof(char *));
  arr_in = apr_array_make(a_p, 1, sizeof(char *));

  *(char **) apr_array_push(arr_out) = apr_psprintf(a_p, "%d", id);
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "SA");
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, a_name);
  *(char **) apr_array_push(arr_out) = apr_psprintf(a_p, "%d", a_parms->nelts);

  apr_array_cat(arr_out, a_parms);

  sendfd(a_ipc.pipe_cli[1], a_ipc.mutex, arr_out);
  receivefd(a_ipc.pipe_cli[0], a_ipc.mutex, arr_in);

  if (id != atoi((((char **)arr_in->elts)[0])))
    return MODVLAD_OUTOFSEQ;
  
  if (strcmp(((char **)arr_in->elts)[1], "SAR"))
    return MODVLAD_FAILURE;

  return !strcmp(((char **)arr_in->elts)[2], "0") ? MODVLAD_OK : MODVLAD_FAILURE;
}

int modvlad_client_seq_del(apr_pool_t *a_p,
                           modvlad_ipc a_ipc,
                           unsigned int a_index)
{
  apr_array_header_t *arr_out = NULL;
  apr_array_header_t *arr_in = NULL;
  unsigned int id = modvlad_idgen();

  if (!a_p || MODVLAD_IPC_CCHECK(a_ipc))
    return MODVLAD_NULLPTR;

  arr_out = apr_array_make(a_p, 1, sizeof(char *));
  arr_in = apr_array_make(a_p, 1, sizeof(char *));

  *(char **) apr_array_push(arr_out) = apr_psprintf(a_p, "%d", id);
  *(char **) apr_array_push(arr_out) = apr_pstrdup(a_p, "SD");
  *(char **) apr_array_push(arr_out) = apr_psprintf(a_p, "%d", a_index);

  sendfd(a_ipc.pipe_cli[1], a_ipc.mutex, arr_out);
  receivefd(a_ipc.pipe_cli[0], a_ipc.mutex, arr_in);

  if (id != atoi((((char **)arr_in->elts)[0])))
    return MODVLAD_OUTOFSEQ;

  if (strcmp(((char **)arr_in->elts)[1], "SDR"))
    return MODVLAD_FAILURE;

  return !strcmp(((char **)arr_in->elts)[2], "0") ? MODVLAD_OK : MODVLAD_FAILURE;
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
    return MODVLAD_NULLPTR;

  if (modvlad_init_kb(a_p, a_ufile, a_docroot, a_kb, &exp)) {
    *a_kb = NULL;
    return MODVLAD_FAILURE;
  }

  if (apr_file_open(&polfile, a_pfile, APR_READ, APR_OS_DEFAULT, a_p) != APR_SUCCESS) {
    *a_kb = NULL;
    return MODVLAD_FAILURE;
  }

  if (modvlad_load_kb(a_p, polfile, *a_kb, exp) != MODVLAD_OK) {
    *a_kb = NULL;
    return MODVLAD_FAILURE;
  }

  if (apr_file_close(polfile) != APR_SUCCESS) {
    *a_kb = NULL;
    return MODVLAD_FAILURE;
  }

  if (vlad_kb_compute_evaluate(*a_kb)) {
    *a_kb = NULL;
    return MODVLAD_FAILURE;
  }

  return MODVLAD_OK;
}

/* start listening on the fd */
void modvlad_server_listen(apr_pool_t *a_p,
                           void *a_kb,
                           modvlad_ipc a_ipc)
{
  apr_pollset_t *pollset = NULL;
  const apr_pollfd_t *pollfd = NULL;
  apr_pollfd_t newfd;
  int count;

  if (!a_p || MODVLAD_IPC_SCHECK(a_ipc))
    return;

  /* setup pollset */
  if (apr_pollset_create(&pollset, 1, a_p, 0) != APR_SUCCESS)
    return;

  newfd.p = a_p;
  newfd.desc_type = APR_POLL_FILE | APR_POLL_LASTDESC;
  newfd.reqevents = APR_POLLIN | APR_POLLPRI;
  newfd.desc.f = a_ipc.pipe_svr[0];
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
      receivefd(a_ipc.pipe_svr[0], NULL, reqarr);
      processreq(a_p, a_kb, reqarr, reparr);
      sendfd(a_ipc.pipe_svr[1], NULL, reparr);
    }
  }
}
