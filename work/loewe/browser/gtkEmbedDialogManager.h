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

#ifndef __GTKEMBEDDIALOGMANAGER_H
#define __GTKEMBEDDIALOGMANAGER_H

#include "gtkEmbedPromptDialog.h"
#include "gtkEmbedSSLDialog.h"
#include "gtkEmbedHelperAppDialog.h"
#include "gtkEmbedParamList.h"
#include "gtkEmbedConst.h"

#include "nsIWebBrowser.h"
#include "nsIDOMWindow.h"

class gtkEmbedDialogManager
{
  public :

    gtkEmbedDialogManager();
    ~gtkEmbedDialogManager();

    bool Init(bool (*aOpenDialog)(nsIDOMWindow *,
                                  int,
                                  int,
                                  const char *,
                                  gtkEmbedParamList *,
                                  int *,
                                  bool *));

  private :

    gtkEmbedPromptDialog    *gPromptDialog;
    gtkEmbedSSLDialog       *gSSLDialog;
    gtkEmbedHelperAppDialog *gHelperAppDialog;

    bool (*gOpenDialog)(nsIDOMWindow *,
                        int,
                        int,
                        const char *,
                        gtkEmbedParamList *,
                        int *,
                        bool *);
} ;

#endif
