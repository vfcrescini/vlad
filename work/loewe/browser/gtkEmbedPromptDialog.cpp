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
#include "nsCOMPtr.h"
#include "nsXPComFactory.h"
#include "nsString.h"

#define DEBUG

#ifdef DEBUG
  #include <stdio.h>
#endif

NS_IMPL_ISUPPORTS1(gtkEmbedPromptDialog, nsIPromptService)

static bool (*gAlertCB)(int, const char *);
static bool (*gPromptCB)(int, const char *, const char *, bool *);
static bool (*gConfirmCB)(int, const char *, bool *);
static gtkEmbedBrowser **gBrowserArray;

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

bool gtkEmbedPromptDialog::Init(gtkEmbedBrowser **aBrowserArray,
                                bool (*aAlertCB)(int, const char *),
                                bool (*aPromptCB)(int, const char *, const char *, bool *),
                                bool (*aConfirmCB)(int, const char *, bool *))
{
  // store our static references so that every
  // instance can have access to it
  gBrowserArray = aBrowserArray;
  gAlertCB      = aAlertCB;
  gPromptCB     = aPromptCB;
  gConfirmCB    = aConfirmCB;

  return true;
}


NS_IMETHODIMP gtkEmbedPromptDialog::Alert(nsIDOMWindow *aParent,
                                           const PRUnichar *aTitle,
                                           const PRUnichar *aText)
{

#ifdef DEBUG
  fprintf(stderr, "gtkEmbedPromptDialog::Alert()\n");
#endif

  if (gAlertCB)
    (*gAlertCB)(1, NS_ConvertUCS2toUTF8(aText).get());

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

  if (gAlertCB)
    (*gAlertCB)(1, NS_ConvertUCS2toUTF8(aText).get());

  // set the result to false for now
  *aChkBoxValue = false;

  return NS_OK;
}

NS_IMETHODIMP gtkEmbedPromptDialog::Confirm(nsIDOMWindow *aParent,
                                             const PRUnichar *aTitle,
                                             const PRUnichar *aText,
                                             PRBool *aRetval)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedPromptDialog::Confirm()\n");
#endif

  if (gConfirmCB) 
    (*gConfirmCB)(1, NS_ConvertUCS2toUTF8(aText).get(), aRetval);
  else
    *aRetval = false;

  return NS_OK;
}

NS_IMETHODIMP gtkEmbedPromptDialog::ConfirmCheck(nsIDOMWindow *aParent,
                                                  const PRUnichar *aTitle,
                                                  const PRUnichar *aText,
                                                  const PRUnichar *aChkBoxMesg,
                                                  PRBool *aChkBoxValue,
                                                  PRBool *aRetval)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedPromptDialog::ConfirmCheck()\n");
#endif

  if (gConfirmCB)
    (*gConfirmCB)(1, NS_ConvertUCS2toUTF8(aText).get());
  else
    *aRetval = false;

  return NS_OK;
}

NS_IMETHODIMP gtkEmbedPromptDialog::Prompt(nsIDOMWindow *aParent,
                                            const PRUnichar *aTitle,
                                            const PRUnichar *aText,
                                            PRUnichar **aValue,
                                            const PRUnichar *aChkBoxMesg,
                                            PRBool *aChkBoxValue,
                                            PRBool *aRetval)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedPromptDialog::Prompt()\n");
#endif

  if (gPromptCB)
    (*gPromptCB)(1, NS_ConvertUCS2toUTF8(aText).get(), *aValue, aRetval); 
  else {
    *aValue  = NULL;
    *aRetval = false;
  }

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
                                                   PRBool *aRetval)
{

#ifdef DEBUG
  fprintf(stderr, "gtkEmbedPromptDialog::PromptUsernameAndPassword()\n");
#endif

  // return false for now
  *aUsername = NULL;
  *aPassword = NULL;
  *aRetval   = false;

  return NS_OK;
}

NS_IMETHODIMP gtkEmbedPromptDialog::PromptPassword(nsIDOMWindow *aParent,
                                                    const PRUnichar *aTitle,
                                                    const PRUnichar *aText,
                                                    PRUnichar **aPassword,
                                                    const PRUnichar *aChkBoxMesg,
                                                    PRBool *aChkBoxValue,
                                                    PRBool *aRetval)
{

#ifdef DEBUG
  fprintf(stderr, "gtkEmbedPromptDialog::PromptPassword()\n");
#endif

  // return false for now
  *aPassword = NULL;
  *aRetval   = false;

  return NS_OK;
}

NS_IMETHODIMP gtkEmbedPromptDialog::Select(nsIDOMWindow *aParent,
                                            const PRUnichar *aTitle,
                                            const PRUnichar *aText,
                                            PRUint32 aCount,
                                            const PRUnichar **aSelectList,
                                            PRInt32 *aOutSelection,
                                            PRBool *aRetval)
{

#ifdef DEBUG
  fprintf(stderr, "gtkEmbedPromptDialog::Select()\n");
#endif

  // return true for now
  *aRetval = true;

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
                                               PRInt32 *aRetval)

{

#ifdef DEBUG
  fprintf(stderr, "gtkEmbedPromptDialog::ConfirmEx\n");
#endif

  *aRetval = true;

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

