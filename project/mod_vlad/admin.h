/*
 * admin.h
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __MODVLAD_ADMIN_H
#define __MODVLAD_ADMIN_H

#include "util.h"

void modvlad_generate_header(request_rec *a_r);
void modvlad_generate_footer(request_rec *a_r);
void modvlad_generate_form(request_rec *a_r, modvlad_config_rec *a_conf);
void modvlad_handle_form(request_rec *a_r, modvlad_config_rec *a_conf);

#endif
