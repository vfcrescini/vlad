/*
 *  This file is part of PolicyUpdater.
 *
 *  Copyright (C) 2003, 2004, 2005, 2006 University of Western Sydney
 *  by Vino Fernando Crescini <jcrescin@scm.uws.edu.au>
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

#include <cstdlib>
#include <cstdio>
#include <new>

#include <vlad/vlad.h>
#include <vlad/mem.h>

#ifdef VLAD_MEMTEST

void *vlad_mem_malloc(size_t a_size)
{
  void *ptr = NULL;

  ptr = malloc(a_size);

  fprintf(stderr, "%s M %p\n", VLAD_STR_MEMPREFIX, ptr);

  return ptr;
}

void *vlad_mem_realloc(void *a_ptr, size_t a_size)
{
  void *ptr = NULL;

  /* the exact behaviour of realloc is different on different systems when
   * given a NULL ptr or 0 size or both. since free(NULL) is legal, we simply
   * treat all calls to realloc as a call to "free" then "malloc" */

  if (a_ptr)
    fprintf(stderr, "%s F %p\n", VLAD_STR_MEMPREFIX, a_ptr);

  ptr = realloc(a_ptr, a_size);

  fprintf(stderr, "%s M %p\n", VLAD_STR_MEMPREFIX, ptr);

  return ptr;
}

void vlad_mem_free(void *a_ptr)
{
  fprintf(stderr, "%s F %p\n", VLAD_STR_MEMPREFIX, a_ptr);

  free(a_ptr);
}

void *vlad_mem::operator new(size_t a_size)
{
  void *ptr = NULL;

  ptr = ::operator new(a_size);

  fprintf(stderr, "%s M %p\n", VLAD_STR_MEMPREFIX, ptr);

  return ptr;
}

void *vlad_mem::operator new(size_t a_size, const std::nothrow_t &a_nt) throw()
{
  void *ptr = NULL;

  ptr = ::operator new(a_size, a_nt);

  fprintf(stderr, "%s M %p\n", VLAD_STR_MEMPREFIX, ptr);

  return ptr;
}

void *vlad_mem::operator new[](size_t a_size)
{
  void *ptr = NULL;

  ptr = ::operator new[](a_size);

  fprintf(stderr, "%s M %p\n", VLAD_STR_MEMPREFIX, ptr);

  return ptr;
}

void *vlad_mem::operator new[](size_t a_size, const std::nothrow_t &a_nt) throw()
{
  void *ptr = NULL;

  ptr = ::operator new[](a_size, a_nt);

  fprintf(stderr, "%s M %p\n", VLAD_STR_MEMPREFIX, ptr);

  return ptr;
}

void vlad_mem::operator delete(void *a_ptr)
{
  fprintf(stderr, "%s F %p\n", VLAD_STR_MEMPREFIX, a_ptr);

  ::operator delete(a_ptr);
}

void vlad_mem::operator delete[](void *a_ptr)
{
  fprintf(stderr, "%s F %p\n", VLAD_STR_MEMPREFIX, a_ptr);

  ::operator delete[](a_ptr);
}

#endif
