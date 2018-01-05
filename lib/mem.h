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

#ifdef VLAD_MEMTEST
  void *vlad_mem_malloc(size_t a_size);
  void *vlad_mem_realloc(void *a_ptr, size_t a_size);
  void vlad_mem_free(void *a_ptr);
#endif

#ifdef __cplusplus
class vlad_mem
{
  public :
  #ifdef VLAD_MEMTEST
    void *operator new(size_t a_size);
    void *operator new(size_t a_size, const std::nothrow_t &a_nt) throw();
    void *operator new[](size_t a_size);
    void *operator new[](size_t a_size, const std::nothrow_t &a_nt) throw();
    void operator delete(void *a_ptr);
    void operator delete[](void *a_ptr);
  #endif
} ;
#endif

/* generic malloc */
#ifdef VLAD_MEMTEST
  #define VLAD_MEM_ADT_MALLOC(X,Y) ((X *) vlad_mem_malloc(sizeof(X) * Y))
#else
  #define VLAD_MEM_ADT_MALLOC(X,Y) ((X *) malloc(sizeof(X) * Y))
#endif

/* special string malloc (allocate enough mem to accomodate string X) */
#ifdef VLAD_MEMTEST
  #define VLAD_MEM_STR_MALLOC(X) \
    ((char *) vlad_mem_malloc(sizeof(char) * (strlen(X) + 1)))
#else
  #define VLAD_MEM_STR_MALLOC(X) \
    ((char *) malloc(sizeof(char) * (strlen(X) + 1)))
#endif

/* reallocate mem X of type Y to size Z */
#ifdef VLAD_MEMTEST
  #define VLAD_MEM_REALLOC(X,Y,Z) ((Y *) vlad_mem_realloc(X, sizeof(Y) * (Z)))
#else
  #define VLAD_MEM_REALLOC(X,Y,Z) ((Y *) realloc(X, sizeof(Y) * (Z)))
#endif

/* generic free */
#ifdef VLAD_MEMTEST
  #define VLAD_MEM_FREE(X) (vlad_mem_free(X))
#else
  #define VLAD_MEM_FREE(X) (free(X))
#endif

/* for c++, we have new and delete */
#ifdef __cplusplus
  #define VLAD_MEM_NEW(X) (new(std::nothrow) X)
  #define VLAD_MEM_DELETE(X) (delete X)
#endif

#endif
