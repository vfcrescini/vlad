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

#include "gtkEmbedPrintManager.h"
#include "nsIComponentManager.h"
#include "nsIServiceManager.h"
#include "nsGfxCIID.h"

static NS_DEFINE_CID(kPrintOptionsCID, NS_PRINTOPTIONS_CID);

gtkEmbedPrintManager::gtkEmbedPrintManager()
{
  gPrintService = nsnull;
}

gtkEmbedPrintManager::~gtkEmbedPrintManager()
{
}

bool gtkEmbedPrintManager::Init()
{
  nsresult retval;

  // get the print service
  gPrintService = do_GetService(kPrintOptionsCID, &retval);

  if (NS_FAILED(retval) || !gPrintService)
    return false;

  // make it silent, ie. don't open the dialog
  gPrintService->SetPrintSilent(true);

  return true;
}

// aWebBrowser = the window to print
// aAll        = true: print entire window; false: print current frame
bool gtkEmbedPrintManager::Print(nsIWebBrowser *aWebBrowser, bool aAll)
{
  int frameFlag;
  nsCOMPtr<nsIDOMWindow> domWindow;
  nsCOMPtr<nsIWebBrowserPrint> browserPrint;

  if (!gPrintService || !aWebBrowser)
    return false;

  // set the flag that identifies what to print
  frameFlag = (aAll) ? nsIPrintOptions::kFramesAsIs : nsIPrintOptions::kSelectedFrame;
  gPrintService->SetPrintFrameType(frameFlag);

  // now get the dom window from the browser
  aWebBrowser->GetContentDOMWindow(getter_AddRefs(domWindow));

  if (!domWindow)
    return false;

  // get the print interface
  browserPrint = do_QueryInterface(aWebBrowser);
  
  if (!browserPrint)
    return false;

  // now we can finally print
  return (browserPrint->Print(domWindow, gPrintService, nsnull) == NS_OK);
}
