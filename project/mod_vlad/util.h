/*
 * util.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au
 */

#ifndef __MODVLAD_UTIL_H
#define __MODVLAD_UTIL_H

#include <stdlib.h>
#include "httpd.h"
#include "apr_pools.h"

#ifdef DEBUG
#define MODVLAD_LOGLEVEL APLOG_NOTICE
#endif

typedef struct {
  char *user_file;
  char *policy_file;
  void *kb;
  char *path;
} modvlad_config_rec;

#define MODVLAD_ACCESS_OPTIONS "options"
#define MODVLAD_ACCESS_GET     "get"
#define MODVLAD_ACCESS_HEAD    "head"
#define MODVLAD_ACCESS_POST    "post"
#define MODVLAD_ACCESS_PUT     "put"
#define MODVLAD_ACCESS_DELETE  "delete"
#define MODVLAD_ACCESS_TRACE   "trace"
#define MODVLAD_ACCESS_CONNECT "connect"

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

/* a version of yyinput that uses apache apr */
int modvlad_apache_yyinput(void *a_stream, char *a_buf, int a_max);

/* a version of yyinput that uses libc fread */
int modvlad_default_yyinput(void *a_stream, char *a_buf, int a_maxsize);

/* initialze conf */
int modvlad_init(apr_pool_t *a_p,
                 server_rec *a_s,
                 modvlad_config_rec *a_conf,
                 const char *a_uname,
                 const char *a_pname);

#endif
