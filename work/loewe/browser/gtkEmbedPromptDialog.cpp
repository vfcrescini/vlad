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

#include "gtkEmbedPromptDialog.h"
#include "gtkEmbedConst.h"
#include "nsCOMPtr.h"
#include "nsXPComFactory.h"
#include "nsString.h"

#ifdef DEBUG
#include <stdio.h>
#endif

NS_IMPL_ISUPPORTS1(gtkEmbedPromptDialog, nsIPromptService)

static bool (*gSSLSecureCB)(nsIDOMWindow *, bool) = NULL;
static bool (*gAlertCB)(nsIDOMWindow *, const char *) = NULL;
static bool (*gPromptCB)(nsIDOMWindow *, const char *, const char *, bool *) = NULL;
static bool (*gConfirmCB)(nsIDOMWindow *, const char *, bool *) = NULL;
static bool (*gPasswdCB)(nsIDOMWindow *, const char *, const char *, bool *) = NULL;
static bool (*gUserPasswdCB)(nsIDOMWindow *, const char *, const char *, const char *, bool *) = NULL;
static bool (*gSelectCB)(nsIDOMWindow *, const char **, int *, bool *) = NULL;

gtkEmbedPromptDialog::gtkEmbedPromptDialog()
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedPromptDialog::gtkEmbedPromptDialog()\n");
#endif

  NS_INIT_REFCNT();
}

gtkEmbedPromptDialog::~gtkEmbedPromptDialog()
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedPromptDialog::~gtkEmbedPromptDialog()\n");
#endif
}

bool gtkEmbedPromptDialog::Init(bool (*aSSLSecureCB)(nsIDOMWindow *, bool),
                                bool (*aAlertCB)(nsIDOMWindow *, const char *),
                                bool (*aPromptCB)(nsIDOMWindow *, const char *, const char *, bool *),
                                bool (*aConfirmCB)(nsIDOMWindow *, const char *, bool *),
                                bool (*aPasswdCB)(nsIDOMWindow *, const char *, const char *, bool *),
                                bool (*aUserPasswdCB)(nsIDOMWindow *, const char *, const char *, const char *, bool *),
                                bool (*aSelectCB)(nsIDOMWindow *, const char **, int *, bool *))

{
  // store our static references so that every
  // instance can have access to it
  gSSLSecureCB  = aSSLSecureCB;
  gAlertCB      = aAlertCB;
  gPromptCB     = aPromptCB;
  gConfirmCB    = aConfirmCB;
  gPasswdCB     = aPasswdCB;
  gUserPasswdCB = aUserPasswdCB;
  gSelectCB     = aSelectCB;
  
  return true;
}

NS_IMETHODIMP gtkEmbedPromptDialog::Alert(nsIDOMWindow *aParent,
                                           const PRUnichar *aTitle,
                                           const PRUnichar *aText)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedPromptDialog::Alert()\n");
#endif

  // check if it's an internal message
  if (!strcmp(NS_ConvertUCS2toUTF8(aTitle).get(), DIALOG_INTERNAL_MESSAGE)) {
    if (!strcmp(NS_ConvertUCS2toUTF8(aText).get(), DIALOG_SSL_ENTER_SECURE)) {
      if (gSSLSecureCB)
        (*gSSLSecureCB)(aParent, true);
    }
    else if (!strcmp(NS_ConvertUCS2toUTF8(aText).get(), DIALOG_SSL_ENTER_WEAK)) {
      if (gSSLSecureCB)
        (*gSSLSecureCB)(aParent, true);
    }
    else if (!strcmp(NS_ConvertUCS2toUTF8(aText).get(), DIALOG_SSL_LEAVE_SECURE)) {
      if (gSSLSecureCB)
        (*gSSLSecureCB)(aParent, false);
    }
  }
  else {
    // regular message
    if (gAlertCB)
      (*gAlertCB)(aParent, NS_ConvertUCS2toUTF8(aText).get());
  }	

  return NS_OK;
}

NS_IMETHODIMP gtkEmbedPromptDialog::AlertCheck(nsIDOMWindow *aParent,
                                                const PRUnichar *aTitle,
                                                const PRUnichar *aText,
                                                const PRUnichar *aChkBoxMesg,
                                                PRBool *aChkBoxValue)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedPromptDialog::AlertCheck()\n");
