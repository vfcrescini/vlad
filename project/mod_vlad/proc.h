/*
 * proc.h
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __MODVLAD_PROC_H
#define __MODVLAD_PROC_H

#include "apr_tables.h"
#include "apr_file_io.h"
#include "apr_proc_mutex.h"

#include "mod_vlad.h"

#define MODVLAD_MAXMSG_LEN  2048

#define MODVLAD_DELIMITER  '\004'
#define MODVLAD_TERMINATOR '\002'

/* functions to send a request (and get reply) to/from the kb process */

int modvlad_check(apr_pool_t *a_p, modvlad_ipc a_ipc);

int modvlad_client_query(apr_pool_t *a_p,
                         modvlad_ipc a_ipc,
                         const char *a_sub,
                         const char *a_acc,
                         const char *a_obj,
                         unsigned char *a_res);

int modvlad_client_trans_total(apr_pool_t *a_p,
                               modvlad_ipc a_ipc,
                               unsigned int *a_tot);

int modvlad_client_trans_get(apr_pool_t *a_p,
                             modvlad_ipc a_ipc,
                             unsigned int a_index,
                             const char **a_name,
                             apr_array_header_t *a_parm);

int modvlad_client_ident_total(apr_pool_t *a_p,
                               modvlad_ipc a_ipc,
                               unsigned int *a_tot);

int modvlad_client_ident_get(apr_pool_t *a_p,
                             modvlad_ipc a_ipc,
                             unsigned char a_type,
                             apr_array_header_t *a_arr);

int modvlad_client_ident_check(apr_pool_t *a_p,
                               modvlad_ipc a_ipc,
                               const char *a_name,
                               unsigned char a_type);

int modvlad_client_seq_total(apr_pool_t *a_p,
                             modvlad_ipc a_ipc,
                             unsigned int *a_tot);

int modvlad_client_seq_get(apr_pool_t *a_p,
                           modvlad_ipc a_ipc,
                           unsigned int a_index,
                           const char **a_name,
                           apr_array_header_t *a_parm);

int modvlad_client_seq_add(apr_pool_t *a_p,
                           modvlad_ipc a_ipc,
                           const char *a_name,
                           apr_array_header_t *a_parms);

int modvlad_client_seq_del(apr_pool_t *a_p,
                           modvlad_ipc a_ipc,
                           unsigned int a_index);

/* server init: create and init kb */
int modvlad_server_init(apr_pool_t *a_p,
                        void **a_kb,
                        const char *a_docroot,
                        const char *a_ufile,
                        const char *a_pfile);

/* start listening on the fd */
void modvlad_server_listen(apr_pool_t *a_p, void *a_kb, modvlad_ipc a_ipc);

#endif