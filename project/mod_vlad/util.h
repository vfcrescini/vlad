/*
 * util.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __MODVLAD_UTIL_H
#define __MODVLAD_UTIL_H

#include <stdlib.h>
#include "httpd.h"
#include "apr_pools.h"

#ifdef MODVLAD_DEBUG
#define MODVLAD_LOGLEVEL        APLOG_NOTICE
#endif

#define MODVLAD_DEFAULTACTION   HTTP_FORBIDDEN
#define MODVLAD_DENYACTION      HTTP_FORBIDDEN

#define MODVLAD_REALM           "VLAD"

#define MODVLAD_XML_HEADER      "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
#define MODVLAD_CONTENT_HEADER  "text/html; charset=\"utf-8\""
#define MODVLAD_DOCTYPE_HEADER  "<!DOCTYPE HTML PUBLIC \"-//W3C// DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
#define MODVLAD_MISC_HEADER     "<!-- Vino Fernando Crescini  <jcrescin@cit.uws.edu.au> -->"

#define MODVLAD_ADMIN_USERNAME  "administrator"
#define MODVLAD_ADMIN_DIRNAME   ".admin"

#define MODVLAD_MAXSTR_LEN      5120

#define MODVLAD_ACCESS_OPTIONS  "OPTIONS"
#define MODVLAD_ACCESS_GET      "GET"
#define MODVLAD_ACCESS_HEAD     "HEAD"
#define MODVLAD_ACCESS_POST     "POST"
#define MODVLAD_ACCESS_PUT      "PUT"
#define MODVLAD_ACCESS_DELETE   "DELETE"
#define MODVLAD_ACCESS_TRACE    "TRACE"
#define MODVLAD_ACCESS_CONNECT  "CONNECT"

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

#define MODVLAD_LASTCHAR(X)     (X[strlen(X) > 0 ? strlen(X) - 1 : 0])
#define MODVLAD_FIRSTCHAR(X)    (X[0])

typedef struct {
  char *user_file;
  char *policy_file;
  void *kb;
} modvlad_config_rec;

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

/* strips out trailing slashes and/or everything after a ? */
const char *modvlad_strip_url(apr_pool_t *a_p, const char *a_url);

/* parse args */
int modvlad_parse_args(apr_pool_t *a_p,
                       const char *a_str,
                       apr_table_t **a_tab);
#endif
