/*
 * mod_vlad.c
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 */

#include "httpd.h"
#include "http_config.h"
#include "http_log.h"
#include "http_main.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_request.h"
#include "ap_config.h"
#include "apr_strings.h"
#include "apr_md5.h"

#include <vlad/vlad.h>
#include <vlad/wrapper.h>

#include "mod_vlad.h"
#include "util.h"
#include "admin.h"
#include "proc.h"

/* including unistd.h causes compiler warnings so we explicitly declare here */
#ifndef _UNISTD_H
extern int getpid();
extern int getppid();
#endif

/* some static functions */
static void *modvlad_create_config(apr_pool_t *a_p, server_rec *a_s);
static char *modvlad_get_passwd(request_rec *a_r,
                                const char *a_user,
                                const char *a_filename);
static int modvlad_check_passwd(request_rec *a_r,
                                const char *a_user,
                                const char *a_passwd,
                                const char *a_filename);
static int modvlad_access(request_rec *a_r);
static int modvlad_handler(request_rec *a_r);
static int modvlad_postconfig(apr_pool_t *a_pconf, 
                              apr_pool_t *a_plog,
                              apr_pool_t *a_ptmp,
                              server_rec *a_s);
static void modvlad_childinit(apr_pool_t *a_p, server_rec *a_s);
static void modvlad_register_hooks (apr_pool_t *a_p);
static const char *modvlad_set_userfile(cmd_parms *a_cmd,
                                        void *a_config,
                                        const char *a_filename);
static const char *modvlad_set_policyfile(cmd_parms *a_cmd,
                                          void *a_config,
                                          const char *a_filename);
static const char *modvlad_set_enable(cmd_parms *a_cmd,
                                      void *a_config,
                                      int a_flag);

/* our command rec */
static const command_rec modvlad_auth_cmds[] =
{
  AP_INIT_TAKE1("VladUserFile",
                modvlad_set_userfile,
                NULL,
                RSRC_CONF,
                "\"path to user-file\""),
  AP_INIT_TAKE1("VladPolicyFile",
                modvlad_set_policyfile,
                NULL,
                RSRC_CONF,
                "\"path to policy-file\""),
  AP_INIT_FLAG("VladEnable",
               modvlad_set_enable,
               NULL,
               RSRC_CONF,
               "\"on/off to enable/disable mod_vlad\""),
  {NULL},
};

/* the module */
module AP_MODULE_DECLARE_DATA modvlad_module =
{
  STANDARD20_MODULE_STUFF,
  NULL,
  NULL,
  modvlad_create_config,
  NULL,
  modvlad_auth_cmds,
  modvlad_register_hooks,
};

/* initalize per-server config */
static void *modvlad_create_config(apr_pool_t *a_p, server_rec *a_s)
{
  modvlad_config_rec *conf = NULL;

  ap_log_perror(APLOG_MARK,
                APLOG_INFO,
                0,
                a_p,
                "mod_vlad: create config");

  if ((conf = (modvlad_config_rec *) apr_palloc(a_p, sizeof(*conf)))) {
    conf->user_file = NULL;
    conf->policy_file = NULL;
    conf->enabled = 0;
    conf->pipe_svr[0] = NULL;
    conf->pipe_svr[1] = NULL;
    conf->pipe_cli[0] = NULL;
    conf->pipe_cli[1] = NULL;
    conf->mutex = NULL;
  }

  return conf;
}

/* gives password of a_user in a_file */
static char *modvlad_get_passwd(request_rec *a_r,
                                const char *a_user,
                                const char *a_filename)
{
  char line[MODVLAD_MAXSTR_LEN];
  const char *ptr = NULL;
  const char *word = NULL;
  ap_configfile_t *cfile = NULL;
  apr_status_t status;

  if (!a_r || !a_user || !a_filename)
    return NULL;

  status = ap_pcfg_openfile(&cfile, a_r->pool, a_filename);

  if (status != APR_SUCCESS) {
    ap_log_rerror(APLOG_MARK,
                  APLOG_ERR,
                  status,
                  a_r,
                  "mod_vlad: could not open user file %s",
                  a_filename);
    return NULL;
  }

  memset(line, 0, MODVLAD_MAXSTR_LEN);

  while (!(ap_cfg_getline(line, MODVLAD_MAXSTR_LEN, cfile))) {
    if ((line[0] == '#') || (!line[0]))
      continue;

    ptr = line;
    word = ap_getword(a_r->pool, &ptr, ':');

    if (!strcmp(a_user, word)) {
      ap_cfg_closefile(cfile);
      return ap_getword(a_r->pool, &ptr, ':');
    }
  }

  ap_cfg_closefile(cfile);

  return NULL;
}

