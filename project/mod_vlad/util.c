/*
 * util.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#define CORE_PRIVATE

#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "http_log.h"
#include "ap_config.h"
#include "apr_pools.h"
#include "apr_file_io.h"
#include "apr_strings.h"

#include <vlad/vlad.h>
#include <vlad/wrapper.h>

#include "util.h"

/* register the users into the kb */
static int add_subject(apr_pool_t *a_p, void *a_kb, const char *a_fname);
/* add built in access rights into the kb */
static int add_access(apr_pool_t *a_p, void *a_kb);
/* add the path's directory tree into the kb */
static int add_object(apr_pool_t *a_p,
                      void *a_kb,
                      const char *a_path,
                      server_rec *a_s);
/* add objects in kb recursively. a_relpath should be NULL or "" */
static int add_object_recurse(apr_pool_t *a_p,
                              void *a_kb,
                              const char *a_basepath,
                              const char *a_relpath);
/* converts / to docroot */
static const char *get_docroot(apr_pool_t *a_p,
                               const char *a_path,
                               server_rec *a_s);

/* a version of yyinput that uses apache apr */
int modvlad_apache_yyinput(void *a_stream, char *a_buf, int a_max)
{
  apr_size_t size = (apr_size_t) a_max;
  apr_file_t *file = (apr_file_t *) a_stream;

  apr_file_read(file, (void *) a_buf, &size);

  return size;
}

/* a version of yyinput that uses libc fread */
int modvlad_default_yyinput(void *a_stream, char *a_buf, int a_maxsize)
{
  int size;
  FILE *tmp = (FILE *) a_stream;

  /* if an error is encountered, assume EOF */
  if (((size = fread(a_buf, 1, a_maxsize, tmp)) == 0) && ferror(tmp)) {
    *a_buf = EOF;
    return 1;
  }

  return size;
}

/* initialze conf */
int modvlad_init(apr_pool_t *a_p,
                 server_rec *a_s,
                 modvlad_config_rec *a_conf,
                 const char *a_uname,
                 const char *a_pname)
{
  int retval;

  if (!a_p || !a_s || !a_conf || !a_uname || !a_pname)
    return -1;

  /* setup filenames */
  a_conf->user_file = ap_server_root_relative(a_p, a_uname);
  a_conf->policy_file = ap_server_root_relative(a_p, a_pname);

  /* create and init kb */
  retval = kb_create(&(a_conf->kb));
  retval = kb_init(a_conf->kb);

  /* register the kb to be destroyed with this pool */

  apr_pool_cleanup_register(a_p,
                            a_conf->kb,
			    kb_destroy,
                            apr_pool_cleanup_null);

  if (add_subject(a_p, a_conf->kb, a_conf->user_file))
    return -1;
  if (add_access(a_p, a_conf->kb))
    return -1;
  if (add_object(a_p, a_conf->kb, a_conf->path, a_s))
    return -1;

  return 0;
}

/* register the users into the kb */
static int add_subject(apr_pool_t *a_p, void *a_kb, const char *a_fname)
{
  int retval;
  char line[MAX_STRING_LEN];
  const char *user;
  const char *lineptr;
  ap_configfile_t *cfgfile = NULL;
  apr_status_t status;

  if (a_fname == NULL)
    return -1;

  status = ap_pcfg_openfile(&cfgfile, a_p, a_fname);

  if (status != APR_SUCCESS) {
    ap_log_perror(APLOG_MARK,
                  APLOG_ERR,
                  status,
                  a_p,
                  "mod_vlad: could not open user file: %s",
                  a_fname);

    return -1;
  }

  /* read one line at a time */
  while (!(ap_cfg_getline(line, MAX_STRING_LEN, cfgfile))) {

    if (line[0] == '#' || !line[0])
      continue;

    lineptr = line;
    user = ap_getword(a_p, &lineptr, ':');

    /* now that we have the user, we can then add it to the kb */
#ifdef DEBUG
    ap_log_perror(APLOG_MARK,
                  MODVLAD_LOGLEVEL,
                  0,
                  a_p,
                  "adding subject %s into kb",
                  user);
#endif

    retval = kb_add_symtab(a_kb, user, VLAD_IDENT_SUBJECT);

    if (retval != VLAD_OK) {
      ap_log_perror(APLOG_MARK,
                    APLOG_ERR,
                    0,
                    a_p,
                    "mod_vlad: could not add user \"%s\" to kb: error %d",
                    user,
                    retval);
      return -1;
    }
  }

  ap_cfg_closefile(cfgfile);

  return 0;
}

