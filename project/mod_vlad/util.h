/*
 * util.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au
 */

#ifndef __MODVLAD_UTIL_H
#define __MODVLAD_UTIL_H

#include <stdlib.h>
#include "httpd.h"
#include "apr_pools.h"

#ifdef MODVLAD_DEBUG
#define MODVLAD_LOGLEVEL APLOG_NOTICE
#endif

#define MODVLAD_DEFAULTACTION HTTP_UNAUTHORIZED

typedef struct {
  char *user_file;
  char *policy_file;
  void *kb;
  char *path;
} modvlad_config_rec;

#define MODVLAD_ACCESS_OPTIONS "OPTIONS"
#define MODVLAD_ACCESS_GET     "GET"
#define MODVLAD_ACCESS_HEAD    "HEAD"
#define MODVLAD_ACCESS_POST    "POST"
#define MODVLAD_ACCESS_PUT     "PUT"
#define MODVLAD_ACCESS_DELETE  "DELETE"
#define MODVLAD_ACCESS_TRACE   "TRACE"
#define MODVLAD_ACCESS_CONNECT "CONNECT"

#define MODVLAD_ACCESS_ARRAY    \
{                               \
  MODVLAD_ACCESS_OPTIONS,       \
  MODVLAD_ACCESS_GET,           \
  MODVLAD_ACCESS_HEAD,          \
  MODVLAD_ACCESS_POST,          \
  MODVLAD_ACCESS_PUT,           \
  MODVLAD_ACCESS_DELETE,        \
  MODVLAD_ACCESS_TRACE,         \
  MODVLAD_ACCESS_CONNECT,       \
  NULL,                         \
}

#define MODVLAD_LASTCHAR(X) (X[strlen(X) - 1])

/* a version of yyinput that uses apache apr */
int modvlad_apache_yyinput(void *a_stream, char *a_buf, int a_max);

/* a version of yyinput that uses libc fread */
int modvlad_default_yyinput(void *a_stream, char *a_buf, int a_maxsize);

/* initialze conf */
int modvlad_init(apr_pool_t *a_p,
                 server_rec *a_s,
                 modvlad_config_rec *a_conf,
                 void **a_exp,
                 const char *a_uname,
                 const char *a_pname);

/* strips out the trailing / from a_str */
const char *modvlad_strip_slash(apr_pool_t *a_p, const char *a_str);

#endif
