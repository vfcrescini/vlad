/*
 * admin.h
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 */

#ifndef __MODVLAD_ADMIN_H
#define __MODVLAD_ADMIN_H

#include "httpd.h"

#include "mod_vlad.h"

void modvlad_generate_header(request_rec *a_r);
void modvlad_generate_footer(request_rec *a_r);
void modvlad_generate_form(request_rec *a_r, modvlad_ipc a_ipc);
void modvlad_handle_form(request_rec *a_r, modvlad_ipc a_ipc);

#endif
