/*
 * util.c
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
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
#include "mod_vlad.h"

/* some external functions from the parser & lexer */
extern void policyparse();
extern void policy_set_kb(void *a_kb);
extern void policy_set_ct(void *a_exp);
extern void policy_set_yyinput(int (*a_func)(void *, char *, int),
                               void *a_stream);

/* register the users into the kb */
static int add_subject(apr_pool_t *a_p, void *a_kb, const char *a_fname);
/* add built in access rights into the kb */
static int add_access(apr_pool_t *a_p, void *a_kb);
/* add the path's directory tree into the kb, a_relpath should be NULL or "" */
static int add_object(apr_pool_t *a_p,
                      void *a_kb,
                      void *a_exp,
                      const char *a_basepath,
                      const char *a_relpath);
/* returns the parent of the given filepath */
static const char *get_parent(apr_pool_t *a_p, const char *a_path);
/* strips out everything after ? */
static const char *strip_question(apr_pool_t *a_p, const char *a_str);
/* strips out the trailing / from a_str */
static const char *strip_slash(apr_pool_t *a_p, const char *a_str);
/* a version of yyinput that uses apache apr */
static int apache_yyinput(void *a_stream, char *a_buf, int a_max);

/* initialze kb */
int modvlad_init_kb(apr_pool_t *a_p,
                    const char *a_userfile,
                    const char *a_docroot,
                    void **a_kb,
                    void **a_exp)
{
  if (!a_p || !a_exp || !a_kb || !a_userfile || !a_docroot)
    return -1;

  /* create and init kb */
  if (vlad_kb_create(a_kb) != VLAD_OK)
    return -2;

  if (vlad_kb_init(*a_kb) != VLAD_OK)
    return -3;

  /* create an expression for those extra constraints */
  if (vlad_exp_create(a_exp) != VLAD_OK)
    return -4;

  /* register the kb to be destroyed with this pool */
  apr_pool_cleanup_register(a_p,
                            *a_kb,
			    vlad_kb_destroy,
			    vlad_kb_destroy);

  if (add_subject(a_p, *a_kb, a_userfile))
    return -5;
  if (add_access(a_p, *a_kb))
    return -6;
  if (add_object(a_p, *a_kb, *a_exp, a_docroot, NULL))
    return -7;

  return 0;
}

/* read the policy file into kb */
int modvlad_load_kb(apr_pool_t *a_p,
                    apr_file_t *a_polfile,
                    void *a_kb,
                    void *a_exp)
{
  if (!a_p || !a_polfile || !a_kb || !a_exp)
    return -1;

  /* give the lexer the proper yyinput function */
  policy_set_yyinput(apache_yyinput, (void *)a_polfile);
  /* give the parser a kb handle */
  policy_set_kb(a_kb);
  /* give the parser a handle to the extra constraints expression */
  policy_set_ct(a_exp);

  /* now, we parse */
  policyparse();

  return 0;
}

/* composes an expression to query */
void *modvlad_create_query(apr_pool_t *a_p,
                           const char *a_subject,
                           const char *a_access,
                           const char *a_object)
{
  int retval;
  void *atom = NULL;
  void *exp = NULL;

  if (!a_p || !a_subject  || !a_access || !a_object)
    return NULL;

  if ((retval = vlad_atom_create(&atom)) != VLAD_OK) {
    ap_log_perror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_p,
                  "mod_vlad: could not create atom: %d",
                  retval);
    return NULL;
  }

  if ((retval = vlad_exp_create(&exp)) != VLAD_OK) {
    ap_log_perror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_p,
                  "mod_vlad: could not create expression: %d",
                  retval);
    return NULL;
  }

  retval = vlad_atom_init_holds(atom, a_subject, a_access, a_object, 1);
  if (retval != VLAD_OK) {
    ap_log_perror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_p,
                  "mod_vlad: could not initialize atom: %d",
                  retval);
    return NULL;
  }

  if ((retval = vlad_exp_add(exp, atom)) != VLAD_OK) {
    ap_log_perror(APLOG_MARK,
                  APLOG_ERR,
                  0,
                  a_p,
                  "mod_vlad: could not add atom into expression: %d",
                  retval);
    return NULL;
  }

  return exp;
}

const char *modvlad_strip_url(apr_pool_t *a_p, const char *a_url)
{
  return strip_slash(a_p, strip_question(a_p, a_url));
}

