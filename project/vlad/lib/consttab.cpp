/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004, 2005, 2006 University of Western Sydney
 * by Vino Fernando Crescini <jcrescin@scm.uws.edu.au>
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

#include <cstdlib>
#include <cstddef>

#include <vlad/vlad.h>
#include <vlad/mem.h>
#include <vlad/consttab.h>

vlad_constraint::vlad_constraint()
{
  m_exp = NULL;
  m_cond = NULL;
  m_ncond = NULL;
  m_rlist = NULL;
  m_init = false;
}

vlad_constraint::~vlad_constraint()
{
  if (m_exp != NULL)
    VLAD_MEM_DELETE(m_exp);
  if (m_cond != NULL)
    VLAD_MEM_DELETE(m_cond);
  if (m_ncond != NULL)
    VLAD_MEM_DELETE(m_ncond);
  if (m_rlist != NULL)
    VLAD_MEM_DELETE(m_rlist);
}

/* return true if the 2 are equal */
bool vlad_constraint::cmp(vlad_list_item *a_item)
{
  vlad_constraint *cons = NULL;

  if (a_item == NULL)
    return false;

  if ((cons = dynamic_cast<vlad_constraint *>(a_item)) == NULL)
    return false;

  /* ensure both are m_init. return true if both are unm_init */
  if (!m_init)
    return !cons->m_init;
  if (!cons->m_init)
    return false;

  /* m_exp cannot be NULL */
  if (!m_exp->cmp(cons->m_exp))
    return false;

  if (!VLAD_LIST_ITEMCMP(m_cond, cons->m_cond))
    return false;

  if (!VLAD_LIST_ITEMCMP(m_ncond, cons->m_ncond))
    return false;

  if (!VLAD_LIST_ITEMCMP(m_rlist, cons->m_rlist))
    return false;

  return true;
}

/* initialise with the given values */
int vlad_constraint::init(vlad_expression *a_exp,
                          vlad_expression *a_cond,
                          vlad_expression *a_ncond,
                          vlad_rlist *a_rlist)
{
  if (a_exp == NULL)
    return VLAD_NULLPTR;

  if (m_exp != NULL)
    VLAD_MEM_DELETE(m_exp);
  if (m_cond != NULL)
    VLAD_MEM_DELETE(m_cond);
  if (m_ncond != NULL)
    VLAD_MEM_DELETE(m_ncond);
  if (m_rlist != NULL)
    VLAD_MEM_DELETE(m_rlist);

  m_exp = a_exp;
  m_cond = a_cond;
  m_ncond = a_ncond;
  m_rlist = a_rlist;
  m_init = true;

  return VLAD_OK;
}

/* gives a reference to the values */
int vlad_constraint::get(vlad_expression **a_exp,
                         vlad_expression **a_cond,
                         vlad_expression **a_ncond,
                         vlad_rlist **a_rlist)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_exp == NULL)
    return VLAD_NULLPTR;

  *a_exp = m_exp;
  if (a_cond != NULL)
    *a_cond = m_cond;
  if (a_ncond != NULL)
    *a_ncond = m_ncond;
  if (a_rlist != NULL)
    *a_rlist = m_rlist;

  return VLAD_OK;
}

vlad_consttab::vlad_consttab() : vlad_list(true)
{
}

vlad_consttab::~vlad_consttab()
{
  purge(true);
}

/* store in list */
int vlad_consttab::add(vlad_expression *a_exp,
                       vlad_expression *a_cond,
                       vlad_expression *a_ncond,
                       vlad_rlist *a_rlist)
{
  int retval;
  vlad_constraint *cons;

  if ((cons = VLAD_MEM_NEW(vlad_constraint())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = cons->init(a_exp, a_cond, a_ncond, a_rlist)) != VLAD_OK) {
    VLAD_MEM_DELETE(cons);
    return retval;
  }

  return vlad_list::add((vlad_list_item *) cons);
}

/* get reference of the constraint matching the index */
int vlad_consttab::get(unsigned int a_index,
                       vlad_expression **a_exp,
                       vlad_expression **a_cond,
                       vlad_expression **a_ncond,
                       vlad_rlist **a_rlist)
{
  int retval;
  vlad_constraint *cons;

  if ((retval = vlad_list::get(a_index, (vlad_list_item **) &cons)) != VLAD_OK)
    return retval;

  return cons->get(a_exp, a_cond, a_ncond, a_rlist);
}
