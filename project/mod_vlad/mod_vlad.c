/*
 * mod_vlad.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include "httpd.h"
#include "http_config.h"
#include "http_log.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_request.h"
#include "ap_config.h"
#include "apr_strings.h"
#include "apr_md5.h"

typedef struct {
  char *user_file;
  char *policy_file;
} modvlad_config_rec;

static void *modvlad_create_dir_config(apr_pool_t *a_p, char *a_d);
static char *modvlad_get_passwd(request_rec *a_r, 
                                char *a_user, 
                                char *a_passwd_file);
static int modvlad_authenticate(request_rec *a_r);
static int modvlad_authorize(request_rec *a_r);
static void modvlad_register_hooks (apr_pool_t *a_p);
static const char *modvlad_set_auth_slot(cmd_parms *a_cmd, 
                                         void *a_offset, 
                                         const char *a_f,
                                         const char *a_t);

static const command_rec modvlad_auth_cmds[] =
{
  AP_INIT_TAKE12("VladUserFile", 
                 modvlad_set_auth_slot, 
                 (void *)APR_OFFSETOF(modvlad_config_rec, user_file),
                 OR_AUTHCFG,
                 "authentication / user file (htpasswd)"),

  AP_INIT_TAKE12("VladPolicyFile",
                 modvlad_set_auth_slot,
                 (void *)APR_OFFSETOF(modvlad_config_rec, policy_file),
                 OR_AUTHCFG,
                 "authorization / policy file"),

  {NULL},
};

module AP_MODULE_DECLARE_DATA modvlad_module =
{
  STANDARD20_MODULE_STUFF,
  modvlad_create_dir_config,
  NULL,
  NULL,
  NULL,
  modvlad_auth_cmds,
  modvlad_register_hooks,
};

static void *modvlad_create_dir_config(apr_pool_t *a_p, char *a_d)
{
  modvlad_config_rec *conf;

  conf = (modvlad_config_rec *) apr_palloc(a_p, sizeof(*conf));
  conf->user_file = NULL;
  conf->policy_file = NULL;

  return conf;
}

static char *modvlad_get_passwd(request_rec *a_r,
                                char *a_user,
                                char *a_passwd_file)
{
  ap_configfile_t *f;
  char l[MAX_STRING_LEN];
  const char *rpw, *w;
  apr_status_t status;

  status = ap_pcfg_openfile(&f, a_r->pool, a_passwd_file);
      
  if (status != APR_SUCCESS) {

    ap_log_rerror(APLOG_MARK,
                  APLOG_ERR,
                  status,
                  a_r,
                  "could not open aunthentication / user file: %s",
                  a_passwd_file);

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

static int modvlad_authenticate(request_rec *a_r)
{
  modvlad_config_rec *conf = NULL;
  const char *sent_passwd;
  char *real_passwd;
  apr_status_t invalid_passwd;
  int retval;

  fprintf(stderr, "authenticate\n");
  fflush(stderr);

  conf = (modvlad_config_rec *) ap_get_module_config(a_r->per_dir_config,
                                                     &modvlad_module);

  /* get password from browser */
  if ((retval = ap_get_basic_auth_pw(a_r, &sent_passwd))) {
    return retval;
  }

  /* hmm... no authentication file. what do we do? */
  if (!conf->user_file) {
    return DECLINED;
  }

  /* get the password from the file */
  real_passwd = modvlad_get_passwd(a_r, a_r->user, conf->user_file);

  if (!real_passwd) {

    ap_log_rerror(APLOG_MARK, APLOG_ERR,
                  0, 
                  a_r, 
                  "user %s not found: %s",
                  a_r->user,
                  a_r->uri);

    ap_note_basic_auth_failure(a_r);

    return HTTP_UNAUTHORIZED;
  }

  /* now validate the password */
  invalid_passwd = apr_password_validate(sent_passwd, real_passwd);

  if (invalid_passwd != APR_SUCCESS) {

    ap_log_rerror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_r,
                  "use %s: authentication failure for \"%s\": %s",
                  "Password Mismatch",
                  a_r->user,
                  a_r->uri);

    ap_note_basic_auth_failure(a_r);

    return HTTP_UNAUTHORIZED;
  }

  return OK;
}

static int modvlad_authorize(request_rec *a_r)
{
  fprintf(stderr, "authorize\n");
  fflush(stderr);
  fprintf(stderr, "uri: %s\n", a_r->uri);
  fprintf(stderr, "unparsed_uri: %s\n", a_r->unparsed_uri);
  fprintf(stderr, "filename: %s\n", a_r->filename);
  fprintf(stderr, "path_info: %s\n", a_r->path_info);
  fprintf(stderr, "args: %s\n", a_r->args);
  fprintf(stderr, "hostname: %s\n", a_r->hostname);
  fprintf(stderr, "method: %s\n", a_r->method);
  fprintf(stderr, "docroot: %s\n", ap_document_root(a_r));
  fflush(stderr);

  return OK;
}

static void modvlad_register_hooks(apr_pool_t *a_p)
{
  ap_hook_check_user_id(modvlad_authenticate, NULL, NULL, APR_HOOK_FIRST);
  ap_hook_auth_checker(modvlad_authorize, NULL, NULL, APR_HOOK_FIRST);
}

static const char *modvlad_set_auth_slot(cmd_parms *a_cmd,
                                         void *a_offset, 
                                         const char *a_f,
                                         const char *a_t)
{
  if (a_t && strcmp(a_t, "standard"))
    return apr_pstrcat(a_cmd->pool, "invalid file type", a_t, NULL);

  return ap_set_file_slot(a_cmd, a_offset, a_f);
}
