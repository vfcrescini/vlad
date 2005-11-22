/*
 *  This file is part of PolicyUpdater.
 * 
 *  Copyright (C) 2003, 2004, 2005 University of Western Sydney
 *  by Vino Fernando Crescini <jcrescin@cit.uws.edu.au>
 * 
 *  PolicyUpdater is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 * 
 *  PolicyUpdater is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with PolicyUpdater; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __VLAD_MEM_H
#define __VLAD_MEM_H

#ifdef __cplusplus
  #include <cstdlib>
  #include <cstring>
  #include <new>
#else
  #include <stdlib.h>
  #include <string.h>
#endif

#include <vlad/vlad.h>

/* malloc/new macros */
#define VLAD_MEM_ADT_MALLOC(X,Y)  ((X *) malloc(sizeof(X) * Y))
#define VLAD_MEM_STR_MALLOC(X) \
  ((char *) malloc(sizeof(char) * (strlen(X) + 1)))
#ifdef __cplusplus
  #define VLAD_MEM_NEW(X)         (new(std::nothrow) X)
#endif

#endif
