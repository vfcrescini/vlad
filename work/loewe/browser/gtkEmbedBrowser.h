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

#ifndef __GTKEMBEDBROWSER_H
#define __GTKEMBEDBROWSER_H

#include <gtk/gtk.h>
#include "nsCOMPtr.h"
#include "nsIInterfaceRequestor.h"
#include "nsICacheService.h"
#include "nsNetCID.h"
#include "nsIWebBrowser.h"
#include "nsIWebBrowserFind.h"
#include "gtkmozembed.h"
#include "gtkmozembed_internal.h"

typedef struct _gtkEmbedBrowser {
  int           browserID;
  bool          usedFlag;
  bool          loadingFlag;
  bool          blockedFlag;
  char          **replyArray;  // this really shouldn't be here
  nsIWebBrowser *webBrowser;
  GtkWidget     *topLevelWindow;
  GtkWidget     *topLevelVBox;
  GtkWidget     *mozEmbed;
} gtkEmbedBrowser;

// error codes

#define BROWSER_ERROR_GENERAL             1
#define BROWSER_ERROR_VIEW_NOT_AVAILABLE  2
#define BROWSER_ERROR_INVALID_MESSAGE     3
#define BROWSER_ERROR_INVALID_PARAMS      4
#define BROWSER_ERROR_INVALID_FONTSIZE    5
#define BROWSER_ERROR_INVALID_VIEW_LIST   6
#define BROWSER_ERROR_CANT_GO_BACK        7
#define BROWSER_ERROR_CANT_GO_FORWARD     8
#define BROWSER_ERROR_CANT_RELOAD         9
#define BROWSER_ERROR_CANT_STOP          10
#define BROWSER_ERROR_INVALID_FINDMODE   11

#endif
