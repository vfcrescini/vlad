/*
 * admin.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include "admin.h"

#include "vlad/wrapper.h"

#include "http_log.h"
#include "http_core.h"
#include "http_protocol.h"
#include "apr_strings.h"

void modvlad_generate_header(request_rec *a_r)
{
  ap_rprintf(a_r, "%s\n", MODVLAD_XML_HEADER);
  ap_rprintf(a_r, "%s\n", MODVLAD_DOCTYPE_HEADER);
  ap_rprintf(a_r, "%s\n", MODVLAD_MISC_HEADER);
  ap_rprintf(a_r, "<html>\n  <body>\n");
  ap_rprintf(a_r, "    <h1>mod_vlad Administration</h1>\n");
  ap_rprintf(a_r, "    <h2>Transformation Sequence Manipulator</h2>\n");
}

void modvlad_generate_footer(request_rec *a_r)
{
  ap_rprintf(a_r, "  </body>\n</html>\n");
}

void modvlad_generate_form(request_rec *a_r, modvlad_config_rec *a_conf)
{
  unsigned int i;
  unsigned int j;
  const char *uri = NULL;

  uri = ap_construct_url(a_r->pool, a_r->uri, a_r);

  ap_rprintf(a_r, "    <h3>Current Sequence</h3>\n");
  ap_rprintf(a_r, "    <form name=\"delform\" method=\"POST\" action=\"%s\">\n", uri);
  ap_rprintf(a_r, "      <input type=\"hidden\" name=\"command\" value=\"delete\"/>\n");
  ap_rprintf(a_r, "      <input type=\"hidden\" name=\"index\" value=\"\"/>\n");
  ap_rprintf(a_r, "      <table cols=\"3\" border=\"1\">\n");

  for (i = 0; i < vlad_kb_length_seqtab(a_conf->kb); i++) {
    char *st_name = NULL;
    void *st_list = NULL;

    vlad_kb_get_seqtab(a_conf->kb, i, &st_name, &st_list);

    ap_rprintf(a_r, "         <tr>\n");
    ap_rprintf(a_r, "           <td align=\"center\">%d</th>\n", i);
    ap_rprintf(a_r, "           <th align=\"center\">%s</th>\n", st_name);
    ap_rprintf(a_r, "           <td align=\"center\"><input type=\"button\" value=\"delete\" onclick=\"delform.index.value=%d;delform.submit();\"/></td>\n", i);
    ap_rprintf(a_r, "         </tr>\n");
  }  

  ap_rprintf(a_r, "      </table>\n");
  ap_rprintf(a_r, "    </form>\n");

  ap_rprintf(a_r, "    <h3>Available Transformations</h3>\n");

  for (i = 0; i <  vlad_kb_length_transtab(a_conf->kb); i++) {
    char *tt_name = NULL;
    void *tt_list = NULL;
    void *tt_prexp = NULL;
    void *tt_poexp = NULL;

    vlad_kb_get_transtab(a_conf->kb, i, &tt_name, &tt_list, &tt_prexp, &tt_poexp);

    ap_rprintf(a_r, "    <form name=\"addform%d\" method=\"POST\" action=\"%s\">\n", i, uri);
    ap_rprintf(a_r, "      <input type=\"hidden\" name=\"command\" value=\"add\"/>\n");
    ap_rprintf(a_r, "      <input type=\"hidden\" name=\"args\" value=\"%d\"/>\n",  vlad_list_length(tt_list));
    ap_rprintf(a_r, "      <input type=\"hidden\" name=\"trans\" value=\"%s\"/>\n", tt_name);
    ap_rprintf(a_r, "      <table cols=\"3\" border=\"1\">\n");
    ap_rprintf(a_r, "        <tr>\n");
    ap_rprintf(a_r, "          <th align=\"center\" colspan=\"2\">%s</th>\n", tt_name);
    ap_rprintf(a_r, "          <th align=\"center\"><input type=\"button\" value=\"add\" onclick=\"addform%d.submit();\"/></th>\n", i);
    ap_rprintf(a_r, "        <tr>\n");

    for (j = 0; j < vlad_list_length(tt_list); j++) {
      ap_rprintf(a_r, "     <tr>\n");
      ap_rprintf(a_r, "       <td align=\"center\">parameter %d</td>\n", j);
      ap_rprintf(a_r, "       <td align=\"center\"><input type=\"text\" name=\"arg%d\" value=\"\" readonly=\"1\"/></td>\n", j);
      ap_rprintf(a_r, "       <td align=\"center\"><input type=\"button\" value=\"set\" onclick=\"addform%d.arg%d.value=document.idform.ident.value;\"/></td>\n", i, j);
      ap_rprintf(a_r, "     </tr>\n");
    }

    ap_rprintf(a_r, "      </table>\n");
    ap_rprintf(a_r, "    </form>\n");
  }

  ap_rprintf(a_r, "    <h3>Available Identifiers</h3>\n");
  ap_rprintf(a_r, "    <form name=\"idform\">\n");
  ap_rprintf(a_r, "      <input type=\"hidden\" name=\"ident\" value=\"\"/>\n");
  ap_rprintf(a_r, "      <ul>\n");

  for (i = 0; i < vlad_kb_length_symtab(a_conf->kb); i++) {
    char *it_name = NULL;

    vlad_kb_get_symtab(a_conf->kb, i, &it_name);

    ap_rprintf(a_r, "        <li>\n");
    ap_rprintf(a_r, "          <input name=\"id\" type=\"radio\" onclick=\"idform.ident.value='%s';\">\n", it_name);
    ap_rprintf(a_r, "            %s\n", it_name);
    ap_rprintf(a_r, "          </input>\n");
    ap_rprintf(a_r, "        </li>\n");
  }

  ap_rprintf(a_r, "      </ul>\n");
  ap_rprintf(a_r, "    </form>\n");
}

void modvlad_handle_form(request_rec *a_r, modvlad_config_rec *a_conf)
{
  char buffer[MODVLAD_MAXSTR_LEN];
  const char *cmd = NULL;
  apr_table_t *tab = NULL;

  memset(buffer, 0, MODVLAD_MAXSTR_LEN);

  ap_get_client_block(a_r, buffer, MODVLAD_MAXSTR_LEN);
  ap_unescape_url(buffer);

  modvlad_parse_args(a_r->pool, buffer, &tab);

  cmd = apr_table_get(tab, "command");

  if (!strcmp(cmd, "add")) {
    int retval;
    unsigned int i;
    unsigned int args;
    const char *name = NULL;
    void *tref = NULL;
    void *vlist = NULL;

    args = atoi(apr_table_get(tab, "args"));
    name = apr_table_get(tab, "trans");

    if ((retval = vlad_strlist_create(&vlist)) != VLAD_OK) {
      ap_log_perror(APLOG_MARK,
                    APLOG_ERR,
                    0,
                    a_r->pool,
                    "mod_vlad: failed to create stringlist: %d",
                    retval);
      ap_rprintf(a_r, "    <blink>add error</blink>\n    <br/>\n");
      return;
    }

    for (i = 0; i < args; i++) {
      const char *value = NULL;
      value = apr_table_get(tab, apr_psprintf(a_r->pool, "arg%d", i));

      if ((retval = vlad_strlist_add(vlist, value)) != VLAD_OK) {
        ap_log_perror(APLOG_MARK,
                      APLOG_ERR,
                      0,
                      a_r->pool,
                      "mod_vlad: failed to add string to strlist: %d",
                      retval);
        ap_rprintf(a_r, "    <blink>add error</blink>\n    <br/>\n");
        vlad_strlist_destroy(vlist);
        return;
      }
    }

    if ((retval = vlad_tref_create(&tref)) != VLAD_OK) {
      ap_log_perror(APLOG_MARK,
                    APLOG_ERR,
                    0,
                    a_r->pool,
                    "mod_vlad: failed to create transref: %d",
                    retval);
      ap_rprintf(a_r, "    <blink>add error</blink>\n    <br/>\n");
      vlad_strlist_destroy(vlist);
      return;
    }
    if ((retval = vlad_tref_init(tref, name, vlist)) != VLAD_OK) {
      ap_log_perror(APLOG_MARK,
                    APLOG_ERR,
                    0,
                    a_r->pool,
                    "mod_vlad: failed to init transref: %d",
                    retval);
      ap_rprintf(a_r, "    <blink>add error</blink>\n    <br/>\n");
      vlad_tref_destroy(tref);
      vlad_strlist_destroy(vlist);
      return;
    }

    if ((retval = vlad_kb_add_seqtab(a_conf->kb, tref)) != VLAD_OK) {
      ap_log_perror(APLOG_MARK,
                    APLOG_ERR,
                    0,
                    a_r->pool,
                    "mod_vlad: failed to add to seqtab: %d",
                    retval);
      ap_rprintf(a_r, "    <blink>add error</blink>\n    <br/>\n");
      vlad_tref_destroy(tref);
      return;
    }

#ifdef MODVLAD_DEBUG 
    retval = vlad_kb_compute_generate(a_conf->kb, stderr);
    fflush(stderr);
    if (retval != VLAD_OK) {
      ap_log_perror(APLOG_MARK,
                    APLOG_ERR,
                    0,
                    a_r->pool,
                    "mod_vlad: could not compute kb (%d)",
                    retval);
      return;
    }
#else
    if ((retval = vlad_kb_compute_evaluate(a_conf->kb)) != VLAD_OK) {
      ap_log_perror(APLOG_MARK,
                    APLOG_ERR,
                    0,
                    a_r->pool,
                    "mod_vlad: could not compute kb (%d)",
                    retval);
      return;
    }
#endif

    ap_rprintf(a_r, "    <blink>add successful</blink>\n    <br/>\n");
  }
  else if (!strcmp(cmd, "delete")) {
    int retval;
    unsigned int args;

    args = atoi(apr_table_get(tab, "index"));

    if ((retval = vlad_kb_del_seqtab(a_conf->kb, args)) != VLAD_OK) {
      ap_log_perror(APLOG_MARK,
                    APLOG_ERR,
                    0,
                    a_r->pool,
                    "mod_vlad: failed to delete from seqtab: %d",
                    retval);
      ap_rprintf(a_r, "    <blink>delete error</blink>\n    <br/>\n");
      return;
    }

#ifdef MODVLAD_DEBUG 
    retval = vlad_kb_compute_generate(a_conf->kb, stderr);
    fflush(stderr);
    if (retval != VLAD_OK) {
      ap_log_perror(APLOG_MARK,
                    APLOG_ERR,
                    0,
                    a_r->pool,
                    "mod_vlad: could not compute kb (%d)",
                    retval);
      return;
    }
#else
    if ((retval = vlad_kb_compute_evaluate(a_conf->kb)) != VLAD_OK) {
      ap_log_perror(APLOG_MARK,
                    APLOG_ERR,
                    0,
                    a_r->pool,
                    "mod_vlad: could not compute kb (%d)",
                    retval);
      return;
    }
#endif

    ap_rprintf(a_r, "    <blink>delete successful</blink>\n    <br/>\n");
  }
  else
    ap_rprintf(a_r, "    <blink>error</blink>\n    <br/>\n");
}