/* parse args */
int modvlad_parse_args(apr_pool_t *a_p,
                       const char *a_str,
                       apr_table_t **a_tab)
{
  char buf[MODVLAD_MAXSTR_LEN];
  char *ptr = buf;
  char *name = NULL;
  char *value = NULL;
  int found = 0;
  int novalue = 0;

  if (!a_str || !a_tab)
   return -1;

  memset(buf, 0, MODVLAD_MAXSTR_LEN);
  strcpy(ptr, a_str);

  *a_tab = apr_table_make(a_p, 0);

  while (*ptr != '\0') {
    name = ptr;
    found = 0;
    novalue = 0;
    while (*ptr != '\0') {
      if (*ptr == '=') {
        *ptr = '\0';
        ptr++;
        found = 1;
        break;
      }
      else if (*ptr == '\0' || *ptr == '&' || *ptr == EOF) {
        *ptr = '\0';
        ptr++;
        found = 1;
        novalue = 1;

        break;
      }
      ptr++;
    }

    if (!found)
      break;

    if (!novalue) {
      value = ptr;
      found = 0;

      while(*ptr != '\0') {
        if (*ptr == '&') {
          *ptr = '\0';
          ptr++;
          found = 1;
          break;
        }

        ptr++;
      }

      if (!found)
        *ptr = '\0';
    }
    else
      value = NULL;

    apr_table_set(*a_tab, name, value);
  }

  return 0;
}

/* gets the document root without request_rec */
const char *modvlad_docroot(apr_pool_t *a_p, server_rec *a_s)
{
  core_server_config *conf = NULL;

  conf = (core_server_config *) ap_get_module_config(a_s->module_config,
                                                     &core_module);

  if (!conf)
    return NULL;

  return apr_pstrdup(a_p, conf->ap_document_root);
}

