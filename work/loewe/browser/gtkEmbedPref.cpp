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

#include "gtkEmbedPref.h"
#include "nsCOMPtr.h"
#include "nsIInterfaceRequestor.h"
#include "nsIComponentManager.h"

gtkEmbedPref::gtkEmbedPref()
{
  gPrefService = NULL;
}

gtkEmbedPref::~gtkEmbedPref()
{
  if (gPrefService)
    NS_RELEASE(gPrefService);
}

bool gtkEmbedPref::Init()
{
  nsCOMPtr<nsIPref> tempPref;

  // get an instance of the pref service
  if (!(tempPref = do_GetService(NS_PREF_CONTRACTID)))
    return false;
  
  gPrefService = tempPref.get();
  NS_ADDREF(gPrefService);

  return gPrefService;
}

// wrapper functions for setting prefs
// these ones save and update the prefs immediately

void gtkEmbedPref::SetBoolPref(char *aString, bool aValue)
{
  if (aString && gPrefService) {
    gPrefService->SetBoolPref(aString, aValue);
    gPrefService->SavePrefFile(NULL);
    gPrefService->ReadUserPrefs(NULL);
  }
}

void gtkEmbedPref::SetCharPref(char *aString, char *aValue)
{
  if (aString && aValue && gPrefService) {
    gPrefService->SetCharPref(aString, aValue);
    gPrefService->SavePrefFile(NULL);
    gPrefService->ReadUserPrefs(NULL);
  }
}

void gtkEmbedPref::SetIntPref(char *aString, int aValue)
{
  if (aString && gPrefService) {
    gPrefService->SetIntPref(aString, aValue);
    gPrefService->SavePrefFile(NULL);
    gPrefService->ReadUserPrefs(NULL);
  }
}

// for the sake of completeness let's include
// the get pref functions

bool gtkEmbedPref::GetBoolPref(char *aString)
{
  int tempInt = 0;

  if (aString && gPrefService)
    gPrefService->GetBoolPref(aString, &tempInt);

  return (tempInt != 0);
}

char *gtkEmbedPref::GetCharPref(char *aString)
{
  char *tempString = "";

  if (aString && gPrefService)
    gPrefService->GetCharPref(aString, &tempString);

  return tempString;
}

int gtkEmbedPref::GetIntPref(char *aString)
{
  int tempInt = 0;

  if (aString && gPrefService)
    gPrefService->GetIntPref(aString, &tempInt);

  return tempInt;
}
