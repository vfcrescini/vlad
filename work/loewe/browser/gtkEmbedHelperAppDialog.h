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

#ifndef __GTKEMBEDHELPERAPPDIALOG_H
#define __GTKEMBEDHELPERAPPDIALOG_H

#include "nsIFactory.h"
#include "nsIHelperAppLauncherDialog.h"

extern nsresult newHelperAppDialogFactory(nsIFactory **aResult);

// structure to hold helper apps registered in prefs
class gtkEmbedHelperAppNode
{
  public :

    gtkEmbedHelperAppNode();
    ~gtkEmbedHelperAppNode();

    bool Get(char **, char **);
    bool Set(char *, char *);

    gtkEmbedHelperAppNode *gNext;

  private :

    char *gMime;
    char *gApplication;
} ;

class gtkEmbedHelperAppList
{
  public :

    gtkEmbedHelperAppList();
    ~gtkEmbedHelperAppList();

    bool Push(char *, char *);
    bool Pop(char **, char **);
    bool Get(char *, char **);

  private :  

    gtkEmbedHelperAppNode *gHead;
} ;

// class to implement (replace) the helper app dialog
class gtkEmbedHelperAppDialog : public nsIHelperAppLauncherDialog
{
  public :

    gtkEmbedHelperAppDialog();
    virtual ~gtkEmbedHelperAppDialog();

    bool Init();

    NS_DECL_ISUPPORTS
    NS_DECL_NSIHELPERAPPLAUNCHERDIALOG
} ;

// CID of the component we want to override
#define NS_HELPERAPPLAUNCHERDIALOG_CID \
  {0xf68578eb, 0x6ec2, 0x4169, {0xae, 0x19, 0x8c, 0x62, 0x43, 0xf0, 0xab, 0xe1}}

#endif