/* register the users into the kb */
static int add_subject(apr_pool_t *a_p, void *a_kb, const char *a_fname)
{
  int retval;
  char line[MODVLAD_MAXSTR_LEN];
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

  memset(line, 0, MODVLAD_MAXSTR_LEN);

  /* read one line at a time */
  while (!(ap_cfg_getline(line, MODVLAD_MAXSTR_LEN, cfgfile))) {

    if (line[0] == '#' || !line[0])
      continue;

    lineptr = line;
    user = ap_getword(a_p, &lineptr, ':');

    /* now that we have the user, we can then add it to the kb */
    ap_log_perror(APLOG_MARK,
                  APLOG_INFO,
                  0,
                  a_p,
                  "mod_vlad: adding subject %s into kb",
                  user);

    /* do not add administrator */
    if (!strcmp(user, MODVLAD_ADMIN_USERNAME))
      continue;

    retval = vlad_kb_add_symtab(a_kb, user, VLAD_IDENT_SUBJECT);

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
  const char *access = NULL;
  int retval;

  while((access = *(array_ptr++)) != NULL) {

    ap_log_perror(APLOG_MARK,
                  APLOG_INFO,
                  0,
                  a_p,
                  "mod_vlad: adding access-right %s into kb",
                  access);

    retval = vlad_kb_add_symtab(a_kb, access, VLAD_IDENT_ACCESS);

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

/* add the path's directory tree into the kb, a_relpath should be NULL or "" */
static int add_object(apr_pool_t *a_p,
                      void *a_kb,
                      void *a_exp,
                      const char *a_basepath,
                      const char *a_relpath)
{
  int retval;
  apr_finfo_t dinfo;
  apr_dir_t *pdir = NULL;
  const char *realrelpath = NULL;
  const char *realfullpath = NULL;

  /* first chech stuff */
  if (!a_kb || !a_exp || !a_basepath || !a_p)
    return -1;

  /* realrelpath is / or path relative to basepath */
  realrelpath = apr_pstrdup(a_p,
                            (!a_relpath || !strcmp(a_relpath, "")) ? "/" : a_relpath);
  /* realfullpath is basepath/realrelpath */
  realfullpath = apr_pstrcat(a_p,
                             a_basepath,
                             (MODVLAD_LASTCHAR(a_basepath) == '/' || MODVLAD_FIRSTCHAR(realrelpath)) ? "" : "/",
                             !strcmp(realrelpath, "/") ? "" : realrelpath,
                             NULL);

  /* we have to ignore the admin trigger */
  if (!strcmp(realrelpath, MODVLAD_ADMIN_DIRNAME))
    return 0;

  ap_log_perror(APLOG_MARK,
                APLOG_INFO,
                0,
                a_p,
                "mod_vlad: adding object from %s as %s into kb",
                realfullpath,
                realrelpath);

  /* now open directory */
  if (apr_dir_open(&pdir, realfullpath, a_p) != APR_SUCCESS)
    return -1;

  /* add this to the symtab */
  retval = vlad_kb_add_symtab(a_kb,
                              realrelpath,
                              VLAD_IDENT_OBJECT | VLAD_IDENT_GROUP);

  /* if this is not the root dir, then make it a subset of its parent dir */
  if (strcmp(realrelpath, "/")) {
    const char *parent = get_parent(a_p, realrelpath);
    void *tmp_atom = NULL;

    if (vlad_atom_create(&tmp_atom) != VLAD_OK)
      return -1;

    if (vlad_atom_init_subset(tmp_atom, realrelpath, parent, 1) != VLAD_OK)
      return -1;

    if (vlad_exp_add(a_exp, tmp_atom) != VLAD_OK)
      return -1;
  }

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
    const char *tmppath = NULL;

    /* ignore ".", ".." and the admin trigger */
    if (!strcmp(".", dinfo.name) ||
        !strcmp("..", dinfo.name) ||
        !strcmp(MODVLAD_ADMIN_DIRNAME, dinfo.name))
      continue;

    /* append the current dir with the relative path */
    tmppath = apr_pstrcat(a_p,
                          realrelpath,
                          (MODVLAD_LASTCHAR(realrelpath) == '/' ? "" : "/"),
                          dinfo.name,
                          NULL);

    if (dinfo.filetype != APR_DIR) {
      void *tmp_atom = NULL;

      /* if it is not a directory, add to kb */
      retval = vlad_kb_add_symtab(a_kb, tmppath, VLAD_IDENT_OBJECT);

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

      /* now add an atom into the extra constraint expression */
      if (vlad_atom_create(&tmp_atom) != VLAD_OK)
        return -1;
      if (vlad_atom_init_member(tmp_atom, tmppath, realrelpath, 1) != VLAD_OK)
        return -1;
      if (vlad_exp_add(a_exp, tmp_atom) != VLAD_OK)
        return -1;
    }
    else {
      /* now recurse */
      if (add_object(a_p, a_kb, a_exp, a_basepath, tmppath) != 0)
        return -1;
    }
  }

  apr_dir_close(pdir);

  return 0;
}

/* returns the parent of the given filepath */
static const char *get_parent(apr_pool_t *a_p, const char *a_path)
{
  char tmpstring[MODVLAD_MAXSTR_LEN];
  int slash = 0;
  int i;

  if (a_path == NULL || a_p == NULL)
    return NULL;

  memset(tmpstring, 0, MODVLAD_MAXSTR_LEN);
  strcpy(tmpstring, a_path);

  for (i = strlen(a_path); i >= 0; i--) {
    if (slash == 0) {
      if (tmpstring[i] == '/')
        slash = 1;
      tmpstring[i] = '\0';
    }
    else {
      if (tmpstring[i] == '/')
        tmpstring[i] = '\0';
      else
        break;
    }
  }

  return apr_pstrdup(a_p, !slash ? a_path : (strcmp(tmpstring, "") ? tmpstring : "/"));
}

/* strips out the trailing / from a_str */
static const char *strip_slash(apr_pool_t *a_p, const char *a_str)
{
  char tmpstring[MODVLAD_MAXSTR_LEN];
  int i;

  if (a_str == NULL || a_p == NULL)
    return NULL;

  memset(tmpstring, 0, MODVLAD_MAXSTR_LEN);
  strcpy(tmpstring, a_str);

  for (i = strlen(a_str) - 1; i >= 0; i--) {
    if (tmpstring[i] == '/')
      tmpstring[i] = '\0';
    else
      break;
  }

  return apr_pstrdup(a_p, (strcmp(tmpstring, "") ? tmpstring : "/"));
}

/* strips out everything after ? */
static const char *strip_question(apr_pool_t *a_p, const char *a_str)
{
  char tmpstring[MODVLAD_MAXSTR_LEN];
  int i;

  if (a_str == NULL || a_p == NULL)
    return NULL;

  memset(tmpstring, 0, MODVLAD_MAXSTR_LEN);
  strcpy(tmpstring, a_str);

  for (i = 0; i < strlen(a_str); i++) {
    if (tmpstring[i] == '?') {
      tmpstring[i] = '\0';
      break;
    }
  }

  return apr_pstrdup(a_p, tmpstring);
}

/* a version of yyinput that uses apache apr */
static int apache_yyinput(void *a_stream, char *a_buf, int a_max)
{
  apr_size_t size = (apr_size_t) a_max;
  apr_file_t *file = (apr_file_t *) a_stream;

  apr_file_read(file, (void *) a_buf, &size);

  return size;
}
