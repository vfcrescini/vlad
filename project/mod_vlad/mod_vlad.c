/*
 * mod_vlad.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
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

#include "util.h"
#include "admin.h"

/* some external functions from the parser & lexer */
extern void policyparse();
extern void policy_set_kb(void *a_kb);
extern void policy_set_ct(void *a_exp);
extern void policy_set_yyinput(int (*a_func)(void *, char *, int),
                               void *a_stream);

/* some static functions */
static void *modvlad_create_config(apr_pool_t *a_p, server_rec *a_s);
static char *modvlad_get_passwd(request_rec *a_r,
                                const char *a_user,
                                const char *a_filename);
static int modvlad_check_passwd(request_rec *a_r,
                                const char *a_user,
                                const char *a_passwd,
                                const char *a_filename);
static void *modvlad_create_query(request_rec *a_r,
                                  const char *a_subject,
                                  const char *a_access,
                                  const char *a_object);
static int modvlad_access(request_rec *a_r);
static int modvlad_handler(request_rec *a_r);
static void modvlad_register_hooks (apr_pool_t *a_p);
static const char *modvlad_set_init(cmd_parms *a_cmd,
                                    void *a_config,
                                    const char *a_uname,
                                    const char *a_pname);

/* our command rec */
static const command_rec modvlad_auth_cmds[] =
{
  AP_INIT_TAKE2("VladFiles",
                modvlad_set_init,
                NULL,
                RSRC_CONF,
                "\"user-file\" \"policy-file\""),
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

static void *modvlad_create_config(apr_pool_t *a_p, server_rec *a_s)
{
  modvlad_config_rec *conf = NULL;

#ifdef MODVLAD_DEBUG
  ap_log_perror(APLOG_MARK,
                MODVLAD_LOGLEVEL,
                0,
                a_p,
                "modvlad_create_dir_config");
#endif

  if ((conf = (modvlad_config_rec *) apr_palloc(a_p, sizeof(*conf)))) {
    conf->user_file = NULL;
    conf->policy_file = NULL;
    conf->kb = NULL;
  }

  return conf;
}

static char *modvlad_get_passwd(request_rec *a_r,
                                const char *a_user,
                                const char *a_filename)
{
  ap_configfile_t *f;
  char l[MAX_STRING_LEN];
  const char *rpw, *w;
  apr_status_t status;

  if (!a_r || !a_user || !a_filename)
    return NULL;

  status = ap_pcfg_openfile(&f, a_r->pool, a_filename);

  if (status != APR_SUCCESS) {
    ap_log_rerror(APLOG_MARK,
                  APLOG_ERR,
                  status,
                  a_r,
                  "mod_vlad: could not open user file %s",
                  a_filename);
    return NULL;
  }

  while (!(ap_cfg_getline(l, MAX_STRING_LEN, f))) {
    if ((l[0] == '#') || (!l[0]))
      continue;

    rpw = l;
    w = ap_getword(a_r->pool, &rpw, ':');

    if (!strcmp(a_user, w)) {
      ap_cfg_closefile(f);
      return ap_getword(a_r->pool, &rpw, ':');
    }
  }

  ap_cfg_closefile(f);

  return NULL;
}

static int modvlad_check_passwd(request_rec *a_r,
                                const char *a_user,
                                const char *a_passwd,
                                const char *a_filename)
{
  const char *real_passwd;
  apr_status_t status;

  if (!a_r || !a_user  || !a_passwd || !a_filename)
    return -1;

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
    return -1;
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
    return -1;
  }

  /* hey it checked out so return 0 */
  return 0;
}

static void *modvlad_create_query(request_rec *a_r,
                                  const char *a_subject,
                                  const char *a_access,
                                  const char *a_object)
{
  int retval;
  void *atom = NULL;
  void *exp = NULL;

  if (!a_r || !a_subject  || !a_access || !a_object)
    return NULL;

  if ((retval = vlad_atom_create(&atom)) != VLAD_OK) {
    ap_log_rerror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_r,
                  "mod_vlad: could not create atom: %d",
                  retval);
    return NULL;
  }

  if ((retval = vlad_exp_create(&exp)) != VLAD_OK) {
    ap_log_rerror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_r,
                  "mod_vlad: could not create expression: %d",
                  retval);
    return NULL;
  }

  retval = vlad_atom_init_holds(atom, a_subject, a_access, a_object, 1);
  if (retval != VLAD_OK) {
    ap_log_rerror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_r,
                  "mod_vlad: could not initialize atom: %d",
                  retval);
    return NULL;
  }
  
  if ((retval = vlad_exp_add(exp, atom)) != VLAD_OK) {
    ap_log_rerror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_r,
                  "mod_vlad: could not add atom into expression: %d",
                  retval);
    return NULL;
  }

  return exp;
}

