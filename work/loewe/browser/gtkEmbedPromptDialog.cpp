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

// open dialog callback
static bool (*gOpenDialogCB)(nsIDOMWindow *,
                             int,
                             int,
                             const char *,
                             gtkEmbedParamList *,
                             int *,
                             bool *);

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

bool gtkEmbedPromptDialog::Init(bool (*aOpenDialogCB)(nsIDOMWindow *,
                                                      int,
                                                      int,
                                                      const char *,
                                                      gtkEmbedParamList *,
                                                      int *,
                                                      bool *))
{
  gOpenDialogCB = aOpenDialogCB;
  return true;
}

NS_IMETHODIMP gtkEmbedPromptDialog::Alert(nsIDOMWindow *aParent,
                                           const PRUnichar *aTitle,
                                           const PRUnichar *aText)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedPromptDialog::Alert()\n");
#endif

  if (gOpenDialogCB) {
    if (!strcmp(NS_ConvertUCS2toUTF8(aTitle).get(), DIALOG_INTERNAL_MESSAGE))
      (*gOpenDialogCB)(aParent,
                       DIALOG_INTERNAL_TYPE,
                       0,
                       NS_ConvertUCS2toUTF8(aText).get(),
                       NULL,
                       NULL,
                       NULL);
    else
      (*gOpenDialogCB)(aParent,
                       DIALOG_ALERT_TYPE,
                       0,
                       NS_ConvertUCS2toUTF8(aText).get(),
                       NULL,
                       NULL,
                       NULL);
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

  return Alert(aParent, aTitle, aText);
}

NS_IMETHODIMP gtkEmbedPromptDialog::Confirm(nsIDOMWindow *aParent,
                                             const PRUnichar *aTitle,
                                             const PRUnichar *aText,
                                             PRBool *aResult)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedPromptDialog::Confirm()\n");
#endif

  if (!gOpenDialogCB) {
    *aResult = false;
    return NS_OK;
  }

  (*gOpenDialogCB)(aParent,
                   DIALOG_CONFIRM_TYPE,
                   0,
                   NS_ConvertUCS2toUTF8(aText).get(),
                   NULL,
                   NULL,
                   (bool *) aResult);

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

  return Confirm(aParent, aTitle, aText, aResult);
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

  char *tempString = NULL;
  gtkEmbedParamList *fieldList = new gtkEmbedParamList();

  if (!gOpenDialogCB) {
    *aValue  = NULL;
    *aResult = false;
    return NS_OK;
  }

  // push the field and the default value into the list
  fieldList->Push("field", NS_ConvertUCS2toUTF8(*aValue).get());

  // open the dialog
  (*gOpenDialogCB)(aParent,
                   DIALOG_PROMPT_TYPE,
                   1,
                   NS_ConvertUCS2toUTF8(aText).get(),
                   fieldList,
                   NULL,
                   (bool *) aResult);

  // get the result from the list
  fieldList->Get("field", &tempString);

  // convert to PRUnichar
  *aValue = NS_ConvertASCIItoUCS2(tempString).ToNewUnicode();

  free(tempString);

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

  char *tempString1 = NULL;
  char *tempString2 = NULL;
  gtkEmbedParamList *fieldList = new gtkEmbedParamList();

  if (!gOpenDialogCB) {
     *aUsername = NULL;
    *aPassword = NULL;
    *aResult   = false;
    return NS_OK;
  }

  // push the field and the default value into the list
  fieldList->Push("username", NS_ConvertUCS2toUTF8(*aUsername).get());
  fieldList->Push("password", NS_ConvertUCS2toUTF8(*aPassword).get());

  // open dialog
  (*gOpenDialogCB)(aParent,
                   DIALOG_USERPASSWORD_TYPE,
                   2,
                   NS_ConvertUCS2toUTF8(aText).get(),
                   fieldList,
                   NULL,
                   (bool *) aResult);

  // get values from list
  fieldList->Get("username", &tempString1); 
  fieldList->Get("password", &tempString2); 

  // convert to PRUnichar
  *aUsername = NS_ConvertASCIItoUCS2(tempString1).ToNewUnicode();
  *aPassword = NS_ConvertASCIItoUCS2(tempString2).ToNewUnicode();

  free(tempString1);
  free(tempString2);

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

  char *tempString = NULL;
  gtkEmbedParamList *fieldList = new gtkEmbedParamList();

  if (!gOpenDialogCB) {
    *aPassword = NULL;
    *aResult   = false;
    return NS_OK;  
  } 

  // push the field and the default value into the list
  fieldList->Push("password", NS_ConvertUCS2toUTF8(*aPassword).get());

  // open the dialog
  (*gOpenDialogCB)(aParent,
                   DIALOG_PASSWORD_TYPE,
                   1,
                   NS_ConvertUCS2toUTF8(aText).get(),
                   fieldList,
                   NULL,
                   (bool *) aResult);

  // get values from list
  fieldList->Get("password", &tempString); 

  // convert to PRUnichar
  *aPassword = NS_ConvertASCIItoUCS2(tempString).ToNewUnicode();

  free(tempString);

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

  gtkEmbedParamList *fieldList = new gtkEmbedParamList();
  int i;

  if (!gOpenDialogCB) {
    *aOutSelection = 0;
    *aResult       = true;
    return NS_OK;
  }

  // push the contents of the array into the list
  for (i = 0; i < (int) aCount; i++) 
    fieldList->Push(NS_ConvertUCS2toUTF8(aSelectList[i]).get(), NULL); 

  // open the dialog
  (*gOpenDialogCB)(aParent,
                   DIALOG_SELECT_TYPE,
                   aCount,
                   NS_ConvertUCS2toUTF8(aText).get(),
                   fieldList,
                   aOutSelection,
                   (bool *) aResult);

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

  nsresult tempReturn;
  PRBool tempResult = false;
  const PRUnichar *tempStrings[] = {
    aButton0Title,
    aButton1Title,
    aButton2Title
  } ;
  
  *aChkBoxValue = false;

  // use select
  tempReturn = Select(aParent,
                      aTitle,
                      aText,
                      3,
                      tempStrings,
                      aResult,
                      &tempResult);
  
  // if select was cancelled, select the default one
  if (!tempResult)
    *aResult = 0;
    
  return tempReturn;
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