static int modvlad_check_passwd(request_rec *a_r,
                                const char *a_user,
                                const char *a_passwd,
                                const char *a_filename)
{
  const char *real_passwd = NULL;
  apr_status_t status;

  if (!a_r || !a_user  || !a_passwd || !a_filename)
    return MODVLAD_NULLPTR;

  /* try to get real password from file */
  real_passwd = modvlad_get_passwd(a_r, a_user, a_filename);

  if (!real_passwd) {
    ap_log_rerror(APLOG_MARK,
                  APLOG_NOTICE,
                  0,
                  a_r,
                  "mod_vlad: invalid user: user=\"%s\" uri=\"%s\"",
                  a_r->user,
                  a_r->uri);
    return MODVLAD_FAILURE;
  }

  /* now validate the password */
  status = apr_password_validate(a_passwd, real_passwd);

  if (status != APR_SUCCESS) {
    ap_log_rerror(APLOG_MARK,
                  APLOG_NOTICE,
                  0,
                  a_r,
                  "mod_vlad: password mismatch: user=\"%s\" uri=\"%s\"",
                  a_r->user,
                  a_r->uri);
    return MODVLAD_FAILURE;
  }

  /* hey it checked out so return 0 */
  return MODVLAD_OK;
}

static int modvlad_access(request_rec *a_r)
{
  unsigned char qres;
  const char *authline = NULL;
  const char *passwd = NULL;
  const char *realuri = NULL;
  const char *filepath = NULL;
  const char *rootpath = NULL;
  modvlad_config_rec *conf = NULL;

  ap_log_rerror(APLOG_MARK,
                APLOG_INFO,
                0,
                a_r,
                "mod_vlad: check access");

  if (!a_r) {
    ap_log_rerror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_r,
                  "mod_vlad: check access request is NULL");
    return HTTP_INTERNAL_SERVER_ERROR;
  }

  conf = (modvlad_config_rec *) ap_get_module_config(a_r->server->module_config,
                                                     &modvlad_module);

  if (!conf) {
    ap_log_rerror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_r,
                  "mod_vlad: conf is null");
    return HTTP_INTERNAL_SERVER_ERROR;
  }

  /* first check if we are enabled */
  if (!conf->enabled)
    return DECLINED;

  /* parse the incoming header to get the authorization line */
  if ((authline = apr_table_get(a_r->headers_in, "Authorization"))) {
    ap_getword(a_r->pool, &authline, ' ');
    while (*authline == ' ' || *authline == '\t')
      authline++;
    passwd = ap_pbase64decode(a_r->pool, authline);
    a_r->ap_auth_type = "Basic";
    a_r->user = ap_getword_nulls(a_r->pool, &passwd, ':');
  }

  /* if there is no authorization, or if the given password does not match 
     we send a http authorization header */
  if (!a_r->ap_auth_type ||
      strcmp(a_r->ap_auth_type, "Basic") ||
      modvlad_check_passwd(a_r, a_r->user, passwd, conf->user_file)) {
    a_r->ap_auth_type = apr_pstrdup(a_r->pool, "Basic");
    apr_table_set(a_r->err_headers_out,
                  "WWW-Authenticate",
                  apr_pstrcat(a_r->pool,
                              "Basic realm=\"",
                              MODVLAD_REALM,
                              "\"",
                              NULL));
    return HTTP_UNAUTHORIZED;
  }

  /* user authenticated, now we check for authorization */

  /* clean url */
  realuri = modvlad_strip_url(a_r->pool, a_r->uri);
  filepath = apr_pstrdup(a_r->pool, realuri);
  apr_filepath_root(&rootpath, &filepath, 0, a_r->pool);

  /* if user is admin, unconditionally allow access to all resources for
     all methods*/
  if (!strcmp(a_r->user, MODVLAD_ADMIN_USERNAME))
    return OK;

  /* before going further, make sure the object is in the kb */
  if (modvlad_client_ident_check(a_r->pool,
                                 conf->pipe_cli[0],
                                 conf->pipe_cli[1],
                                 conf->mutex,
                                 realuri,
                                 VLAD_IDENT_OBJECT) != MODVLAD_OK &&
      modvlad_client_ident_check(a_r->pool,
                                 conf->pipe_cli[0],
                                 conf->pipe_cli[1],
                                 conf->mutex,
                                 realuri,
                                 VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP) != MODVLAD_OK)
    return HTTP_NOT_FOUND;

  if (modvlad_client_query(a_r->pool,
                           conf->pipe_cli[0],
                           conf->pipe_cli[1],
                           conf->mutex,
                           a_r->user,
                           a_r->method,
                           realuri,
                           &qres) != MODVLAD_OK) {
    ap_log_rerror(APLOG_MARK,
                  APLOG_NOTICE,
                  0,
                  a_r,
                  "mod_vlad: unable to query kb");
    return HTTP_INTERNAL_SERVER_ERROR;
  }

  switch(qres) {
    case VLAD_RESULT_TRUE :
      ap_log_rerror(APLOG_MARK,
                    APLOG_NOTICE,
                    0,
                    a_r,
                    "mod_vlad: accepting request subject=%s access=%s object=%s",
                    a_r->user,
                    a_r->method,
                    realuri);
      return OK;
    case VLAD_RESULT_FALSE :
      ap_log_rerror(APLOG_MARK,
                    APLOG_NOTICE,
                    0,
                    a_r,
                    "mod_vlad: rejecting request subject=%s access=%s object=%s",
                    a_r->user,
                    a_r->method,
                    realuri);
      return MODVLAD_DENYACTION;
    default :
      ap_log_rerror(APLOG_MARK,
                    APLOG_NOTICE,
                    0,
                    a_r,
                    "mod_vlad: using default action on request subject=%s access=%s object=%s",
                    a_r->user,
                    a_r->method,
                    realuri);
      return MODVLAD_DEFAULTACTION;
  }

  return MODVLAD_DEFAULTACTION;
}

