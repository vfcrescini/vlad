/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004 University of Western Sydney
 * by Vino Fernando Crescini <jcrescin@cit.uws.edu.au>
 *
 * PolicyUpdater is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * PolicyUpdater is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PolicyUpdater; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "httpd.h"
#include "http_log.h"
#include "http_core.h"
#include "http_protocol.h"
#include "apr_strings.h"

#include <vlad/wrapper.h>

#include "mod_vlad.h"
#include "util.h"
#include "admin.h"
#include "proc.h"

static void generate_ident_cell(request_rec *a_r,
                                modvlad_ipc a_ipc,
                                unsigned char a_type);

static void generate_ident_cell(request_rec *a_r,
                                modvlad_ipc a_ipc,
                                unsigned char a_type)
{
  apr_pool_t *pool = NULL;
  apr_array_header_t *array = NULL;
  unsigned int i;

  apr_pool_create(&pool, a_r->pool);
  array = apr_array_make(pool, 1, sizeof(char *));

  modvlad_client_ident_get(pool,
                           a_ipc,
                           a_type,
                           array);

  for (i = 0; i < array->nelts; i++) {
    const char *name = ((char **)array->elts)[i];

    ap_rprintf(a_r, "            <input name=\"id\" type=\"radio\" onclick=\"idform.ident.value='%s';\">\n", name);
    ap_rprintf(a_r, "              %s\n", name);
    ap_rprintf(a_r, "            </input>\n");
    ap_rprintf(a_r, "            <br/>\n");
  }

  apr_pool_destroy(pool);
}

void modvlad_generate_header(request_rec *a_r)
{
  if (!a_r)
    return;

  ap_rprintf(a_r, "%s\n", MODVLAD_XML_HEADER);
  ap_rprintf(a_r, "%s\n", MODVLAD_DOCTYPE_HEADER);
  ap_rprintf(a_r, "%s\n", MODVLAD_MISC_HEADER);
  ap_rprintf(a_r, "<html>\n  <body>\n");
  ap_rprintf(a_r, "    <h1>mod_vlad %s Administration</h1>\n", MODVLAD_VERSION);
  ap_rprintf(a_r, "    <h2>Transformation Sequence Manipulator</h2>\n");
}

void modvlad_generate_footer(request_rec *a_r)
{
  if (!a_r)
    return;

  ap_rprintf(a_r, "  </body>\n</html>\n");
}

