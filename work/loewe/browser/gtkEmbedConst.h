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

#ifndef __GTKEMBEDCONST_H
#define __GTKEMBEDCONST_H

// default url
#define BROWSER_STARTUP_DEFAULTURL        "about:blank"

// constants used by the socket listener
#define BROWSER_LISTENER_MAXPARAMS        16
#define BROWSER_LISTENER_MAXLEN           1024
#define BROWSER_LISTENER_DELIMITER        '\004'
#define BROWSER_LISTENER_TERMINATOR       '\000'

// constants used by the window manager
#define BROWSER_WINDOW_MAX                64
#define BROWSER_WINDOW_DEFAULT            2
#define BROWSER_WINDOW_WIDTH              500
#define BROWSER_WINDOW_HEIGHT             500
#define BROWSER_WINDOW_X                  0
#define BROWSER_WINDOW_Y                  0

#define BROWSER_WM_CLASSNAME              "L3WM_ID"
#define BROWSER_WM_PREFIX                 "Browser-View"

// default font sizes
#define BROWSER_FONT_DEFAULTSIZE          16
#define BROWSER_FONT_MINSIZE              8
#define BROWSER_FONT_MAXSIZE              72

// find modes
#define BROWSER_FIND_CASESENSITIVE        1
#define BROWSER_FIND_WRAPAROUND           2
#define BROWSER_FIND_BACKWARDS            4
#define BROWSER_FIND_MAX                  4

// profile constants
#define BROWSER_PROF_NAME                 "default"
#define BROWSER_PROF_PATH                 "gtkEmbedBrowser"

// constants used by the dialog manager
#define DIALOG_INTERNAL_MESSAGE           "__GTK_EMBED_INTERNAL_MESSAGE"
#define DIALOG_SSL_ENTER_SECURE           "__GTK_EMBED_INTERNAL_SSL_ENTER_SECURE"
#define DIALOG_SSL_ENTER_WEAK             "__GTK_EMBED_INTERNAL_SSL_ENTER_WEAK"
#define DIALOG_SSL_LEAVE_SECURE           "__GTK_EMBED_INTERNAL_SSL_LEAVE_SECURE"

// constants used by the id manager
#define IDMANAGER_MAX_DIGIT               3
#define IDMANAGER_MAX_ID                  BROWSER_WINDOW_MAX
#define IDMANAGER_DELIMITER               ','

// constants used by the socket listener
#define SOCK_LISTENER_MAX(x,y)            ((x)>(y)?(x):(y))
#define SOCK_LISTENER_MAXLEN              BROWSER_LISTENER_MAXLEN

// constants used by the helper app
#define HELPERAPP_PREF_HANDLERMAX         255
#define HELPERAPP_PREF_MAXLEN             1024
#define HELPERAPP_PREF_MIMEPREFIX         "browser.helperapp.mime"
#define HELPERAPP_PREF_HANDLERPREFIX      "browser.helperapp.handler"

// error codes sent by the browser
#define BROWSER_ERROR_GENERAL             1
#define BROWSER_ERROR_VIEW_NOT_AVAILABLE  2
#define BROWSER_ERROR_INVALID_MESSAGE     3
#define BROWSER_ERROR_INVALID_PARAMS      4
#define BROWSER_ERROR_INVALID_FONTSIZE    5
#define BROWSER_ERROR_INVALID_VIEW_LIST   6
#define BROWSER_ERROR_INVALID_FINDMODE    7
#define BROWSER_ERROR_CANT_GO_BACK        8
#define BROWSER_ERROR_CANT_GO_FORWARD     9
#define BROWSER_ERROR_CANT_RELOAD         10
#define BROWSER_ERROR_CANT_STOP           11
#define BROWSER_ERROR_CANT_PRINT          12

#endif
