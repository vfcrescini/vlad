/*
 * consttab.cpp
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <cstdlib>
#include <cstddef>
#include <new>

#include <vlad/vlad.h>
#include <vlad/consttab.h>

constraint::constraint()
{
  m_exp = NULL;
  m_cond = NULL;
  m_ncond = NULL;
  m_init = false;
}

constraint::~constraint()
{
  if (m_exp != NULL)
    delete m_exp;
  if (m_cond != NULL)
    delete m_cond;
  if (m_ncond != NULL)
    delete m_ncond;
}

bool constraint::cmp(list_item *a_item)
{
  constraint *tmp = NULL;

  if (a_item == NULL)
    return false;

  if ((tmp = dynamic_cast<constraint *>(a_item)) == NULL)
    return false;

  /* ensure both are m_init. return true if both are unm_init */
  if (!m_init)
    return !tmp->m_init;
  if (!tmp->m_init)
    return false;

  /* m_exp cannot be NULL */
  if (!m_exp->cmp(tmp->m_exp))
    return false;

  if (!VLAD_LIST_ITEMCMP(m_cond, tmp->m_cond))
    return false;

  if (!VLAD_LIST_ITEMCMP(m_ncond, tmp->m_ncond))
    return false;

  return true;
}

int constraint::init(expression *a_exp, expression *a_cond, expression *a_ncond)
{
  if (a_exp == NULL)
    return VLAD_NULLPTR;

  if (m_exp != NULL)
    delete m_exp;
  if (m_cond != NULL)
    delete m_cond;
  if (m_ncond != NULL)
    delete m_ncond;

  m_exp = a_exp;
  m_cond = a_cond;
  m_ncond = a_ncond;
  m_init = true;

  return VLAD_OK;
}

int constraint::get(expression **a_exp, expression **a_cond, expression **a_ncond)
{
  if (!m_init)
    return VLAD_UNINITIALISED;

  if (a_exp == NULL || a_cond == NULL || a_ncond == NULL)
    return VLAD_NULLPTR;

  *a_exp = m_exp;
  *a_cond = m_cond;
  *a_ncond = m_ncond;

  return VLAD_OK;
}

consttab::consttab() : list(true)
{
}

consttab::~consttab()
{
  purge(true);
}

int consttab::add(expression *a_exp, expression *a_cond, expression *a_ncond)
{
  int retval;
  constraint *tmp;

  if ((tmp = VLAD_NEW(constraint())) == NULL)
    return VLAD_MALLOCFAILED;

  if ((retval = tmp->init(a_exp, a_cond, a_ncond)) != VLAD_OK) {
    delete tmp;
    return retval;
  }

  return list::add((list_item *) tmp);
}

int consttab::get(unsigned int a_index,
                  expression **a_exp,
                  expression **a_cond,
                  expression **a_ncond)
{
  int retval;
  constraint *tmp;

  if ((retval = list::get(a_index, (list_item **) &tmp)) != VLAD_OK)
    return retval;

  return tmp->get(a_exp, a_cond, a_ncond);
}
