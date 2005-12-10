/*
 * This file is part of PolicyUpdater.
 *
 * Copyright (C) 2003, 2004, 2005 University of Western Sydney
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

/* returns VLAD_OK if the given identifier is a valid entity */
int vlad_identifier::validate_ent_ident(const char *a_ident)
{
  /* a valid entity is any string that starts with a lowercase char */

  if (a_ident == NULL)
    return VLAD_NULLPTR;

  return (islower(*a_ident) ? VLAD_OK : VLAD_INVALIDINPUT);
}

/* returns VLAD_OK if the given identifier is a valid variable */
int vlad_identifier::validate_var_ident(const char *a_ident)
{
  /* a valid variable is any string that starts with any of the following
   * prefixes: "S", "A", "O", "SS", "SG", "AS", "AG", "OS" or "OG". */

  if (a_ident == NULL)
    return VLAD_NULLPTR;

  switch(a_ident[0]) {
    case 'S' :
    case 'A' :
    case 'O' :
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
      return VLAD_OK;
  }

  return VLAD_INVALIDINPUT;
}

/* returns the type of the given variable */
unsigned char vlad_identifier::get_var_type(const char *a_ident)
{
  unsigned char type = VLAD_IDENT_VAR_MASK;

  if (a_ident == NULL)
    return VLAD_IDENT_NUL;

  /* get the base type */
  switch(a_ident[0]) {
    case 'S' :
      type |= VLAD_IDENT_SUB;
      break;
    case 'A' :
      type |= VLAD_IDENT_ACC;
      break;
    case 'O' :
      type |= VLAD_IDENT_OBJ;
      break;
    default :
      return VLAD_IDENT_NUL;
  }

  /* check whether it is single, group or both */
  if (strlen(a_ident) >= 2) {
    switch(a_ident[1]) {
      case 'S' :
        return (type | VLAD_IDENT_SIN_MASK);
      case 'G' :
        return (type | VLAD_IDENT_GRP_MASK);
    }
  }

  return (type | VLAD_IDENT_SIN_MASK | VLAD_IDENT_GRP_MASK);
}

/* returns VLAD_OK if the entity type is compatible with the var */
int vlad_identifier::check_compat(const char *a_var, unsigned char a_type)
{
  unsigned char vtype;

  if ((vtype = get_var_type(a_var)) == VLAD_IDENT_NUL)
    return VLAD_INVALIDINPUT;

  if (validate_ent_type(a_type) != VLAD_OK)
    return VLAD_INVALIDINPUT;

  /* first, make sure the basetypes are equal */
  if (VLAD_IDENT_TYPE_BASETYPE(a_type) != VLAD_IDENT_TYPE_BASETYPE(vtype))
    return VLAD_INVALIDINPUT;

  /* now check the rest */
  if (((VLAD_IDENT_SIN_MASK | VLAD_IDENT_GRP_MASK) & vtype) & a_type)
    return VLAD_OK;

  return VLAD_INVALIDINPUT;
}
