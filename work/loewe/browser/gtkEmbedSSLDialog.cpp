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

#include "gtkEmbedSSLDialog.h"
#include "nsCOMPtr.h"
#include "nsXPComFactory.h"

#define DEBUG

#ifdef DEBUG
  #include <stdio.h>
#endif

NS_IMPL_ISUPPORTS8(gtkEmbedSSLDialog, nsINSSDialogs, 
                                      nsITokenPasswordDialogs,
                                      nsISecurityWarningDialogs,
                                      nsIBadCertListener,
                                      nsICertificateDialogs,
                                      nsIClientAuthDialogs,
                                      nsITokenDialogs,
                                      nsIDOMCryptoDialogs);

static bool (*gAlertCB)(int, const char *);
static bool (*gPromptCB)(int, const char *, const char *, bool *);
static bool (*gConfirmCB)(int, const char *, bool *);
static gtkEmbedBrowser **gBrowserArray;

gtkEmbedSSLDialog::gtkEmbedSSLDialog()
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedSSLDialog::gtkEmbedSSLDialog()\n");
#endif

  NS_INIT_REFCNT();
}

gtkEmbedSSLDialog::~gtkEmbedSSLDialog()
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedSSLDialog::~gtkEmbedSSLDialog()\n");
#endif
}

bool gtkEmbedSSLDialog::Init(gtkEmbedBrowser **aBrowserArray,
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

// nsITokenPasswordDialogs

NS_IMETHODIMP gtkEmbedSSLDialog::SetPassword(nsIInterfaceRequestor *aCTX,
                                             const PRUnichar *aTokenName,
                                             PRBool *aCancelled)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedSSLDialog::~gtkEmbedSSLDialog()\n");
#endif

  return NS_OK;
}

NS_IMETHODIMP gtkEmbedSSLDialog::GetPassword(nsIInterfaceRequestor *aCTX,
                                             const PRUnichar *aTokenName, 
                                             PRUnichar **aPassword,
                                             PRBool *aCancelled)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedSSLDialog::GetPassword()\n");
#endif

  return NS_OK;
}

// nsISecurityWarningDialogs

NS_IMETHODIMP gtkEmbedSSLDialog::AlertEnteringSecure(nsIInterfaceRequestor *aCTX)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedSSLDialog::AlertEnteringSecure()\n");
#endif

  return NS_OK;
}

NS_IMETHODIMP gtkEmbedSSLDialog::AlertEnteringWeak(nsIInterfaceRequestor *aCTX)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedSSLDialog::AlertEnteringWeak()\n");
#endif

  return NS_OK;
}

NS_IMETHODIMP gtkEmbedSSLDialog::AlertLeavingSecure(nsIInterfaceRequestor *aCTX)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedSSLDialog::AlertLeavingSecure()\n");
#endif

  return NS_OK;
}

NS_IMETHODIMP gtkEmbedSSLDialog::AlertMixedMode(nsIInterfaceRequestor *aCTX)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedSSLDialog::AlertMixedMode()\n");
#endif

  return NS_OK;
}

NS_IMETHODIMP gtkEmbedSSLDialog::ConfirmPostToInsecure(nsIInterfaceRequestor *aCTX,
                                                       PRBool *aResult)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedSSLDialog::ConfirmPostToInsecure()\n");
#endif

  // just return true for now
  *aResult = true;
  return NS_OK;
}

NS_IMETHODIMP gtkEmbedSSLDialog::ConfirmPostToInsecureFromSecure(nsIInterfaceRequestor *aCTX, 
                                                                 PRBool *aResult)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedSSLDialog::ConfirmPostToInsecureFromSecure\n");
#endif

  // just return true for now
  *aResult = true;

  return NS_OK;
}

// nsIBadCertListener

NS_IMETHODIMP gtkEmbedSSLDialog::UnknownIssuer(nsITransportSecurityInfo *aSocketInfo,
                                               nsIX509Cert *aCert,
                                               PRInt16 *aOutAddType,
                                               PRBool *aRetval)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedSSLDialog::UnknownIssuer()\n");
