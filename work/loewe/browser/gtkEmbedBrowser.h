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
#include <gdk/gdk.h>
#include "nsCOMPtr.h"
#include "nsIInterfaceRequestor.h"
#include "nsICacheService.h"
#include "nsNetCID.h"
#include "nsIWebBrowser.h"
#include "nsIWebBrowserFind.h"
#include "nsIWebBrowserSetup.h"
#include "gtkmozembed.h"
#include "gtkmozembed_internal.h"

typedef struct _gtkEmbedBrowser {
  int           browserID;
  bool          usedFlag;
  bool          loadingFlag;
  nsIWebBrowser *webBrowser;
  GtkWidget     *topLevelWindow;
  GtkWidget     *topLevelVBox;
  GtkWidget     *mozEmbed;
} gtkEmbedBrowser;

typedef struct _gtkEmbedGeometry {
  int x;
  int y;
  int width;
  int height;
} gtkEmbedGeometry;

#endif
