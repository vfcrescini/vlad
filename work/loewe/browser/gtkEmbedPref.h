/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is Goanna.
 *
 * The Initial Developer of the Original Code is Tuxia
 * Labs PTY LTD.  Portions created by Tuxia Labs 
 * PTY LTD are Copyright (C) 2001 Tuxia Labs PTY LTD. All
 * Rights Reserved.
 *
 * Contributor(s):
 *   Vino Crescini <vino.crescini@tuxia.com>
 */

#ifndef __GTKEMBEDPREF_H
#define __GTKEMBEDPREF_H

#include "nsIPref.h"

class gtkEmbedPref
{
  public :

    gtkEmbedPref();
    ~gtkEmbedPref();
    bool Init();

    void SetBoolPref(char *, bool);
    bool GetBoolPref(char *);
    void SetCharPref(char *, char *);
    char *GetCharPref(char *);
    void SetIntPref(char *, int);
    int GetIntPref(char *);

  private :

    nsIPref *gPrefService;
} ;

#endif
