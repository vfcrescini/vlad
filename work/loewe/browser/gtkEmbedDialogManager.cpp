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

#include "nsCOMPtr.h"
#include "nsIFactory.h"
#include "nsIComponentManager.h"
#include "gtkEmbedDialogManager.h"

#define DIALOGMANAGER_PROMPTDIALOG_CONTRACTID "@mozilla.org/embedcomp/prompt-service;1"
#define DIALOGMANAGER_SSLDIALOG_CONTRACTID    "@mozilla.org/nsNSSDialogs;1"

static NS_DEFINE_CID(kPromptDialogCID, NS_PROMPTSERVICE_CID);
static NS_DEFINE_CID(kSSLDialogCID, NS_NSSDIALOGS_CID);

gtkEmbedDialogManager::gtkEmbedDialogManager()
{
  gPromptDialog = NULL;
  gSSLDialog    = NULL;
}

gtkEmbedDialogManager::~gtkEmbedDialogManager()
{
  if (gPromptDialog)
    delete gPromptDialog;

  if (gSSLDialog)
    delete gSSLDialog;
}

bool gtkEmbedDialogManager::Init(bool (*aAlertCB)(nsIDOMWindow *, const char *),
                                 bool (*aPromptCB)(nsIDOMWindow *, const char *, const char *, bool *),
                                 bool (*aConfirmCB)(nsIDOMWindow *, const char *, bool *))
{
  nsCOMPtr<nsIFactory> promptDialog;
  nsCOMPtr<nsIFactory> sslDialog;
  gtkEmbedPromptDialog *promptDialogInstance;
  gtkEmbedSSLDialog    *sslDialogInstance;

  // register our own implementation of nsIPromptService
  if (NS_FAILED(newPromptDialogFactory(getter_AddRefs(promptDialog))))
    return false;

  nsComponentManager::RegisterFactory(kPromptDialogCID,
                                      "Prompt Dialog",
                                      DIALOGMANAGER_PROMPTDIALOG_CONTRACTID,
                                      promptDialog,
                                      true);

  // register our own implementation of nsINSSDialogs.h
  if (NS_FAILED(newSSLDialogFactory(getter_AddRefs(sslDialog))))
    return false; 

  nsComponentManager::RegisterFactory(kSSLDialogCID,
                                      "SSL Dialog",
                                      DIALOGMANAGER_SSLDIALOG_CONTRACTID,
                                      sslDialog,
                                      true);

  // create an instance of each so we can give a static reference
  // to our callback functions

  promptDialogInstance = new gtkEmbedPromptDialog();
  sslDialogInstance    = new gtkEmbedSSLDialog();  

  if (!promptDialogInstance || !sslDialogInstance)
    return false;

  promptDialogInstance->Init(aAlertCB,
                             aPromptCB,
                             aConfirmCB);

  sslDialogInstance->Init(aAlertCB,
                          aPromptCB, 
                          aConfirmCB);

  // now keep a reference to these objects to retain
  // the static values
  gPromptDialog = promptDialogInstance;
  gSSLDialog    = sslDialogInstance;

  return true;
}
