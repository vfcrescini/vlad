/*
 * util.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include "httpd.h"
#include "http_config.h"
#include "http_log.h"
#include "ap_config.h"
#include "apr_file_io.h"

#include <vlad/vlad.h>
#include <vlad/wrapper.h>

#include "util.h"

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

/* register the users into the kb */
int modvlad_add_subject(void *a_kb, const char *a_fname, apr_pool_t *a_p)
{
  ap_configfile_t *cfgfile = NULL;
  apr_status_t status;
  int retval;
  char line[MAX_STRING_LEN];
  const char *user;
  const char *lineptr;

  if (a_fname == NULL)
    return -1;

  status = ap_pcfg_openfile(&cfgfile, a_p, a_fname);

  if (status != APR_SUCCESS) {
    ap_log_perror(APLOG_MARK,
                  APLOG_ERR,
                  status,
                  a_p,
                  "could not open authentication/user file: %s",
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
                    "vlad error: kb_add_symtab(%s) returned %d",
                    user,
                    retval);
      return -1;
    }
  }

  ap_cfg_closefile(cfgfile);

  return 0;
}

/* add built in access rights into the kb */
int modvlad_add_access(void *a_kb, apr_pool_t *a_p)
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
                    "vlad error: kb_add_symtab(%s) returned %d",
                    access,
                    retval);
      return -1;
    }
  } 

  return 0;
}

/* add the path's directory structure into the kb */
int modvlad_add_object(void *a_kb, const char *a_path, apr_pool_t *a_p)
{
#ifdef DEBUG
  ap_log_perror(APLOG_MARK,
                MODVLAD_LOGLEVEL,
                0,
                a_p,
                "adding objects from %s into kb",
                a_path);
#endif

  return 0;
}