static int modvlad_handler(request_rec *a_r)
{
  modvlad_config_rec *conf = NULL;
  const char *filepath = NULL;
  const char *rootpath = NULL;

  ap_log_perror(APLOG_MARK,
                APLOG_INFO,
                0,
                a_r->pool,
                "mod_vlad: handling request subject=%s access=%s object=%s",
                a_r->user,
                a_r->method,
                a_r->uri);

  conf = (modvlad_config_rec *) ap_get_module_config(a_r->server->module_config,
                                                     &modvlad_module);

  if (!conf) {
    ap_log_rerror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_r,
                  "mod_vlad: conf is NULL");
    return HTTP_INTERNAL_SERVER_ERROR;
  }

  /* first check if we are enabled */
  if (!conf->enabled)
    return DECLINED;

  filepath = apr_pstrdup(a_r->pool, modvlad_strip_url(a_r->pool, a_r->uri));
  apr_filepath_root(&rootpath, &filepath, 0, a_r->pool);

  /* we only accept if the request is (administrator, GET/POST, *.admin) */
  if (!strcmp(a_r->user, MODVLAD_ADMIN_USERNAME) &&
      !strcmp(filepath, MODVLAD_ADMIN_DIRNAME)) {

    if (!strcmp(a_r->method, MODVLAD_ACCESS_GET)) {
      ap_set_content_type(a_r, MODVLAD_CONTENT_HEADER);
      modvlad_generate_header(a_r);
      modvlad_generate_form(a_r, conf);
      modvlad_generate_footer(a_r);
      return OK;
    }
    else if (!strcmp(a_r->method, MODVLAD_ACCESS_POST)) {
      ap_set_content_type(a_r, MODVLAD_CONTENT_HEADER);
      ap_setup_client_block(a_r, REQUEST_CHUNKED_DECHUNK);
      modvlad_generate_header(a_r);
      modvlad_handle_form(a_r, conf);
      modvlad_generate_form(a_r, conf);
      modvlad_generate_footer(a_r);
      return OK;
    }
  }

  return DECLINED;
}