#endif

  // just add trust for this session
  *aOutAddType = ADD_TRUSTED_FOR_SESSION;

  // just return true for now
  *aRetval = true;
  return NS_OK;
}

NS_IMETHODIMP gtkEmbedSSLDialog::MismatchDomain(nsITransportSecurityInfo *aSocketInfo,
                                                const PRUnichar *aTargetURL, 
                                                nsIX509Cert *aCert,
                                                PRBool *aRetval)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedSSLDialog::MismatchedDomain()\n");
#endif

  // just return true for now
  *aRetval = true;
  return NS_OK;
}

NS_IMETHODIMP gtkEmbedSSLDialog::CertExpired(nsITransportSecurityInfo *aSocketInfo,
                                             nsIX509Cert *aCert,
                                             PRBool *aRetval)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedSSLDialog::CertExpired()\n");
#endif

  // just return true for now
  *aRetval = true;
  return NS_OK;
}

// nsICertificateDialogs

NS_IMETHODIMP gtkEmbedSSLDialog::DownloadCACert(nsIInterfaceRequestor *aCTX,
                                                nsIX509Cert *aCert,
                                                PRUint32 *aTrust,
                                                PRBool *aCancelled)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedSSLDialog::DownloadCACert()\n");
#endif

  return NS_OK;
}

NS_IMETHODIMP gtkEmbedSSLDialog::SetPKCS12FilePassword(nsIInterfaceRequestor *aCTX,
                                                       PRUnichar **aPassword,
                                                       PRBool *aCancelled)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedSSLDialog::SetPKCS12FilePassword\n");
#endif

  return NS_OK;
}

NS_IMETHODIMP gtkEmbedSSLDialog::GetPKCS12FilePassword(nsIInterfaceRequestor *aCTX,
                                                       PRUnichar **aPassword,
                                                       PRBool *aCancelled)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedSSLDialog::GetPKCS12FilePassword()\n");
#endif

  return NS_OK;
}

NS_IMETHODIMP gtkEmbedSSLDialog::ViewCert(nsIX509Cert *aCert)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedSSLDialog::ViewCert()\n");
#endif

  return NS_OK;
}

// nsIClientAuthDialogs

NS_IMETHODIMP gtkEmbedSSLDialog::ChooseCertificate(nsIInterfaceRequestor *CTX,
                                                   const PRUnichar *aCN,
                                                   const PRUnichar *aOrg,
                                                   const PRUnichar *aIssuer,
                                                   const PRUnichar **aNickList,
                                                   PRUint32 aCount,
                                                   PRUnichar **aNick,
                                                   PRBool *aCancelled)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedSSLDialog::ChooseCertificate()\n");
#endif

  return NS_OK;
}

// nsITokenDialogs

NS_IMETHODIMP gtkEmbedSSLDialog::ChooseToken(nsIInterfaceRequestor *aCTX,
                                              const PRUnichar **aTokenList,
                                              PRUint32 aCount,
                                              PRUnichar **aTokenChosen,
                                              PRBool *aCancelled)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedSSLDialog::ChooseToken()\n");
#endif

  return NS_OK;
}

// nsIDOMCryptoDialogs

NS_IMETHODIMP gtkEmbedSSLDialog::ConfirmKeyEscrow(nsIX509Cert *aEscrowAuth, 
                                                  PRBool *aRetval)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedSSLDialog::ConfirmKeyEscrow()\n");
#endif

  // just return true for now
  *aRetval = true;

  return NS_OK;
}

NS_DEF_FACTORY(NSSDialogs, gtkEmbedSSLDialog);

nsresult newSSLDialogFactory(nsIFactory **aResult)
{
  nsresult tempRetval = NS_OK;
  nsIFactory *tempInst = new nsNSSDialogsFactory;

  if (tempInst == nsnull)
    tempRetval = NS_ERROR_OUT_OF_MEMORY;
  else
    NS_ADDREF(tempInst);

  *aResult = tempInst;

  return tempRetval;
} 
