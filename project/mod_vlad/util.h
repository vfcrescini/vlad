/*
 * util.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au
 */

#ifndef __MODVLAD_UTIL_H
#define __MODVLAD_UTIL_H

#include "apr_pools.h"

#ifdef DEBUG
#define MODVLAD_LOGLEVEL APLOG_NOTICE
#endif

/* a version of yyinput that uses apache apr */
int modvlad_apache_yyinput(void *a_stream, char *a_buf, int a_max);

/* a version of yyinput that uses libc fread */
int modvlad_default_yyinput(void *a_stream, char *a_buf, int a_maxsize);

/* register the users into the kb */
int modvlad_add_users(void *a_kb, const char *a_fname, apr_pool_t *a_p);

#endif
