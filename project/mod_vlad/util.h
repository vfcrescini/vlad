/*
 * util.h
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __MODVLAD_UTIL_H
#define __MODVLAD_UTIL_H

#include <stdarg.h>

#include "httpd.h"
#include "apr_pools.h"
#include "apr_file_io.h"

#include "mod_vlad.h"

/* initialze kb */
int modvlad_init_kb(apr_pool_t *a_p,
                    const char *a_userfile,
                    const char *a_docroot,
                    void **a_kb,
                    void **a_exp);

/* read the policy file into kb */
int modvlad_load_kb(apr_pool_t *a_p,
                    apr_file_t *a_polfile,
                    void *a_kb,
                    void *a_exp);

/* composes an expression to query */
void *modvlad_create_query(apr_pool_t *a_p,
                           const char *a_subject,
                           const char *a_access,
                           const char *a_object);

/* strips out trailing slashes and/or everything after a ? */
const char *modvlad_strip_url(apr_pool_t *a_p, const char *a_url);

/* parse args */
int modvlad_parse_args(apr_pool_t *a_p,
                       const char *a_str,
                       apr_table_t **a_tab);

/* gets the document root without request_rec */
const char *modvlad_docroot(apr_pool_t *a_p, server_rec *a_s);

/* generates a random id between 0 and MODVLAD_MAXID */
unsigned int modvlad_idgen();

#endif
