/*
 * util.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __MODVLAD_UTIL_H
#define __MODVLAD_UTIL_H

#include "httpd.h"
#include "apr_pools.h"
#include "apr_file_io.h"

#include "mod_vlad.h"

/* initialze kb */
int modvlad_init_kb(apr_pool_t *a_p,
                    server_rec *a_s,
                    modvlad_config_rec *a_conf,
                    void **a_exp);

/* read the policy file into kb */
int modvlad_load_kb(apr_pool_t *a_p,
                    apr_file_t *a_polfile,
                    void *a_kb,
                    void *a_exp);
 
/* strips out trailing slashes and/or everything after a ? */
const char *modvlad_strip_url(apr_pool_t *a_p, const char *a_url);

/* parse args */
int modvlad_parse_args(apr_pool_t *a_p,
                       const char *a_str,
                       apr_table_t **a_tab);
#endif
