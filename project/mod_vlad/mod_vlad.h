/*
 * mod_vlad.h
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __MODVLAD_MOD_VLAD_H
#define __MODVLAD_MOD_VLAD_H

#include <stdlib.h>

#include "apr_file_io.h"
#include "apr_proc_mutex.h"

#include "config.h"

#define MODVLAD_DEFAULTACTION   HTTP_FORBIDDEN
#define MODVLAD_DENYACTION      HTTP_FORBIDDEN

#define MODVLAD_REALM           "VLAD"

#define MODVLAD_XML_HEADER      "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
#define MODVLAD_CONTENT_HEADER  "text/html; charset=\"utf-8\""
#define MODVLAD_DOCTYPE_HEADER  "<!DOCTYPE HTML PUBLIC \"-//W3C// DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
#define MODVLAD_MISC_HEADER     "<!-- Vino Fernando Crescini  <jcrescin@cit.uws.edu.au> -->"

#define MODVLAD_ADMIN_USERNAME  "administrator"
#define MODVLAD_ADMIN_DIRNAME   ".admin"

#define MODVLAD_MUTEX_PATH      "/tmp/mod_vlad.lock"

#define MODVLAD_MAXSTR_LEN      5120
#define MODVLAD_MAXMSG_LEN      2048
#define MODVLAD_MAXID           999999

#define MODVLAD_DELIMITER       '\004'
#define MODVLAD_TERMINATOR      '\002'

/* access-rights (http methods) */
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
  NULL                          \
}

/* error codes */
/* success */
#define MODVLAD_OK              0
/* general failure code */
#define MODVLAD_FAILURE         -1
/* null pointer input */
#define MODVLAD_NULLPTR         -2
/* failed memory allocation */
#define MODVLAD_MALLOCFAILED    -3
/* input is invalid */
#define MODVLAD_INVALIDINPUT    -4
/* buffer capacity exceeded */
#define MODVLAD_BUFFEROVERFLOW  -5
/* message received/sent is out of sequence */
#define MODVLAD_OUTOFSEQ        -6

/* convenience marcos */
#define MODVLAD_LASTCHAR(X)     (X[strlen(X) > 0 ? strlen(X) - 1 : 0])
#define MODVLAD_FIRSTCHAR(X)    (X[0])

typedef struct {
  char *user_file;
  char *policy_file;
  int enabled;
  apr_file_t *pipe_svr[2];
  apr_file_t *pipe_cli[2];
  apr_proc_mutex_t *mutex;
} modvlad_config_rec;

#endif
