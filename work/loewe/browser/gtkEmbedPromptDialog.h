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

#ifndef __GTKEMBEDPROMPTDIALOG_H
#define __GTKEMBEDPROMPTDIALOG_H

#include "nsIFactory.h"
#include "nsIPromptService.h"
#include "nsIDOMWindow.h"

extern nsresult newPromptDialogFactory(nsIFactory **aResult);

class gtkEmbedPromptDialog : public nsIPromptService
{
  public :

    gtkEmbedPromptDialog();
    virtual ~gtkEmbedPromptDialog();

    bool Init(bool (*aSSLSecureCB)(nsIDOMWindow *, bool),
              bool (*aAlertCB)(nsIDOMWindow *, const char *),
              bool (*aPromptCB)(nsIDOMWindow *, const char *, const char *, bool *),
              bool (*aConfirmCB)(nsIDOMWindow *, const char *, bool *),
	      bool (*aPasswdCB)(nsIDOMWindow *, const char * , const char *, bool *),
              bool (*aUserPasswdCB)(nsIDOMWindow *, const char *, const char *, const char *, bool *),
              bool (*aSelectCB)(nsIDOMWindow *, const char **, int *, bool *));
    
    NS_DECL_ISUPPORTS
    NS_DECL_NSIPROMPTSERVICE
} ;

// CID of the component we want to override
#define NS_PROMPTSERVICE_CID \
 {0xa2112d6a, 0x0e28, 0x421f, {0xb4, 0x6a, 0x25, 0xc0, 0xb3, 0x8, 0xcb, 0xd0}}

#endif