#endif

  *aChkBoxValue = false;

  return NS_OK;
}

NS_IMETHODIMP gtkEmbedPromptDialog::Confirm(nsIDOMWindow *aParent,
                                             const PRUnichar *aTitle,
                                             const PRUnichar *aText,
                                             PRBool *aResult)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedPromptDialog::Confirm()\n");
#endif

  *aResult = true;

  return NS_OK;
}

NS_IMETHODIMP gtkEmbedPromptDialog::ConfirmCheck(nsIDOMWindow *aParent,
                                                  const PRUnichar *aTitle,
                                                  const PRUnichar *aText,
                                                  const PRUnichar *aChkBoxMesg,
                                                  PRBool *aChkBoxValue,
                                                  PRBool *aResult)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedPromptDialog::ConfirmCheck()\n");
#endif

  *aChkBoxValue = false;
  *aResult      = true;

  return NS_OK;
}

NS_IMETHODIMP gtkEmbedPromptDialog::Prompt(nsIDOMWindow *aParent,
                                            const PRUnichar *aTitle,
                                            const PRUnichar *aText,
                                            PRUnichar **aValue,
                                            const PRUnichar *aChkBoxMesg,
                                            PRBool *aChkBoxValue,
                                            PRBool *aResult)
{

#ifdef DEBUG
  fprintf(stderr, "gtkEmbedPromptDialog::Prompt()\n");
#endif

  *aValue  = NULL;
  *aResult = false;

  return NS_OK;
}

NS_IMETHODIMP gtkEmbedPromptDialog::PromptUsernameAndPassword(
                                                   nsIDOMWindow *aParent,
                                                   const PRUnichar *aTitle,
                                                   const PRUnichar *aText,
                                                   PRUnichar **aUsername,
                                                   PRUnichar **aPassword,
                                                   const PRUnichar *aChkBoxMesg,
                                                   PRBool *aChkBoxValue,
                                                   PRBool *aResult)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedPromptDialog::PromptUsernameAndPassword()\n");
#endif

  *aUsername = NULL;
  *aPassword = NULL;
  *aResult   = false;

  return NS_OK;
}

NS_IMETHODIMP gtkEmbedPromptDialog::PromptPassword(nsIDOMWindow *aParent,
                                                    const PRUnichar *aTitle,
                                                    const PRUnichar *aText,
                                                    PRUnichar **aPassword,
                                                    const PRUnichar *aChkBoxMesg,
                                                    PRBool *aChkBoxValue,
                                                    PRBool *aResult)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedPromptDialog::PromptPassword()\n");
#endif

  *aPassword = NULL;
  *aResult   = false;

  return NS_OK;
}

NS_IMETHODIMP gtkEmbedPromptDialog::Select(nsIDOMWindow *aParent,
                                            const PRUnichar *aTitle,
                                            const PRUnichar *aText,
                                            PRUint32 aCount,
                                            const PRUnichar **aSelectList,
                                            PRInt32 *aOutSelection,
                                            PRBool *aResult)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedPromptDialog::Select()\n");
#endif

  *aOutSelection = 0;
  *aResult       = true;

  return NS_OK;
}

NS_IMETHODIMP gtkEmbedPromptDialog::ConfirmEx(nsIDOMWindow *aParent,
                                               const PRUnichar *aTitle,
                                               const PRUnichar *aText,
                                               PRUint32 aButtonFlags,
                                               const PRUnichar *aButton0Title,
                                               const PRUnichar *aButton1Title,
                                               const PRUnichar *aButton2Title,
                                               const PRUnichar *aChkBoxMesg,
                                               PRBool *aChkBoxValue,
                                               PRInt32 *aResult)

{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedPromptDialog::ConfirmEx\n");
#endif

  *aChkBoxValue = false;
  *aResult      = true;

  return NS_OK;
}

NS_DEF_FACTORY(PromptDialog, gtkEmbedPromptDialog);

nsresult newPromptDialogFactory(nsIFactory **aResult)
{
  nsresult tempRetval = NS_OK;
  nsIFactory *tempInst = new nsPromptDialogFactory;

  if (tempInst == nsnull)
    tempRetval = NS_ERROR_OUT_OF_MEMORY;
  else
    NS_ADDREF(tempInst);

  *aResult = tempInst;

  return tempRetval;
} 