static int modvlad_postconfig(apr_pool_t *a_pconf, 
                              apr_pool_t *a_plog,
                              apr_pool_t *a_ptmp,
                              server_rec *a_s)
{
  apr_status_t status;
  const char *docroot = NULL;
  modvlad_config_rec *conf = NULL;
  apr_proc_t *proc = NULL;

  /* it seems that this function is called twice, once on 2 processes.
   * since the first one seems to die anyway, we only initialize the
   * module on the second process whose parent seems to be init. */

  if (getppid() != 1)
    return DECLINED;

  ap_log_perror(APLOG_MARK,
                APLOG_INFO,
                0,
                a_plog,
                "mod_vlad: postconfig: pid=%d", getpid());

  conf = ap_get_module_config(a_s->module_config, &modvlad_module);

  if (!conf) {
    ap_log_perror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_plog,
                  "mod_vlad: conf is NULL");
    return HTTP_INTERNAL_SERVER_ERROR;
  }

  /* first check if we are enabled */
  if (!conf->enabled)
    return DECLINED;

  /* create pipes */
  apr_file_pipe_create(&(conf->pipe_svr[1]), &(conf->pipe_cli[0]), a_pconf);
  apr_file_pipe_create(&(conf->pipe_cli[1]), &(conf->pipe_svr[0]), a_pconf);

  /* create mutex */
  if (apr_proc_mutex_create(&(conf->mutex),
                            apr_pstrdup(a_pconf,
                                         MODVLAD_MUTEX_PATH),
                            APR_LOCK_DEFAULT,
                            a_pconf) != APR_SUCCESS) {
    ap_log_perror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_plog,
                  "mod_vlad: failed to init mutex in parent");
    return HTTP_INTERNAL_SERVER_ERROR;
  }

  /* get document root */
  docroot = modvlad_docroot(a_pconf, a_s);

  /* now f**k (fork?) off */
  proc = apr_palloc(a_pconf, sizeof(apr_proc_t *));
  status = apr_proc_fork(proc, a_pconf);

  if (status == APR_INCHILD) {
    apr_pool_t *childpool = NULL;
    void *kbase = NULL;

    /* create a new pool for this process */
    apr_pool_create(&childpool, NULL);

    if (modvlad_server_init(childpool,
                            &kbase,
                            docroot,
                            conf->user_file,
                            conf->policy_file) != MODVLAD_OK) {
      ap_log_perror(APLOG_MARK,
                    APLOG_ERR,
                    0,
                    a_plog,
                    "mod_vlad: failed to init parent");
      return HTTP_INTERNAL_SERVER_ERROR;
    }

    /* this should never return */
    modvlad_server_listen(childpool, kbase, conf->pipe_svr[0], conf->pipe_svr[1]);

    apr_pool_destroy(childpool);
  }
  else if (status == APR_INPARENT) {
    /* register the child to die with this process */
    apr_pool_note_subprocess(a_pconf, proc, APR_KILL_AFTER_TIMEOUT);
    return OK;
  }
  else {
    return HTTP_INTERNAL_SERVER_ERROR;
  }

  return OK;
}

static void modvlad_childinit(apr_pool_t *a_p, server_rec *a_s)
{
  modvlad_config_rec *conf = NULL;

  ap_log_perror(APLOG_MARK,
                APLOG_INFO,
                0,
                a_p,
                "mod_vlad: childinit: pid=%d", getpid());

  /* set seed for rand */
  srand(time(NULL));

  /* we need conf to init the mutex */
  conf = (modvlad_config_rec *) ap_get_module_config(a_s->module_config,
                                                     &modvlad_module);

  if (!conf) {
    ap_log_perror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_p,
                  "mod_vlad: conf is null");
    return;
  }

  /* first check if we are enabled */
  if (!conf->enabled)
    return;

  /* reinit the mutex for this process */
  if (apr_proc_mutex_child_init(&(conf->mutex), MODVLAD_MUTEX_PATH, a_p) != APR_SUCCESS) {
    ap_log_perror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_p,
                  "mod_vlad: failed to reinit mutex in child");
    return;
  }
}

static void modvlad_register_hooks(apr_pool_t *a_p)
{
  ap_log_perror(APLOG_MARK,
                APLOG_INFO,
                0,
                a_p,
                "mod_vlad: register hooks");

  ap_hook_handler(modvlad_handler, NULL, NULL, APR_HOOK_FIRST);
  ap_hook_access_checker(modvlad_access, NULL, NULL, APR_HOOK_FIRST);
  ap_hook_post_config(modvlad_postconfig, NULL, NULL, APR_HOOK_FIRST);
  ap_hook_child_init(modvlad_childinit, NULL, NULL, APR_HOOK_FIRST);
}

static const char *modvlad_set_userfile(cmd_parms *a_cmd,
                                        void *a_config,
                                        const char *a_filename)
{
  modvlad_config_rec *conf = NULL;

  conf = ap_get_module_config(a_cmd->server->module_config, &modvlad_module);

  if (!conf) {
    ap_log_perror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_cmd->pool,
                  "mod_vlad: conf is NULL");
    return NULL;
  }

  conf->user_file = ap_server_root_relative(a_cmd->pool, a_filename);

  return NULL;
}

static const char *modvlad_set_policyfile(cmd_parms *a_cmd,
                                          void *a_config,
                                          const char *a_filename)
{
  modvlad_config_rec *conf = NULL;

  conf = ap_get_module_config(a_cmd->server->module_config, &modvlad_module);

  if (!conf) {
    ap_log_perror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_cmd->pool,
                  "mod_vlad: conf is NULL");
    return NULL;
  }

  conf->policy_file = ap_server_root_relative(a_cmd->pool, a_filename);

  return NULL;
}

static const char *modvlad_set_enable(cmd_parms *a_cmd,
                                      void *a_config,
                                      int a_flag)
{
  modvlad_config_rec *conf = NULL;

  conf = ap_get_module_config(a_cmd->server->module_config, &modvlad_module);

  if (!conf) {
    ap_log_perror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_cmd->pool,
                  "mod_vlad: conf is NULL");
    return NULL;
  }

  conf->enabled = a_flag;

  return NULL;
}