static int modvlad_access(request_rec *a_r)
{
  int retval;
  unsigned char qres;
  const char *authline = NULL;
  const char *passwd = NULL;
  const char *realuri = NULL;
  const char *filepath = NULL;
  const char *rootpath = NULL;
  modvlad_config_rec *conf = NULL;
  void *exp;

  ap_log_perror(APLOG_MARK,
                APLOG_NOTICE,
                0,
                a_r->pool,
                "mod_vlad: checking for request subject=%s access=%s object=%s",
                a_r->user,
                a_r->method,
                a_r->uri);

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
                              "\""));
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
  if (vlad_kb_check_symtab(conf->kb, realuri, VLAD_IDENT_OBJECT) != VLAD_OK &&
      vlad_kb_check_symtab(conf->kb, realuri, VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP) != VLAD_OK) {
    return HTTP_NOT_FOUND;
  }

  /* compose query expression */
  exp = modvlad_create_query(a_r, a_r->user, a_r->method, realuri);
  if (!exp)
    return HTTP_INTERNAL_SERVER_ERROR;

  /* finally, make the query */
#ifndef MODVLAD_DEBUG
  if ((retval = vlad_kb_query_evaluate(conf->kb, exp, &qres)) != VLAD_OK) {
    ap_log_rerror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_r,
                  "mod_vlad: could not evaluate query: %d",
                  retval);
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
#else
  if ((retval = vlad_kb_query_generate(conf->kb, exp, stderr)) != VLAD_OK) {
    ap_log_rerror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_r,
                  "mod_vlad: could not evaluate query: %d",
                  retval);
    return HTTP_INTERNAL_SERVER_ERROR;
  }

  fflush(stderr);

  ap_log_rerror(APLOG_MARK,
                APLOG_NOTICE,
                0,
                a_r,
                "mod_vlad: using default action on request subject=%s access=%s object=%s",
                a_r->user,
                a_r->method,
                realuri);
#endif

  return MODVLAD_DEFAULTACTION;
}

static int modvlad_handler(request_rec *a_r)
{
  modvlad_config_rec *conf = NULL;
  const char *filepath = NULL;
  const char *rootpath = NULL;

  ap_log_perror(APLOG_MARK,
                APLOG_NOTICE,
                0,
                a_r->pool,
                "mod_vlad: handling request subject=%s access=%s object=%s",
                a_r->user,
                a_r->method,
                a_r->uri);

  conf = (modvlad_config_rec *) ap_get_module_config(a_r->server->module_config,
                                                     &modvlad_module);

  /* first we make sure we are activated */
  if (!conf || !conf->kb) {
    ap_log_rerror(APLOG_MARK,
                  APLOG_NOTICE,
                  0,
                  a_r,
                  "mod_vlad: kb uninitialized, declining");
    return DECLINED;
  }

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

    /* all other methods should fail */
    return DECLINED;
  }

  return DECLINED;
}

static void modvlad_register_hooks(apr_pool_t *a_p)
{
#ifdef MODVLAD_DEBUG
  ap_log_perror(APLOG_MARK,
                MODVLAD_LOGLEVEL,
                0,
                a_p,
                "modvlad_register_hooks");
#endif

  ap_hook_handler(modvlad_handler, NULL, NULL, APR_HOOK_FIRST);
  ap_hook_access_checker(modvlad_access, NULL, NULL, APR_HOOK_FIRST);
}

static const char *modvlad_set_init(cmd_parms *a_cmd,
                                    void *a_dummy,
                                    const char *a_uname,
                                    const char *a_pname)
{
  int retval;
  modvlad_config_rec *conf;
  apr_file_t *polfile;
  apr_status_t status;
  void *const_exp;

#ifdef MODVLAD_DEBUG
  ap_log_perror(APLOG_MARK,
                MODVLAD_LOGLEVEL,
                0,
                a_cmd->pool,
                "modvlad_set_init");
#endif
  conf = ap_get_module_config(a_cmd->server->module_config, &modvlad_module);

  if (!conf) {
    ap_log_perror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_cmd->pool,
                  "mod_vlad: NULL config pointer");
    return NULL;
  }

  /* initialize the module */
  retval = modvlad_init(a_cmd->pool,
                        a_cmd->server,
                        conf,
                        &const_exp,
                        a_uname,
                        a_pname);

  if (retval) {
    ap_log_perror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_cmd->pool,
                  "mod_vlad: error occurred while initializing module");
    return NULL;
  }


  /* parse the policy file */
  status = apr_file_open(&polfile,
                        conf->policy_file,
                        APR_READ,
                        APR_OS_DEFAULT,
                        a_cmd->pool);

  if (status != APR_SUCCESS) {
    ap_log_perror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_cmd->pool,
                  "mod_vlad: error occurred while opening policy file");
    return NULL;
  }

  /* give the lexer the proper yyinput function */
  policy_set_yyinput(modvlad_apache_yyinput, (void *)polfile);
  /* give the parser a kb handle */
  policy_set_kb(conf->kb);
  /* give the parser a handle to the extra constraints expression */
  policy_set_ct(const_exp);

  /* now, we parse */
  policyparse();

  apr_file_close(polfile);

  /* finally, compute */

#ifdef MODVLAD_DEBUG
  retval = vlad_kb_compute_generate(conf->kb, stderr);
  fflush(stderr);
  if (retval != VLAD_OK) {
    ap_log_perror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_cmd->pool,
                  "mod_vlad: could not compute kb (%d)",
                  retval);
  }
#else
  retval = vlad_kb_compute_evaluate(conf->kb);
  if (retval != VLAD_OK) {
    ap_log_perror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_cmd->pool,
                  "mod_vlad: could not compute kb (%d)",
                  retval);
  }
#endif

  return NULL;
}