/* add built in access rights into the kb */
static int add_access(apr_pool_t *a_p, void *a_kb)
{
  const char *acc_array[] = MODVLAD_ACCESS_ARRAY;
  const char **array_ptr = acc_array;
  const char *access;
  int retval;

  while((access = *(array_ptr++)) != NULL) {

#ifdef DEBUG
    ap_log_perror(APLOG_MARK,
                  MODVLAD_LOGLEVEL,
                  0,
                  a_p,
                  "adding access-right %s into kb",
                  access);
#endif

    retval = kb_add_symtab(a_kb, access, VLAD_IDENT_ACCESS);

    if (retval != VLAD_OK) {
      ap_log_perror(APLOG_MARK,
                    APLOG_ERR,
                    0,
                    a_p,
                    "mod_vlad: could not add access \"%s\" to kb: error %d",
                    access,
                    retval);
      return -1;
    }
  }

  return 0;
}

/* add the path's directory tree into the kb */
static int add_object(apr_pool_t *a_p,
                      void *a_kb,
                      const char *a_path,
                      server_rec *a_s)
{
#ifdef DEBUG
  ap_log_perror(APLOG_MARK,
                MODVLAD_LOGLEVEL,
                0,
                a_p,
                "adding object from %s into kb",
                a_path);
#endif

  return add_object_recurse(a_p, a_kb, get_docroot(a_p, a_path, a_s), "");
}

/* add objects in kb recursively. a_relpath should be NULL or "" */
static int add_object_recurse(apr_pool_t *a_p,
                              void *a_kb,
                              const char *a_basepath,
                              const char *a_relpath)
{
  int retval;
  apr_dir_t *pdir;
  apr_finfo_t dinfo;
  const char *realrelpath = NULL;
  const char *realfullpath = NULL;

  /* first chech stuff */
  if (!a_kb || !a_basepath || !a_p)
    return -1;

  realrelpath = apr_pstrdup(a_p, (!a_relpath ? "" : a_relpath));
  realfullpath = apr_pstrcat(a_p, a_basepath, "/", realrelpath, NULL);

  /* now open directory */
  if (apr_dir_open(&pdir, realfullpath, a_p) != APR_SUCCESS)
    return -1;

  /* add this to the symtab */
  retval = kb_add_symtab(a_kb,
                         !strcmp(realrelpath, "") ? "/" : realrelpath,
                         VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP);

  if (retval != VLAD_OK) {
    ap_log_perror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_p,
                  "mod_vlad: could not add object \"%s\" to kb: error %d",
                  realrelpath,
                  retval);
    return -1;
  }

  while (apr_dir_read(&dinfo, APR_FINFO_NAME | APR_FINFO_TYPE, pdir) == APR_SUCCESS) {
    const char *tmppath;

    /* ignore . and .. */
    if (!strcmp(".", dinfo.name) || !strcmp("..", dinfo.name))
      continue;

    /* append the current dir with the relative path */
    tmppath = apr_pstrcat(a_p, realrelpath, "/", dinfo.name, NULL);

    if (dinfo.filetype != APR_DIR) {
      /* if it is not a directory, add to kb */
      retval = kb_add_symtab(a_kb, tmppath, VLAD_IDENT_OBJECT);

      if (retval != VLAD_OK) {
        ap_log_perror(APLOG_MARK,
                      APLOG_ERR,
                      0,
                      a_p,
                      "mod_vlad: could not add object \"%s\" to kb: error %d",
                      tmppath,
                      retval);
        return -1;
      }
    }
    else {
      /* now recurse */
      if (add_object_recurse(a_p, a_kb, a_basepath, tmppath) != 0)
        return -1;
    }
  }

  apr_dir_close(pdir);

  return 0;
}

/* converts / to docroot */
static const char *get_docroot(apr_pool_t *a_p,
                               const char *a_path,
                               server_rec *a_s)
{
  core_server_config *conf;

  conf = (core_server_config *) ap_get_module_config(a_s->module_config,
                                                     &core_module);

  if (!a_path || !conf)
    return NULL;

  return strcmp("/", a_path) ?
         apr_pstrdup(a_p, a_path) :
         apr_pstrdup(a_p, conf->ap_document_root);
}
