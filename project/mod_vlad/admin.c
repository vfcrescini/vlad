/*
 * admin.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include "admin.h"

#include "vlad/wrapper.h"

#include "apr_strings.h"
#include "http_protocol.h"

void modvlad_generate_header(request_rec *a_r)
{
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

  ap_rprintf(a_r, "    <h3>Current Sequence</h3>\n");
  ap_rprintf(a_r, "    <form name=\"delform\" method=\"POST\" action=\"\">\n");
  ap_rprintf(a_r, "    <form name=\"delform\" method=\"POST\" action=\"\">\n");
  ap_rprintf(a_r, "      <input type=\"hidden\" name=\"command\" value=\"delete\">\n");
  ap_rprintf(a_r, "      <input type=\"hidden\" name=\"sequence\" value=\"\">\n");
  ap_rprintf(a_r, "      <ul>\n");

  for (i = 0; i < vlad_kb_length_seqtab(a_conf->kb); i++) {
    char *st_name = NULL;
    void *st_list = NULL;

    vlad_kb_get_seqtab(a_conf->kb, i, &st_name, &st_list);

    ap_rprintf(a_r, "         <li>\n");
    ap_rprintf(a_r, "           <input type=\"button\" value=\"delete\" onclick=\"delform.sequence.value=%d;delform.submit();\">\n", i);
    ap_rprintf(a_r, "           %s\n", st_name);
    ap_rprintf(a_r, "         </li>\n");
  }  

  ap_rprintf(a_r, "      </ul>\n");
  ap_rprintf(a_r, "    </form>\n");

  ap_rprintf(a_r, "    <h3>Available Identifiers</h3>\n");
  ap_rprintf(a_r, "    <form name=\"idform\">\n");
  ap_rprintf(a_r, "      <input type=\"hidden\" name=\"ident\" value=\"\">\n");
  ap_rprintf(a_r, "      <ul>\n");


  for (i = 0; i < vlad_kb_length_symtab(a_conf->kb); i++) {
    char *it_name = NULL;

    vlad_kb_get_symtab(a_conf->kb, i, &it_name);

    ap_rprintf(a_r, "        <li>\n");
    ap_rprintf(a_r, "          <input type=\"radio\" onclick=\"idform.ident.value='%s';\">\n", it_name);
    ap_rprintf(a_r, "          %s\n", it_name);
    ap_rprintf(a_r, "        </li>\n");
  }

  ap_rprintf(a_r, "      </ul>\n");
  ap_rprintf(a_r, "    </form>\n");

  ap_rprintf(a_r, "    <h3>Available Transformations</h3>\n");
  ap_rprintf(a_r, "    <ul>\n");

  for (i = 0; i <  vlad_kb_length_transtab(a_conf->kb); i++) {
    char *tt_name = NULL;
    void *tt_list = NULL;
    void *tt_prexp = NULL;
    void *tt_poexp = NULL;

    vlad_kb_get_transtab(a_conf->kb, i, &tt_name, &tt_list, &tt_prexp, &tt_poexp);

    ap_rprintf(a_r, "      <li>\n");
    ap_rprintf(a_r, "        <form name=\"addform%d\" method=\"POST\" action=\"\">\n", i);
    ap_rprintf(a_r, "          <input type=\"hidden\" name=\"command\" value=\"add\">\n");
    ap_rprintf(a_r, "          <input type=\"hidden\" name=\"args\" value=\"%d\">\n",  vlad_list_length(tt_list));
    ap_rprintf(a_r, "          <input type=\"hidden\" name=\"trans\" value=\"%s\">\n", tt_name);
    ap_rprintf(a_r, "          %s\n", tt_name);
    ap_rprintf(a_r, "          <input type=\"button\" value=\"add\" onclick=\"addform%d.submit();\">\n", i);
    ap_rprintf(a_r, "          <br>\n");

    for (j = 0; j < vlad_list_length(tt_list); j++) {
      ap_rprintf(a_r, "          arg%d\n", j);
      ap_rprintf(a_r, "          <input type=\"text\" name=\"arg%d\" value=\"\" readonly>\n", j);
      ap_rprintf(a_r, "          <input type=\"button\" value=\"set\" onclick=\"addform%d.arg%d.value=document.idform.ident.value;\">\n", i, j);
    }

    ap_rprintf(a_r, "        </form>\n");
    ap_rprintf(a_r, "      </li>\n");
  }

  ap_rprintf(a_r, "    </ul>\n");
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

  ap_rprintf(a_r, "    <pre>%s</pre>\n    <br>\n", buffer);

  if (!strcmp(cmd, "add")) {
    unsigned int i;
    int args;
    const char *name = NULL;
    void *tref = NULL;
    void *vlist = NULL;

    ap_rprintf(a_r, "    <blink>add</blink>\n    <br>\n");

    args = atoi(apr_table_get(tab, "args"));
    name = apr_table_get(tab, "trans");

    vlad_strlist_create(&vlist);

    for (i = 0; i < args; i++) {
      const char *value = NULL;
      value = apr_table_get(tab, apr_psprintf(a_r->pool, "arg%d", i));
      vlad_strlist_add(vlist, value);
    }

    vlad_tref_create(&tref);
    vlad_tref_init(tref, name, vlist);
    vlad_kb_add_seqtab(a_conf->kb, tref);
  }
  else if (!strcmp(cmd, "delete")) {
    ap_rprintf(a_r, "    <blink>delete</blink>\n    <br>\n");
  }
  else
    ap_rprintf(a_r, "    <blink>error</blink>\n    <br>\n");
}
