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

#include <stdlib.h>
#include "gtkEmbedHelperAppDialog.h"
#include "gtkEmbedPref.h"
#include "gtkEmbedConst.h"
#include "gtkEmbedParamList.h"
#include "nsExternalHelperAppService.h"
#include "nsCOMPtr.h"
#include "nsXPComFactory.h"
#include "nsString.h"
#include "nsILocalFile.h"
#include "nsIMIMEInfo.h"

#ifdef DEBUG
#include <stdio.h>
#endif

// static list to store the mime handlers from prefs
static gtkEmbedParamList *sList = NULL;

NS_IMPL_ISUPPORTS1(gtkEmbedHelperAppDialog, nsIHelperAppLauncherDialog);

gtkEmbedHelperAppDialog::gtkEmbedHelperAppDialog()
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedHelperAppDialog::gtkEmbedHelperAppDialog()\n");
#endif

  NS_INIT_REFCNT();
}

gtkEmbedHelperAppDialog::~gtkEmbedHelperAppDialog()
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedHelperAppDialog::~gtkEmbedHelperAppDialog()\n");
#endif
}

bool gtkEmbedHelperAppDialog::Init()
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedHelperAppDialog::Init()\n");
#endif

  gtkEmbedPref *prefService;
  char prefString[HELPERAPP_PREF_MAXLEN];
  char *tempMime;
  char *tempHandler;
  int i;

  // create an instance of the app list
  if (!(sList = new gtkEmbedParamList()))
    return false;

  // create an instance of the pref service
  if (!(prefService = new gtkEmbedPref()))
    return false;

  if (!prefService->Init())
    return false;

  // now fill the list with the app handlers
  for (i = 1; i <= HELPERAPP_PREF_HANDLERMAX; i++) {
    tempMime    = NULL;
    tempHandler = NULL;
    // get the mimetype
    sprintf(prefString, "%s.%d", HELPERAPP_PREF_MIMEPREFIX, i);
    tempMime = prefService->GetCharPref(prefString);
    // get the handler
    sprintf(prefString, "%s.%d", HELPERAPP_PREF_HANDLERPREFIX, i);
    tempHandler = prefService->GetCharPref(prefString);

    // if either one is empty, we have reached the end
    if (!tempMime || *tempMime == '\0' || !tempHandler || *tempHandler == '\0')
      break;

    // store this mime handler in the list
    sList->Push(tempMime, tempHandler);
  }

  // cleanup
  delete prefService;

  return true;
}

NS_IMETHODIMP gtkEmbedHelperAppDialog::Show(nsIHelperAppLauncher *aLauncher, 
                                            nsISupports *aContext)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedHelperAppDialog::Show()\n");
#endif

  nsIMIMEInfo *mimeInfo                  = NULL;
  nsMIMEInfoHandleAction preferredAction = 0;
  nsCOMPtr<nsILocalFile> localFile       = NULL;
  char *command                          = NULL;
  char *mime                             = NULL;
  char *wrapper                          = NULL;
 
  if (!aLauncher)
    return NS_OK;

  // get mime type
  aLauncher->GetMIMEInfo(&mimeInfo);

  if (!mimeInfo) {
    // cant get mimetype so just ignore
    return NS_OK;
  }
    
  mimeInfo->GetMIMEType(&mime);

  // check if we have the handler for this mime type
  if (sList && sList->Get(mime, &wrapper)) {
    // check if it's a variable or the actual program
    if (*wrapper == '$')
      command = getenv(wrapper + 1);
    else
      command = wrapper;

    NS_NewLocalFile(command, 1, getter_AddRefs(localFile));
  }

#ifdef DEBUG
  fprintf(stderr, "mime is    : %s\n", mime);
  fprintf(stderr, "handler is : %s\n", wrapper);
  fprintf(stderr, "command is : %s\n", command);
#endif

  // set the progress listener to null to begin action immediately
  aLauncher->SetWebProgressListener(NULL);

  if (localFile) {
    // good, we got it so launch the app
    aLauncher->LaunchWithApplication(localFile, false);
  }
  else {
    // cant get the wrapper command so just use preferred
    mimeInfo->GetPreferredAction(&preferredAction);
  
    if (preferredAction != nsIMIMEInfo::saveToDisk)
      aLauncher->LaunchWithApplication(NULL, false);
    else
      aLauncher->Cancel();
  }

  return NS_OK;
}

NS_IMETHODIMP gtkEmbedHelperAppDialog::PromptForSaveToFile(nsISupports *aWindowContext,
                                                           const PRUnichar *aDefaultFile,
                                                           const PRUnichar *aSuggestedFileExtension,
                                                           nsILocalFile **aRetval)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedHelperAppDialog::PromptForSaveToFile()\n");
#endif

  *aRetval = NULL;

  return NS_OK;
}

NS_IMETHODIMP gtkEmbedHelperAppDialog::ShowProgressDialog(nsIHelperAppLauncher *aLauncher, 
                                                          nsISupports *aContext)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedHelperAppDialog::ShowProgressDialog()\n");
#endif

    return NS_OK;
}

NS_DEF_FACTORY(HelperAppDialog, gtkEmbedHelperAppDialog);

nsresult newHelperAppDialogFactory(nsIFactory **aResult)
{
  nsresult tempRetval = NS_OK;
  nsIFactory *tempInst = new nsHelperAppDialogFactory;

  if (tempInst == nsnull)
    tempRetval = NS_ERROR_OUT_OF_MEMORY;
  else
    NS_ADDREF(tempInst);

  *aResult = tempInst;

  return tempRetval;
} 
