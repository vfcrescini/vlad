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

#include <vlad/vlad.h>
#include <vlad/wrapper.h>

#include "util.h"

/* some external functions from the parser & lexer */
extern void policyparse();
extern void policy_set_yyinput(int (*a_func)(void *, char *, int), 
                               void *a_stream);

typedef struct {
  char *user_file;
  char *policy_file;
  void *kb;
  char *path;
} modvlad_config_rec;

static void *modvlad_create_dir_config(apr_pool_t *a_p, char *a_d);
static char *modvlad_get_passwd(request_rec *a_r,
                                char *a_user,
                                char *a_passwd_file);
static int modvlad_authenticate(request_rec *a_r);
static int modvlad_authorize(request_rec *a_r);
static void modvlad_register_hooks (apr_pool_t *a_p);
static const char *modvlad_set_init(cmd_parms *a_cmd,
                                    void *a_config,
                                    const char *a_uname,
                                    const char *a_pname);

static const command_rec modvlad_auth_cmds[] =
{
  AP_INIT_TAKE2("VladFiles",
                modvlad_set_init,
                NULL,
                OR_AUTHCFG,
                "\"user-file\" \"policy-file\""),
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

  if (!a_d)
    return NULL;

#ifdef DEBUG
  ap_log_perror(APLOG_MARK,
                MODVLAD_LOGLEVEL,
                0,
                a_p,
                "modvlad_create_dir_config: %s",
                a_d);
#endif

  if ((conf = (modvlad_config_rec *) apr_palloc(a_p, sizeof(*conf)))) {
    conf->user_file = NULL;
    conf->policy_file = NULL;
    conf->kb = NULL;
    conf->path = strcmp(a_d, "/") ? 
                 apr_pstrdup(a_p, a_d) : 
                 ap_server_root_relative(a_p, a_d);
  }

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
                  "could not open aunthentication/user file: %s",
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

#ifdef DEBUG
  ap_log_rerror(APLOG_MARK,
                MODVLAD_LOGLEVEL,
                0,
                a_r,
                "modvlad_authenticate");
#endif

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

    ap_log_rerror(APLOG_MARK,
                  APLOG_ERR,
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
                  "user %s: authentication failure for \"%s\": %s",
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
  modvlad_config_rec *conf = NULL;

  conf = (modvlad_config_rec *) ap_get_module_config(a_r->per_dir_config,
                                                     &modvlad_module);

#ifdef DEBUG
  ap_log_rerror(APLOG_MARK,
                MODVLAD_LOGLEVEL,
                0,
                a_r,
                "modvlad_authorize\n\turi: %s\n\tunparsed-uri: %s\n\tfilename: %s\n\tpath-info: %s\n\targs: %s\n\thostname: %s\n\tmethod: %s\n\tdocroot: %s\n\tconf: %x\n\tkb: %x\n\tpolicy-file: %s\n\tpath: %s",
                a_r->uri,
                a_r->unparsed_uri,
                a_r->filename,
                a_r->path_info,
                a_r->args,
                a_r->hostname,
                a_r->method,
                ap_document_root(a_r),
                (unsigned int) conf,
                (unsigned int) conf->kb,
                conf->policy_file,
                conf->path);
#endif

  return OK;
}

static void modvlad_register_hooks(apr_pool_t *a_p)
{
#ifdef DEBUG
  ap_log_perror(APLOG_MARK,
                MODVLAD_LOGLEVEL,
                0,
                a_p,
                "modvlad_register_hooks");
#endif

  ap_hook_check_user_id(modvlad_authenticate, NULL, NULL, APR_HOOK_FIRST);
  ap_hook_auth_checker(modvlad_authorize, NULL, NULL, APR_HOOK_FIRST);
}

static const char *modvlad_set_init(cmd_parms *a_cmd,
                                    void *a_config,
                                    const char *a_uname,
                                    const char *a_pname)
{
  modvlad_config_rec *conf = NULL;
  apr_file_t *polfile;

#ifdef DEBUG
  ap_log_perror(APLOG_MARK,
                MODVLAD_LOGLEVEL,
                0,
                a_cmd->pool,
                "modvlad_set_init");
#endif

  conf = (modvlad_config_rec *) a_config;

  /* set up filenames */
  conf->user_file = ap_server_root_relative(a_cmd->pool, a_uname);
  conf->policy_file = ap_server_root_relative(a_cmd->pool, a_pname);

  /* set up kb */
  kb_create(&(conf->kb));
  kb_init(conf->kb);

  /* register the kb to be destroyed with this pool */
  apr_pool_cleanup_register(a_cmd->pool,
                            conf->kb,
                            kb_destroy,
                            apr_pool_cleanup_null);

  /* now for some real initialisation */
  modvlad_add_subject(conf->kb, conf->user_file, a_cmd->pool); 
  modvlad_add_access(conf->kb, a_cmd->pool);
  modvlad_add_object(conf->kb, conf->path, a_cmd->pool);

  /* parse the policy file */
  apr_file_open(&polfile,
                conf->policy_file,
                APR_READ,
                APR_OS_DEFAULT,
                a_cmd->pool);

  /* give the lexer the proper yyinput function */
  policy_set_yyinput(modvlad_apache_yyinput, (void *)polfile);

  policyparse();

  apr_file_close(polfile);

  return NULL;
}
