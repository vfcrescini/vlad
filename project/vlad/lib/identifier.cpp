/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004, 2005, 2006 University of Western Sydney
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

#include <cctype>
#include <cstring>

#include <vlad/vlad.h>
#include <vlad/identifier.h>

vlad_identifier::vlad_identifier()
{
}

vlad_identifier::~vlad_identifier()
{
}

/* returns VLAD_OK if the given identifier is a valid non-variable */
int vlad_identifier::validate_nvar_ident(const char *a_ident)
{
  /* a valid non-variable is any string that starts with a lowercase char */

  if (a_ident == NULL)
    return VLAD_NULLPTR;

  return (islower(*a_ident) ? VLAD_OK : VLAD_INVALIDINPUT);
}

/* returns VLAD_OK if the given identifier is a valid variable */
int vlad_identifier::validate_var_ident(const char *a_ident)
{
  /* a valid variable is any string that starts with any of the following
   * prefixes: "S", "A", "O", "SS", "SG", "AS", "AG", "OS", "OG" or "I" */

  if (a_ident == NULL)
    return VLAD_NULLPTR;

  switch(a_ident[0]) {
    case 'S' :
    case 'A' :
    case 'O' :
    case 'I' :
      /* no need to check the second char here */
      return VLAD_OK;
  }

  return VLAD_INVALIDINPUT;
}

/* returns VLAD_OK if the given type is a valid entity type */
int vlad_identifier::validate_ent_type(unsigned char a_type)
{
  switch(a_type) {
    case VLAD_IDENT_ENT_SUB_SIN :
    case VLAD_IDENT_ENT_ACC_SIN :
    case VLAD_IDENT_ENT_OBJ_SIN :
    case VLAD_IDENT_ENT_SUB_GRP :
    case VLAD_IDENT_ENT_ACC_GRP :
    case VLAD_IDENT_ENT_OBJ_GRP :
      return VLAD_OK;
  }

  return VLAD_INVALIDINPUT;
}

/* returns VLAD_OK if the given type is a valid interval type */
int vlad_identifier::validate_int_type(unsigned char a_type)
{
  return (a_type == VLAD_IDENT_INT) ? VLAD_OK : VLAD_INVALIDINPUT;
}

/* returns VLAD_OK if the given type is a valid variable type */
int vlad_identifier::validate_var_type(unsigned char a_type)
{
  switch(a_type) {
    case VLAD_IDENT_VAR_SUB :
    case VLAD_IDENT_VAR_ACC :
    case VLAD_IDENT_VAR_OBJ :
    case VLAD_IDENT_VAR_SUB_SIN :
    case VLAD_IDENT_VAR_ACC_SIN :
    case VLAD_IDENT_VAR_OBJ_SIN :
    case VLAD_IDENT_VAR_SUB_GRP :
    case VLAD_IDENT_VAR_ACC_GRP :
    case VLAD_IDENT_VAR_OBJ_GRP :
    case VLAD_IDENT_VAR_INT :
      return VLAD_OK;
  }

  return VLAD_INVALIDINPUT;
}

/* returns the type of the given variable */
unsigned char vlad_identifier::get_var_type(const char *a_ident)
{
  unsigned char type;

  if (a_ident == NULL)
    return VLAD_IDENT_NUL;

  /* get the base type */
  switch(a_ident[0]) {
    case 'I' :
      return VLAD_IDENT_MASK_INT | VLAD_IDENT_MASK_VAR;
    case 'S' :
      type = VLAD_IDENT_MASK_ENT | VLAD_IDENT_MASK_VAR | VLAD_IDENT_MASK_ENT_SUB ;
      break;
    case 'A' :
      type = VLAD_IDENT_MASK_ENT | VLAD_IDENT_MASK_VAR | VLAD_IDENT_MASK_ENT_ACC ;
      break;
    case 'O' :
      type = VLAD_IDENT_MASK_ENT | VLAD_IDENT_MASK_VAR | VLAD_IDENT_MASK_ENT_OBJ ;
      break;
    default :
      return VLAD_IDENT_NUL;
  }

  /* at this point, we are dealing with an entity var */

  if (strlen(a_ident) >= 2) {
    switch(a_ident[1]) {
      case 'S' :
        return (type | VLAD_IDENT_MASK_ENT_SIN);
      case 'G' :
        return (type | VLAD_IDENT_MASK_ENT_GRP);
    }
  }

  return (type | VLAD_IDENT_MASK_ENT_SIN | VLAD_IDENT_MASK_ENT_GRP);
}

/* returns VLAD_OK if the entity type is compatible with the var */
int vlad_identifier::check_compat(const char *a_var, unsigned char a_type)
{
  unsigned char vtype;

  vtype = get_var_type(a_var);

  /* make sure type is entity variable */
  if (!VLAD_IDENT_TYPE_IS_ENT(vtype) || !VLAD_IDENT_TYPE_IS_VAR(vtype))
    return VLAD_INVALIDINPUT;
  
  /* make sure type is a valid entity type */
  if (validate_ent_type(a_type) != VLAD_OK)
    return VLAD_INVALIDINPUT;

  /* make sure the base types (sub, acc, obj) are equal */
  if ((vtype & VLAD_IDENT_MASK_ENT_BASE) != (a_type & VLAD_IDENT_MASK_ENT_BASE))
    return VLAD_INVALIDINPUT;

  /* now check the rest */
  if (((VLAD_IDENT_MASK_ENT_SIN | VLAD_IDENT_MASK_ENT_GRP) & vtype) & a_type)
    return VLAD_OK;

  return VLAD_INVALIDINPUT;
}
