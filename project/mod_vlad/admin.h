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

#ifndef __MODVLAD_ADMIN_H
#define __MODVLAD_ADMIN_H

#include "httpd.h"

#include "mod_vlad.h"

void modvlad_generate_header(request_rec *a_r);
void modvlad_generate_footer(request_rec *a_r);
void modvlad_generate_form(request_rec *a_r, modvlad_ipc a_ipc);
void modvlad_handle_form(request_rec *a_r, modvlad_ipc a_ipc);

#endif