void modvlad_generate_form(request_rec *a_r, modvlad_ipc a_ipc)
{
  unsigned int i;
  unsigned int slen = 0;
  unsigned int tlen = 0;
  const char *uri = NULL;

  if (!a_r || MODVLAD_IPC_CCHECK(a_ipc))
    return;

  if (!(uri = ap_construct_url(a_r->pool, a_r->uri, a_r)))
    return;

  /* generate sequece list */
  ap_rprintf(a_r, "    <h3>Current Sequence</h3>\n");
  ap_rprintf(a_r, "    <form name=\"delform\" method=\"POST\" action=\"%s\">\n", uri);
  ap_rprintf(a_r, "      <input type=\"hidden\" name=\"command\" value=\"delete\"/>\n");
  ap_rprintf(a_r, "      <input type=\"hidden\" name=\"index\" value=\"\"/>\n");
  ap_rprintf(a_r, "      <table cols=\"3\" border=\"1\">\n");

  modvlad_client_seq_total(a_r->pool,
                           a_ipc,
                           &slen);

  for (i = 0; i < slen; i++) {
    const char *st_name = NULL;
    apr_array_header_t *st_list = NULL;
    unsigned int j;

    st_list = apr_array_make(a_r->pool, 1, sizeof(char *));

    modvlad_client_seq_get(a_r->pool,
                           a_ipc,
                           i,
                           &st_name,
                           st_list);

    ap_rprintf(a_r, "         <tr>\n");
    ap_rprintf(a_r, "           <td align=\"center\">%d</th>\n", i);
    ap_rprintf(a_r, "           <th align=\"center\">\n");
    ap_rprintf(a_r, "             %s(", st_name);

    /* parameters for this sequence */
    for (j = 0; j < st_list->nelts; j++)
      ap_rprintf(a_r, j == 0 ? "%s" : ", %s", ((char **)st_list->elts)[j]);

    ap_rprintf(a_r, ")\n");
    ap_rprintf(a_r, "           </th>\n");
    ap_rprintf(a_r, "           <td align=\"center\"><input type=\"button\" value=\"delete\" onclick=\"delform.index.value=%d;delform.submit();\"/></td>\n", i);
    ap_rprintf(a_r, "         </tr>\n");
  }  

  ap_rprintf(a_r, "      </table>\n");
  ap_rprintf(a_r, "    </form>\n");

  /* available transformations list */
  ap_rprintf(a_r, "    <h3>Available Transformations</h3>\n");

  modvlad_client_trans_total(a_r->pool,
                             a_ipc,
                             &tlen);

  for (i = 0; i <  tlen; i++) {
    const char *tt_name = NULL;
    apr_array_header_t *tt_list = NULL;
    unsigned int j;

    tt_list = apr_array_make(a_r->pool, 1, sizeof(char *));

    modvlad_client_trans_get(a_r->pool,
                             a_ipc,
                             i,
                             &tt_name,
                             tt_list);

    ap_rprintf(a_r, "    <form name=\"addform%d\" method=\"POST\" action=\"%s\">\n", i, uri);
    ap_rprintf(a_r, "      <input type=\"hidden\" name=\"command\" value=\"add\"/>\n");
    ap_rprintf(a_r, "      <input type=\"hidden\" name=\"args\" value=\"%d\"/>\n",  tt_list->nelts);
    ap_rprintf(a_r, "      <input type=\"hidden\" name=\"trans\" value=\"%s\"/>\n", tt_name);
    ap_rprintf(a_r, "      <table cols=\"3\" border=\"1\">\n");
    ap_rprintf(a_r, "        <tr>\n");
    ap_rprintf(a_r, "          <th align=\"center\" colspan=\"2\">%s</th>\n", tt_name);
    ap_rprintf(a_r, "          <th align=\"center\"><input type=\"button\" value=\"add\" onclick=\"addform%d.submit();\"/></th>\n", i);
    ap_rprintf(a_r, "        </tr>\n");

    /* the parameters of this transformation */
    for (j = 0; j < tt_list->nelts; j++) {
      ap_rprintf(a_r, "        <tr>\n");
      ap_rprintf(a_r, "          <td align=\"center\">parameter %d (%s)</td>\n", j, ((char **)tt_list->elts)[j]);
      ap_rprintf(a_r, "          <td align=\"center\"><input type=\"text\" name=\"arg%d\" value=\"\" readonly=\"1\"/></td>\n", j);
      ap_rprintf(a_r, "          <td align=\"center\"><input type=\"button\" value=\"set\" onclick=\"addform%d.arg%d.value=document.idform.ident.value;\"/></td>\n", i, j);
      ap_rprintf(a_r, "        </tr>\n");
    }

    ap_rprintf(a_r, "      </table>\n");
    ap_rprintf(a_r, "    </form>\n");
  }

  /* identifier list */
  ap_rprintf(a_r, "    <h3>Available Identifiers</h3>\n");
  ap_rprintf(a_r, "    <form name=\"idform\">\n");
  ap_rprintf(a_r, "      <input type=\"hidden\" name=\"ident\" value=\"\"/>\n");
  ap_rprintf(a_r, "      <table cols=\"3\" border=\"1\">\n");
  ap_rprintf(a_r, "        <tr>\n");
  ap_rprintf(a_r, "          <th>Subject</th>\n");
  ap_rprintf(a_r, "          <th>Access</th>\n");
  ap_rprintf(a_r, "          <th>Object</th>\n");
  ap_rprintf(a_r, "        </tr>\n");
  ap_rprintf(a_r, "        <tr valign=\"top\">\n");

  ap_rprintf(a_r, "          <td>\n");
  generate_ident_cell(a_r, a_ipc, VLAD_IDENT_SUBJECT);
  ap_rprintf(a_r, "          </td>\n");
  ap_rprintf(a_r, "          <td>\n");
  generate_ident_cell(a_r, a_ipc, VLAD_IDENT_ACCESS);
  ap_rprintf(a_r, "          </td>\n");
  ap_rprintf(a_r, "          <td>\n");
  generate_ident_cell(a_r, a_ipc, VLAD_IDENT_OBJECT);
  ap_rprintf(a_r, "          </td>\n");

  ap_rprintf(a_r, "        </tr>\n");
  ap_rprintf(a_r, "      </table>\n");
  ap_rprintf(a_r, "    </form>\n");
}

void modvlad_handle_form(request_rec *a_r, modvlad_ipc a_ipc)
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
    unsigned int i;
    unsigned int args;
    const char *name = NULL;
    apr_array_header_t *parms = NULL;

    parms = apr_array_make(a_r->pool, 1, sizeof(char *));

    args = atoi(apr_table_get(tab, "args"));
    name = apr_table_get(tab, "trans");

    for (i = 0; i < args; i++) {
      const char *value = NULL;

      value = apr_table_get(tab, apr_psprintf(a_r->pool, "arg%d", i));
      *(char **) apr_array_push(parms) = apr_pstrdup(a_r->pool, apr_table_get(tab, apr_psprintf(a_r->pool, "arg%d", i)));
    }

    if (modvlad_client_seq_add(a_r->pool,
                               a_ipc,
                               name,
                               parms) != MODVLAD_OK)
      ap_rprintf(a_r, "    <blink>add error</blink>\n    <br/>\n");
    else
      ap_rprintf(a_r, "    <blink>add successful</blink>\n    <br/>\n");
  }
  else if (!strcmp(cmd, "delete")) {
    unsigned int args;

    args = atoi(apr_table_get(tab, "index"));

    if (modvlad_client_seq_del(a_r->pool,
                               a_ipc,
                               args) != MODVLAD_OK)
      ap_rprintf(a_r, "    <blink>delete error</blink>\n    <br/>\n");
    else
      ap_rprintf(a_r, "    <blink>delete successful</blink>\n    <br/>\n");
  }
  else
    ap_rprintf(a_r, "    <blink>form error</blink>\n    <br/>\n");

  return;
}
