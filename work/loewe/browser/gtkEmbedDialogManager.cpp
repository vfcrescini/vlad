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

#define DIALOGMANAGER_PROMPTDIALOG_CONTRACTID    "@mozilla.org/embedcomp/prompt-service;1"
#define DIALOGMANAGER_SSLDIALOG_CONTRACTID       "@mozilla.org/nsNSSDialogs;1"
#define DIALOGMANAGER_HELPERAPPDIALOG_CONTRACTID "@mozilla.org/helperapplauncherdialog;1"

static NS_DEFINE_CID(kPromptDialogCID, NS_PROMPTSERVICE_CID);
static NS_DEFINE_CID(kSSLDialogCID, NS_NSSDIALOGS_CID);
static NS_DEFINE_CID(kHelperAppDialogCID, NS_HELPERAPPLAUNCHERDIALOG_CID);

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

bool gtkEmbedDialogManager::Init(bool (*aSSLActiveCB)(nsIDOMWindow *, bool),
                                 bool (*aAlertCB)(nsIDOMWindow *, const char *),
                                 bool (*aPromptCB)(nsIDOMWindow *, const char *, const char *, bool *),
                                 bool (*aConfirmCB)(nsIDOMWindow *, const char *, bool *),
                                 bool (*aPasswdCB)(nsIDOMWindow *, const char *, const char *, bool *),
                                 bool (*aUserPasswdCB)(nsIDOMWindow *, const char *, const char *, const char *, bool *),
                                 bool (*aSelectCB)(nsIDOMWindow *, const char **, int *, bool *))
{
  nsCOMPtr<nsIFactory>    promptDialog;
  nsCOMPtr<nsIFactory>    sslDialog;
  nsCOMPtr<nsIFactory>    helperAppDialog;
  gtkEmbedPromptDialog    *promptDialogInstance;
  gtkEmbedSSLDialog       *sslDialogInstance;
  gtkEmbedHelperAppDialog *helperAppDialogInstance;

  // register our own implementation of nsIPromptService
  if (NS_FAILED(newPromptDialogFactory(getter_AddRefs(promptDialog))))
    return false;

  nsComponentManager::RegisterFactory(kPromptDialogCID,
                                      "Prompt Dialog",
                                      DIALOGMANAGER_PROMPTDIALOG_CONTRACTID,
                                      promptDialog,
                                      true);

  // register our own implementation of nsINSSDialogs
  if (NS_FAILED(newSSLDialogFactory(getter_AddRefs(sslDialog))))
    return false; 

  nsComponentManager::RegisterFactory(kSSLDialogCID,
                                      "SSL Dialog",
                                      DIALOGMANAGER_SSLDIALOG_CONTRACTID,
                                      sslDialog,
                                      true);

  // register our own implementation of nsIHelperAppLauncherDialog
  if (NS_FAILED(newHelperAppDialogFactory(getter_AddRefs(helperAppDialog))))
    return false; 

  nsComponentManager::RegisterFactory(kHelperAppDialogCID,
                                      "Helper App Dialog",
                                      DIALOGMANAGER_HELPERAPPDIALOG_CONTRACTID,
                                      helperAppDialog,
                                      true);
  
  // create an instance of each
  promptDialogInstance    = new gtkEmbedPromptDialog();
  sslDialogInstance       = new gtkEmbedSSLDialog();  
  helperAppDialogInstance = new gtkEmbedHelperAppDialog();

  if (!promptDialogInstance || !sslDialogInstance || !helperAppDialogInstance)
    return false;

  // call their respective init functions
  promptDialogInstance->Init(aSSLActiveCB,
                             aAlertCB,
                             aPromptCB,
                             aConfirmCB,
                             aPasswdCB,
                             aUserPasswdCB,
                             aSelectCB);
  sslDialogInstance->Init();
  helperAppDialogInstance->Init();

  // now keep a reference to these objects to retain
  // the static values
  gPromptDialog    = promptDialogInstance;
  gSSLDialog       = sslDialogInstance;
  gHelperAppDialog = helperAppDialogInstance;

  return true;
}
