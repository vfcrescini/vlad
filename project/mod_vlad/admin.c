/*
 * admin.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include "admin.h"

#include "vlad/wrapper.h"

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
  unsigned int st_len;
  char *st_name;
  void *st_list;
  unsigned int tt_len;
  char *tt_name;
  void *tt_list;
  void *tt_prexp;
  void *tt_poexp;
  unsigned int arg_len;
  unsigned int it_len;
  char *it_name;

  vlad_kb_length_seqtab(a_conf->kb, &st_len);
  vlad_kb_length_transtab(a_conf->kb, &tt_len);

  ap_rprintf(a_r, "    <form name=\"delform\" method=\"POST\" action=\"\">\n");
  ap_rprintf(a_r, "      <h3>Current Sequence</h3>\n");
  ap_rprintf(a_r, "      <input type=\"hidden\" name=\"delete\" value=\"\">\n");
  ap_rprintf(a_r, "      <ul>\n");

  for (i = 0; i < st_len; i++) {
    vlad_kb_get_seqtab(a_conf->kb, i, &st_name, &st_list);
    ap_rprintf(a_r,
               "         <li>%s<input type=\"button\" value=\"delete\" onclick=\"aform.delete.value='%d';aform.submit();\"></li>\n",
               st_name,
               i);
  }  

  ap_rprintf(a_r, "      </ul>\n");
  ap_rprintf(a_r, "    </form>\n");

  ap_rprintf(a_r, "    <h3>Available Identifiers</h3>\n");
  ap_rprintf(a_r, "    <form name=\"idform\">\n");
  ap_rprintf(a_r, "      <input type=\"hidden\" name=\"ident\" value=\"\">\n");
  ap_rprintf(a_r, "      <ul>\n");

  vlad_kb_length_symtab(a_conf->kb, &it_len);

  for (i = 0; i < it_len; i++) {
    vlad_kb_get_symtab(a_conf->kb, i, &it_name);
    ap_rprintf(a_r, "        <li>\n");
    ap_rprintf(a_r, "          <input type=\"radio\" name=\"tmp\" onclick=\"idform.ident.value='%s';\">%s\n",
                               it_name,
                               it_name);
    ap_rprintf(a_r, "        </li>\n");
  }

  ap_rprintf(a_r, "      </ul>\n");
  ap_rprintf(a_r, "    </form>\n");

  ap_rprintf(a_r, "    <h3>Available Transformations</h3>\n");
  ap_rprintf(a_r, "    <ul>\n");

  for (i = 0; i < tt_len; i++) {
    vlad_kb_get_transtab(a_conf->kb, i, &tt_name, &tt_list, &tt_prexp, &tt_poexp);
    arg_len = vlad_list_length(tt_list);

    ap_rprintf(a_r, "      <li>\n");
    ap_rprintf(a_r, "        <form name=\"addform%d\" method=\"POST\" action=\"\">\n", i);
    ap_rprintf(a_r, "          <input type=\"hidden\" name=\"argnum\" value=\"%d\">\n", arg_len);
    ap_rprintf(a_r, "          %s\n", tt_name);
    ap_rprintf(a_r, "          <input type=\"button\" name=\"add\" value=\"add\" onclick=\"addform%d.submit();\">\n", i);
    ap_rprintf(a_r, "          <br>\n");

    for (j = 0; j < arg_len; j++) {
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
}
