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

#ifndef __GTKEMBEDSSLDIALOG_H
#define __GTKEMBEDSSLDIALOG_H

#include "nsIFactory.h"
#include "nsINSSDialogs.h"
#include "nsIBadCertListener.h"

extern nsresult newSSLDialogFactory(nsIFactory **aResult);

// class to implement (replace) all SSL/NSS dialogs
class gtkEmbedSSLDialog : public nsINSSDialogs,
                          public nsITokenPasswordDialogs,
                          public nsIBadCertListener,
                          public nsISecurityWarningDialogs,
                          public nsICertificateDialogs,
                          public nsIClientAuthDialogs,
                          public nsITokenDialogs,
                          public nsIDOMCryptoDialogs
{
  public :

    gtkEmbedSSLDialog();
    virtual ~gtkEmbedSSLDialog();

    bool Init();

    NS_DECL_ISUPPORTS
    NS_DECL_NSINSSDIALOGS
    NS_DECL_NSITOKENPASSWORDDIALOGS
    NS_DECL_NSIBADCERTLISTENER
    NS_DECL_NSISECURITYWARNINGDIALOGS
    NS_DECL_NSICERTIFICATEDIALOGS
    NS_DECL_NSICLIENTAUTHDIALOGS
    NS_DECL_NSITOKENDIALOGS
    NS_DECL_NSIDOMCRYPTODIALOGS
} ;

// CID of the component we want to override
#define NS_NSSDIALOGS_CID \
  {0x518e071f, 0x1dd2, 0x11b2, {0x93, 0x7e, 0xc4, 0x5f, 0x14, 0xde, 0xf7, 0x78}}

#endif
