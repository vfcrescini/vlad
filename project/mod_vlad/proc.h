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

int modvlad_client_query(apr_pool_t *a_p,
                         apr_file_t *a_fdin,
                         apr_file_t *a_fdout,
                         apr_proc_mutex_t *a_mx,
                         const char *a_sub,
                         const char *a_acc,
                         const char *a_obj,
                         unsigned char *a_res);

int modvlad_client_trans_total(apr_pool_t *a_p,
                               apr_file_t *a_fdin,
                               apr_file_t *a_fdout,
                               apr_proc_mutex_t *a_mx,
                               unsigned int *a_tot);

int modvlad_client_trans_get(apr_pool_t *a_p,
                             apr_file_t *a_fdin,
                             apr_file_t *a_fdout,
                             apr_proc_mutex_t *a_mx,
                             unsigned int a_index,
                             const char **a_name,
                             unsigned int *a_tot);

int modvlad_client_ident_total(apr_pool_t *a_p,
                               apr_file_t *a_fdin,
                               apr_file_t *a_fdout,
                               apr_proc_mutex_t *a_mx,
                               unsigned int *a_tot);

int modvlad_client_ident_get(apr_pool_t *a_p,
                             apr_file_t *a_fdin,
                             apr_file_t *a_fdout,
                             apr_proc_mutex_t *a_mx,
                             unsigned int a_index,
                             const char **a_name);

int modvlad_client_seq_total(apr_pool_t *a_p,
                             apr_file_t *a_fdin,
                             apr_file_t *a_fdout,
                             apr_proc_mutex_t *a_mx,
                             unsigned int *a_tot);

int modvlad_client_seq_get(apr_pool_t *a_p,
                           apr_file_t *a_fdin,
                           apr_file_t *a_fdout,
                           apr_proc_mutex_t *a_mx,
                           unsigned int a_index,
                           const char **a_name);

int modvlad_client_seq_add(apr_pool_t *a_p,
                           apr_file_t *a_fdin,
                           apr_file_t *a_fdout,
                           apr_proc_mutex_t *a_mx,
                           const char *a_name,
                           apr_array_header_t *a_parms);

int modvlad_client_seq_del(apr_pool_t *a_p,
                           apr_file_t *a_fdin,
                           apr_file_t *a_fdout,
                           apr_proc_mutex_t *a_mx,
                           unsigned int a_index);

/* server init: create and init kb */
int modvlad_server_init(apr_pool_t *a_p,
                        void **a_kb,
                        const char *a_docroot,
                        const char *a_ufile,
                        const char *a_pfile);

/* start listening on the fd */
void modvlad_server_listen(apr_pool_t *a_p,
                           void *a_kb,
                           apr_file_t *a_fdin,
                           apr_file_t *a_fdout);

#endif
