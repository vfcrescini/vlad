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

#ifndef __MODVLAD_UTIL_H
#define __MODVLAD_UTIL_H

#include <stdarg.h>

#include "httpd.h"
#include "apr_pools.h"
#include "apr_file_io.h"

#include "mod_vlad.h"

/* initialze kb */
int modvlad_init_kb(apr_pool_t *a_p,
                    const char *a_userfile,
                    const char *a_docroot,
                    void **a_kb,
                    void **a_exp);

/* read the policy file into kb */
int modvlad_load_kb(apr_pool_t *a_p,
                    apr_file_t *a_polfile,
                    void *a_kb,
                    void *a_exp);

/* composes an expression to query */
void *modvlad_create_query(apr_pool_t *a_p,
                           const char *a_subject,
                           const char *a_access,
                           const char *a_object);

/* strips out trailing slashes and/or everything after a ? */
const char *modvlad_strip_url(apr_pool_t *a_p, const char *a_url);

/* parse args */
int modvlad_parse_args(apr_pool_t *a_p,
                       const char *a_str,
                       apr_table_t **a_tab);

/* gets the document root without request_rec */
const char *modvlad_docroot(apr_pool_t *a_p, server_rec *a_s);

/* generates a random id between 0 and MODVLAD_MAXID */
unsigned int modvlad_idgen();

#endif
