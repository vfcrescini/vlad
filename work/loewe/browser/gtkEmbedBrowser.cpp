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

/*
 * based upon Chris Blizzard's TestGtkEmbed
 * 26 July 2001
 */

#include <string.h>
#include <unistd.h>

#include "gtkEmbedIDManager.h"
#include "gtkEmbedBrowser.h"
#include "gtkEmbedSocketListener.h"
#include "gtkEmbedPref.h"
#include "gtkEmbedDialogManager.h"
#include "gtkEmbedPrintManager.h"
#include "gtkEmbedConst.h"

// browser id list
static gtkEmbedIDManager *gIDManager = NULL;

// array index of the currently visible view
static int gCurrentIndex = 0;

// socket listener
static gtkEmbedSocketListener *gSocketListener = NULL;

// browser array
static gtkEmbedBrowser **gBrowserArray = NULL;

// max number of windows
static int gMaxWin = 0;

// message buffer
static char gMessageBuffer[BROWSER_LISTENER_MAXLEN] = "";
static char *gMessagePtr = NULL;

// pref service
static gtkEmbedPref *gPrefManager = NULL;

// dialog manager
static gtkEmbedDialogManager *gDialogManager = NULL;

// print manager
static gtkEmbedPrintManager *gPrintManager = NULL;

// message pipes to the socket listener
static int gIncomingFD = -1;
static int gOutgoingFD = -1;

// callback function for incoming messages from socket
static void incomingMessageCB(gpointer, gint, GdkInputCondition);

// callback functions from the embedded widget
static void changeLocationCB(GtkMozEmbed *, gtkEmbedBrowser *);
static void changeTitleCB(GtkMozEmbed *, gtkEmbedBrowser *);
static void loadStartCB(GtkMozEmbed *, gtkEmbedBrowser *);
static void loadEndCB(GtkMozEmbed *, gtkEmbedBrowser *);
static void changeNetStateCB(GtkMozEmbed *, gint, guint, gtkEmbedBrowser *);
static void changeNetStateAllCB(GtkMozEmbed *, const char *, gint, guint, gtkEmbedBrowser *);
static void changeProgressCB(GtkMozEmbed *, gint, gint, gtkEmbedBrowser *);
static void changeProgressAllCB(GtkMozEmbed *, const char *, gint, guint, gtkEmbedBrowser *);
static void messageLinkCB(GtkMozEmbed *, gtkEmbedBrowser *);
static void changeStatusJSCB(GtkMozEmbed *, gtkEmbedBrowser *);
static void newWindowCB(GtkMozEmbed *, GtkMozEmbed **, guint, gtkEmbedBrowser *);
static void visibilityCB(GtkMozEmbed *, gboolean, gtkEmbedBrowser *);
static void destroyBrowserCB(GtkMozEmbed *, gtkEmbedBrowser *);
static gint openURICB(GtkMozEmbed *, const char *, gtkEmbedBrowser *);
static void changeSizeCB(GtkMozEmbed *, gint, gint, gtkEmbedBrowser *);
static void destroyCB(GtkWidget *, gtkEmbedBrowser *);
static gboolean deleteCB(GtkWidget *, GdkEventAny *, gtkEmbedBrowser *);

// incoming message functions
static void stopRequest(gtkEmbedBrowser *);
static void reloadRequest(gtkEmbedBrowser *);
static void forwardRequest(gtkEmbedBrowser *);
static void backRequest(gtkEmbedBrowser *);
static void pingRequest();
static void shutdownRequest();
static void openURLRequest(char *, int, char *);
static void toggleViewRequest(int);
static void availableViewsRequest(char *);
static void getURLRequest(int);
static void setGeometryRequest(int, int, int, int);
static void enableJScriptRequest(int);
static void enableJavaRequest(int);
static void fontSizeRequest(char *);
static void enableCookiesRequest(int);
static void findRequest(char *, int);
static void purgeCacheRequest();
static void printRequest(int, bool);
static void enablePluginsRequest(int);

/*
static void requestCertificateRequest(int);
static void cleanupRequest();
static void memUsageRequest();
*/

// outgoing message functions
static void pongSend();
static void statusSend(int, int, const char *);
static void loadStartSend(int, const char *);
static void loadEndSend(int, const char *);
static void loadProgressSend(int, int);
static void toggleViewSend(int);
static void getURLSend(int, const char *);
static void errorMessageSend(int, int);
static void enableForwardSend(int, bool);
static void enableBackSend(int, bool);
static void enableReloadSend(int, bool);
static void enableStopSend(int, bool);
static void setLocationBarURLSend(int, const char *);
static void setTitleSend(int, const char *);
static void windowOpenedSend(int);
static void findReplySend(const char *, bool);
static void sslActiveSend(int, int);

/*
static void yesNoDialogSend(int, int, int);
static void showCertificateSend(char *);
static void memUsageReplySend(char *);
*/

// dialog callback functions
static bool sslActivateCB(nsIDOMWindow *, bool);
static bool alertCB(nsIDOMWindow *, const char *);

// utility functions
static int getWindowID(nsIDOMWindow *aWindow);
static void readMessage();
static bool isIDValid(int);
static int getNumberOfUsedWindows();
static int getNextUsedWindow();
static void destroyAll();
static void sendMessage(const char *);
static bool getGeometryFromPrefs(gtkEmbedGeometry *);
static bool getPluginStateFromPrefs();
static void initialiseBrowserArray(bool);
static void setVisible(gtkEmbedBrowser *, bool);
static void setGeometry(gtkEmbedBrowser *, gtkEmbedGeometry);
static void setGeometry(gtkEmbedBrowser *, int, int, int, int);
static bool setPluginFlag(gtkEmbedBrowser *, bool);
static gtkEmbedBrowser *createNewBrowser(guint32, 
                                         guint32, 
                                         bool, 
                                         bool,
                                         gtkEmbedGeometry);
static int parseMessage(char *,
                        char [BROWSER_LISTENER_MAXPARAMS][BROWSER_LISTENER_MAXLEN]);

// main
int main(int argc, char **argv)
{
  GtkWidget *dummy     = NULL;
  char *homePath       = NULL;
  char *profilePath    = NULL;
  char *socketPath     = NULL;
  bool revertStateFlag = false;
  int tempOption;

  gMaxWin = BROWSER_WINDOW_DEFAULT;

  gtk_set_locale();
  gtk_init(&argc, &argv);

  // command line option stuff
  opterr = 0;
  
  if (getopt(argc, argv, "u:") == 'u' && optarg)
    socketPath = g_strdup(optarg);
  else {
    fprintf(stderr, 
            "usage %s -u <socket path> [-s] [-n <window num>] [url]\n", 
            argv[0]);
    return -1;
  }

  // get all the options
  while ((tempOption = getopt(argc, argv, "n:s")) != -1) {
    switch(tempOption) {
      case 's' :
        // revert to previous state
        revertStateFlag = true;
        break;
      case 'n' :
        // number of windows
        if (optarg) {
          if (atoi(optarg) > 0 && atoi(optarg) < BROWSER_WINDOW_MAX) {
            gMaxWin = atoi(optarg);
            break;
          }
          else {
            fprintf(stderr, 
                    "<window num> must be an integer between 0 and %d\n",
                    BROWSER_WINDOW_MAX);
            return -1;
          }
        }
      default :
        fprintf(stderr, 
                "usage %s -u <socket path> [-s] [-n <window num>] [url]\n", 
                argv[0]);
        return -1;
    }
  }

  // profile stuff
  homePath = g_getenv("HOME");

  if (!homePath) {
    fprintf(stderr, "failed to get HOME\n");
    return -1;
  }

  profilePath = g_strdup_printf("%s/.%s", homePath, BROWSER_PROF_PATH);
  gtk_moz_embed_set_profile_path(profilePath, BROWSER_PROF_NAME);

  // create a dummy one to initialise everything
  dummy = gtk_moz_embed_new();
 
  // pref stuff
  gPrefManager = new gtkEmbedPref();

  if (!gPrefManager || !gPrefManager->Init())
    return -1;
  
  // id manager stuff
  gIDManager = new gtkEmbedIDManager(gMaxWin);

  if (!gIDManager)
    return -1;

  // print manager stuff
  gPrintManager = new gtkEmbedPrintManager();

  if (!gPrintManager || !gPrintManager->Init())
    return -1;

  // dialog stuff
  gDialogManager = new gtkEmbedDialogManager();

  if (!gDialogManager || !gDialogManager->Init(&sslActivateCB, &alertCB, NULL, NULL, NULL, NULL, NULL))
    return -1;
  
  // message buffer stuff
  gMessagePtr = gMessageBuffer;
  memset(gMessageBuffer, 0, BROWSER_LISTENER_MAXLEN);

  // socket stuff
  gSocketListener = new gtkEmbedSocketListener();

  if (!gSocketListener)
    return -1;
 
  gSocketListener->Init(socketPath);
  gIncomingFD = gSocketListener->GetIncomingFD();
  gOutgoingFD = gSocketListener->GetOutgoingFD();
  gSocketListener->Start();

  gdk_input_add(gIncomingFD, GDK_INPUT_READ, incomingMessageCB, nsnull);

  // browser stuff
  initialiseBrowserArray(revertStateFlag);

  // open the default window
  gCurrentIndex = 0;
  setVisible(gBrowserArray[0], true);

  // load startup url, if any
  if (argv[optind])
    openURLRequest(argv[optind], 1, NULL);  
  else if (revertStateFlag && gPrefManager && gPrefManager->GetCharPref("browser.history.last_page_visited"))
    openURLRequest(gPrefManager->GetCharPref("browser.history.last_page_visited"), 1, NULL);
  else
    openURLRequest(BROWSER_STARTUP_DEFAULTURL, 1, NULL);

  // main event loop
  gtk_main();

  destroyAll();

  // cleanup
  if (homePath)
    g_free(homePath);
  if (profilePath)
    g_free(profilePath);
  if (socketPath)
    g_free(socketPath);
  if (dummy)
    g_free(dummy);

  return 0;
}

// callback function from socket

static void incomingMessageCB(gpointer aData,
                              gint aFD,
                              GdkInputCondition aCondition)
{
  char tempChar;

  // read one character and store in the buffer
  if (read(aFD, &tempChar, 1) <= 0)
    return;

  // check if we still have space
  if (strlen(gMessagePtr) >= BROWSER_LISTENER_MAXLEN)
    return;

  *gMessagePtr++ = tempChar;

  // if we get the null|teminator char, we have a complete message
  if (tempChar == BROWSER_LISTENER_TERMINATOR ||
      tempChar == '\0') {
    readMessage();

    // after reading, clear the buffer
    gMessagePtr = gMessageBuffer;
    memset(gMessageBuffer, 0,  BROWSER_LISTENER_MAXLEN);
  }
}

static void readMessage()
{
  char tempArray[BROWSER_LISTENER_MAXPARAMS][BROWSER_LISTENER_MAXLEN];
  int paramCount = 0;
  
  paramCount = parseMessage(gMessageBuffer, tempArray);

  if (paramCount <= 0) {
    errorMessageSend(-1, BROWSER_ERROR_INVALID_MESSAGE);
    return;
  }

  if (!strcmp(tempArray[0], "toggleView")) {
    if (paramCount == 2) {
      if (!isIDValid(atoi(tempArray[1])))
        errorMessageSend(-1, BROWSER_ERROR_VIEW_NOT_AVAILABLE);
      else
        toggleViewRequest(atoi(tempArray[1]));
    }
    else if (paramCount == 1)
      toggleViewRequest(0);
    else
      errorMessageSend(-1, BROWSER_ERROR_INVALID_PARAMS);
  }
  else if (!strcmp(tempArray[0], "availableViews")) {
    if (paramCount == 2)
      availableViewsRequest(tempArray[1]);
    else
      errorMessageSend(-1, BROWSER_ERROR_INVALID_PARAMS);
  }
  else if (!strcmp(tempArray[0], "openURL")) { 
    if (paramCount == 4) {
      if (!isIDValid(atoi(tempArray[2])) && atoi(tempArray[2]) != 0)
        errorMessageSend(-1, BROWSER_ERROR_VIEW_NOT_AVAILABLE);
      else
        openURLRequest(tempArray[1], atoi(tempArray[2]), tempArray[3]);
    }
    else if (paramCount == 3) {
      if (!isIDValid(atoi(tempArray[2])) && atoi(tempArray[2]) != 0)
        errorMessageSend(-1, BROWSER_ERROR_VIEW_NOT_AVAILABLE);
      else
        openURLRequest(tempArray[1], atoi(tempArray[2]), "");
    }
    else if (paramCount == 2)
      openURLRequest(tempArray[1], 0, "");
    else
      errorMessageSend(-1, BROWSER_ERROR_INVALID_PARAMS);
  }
  else if (!strcmp(tempArray[0], "stop"))
    stopRequest(gBrowserArray[gCurrentIndex]);
  else if (!strcmp(tempArray[0], "reload"))
    reloadRequest(gBrowserArray[gCurrentIndex]);
  else if (!strcmp(tempArray[0], "back"))
    backRequest(gBrowserArray[gCurrentIndex]);
  else if (!strcmp(tempArray[0], "forward"))
    forwardRequest(gBrowserArray[gCurrentIndex]);
  else if (!strcmp(tempArray[0], "getURL")) {
    if (paramCount == 1)
      getURLRequest(gCurrentIndex + 1);
    else if (paramCount == 2) {
      if (!isIDValid(atoi(tempArray[1])))
        errorMessageSend(-1, BROWSER_ERROR_VIEW_NOT_AVAILABLE);
      else
        getURLRequest(atoi(tempArray[1]));
    }
    else
      errorMessageSend(-1, BROWSER_ERROR_INVALID_PARAMS);
  }
  else if (!strcmp(tempArray[0], "ping"))
    pingRequest();
  else if (!strcmp(tempArray[0], "shutdown"))
    shutdownRequest();
  else if (!strcmp(tempArray[0], "setGeometry")) {
    if (paramCount == 5 &&
        atoi(tempArray[1]) >= 0 && 
        atoi(tempArray[2]) >= 0 &&
        atoi(tempArray[3]) > 0 &&
        atoi(tempArray[4]) > 0) {
       
      setGeometryRequest(atoi(tempArray[1]), 
                         atoi(tempArray[2]),
                         atoi(tempArray[3]),
                         atoi(tempArray[4]));
    }
    else
      errorMessageSend(-1, BROWSER_ERROR_INVALID_PARAMS);
  }
  else if (!strcmp(tempArray[0], "enableJScript")) {
    if (paramCount == 2 && (atoi(tempArray[1]) == 1 || atoi(tempArray[1]) == 0))
      enableJScriptRequest(atoi(tempArray[1]) == 1);
    else
      errorMessageSend(-1, BROWSER_ERROR_INVALID_PARAMS);
  }
  else if (!strcmp(tempArray[0], "enablePlugins")) {
    if (paramCount == 2 && (atoi(tempArray[1]) == 1 || atoi(tempArray[1]) == 0))
      enablePluginsRequest(atoi(tempArray[1]) == 1);
    else
      errorMessageSend(-1, BROWSER_ERROR_INVALID_PARAMS);
  }
  else if (!strcmp(tempArray[0], "enableJava")) {
    if (paramCount == 2 && (atoi(tempArray[1]) == 1 || atoi(tempArray[1]) == 0))
      enableJavaRequest(atoi(tempArray[1]) == 1);
    else
      errorMessageSend(-1, BROWSER_ERROR_INVALID_PARAMS);
  }
  else if (!strcmp(tempArray[0], "enableCookies")) {
    if (paramCount == 2 && (atoi(tempArray[1]) == 1 || atoi(tempArray[1]) == 0))
      enableCookiesRequest(atoi(tempArray[1]) == 1);
    else
      errorMessageSend(-1, BROWSER_ERROR_INVALID_PARAMS);
  }
  else if (!strcmp(tempArray[0], "changeFontSize")) {
    if (paramCount == 2)
      fontSizeRequest(tempArray[1]);
    else
      errorMessageSend(-1, BROWSER_ERROR_INVALID_PARAMS);
  }
  else if (!strcmp(tempArray[0], "find")) {
    if (paramCount == 3)
      findRequest(tempArray[1], atoi(tempArray[2]));
    else
      errorMessageSend(-1, BROWSER_ERROR_INVALID_PARAMS);
  }
  else if (!strcmp(tempArray[0], "purgeCache")) {
    purgeCacheRequest();
  }
  else if (!strcmp(tempArray[0], "print")) {
    if (paramCount == 3) {
      if (!isIDValid(atoi(tempArray[1])))
        errorMessageSend(-1, BROWSER_ERROR_VIEW_NOT_AVAILABLE);
      else if (atoi(tempArray[2]) != 1 && atoi(tempArray[2]) != 2)
        errorMessageSend(-1, BROWSER_ERROR_INVALID_PARAMS);
      else
        printRequest(atoi(tempArray[1]), atoi(tempArray[2]) == 1);
    }
    else
      errorMessageSend(-1, BROWSER_ERROR_INVALID_PARAMS);
  }
  else
    errorMessageSend(-1, BROWSER_ERROR_INVALID_MESSAGE);
}

// callback functions from the embedded widget

gboolean deleteCB(GtkWidget *aWidget, GdkEventAny *aEvent, gtkEmbedBrowser *aBrowser)
{
  gtk_widget_destroy(aWidget);
  return true;
}

void changeLocationCB(GtkMozEmbed *aEmbed, gtkEmbedBrowser *aBrowser)
{
  if (!aEmbed || !aBrowser) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  setLocationBarURLSend(aBrowser->browserID, 
                        gtk_moz_embed_get_location(GTK_MOZ_EMBED(aEmbed)));
  enableForwardSend(aBrowser->browserID, 
                    gtk_moz_embed_can_go_forward(GTK_MOZ_EMBED(aEmbed)));
  enableBackSend(aBrowser->browserID, 
                 gtk_moz_embed_can_go_back(GTK_MOZ_EMBED(aEmbed)));
}

void changeTitleCB(GtkMozEmbed *aEmbed, gtkEmbedBrowser *aBrowser)
{
  if (!aEmbed || !aBrowser) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  setTitleSend(aBrowser->browserID, 
               gtk_moz_embed_get_title(GTK_MOZ_EMBED(aEmbed)));
}

void loadStartCB(GtkMozEmbed *aEmbed, gtkEmbedBrowser *aBrowser)
{
  if (!aEmbed || !aBrowser) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  loadStartSend(aBrowser->browserID, gtk_moz_embed_get_location(aEmbed));
  loadProgressSend(aBrowser->browserID, 0); 
  enableReloadSend(aBrowser->browserID, false);
  enableStopSend(aBrowser->browserID, true);

  // set load flag
  aBrowser->loadingFlag = true;
}

void loadEndCB(GtkMozEmbed *aEmbed, gtkEmbedBrowser *aBrowser)
{
  if (!aEmbed || !aBrowser) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  loadEndSend(aBrowser->browserID, gtk_moz_embed_get_location(aEmbed));
  loadProgressSend(aBrowser->browserID, 0); 
  enableReloadSend(aBrowser->browserID, true);
  enableStopSend(aBrowser->browserID, false);

  // set load flag
  aBrowser->loadingFlag = false;

  // save this url into the prefs as the last page visited
  if (!gPrefManager) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  gPrefManager->SetCharPref("browser.history.last_page_visited",
                            gtk_moz_embed_get_location(aEmbed));
}

void changeNetStateCB(GtkMozEmbed *aEmbed, 
                      gint aFlags, 
                      guint aStatus, 
                      gtkEmbedBrowser *aBrowser)
{
}

void changeNetStateAllCB(GtkMozEmbed *aEmbed, 
                         const char *aURI, 
                         gint aFlags, 
                         guint aStatus, 
                         gtkEmbedBrowser *aBrowser)
{
  if (!aEmbed || !aBrowser) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  if (aFlags & GTK_MOZ_EMBED_FLAG_IS_REQUEST) {
    if (aFlags & GTK_MOZ_EMBED_FLAG_REDIRECTING)
      statusSend(aBrowser->browserID, 0, "Redirecting to site...");
    else if (aFlags & GTK_MOZ_EMBED_FLAG_TRANSFERRING)
      statusSend(aBrowser->browserID, 0, "Transferring data from site...");
    else if (aFlags & GTK_MOZ_EMBED_FLAG_NEGOTIATING)
      statusSend(aBrowser->browserID, 0, "Waiting for authorisation...");
  }

  if (aStatus == GTK_MOZ_EMBED_STATUS_FAILED_DNS)
    statusSend(aBrowser->browserID, 0, "Site not found.");
  else if (aStatus == GTK_MOZ_EMBED_STATUS_FAILED_CONNECT)
    statusSend(aBrowser->browserID, 0, "Connection failed.");
  else if (aStatus == GTK_MOZ_EMBED_STATUS_FAILED_TIMEOUT)
    statusSend(aBrowser->browserID, 0, "Connection timed out.");
  else if (aStatus == GTK_MOZ_EMBED_STATUS_FAILED_USERCANCELED)
    statusSend(aBrowser->browserID, 0, "Connection cancelled.");

  if (aFlags & GTK_MOZ_EMBED_FLAG_IS_DOCUMENT) {
    if (aFlags & GTK_MOZ_EMBED_FLAG_START)
      statusSend(aBrowser->browserID, 0, "Loading site...");
    else if (aFlags & GTK_MOZ_EMBED_FLAG_STOP)
      statusSend(aBrowser->browserID, 0, "Done.");
  }
}

void changeProgressCB(GtkMozEmbed *aEmbed, 
                      gint aCurr, 
                      gint aMax, 
                      gtkEmbedBrowser *aBrowser)
{
  int tempPercentage;
  char *tempStatus = NULL;

  if (!aEmbed || !aBrowser) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  if (aMax < 1) {
    tempPercentage = 0;
    tempStatus = g_strdup_printf("%d bytes loaded", aCurr);
  }
  else {
    tempPercentage = (aCurr > aMax) ? 100 : (aCurr * 100) / aMax;
    tempStatus = g_strdup_printf("%d%% complete, %d bytes of %d loaded", 
                                 tempPercentage,
                                 aCurr, 
                                 aMax);
  }

  loadProgressSend(aBrowser->browserID, tempPercentage);
  statusSend(aBrowser->browserID, 0, tempStatus);
}

void changeProgressAllCB(GtkMozEmbed *aEmbed, 
                         const char *aURI, 
                         gint aCurr, 
                         guint aMax, 
                         gtkEmbedBrowser *aBrowser)
{
}

void messageLinkCB(GtkMozEmbed *aEmbed, gtkEmbedBrowser *aBrowser)
{
  if (!aEmbed || !aBrowser) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  statusSend(aBrowser->browserID, 0, gtk_moz_embed_get_link_message(aEmbed));
}

void changeStatusJSCB(GtkMozEmbed *aEmbed, gtkEmbedBrowser *aBrowser)
{
  if (!aEmbed || !aBrowser) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  statusSend(aBrowser->browserID, 0, gtk_moz_embed_get_js_status(aEmbed));
}

void newWindowCB(GtkMozEmbed *aEmbed, 
                 GtkMozEmbed **aRetval, 
                 guint aChromeMask, 
                 gtkEmbedBrowser *aBrowser)
{
  int tempIndex;

  if (!gIDManager || !gBrowserArray) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  // if the new window is a dialog, reject request
  if ((aChromeMask & GTK_MOZ_EMBED_FLAG_OPENASDIALOG) == 
      GTK_MOZ_EMBED_FLAG_OPENASDIALOG)
    return;

  // get the index of the new window to be opened
  tempIndex = gIDManager->GetNextIndex();

  // give a reference to the embeded widget back
  *aRetval = GTK_MOZ_EMBED(gBrowserArray[tempIndex]->mozEmbed);

  // send a windowOpened message regarless of whether the window
  // was already open or not
  gBrowserArray[tempIndex]->usedFlag = true;
  windowOpenedSend(tempIndex + 1);
}

void visibilityCB(GtkMozEmbed *aEmbed, 
                  gboolean aVisible, 
                  gtkEmbedBrowser *aBrowser)
{
  if (!aEmbed || !aBrowser) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  // only allow the current window to be visible
  if (aBrowser->browserID != gCurrentIndex + 1)
    return;

  setVisible(aBrowser, aVisible);
}

void destroyBrowserCB(GtkMozEmbed *aEmbed, gtkEmbedBrowser *aBrowser)
{
  if (!aBrowser) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  gtk_widget_destroy(aBrowser->topLevelWindow);
}

gint openURICB(GtkMozEmbed *aEmbed, 
               const char *aURI, 
               gtkEmbedBrowser *aBrowser)
{
  return 0;
}

void changeSizeCB(GtkMozEmbed *aEmbed, 
                  gint aWidth, 
                  gint aHeight,
                  gtkEmbedBrowser *aBrowser)
{
}

void destroyCB(GtkWidget *aWidget, gtkEmbedBrowser *aBrowser)
{
  int tempID;
  gtkEmbedGeometry tempGeometry;

  if (!aWidget || !aBrowser) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  // get the id of this browser
  tempID = aBrowser->browserID;

  // free up the memory used by this browser
  g_free(gBrowserArray[tempID - 1]);

  // get the default or preferred geometry
  getGeometryFromPrefs(&tempGeometry);

  // create a new unused browser
  gBrowserArray[tempID - 1] = createNewBrowser(GTK_MOZ_EMBED_FLAG_DEFAULTCHROME, 
                                               tempID,
                                               false,
                                               getPluginStateFromPrefs(),
                                               tempGeometry);

  // if this was the only opened window, we can shutdown
  if (getNumberOfUsedWindows() <= 0) {
    gtk_main_quit();
    return;
  }
}

// outgoing messages

static void errorMessageSend(int aID, int aErrorCode)
{
  char *tempString;
  
  tempString = g_strdup_printf("error%c%d%c%d%c",
                               BROWSER_LISTENER_DELIMITER,
                               aID,
                               BROWSER_LISTENER_DELIMITER,
                               aErrorCode,
                               BROWSER_LISTENER_TERMINATOR);
  sendMessage(tempString); 
  g_free(tempString);
}

static void windowOpenedSend(int aID)
{
  char *tempString;

  tempString = g_strdup_printf("windowOpened%c%d%c",
                               BROWSER_LISTENER_DELIMITER,
                               aID,
                               BROWSER_LISTENER_TERMINATOR);

  sendMessage(tempString);
  g_free(tempString);
}

static void enableForwardSend(int aID, bool aFlag)
{
  char *tempString;

  tempString = g_strdup_printf("enableForward%c%d%c%d%c",
                               BROWSER_LISTENER_DELIMITER,
                               aID,
                               BROWSER_LISTENER_DELIMITER,
                               (aFlag) ? 1 : 2,
                               BROWSER_LISTENER_TERMINATOR);

  sendMessage(tempString);
  g_free(tempString);
}

static void enableBackSend(int aID, bool aFlag)
{
  char *tempString;

  tempString = g_strdup_printf("enableBack%c%d%c%d%c",
                               BROWSER_LISTENER_DELIMITER,
                               aID,
                               BROWSER_LISTENER_DELIMITER,
                               (aFlag) ? 1 : 2,
                               BROWSER_LISTENER_TERMINATOR);

  sendMessage(tempString);
  g_free(tempString);
}

static void enableReloadSend(int aID, bool aFlag)
{
  char *tempString;

  tempString = g_strdup_printf("enableReload%c%d%c%d%c",
                               BROWSER_LISTENER_DELIMITER,
                               aID,
                               BROWSER_LISTENER_DELIMITER,
                               (aFlag) ? 1 : 2,
                               BROWSER_LISTENER_TERMINATOR);

  sendMessage(tempString);
  g_free(tempString);
}

static void enableStopSend(int aID, bool aFlag)
{
  char *tempString;

  tempString = g_strdup_printf("enableStop%c%d%c%d%c",
                               BROWSER_LISTENER_DELIMITER,
                               aID,
                               BROWSER_LISTENER_DELIMITER,
                               (aFlag) ? 1 : 2,
                               BROWSER_LISTENER_TERMINATOR);

  sendMessage(tempString);
  g_free(tempString);
}

static void setLocationBarURLSend(int aID, const char *aURL)
{
  char *tempString;

  tempString = g_strdup_printf("setLocationBarURL%c%d%c%s%c",
                               BROWSER_LISTENER_DELIMITER,
                               aID,
                               BROWSER_LISTENER_DELIMITER,
                               (aURL ? aURL : ""),
                               BROWSER_LISTENER_TERMINATOR);

  sendMessage(tempString);
  g_free(tempString);
}

static void getURLSend(int aID, const char *aURL)
{
  char *tempString;

  tempString = g_strdup_printf("getURLReply%c%d%c%s%c",
                               BROWSER_LISTENER_DELIMITER,
                               aID,
                               BROWSER_LISTENER_DELIMITER,
                               (aURL ? aURL : ""),
                               BROWSER_LISTENER_TERMINATOR);

  sendMessage(tempString);
  g_free(tempString);
}

static void toggleViewSend(int aID)
{
  char *tempString;

  tempString = g_strdup_printf("toggleViewReply%c%d%c", 
                               BROWSER_LISTENER_DELIMITER,
                               aID,
                               BROWSER_LISTENER_TERMINATOR);
  sendMessage(tempString);
  g_free(tempString);
}

static void pongSend()
{
  char *tempString;

  tempString = g_strdup_printf("pong%c", BROWSER_LISTENER_TERMINATOR);

  sendMessage(tempString);
  g_free(tempString);
}

static void loadStartSend(int aWindow, const char *aURL)
{
  char *tempString;

  tempString = g_strdup_printf("loadingStarted%c%d%c%s%c",
                               BROWSER_LISTENER_DELIMITER,
                               aWindow,
                               BROWSER_LISTENER_DELIMITER,
                               (aURL ? aURL : ""),
                               BROWSER_LISTENER_TERMINATOR);
  
  sendMessage(tempString);
  g_free(tempString);
}

static void loadEndSend(int aID, const char *aURL)
{
  char *tempString;

  tempString = g_strdup_printf("loadingFinished%c%d%c%s%c",
                               BROWSER_LISTENER_DELIMITER,
                               aID,
                               BROWSER_LISTENER_DELIMITER,
                               (aURL ? aURL : ""),
                               BROWSER_LISTENER_TERMINATOR);

  sendMessage(tempString);
  g_free(tempString);
}

static void loadProgressSend(int aID, int aPercent)
{
  char *tempString;
  
  tempString = g_strdup_printf("loadingProgress%c%d%c%d%c",
                               BROWSER_LISTENER_DELIMITER,
                               aID,
                               BROWSER_LISTENER_DELIMITER,
                               aPercent,
                               BROWSER_LISTENER_TERMINATOR);
  sendMessage(tempString);
  g_free(tempString);
}

static void statusSend(int aID, int aMessageNum, const char *aStatus)
{
  char *tempString;

  if (aStatus == NULL)
    return;

  tempString = g_strdup_printf("statusMessage%c%d%c%d%c%s%c", 
                               BROWSER_LISTENER_DELIMITER,
                               aID,
                               BROWSER_LISTENER_DELIMITER,
                               aMessageNum,
                               BROWSER_LISTENER_DELIMITER,
                               (aStatus ? aStatus : ""),
                               BROWSER_LISTENER_TERMINATOR);

  sendMessage(tempString);
  g_free(tempString);
}

static void setTitleSend(int aID, const char *aTitle)
{
  char *tempString;

  tempString = g_strdup_printf("setTitle%c%d%c%s%c",
                               BROWSER_LISTENER_DELIMITER,
                               aID,
                               BROWSER_LISTENER_DELIMITER,
                               (aTitle ? aTitle : ""),
                               BROWSER_LISTENER_TERMINATOR);
  sendMessage(tempString);
  g_free(tempString);
}

static void findReplySend(const char *aString, bool aFound)
{
  char *tempString;

  if (!aString)
    return;
  
  tempString = g_strdup_printf("findReply%c%s%c%d%c",
                               BROWSER_LISTENER_DELIMITER,
                               (aString ? aString : ""),
                               BROWSER_LISTENER_DELIMITER,
                               (aFound ? 1 : 0),
                               BROWSER_LISTENER_TERMINATOR);

  sendMessage(tempString);
  g_free(tempString);
}

static void sslActiveSend(int aID, int aFlag)
{
  char *tempString;

  tempString = g_strdup_printf("sslActive%c%d%c%d%c",
                               BROWSER_LISTENER_DELIMITER,
                               aID,
                               BROWSER_LISTENER_DELIMITER,
                               (aFlag ? 1 : 2),
                               BROWSER_LISTENER_TERMINATOR);

  sendMessage(tempString);
  g_free(tempString);
}

static void alertSend(int aID, const char *aText)
{
  char *tempString;

  tempString = g_strdup_printf("alert%c%d%c%s%c",
                               BROWSER_LISTENER_DELIMITER,
                               aID,
                               BROWSER_LISTENER_DELIMITER,
                               (aText ? aText : ""),
                               BROWSER_LISTENER_TERMINATOR);

  sendMessage(tempString);
  g_free(tempString);
}

/*
static void showCertificateSend(char *aFilename)
{
}
static void memUsageReplySend(char *aUsage)
{
}
static void yesNoDialogSend(int aID, int aRequestID, int aMessageID)
{
}
*/

// incoming messages

static void setGeometryRequest(int aX, int aY, int aWidth, int aHeight)
{
  if (!gBrowserArray || !gBrowserArray[gCurrentIndex]) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  setGeometry(gBrowserArray[gCurrentIndex], aX, aY, aWidth, aHeight);

  // save this geometry in prefs
  if (!gPrefManager) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  gPrefManager->SetIntPref("browser.geometry.x",      aX);
  gPrefManager->SetIntPref("browser.geometry.y",      aY);
  gPrefManager->SetIntPref("browser.geometry.width",  aWidth);
  gPrefManager->SetIntPref("browser.geometry.height", aHeight);
}

static void getURLRequest(int aID)
{
  if (isIDValid(aID))
    getURLSend(aID, gtk_moz_embed_get_location(GTK_MOZ_EMBED(gBrowserArray[aID - 1]->mozEmbed)));
  else
    errorMessageSend(aID, BROWSER_ERROR_VIEW_NOT_AVAILABLE);
}

static void availableViewsRequest(char *aViewString)
{
  if (!gIDManager) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  if (!aViewString || !gIDManager->SetViews(aViewString))
    errorMessageSend(-1, BROWSER_ERROR_INVALID_VIEW_LIST);
}

static void toggleViewRequest(int aID)
{
  int tempIndex;

  if (aID == 0) {
    // zero so get the next used window
    tempIndex = getNextUsedWindow();
  }
  else if (isIDValid(aID) && gBrowserArray[aID - 1]->usedFlag) {
    // wasn't zero either so if it's valid and used
    tempIndex = aID - 1;
  }
  else {
    errorMessageSend(aID, BROWSER_ERROR_VIEW_NOT_AVAILABLE);
    return;
  }

  // we got through so make the selected one to be the
  // current one
  setVisible(gBrowserArray[gCurrentIndex], false);
  setVisible(gBrowserArray[tempIndex], true);
  toggleViewSend(tempIndex + 1);
  gCurrentIndex = tempIndex;
}

static void stopRequest(gtkEmbedBrowser *aBrowser)
{
  if (!aBrowser || !aBrowser->mozEmbed) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  // only allow stop if we're loading something
  if (!aBrowser->loadingFlag) {
    errorMessageSend(aBrowser->browserID, BROWSER_ERROR_CANT_STOP);
    return;
  }

  gtk_moz_embed_stop_load(GTK_MOZ_EMBED(aBrowser->mozEmbed));
}

static void reloadRequest(gtkEmbedBrowser *aBrowser)
{
  if (!aBrowser || !aBrowser->mozEmbed) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  // only allow reload if we're not loading something
  if (aBrowser->loadingFlag) {
    errorMessageSend(aBrowser->browserID, BROWSER_ERROR_CANT_RELOAD);
    return;
  }

  gtk_moz_embed_reload(GTK_MOZ_EMBED(aBrowser->mozEmbed),
                       GTK_MOZ_EMBED_FLAG_RELOADNORMAL);
}

static void forwardRequest(gtkEmbedBrowser *aBrowser)
{
  if (!aBrowser || !aBrowser->mozEmbed) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  if (!gtk_moz_embed_can_go_forward(GTK_MOZ_EMBED(aBrowser->mozEmbed))) {
    errorMessageSend(aBrowser->browserID, BROWSER_ERROR_CANT_GO_FORWARD);
    return;
  } 

  gtk_moz_embed_go_forward(GTK_MOZ_EMBED(aBrowser->mozEmbed));
}

static void backRequest(gtkEmbedBrowser *aBrowser)
{
  if (!aBrowser || !aBrowser->mozEmbed) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  if (!gtk_moz_embed_can_go_back(GTK_MOZ_EMBED(aBrowser->mozEmbed))) {
    errorMessageSend(aBrowser->browserID, BROWSER_ERROR_CANT_GO_BACK);
    return;
  } 

  gtk_moz_embed_go_back(GTK_MOZ_EMBED(aBrowser->mozEmbed));
}

static void pingRequest()
{
  pongSend();
}

static void shutdownRequest()
{
  gtk_main_quit();
  return;
}

static void openURLRequest(char *aURL, int aID, char *aFrameName)
{
  int tempIndex = 0;

  if (aID == 0 || aID == gCurrentIndex + 1) {
    // zero or the current one
    tempIndex = gCurrentIndex;
  }
  else if (isIDValid(aID) && gBrowserArray[aID - 1]->usedFlag) {
    // different view and used
    tempIndex = aID - 1;
  }
  else if (gIDManager->IsIDValid(aID)) {
    // not zero, not used and in the available view list
    tempIndex = aID - 1;
    // send an openwindow message
    windowOpenedSend(aID);
  }
  else {
    errorMessageSend(aID, BROWSER_ERROR_VIEW_NOT_AVAILABLE);
    return;
  }

  // set to used
  gBrowserArray[tempIndex]->usedFlag = true;
  // load url
  gtk_moz_embed_load_url(GTK_MOZ_EMBED(gBrowserArray[tempIndex]->mozEmbed), 
                         aURL);
}

static void enableJScriptRequest(int aFlag)
{
  if (!gPrefManager) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  gPrefManager->SetBoolPref("javascript.enabled", aFlag);
}

static void enableJavaRequest(int aFlag)
{
  if (!gPrefManager) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  gPrefManager->SetBoolPref("security.enable_java", aFlag);
}

static void fontSizeRequest(char *aSize)
{
  int newSize = BROWSER_FONT_DEFAULTSIZE;

  if (!gPrefManager) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  if (!aSize || atoi(aSize) == 0) {
    errorMessageSend(-1, BROWSER_ERROR_INVALID_FONTSIZE);
    return;
  }
  
  if (*aSize == '+' || *aSize == '-') {
    // relative font size, use x-western as base
    newSize = atoi(aSize) + gPrefManager->GetIntPref("font.size.variable.x-western");
  }
  else {
    // absolute font size
    newSize = atoi(aSize);
  }

  // check if the new size is not ridiculously too small or too large
  if (newSize < BROWSER_FONT_MINSIZE || newSize > BROWSER_FONT_MAXSIZE) {
    errorMessageSend(-1, BROWSER_ERROR_INVALID_FONTSIZE);
    return;
  }

  // finally, set the new size for all fonts
  gPrefManager->SetIntPref("font.size.variable.x-western", newSize);
  gPrefManager->SetIntPref("font.size.fixed.x-western", newSize);
  gPrefManager->SetIntPref("font.size.variable.zh-CN", newSize);
  gPrefManager->SetIntPref("font.size.fixed.zh-CN", newSize);
  gPrefManager->SetIntPref("font.size.variable.zh-TW", newSize);
  gPrefManager->SetIntPref("font.size.fixed.zh-TW", newSize);
  gPrefManager->SetIntPref("font.size.variable.ar", newSize);
  gPrefManager->SetIntPref("font.size.fixed.ar", newSize);
  gPrefManager->SetIntPref("font.size.variable.el", newSize);
  gPrefManager->SetIntPref("font.size.fixed.el", newSize);
  gPrefManager->SetIntPref("font.size.variable.he", newSize);
  gPrefManager->SetIntPref("font.size.fixed.he", newSize);
  gPrefManager->SetIntPref("font.size.variable.ja", newSize);
  gPrefManager->SetIntPref("font.size.fixed.ja", newSize);
  gPrefManager->SetIntPref("font.size.variable.ko", newSize);
  gPrefManager->SetIntPref("font.size.fixed.ko", newSize);
  gPrefManager->SetIntPref("font.size.variable.th", newSize);
  gPrefManager->SetIntPref("font.size.fixed.th", newSize);
  gPrefManager->SetIntPref("font.size.variable.tr", newSize);
  gPrefManager->SetIntPref("font.size.fixed.tr", newSize);
  gPrefManager->SetIntPref("font.size.variable.x-baltic", newSize);
  gPrefManager->SetIntPref("font.size.fixed.x-baltic", newSize);
  gPrefManager->SetIntPref("font.size.variable.x-central-euro", newSize);
  gPrefManager->SetIntPref("font.size.fixed.x-central-euro", newSize);
  gPrefManager->SetIntPref("font.size.variable.x-cyrillic", newSize);
  gPrefManager->SetIntPref("font.size.fixed.x-cyrillic", newSize);
  gPrefManager->SetIntPref("font.size.variable.x-unicode", newSize);
  gPrefManager->SetIntPref("font.size.fixed.x-unicode", newSize);
}

static void enableCookiesRequest(int aFlag)
{
  if (!gPrefManager) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  // set prefs
  gPrefManager->SetIntPref("network.accept_cookies", (aFlag ? 0 : 2));
  gPrefManager->SetIntPref("network.foreign_cookies", (aFlag ? 0 : 1));
  gPrefManager->SetIntPref("network.cookie.cookieBehavior", (aFlag ? 0 : 2));
}

static void findRequest(char *aString, int aMode)
{
   nsCOMPtr<nsIWebBrowserFind> findInstance;
   nsString tempString;
   PRBool tempResult = false;

   if (!aString) {
     errorMessageSend(-1, BROWSER_ERROR_INVALID_PARAMS);
     return;
   }

   // check the given mode is valid
   if (aMode < 0 && aMode > BROWSER_FIND_MAX) {
     errorMessageSend(-1, BROWSER_ERROR_INVALID_FINDMODE);
     return;
   }

   // get an instance of nsIWebBrowserFind
   findInstance = do_GetInterface(gBrowserArray[gCurrentIndex]->webBrowser);

   if (!findInstance) {
     errorMessageSend(-1, BROWSER_ERROR_GENERAL);
     return;
   }

   // convert the search string
   tempString.AssignWithConversion(aString);

   // set the parameters
   if ((aMode & BROWSER_FIND_CASESENSITIVE) == BROWSER_FIND_CASESENSITIVE)
     findInstance->SetMatchCase(true);
   else
     findInstance->SetMatchCase(false);

   if ((aMode & BROWSER_FIND_BACKWARDS) == BROWSER_FIND_BACKWARDS)
     findInstance->SetFindBackwards(true);
   else
     findInstance->SetFindBackwards(false);

   if ((aMode & BROWSER_FIND_WRAPAROUND) == BROWSER_FIND_WRAPAROUND)
     findInstance->SetWrapFind(true);
   else
     findInstance->SetWrapFind(false);

   // set the search string
   findInstance->SetSearchString(tempString.get());

   // now find it
   findInstance->FindNext(&tempResult);
   
   // send results
   findReplySend(aString, tempResult);
}

static void purgeCacheRequest()
{
  nsCOMPtr<nsICacheService> tempService;

  // get an instance of nsICacheService
  if (!(tempService = do_GetService(NS_CACHESERVICE_CONTRACTID))) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  // purge both memory and disk cache
  tempService->EvictEntries(nsICache::STORE_ANYWHERE);
}

static void printRequest(int aID, bool aFlag)
{
  if (!gPrintManager) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }
 
  if (!gBrowserArray || !gBrowserArray[aID - 1]) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  if (!gPrintManager->Print(gBrowserArray[aID - 1]->webBrowser, aFlag)) {
    errorMessageSend(-1, BROWSER_ERROR_CANT_PRINT);
    return;
  }
}

static void enablePluginsRequest(int aFlag)
{
  int i;

  if (!gBrowserArray || !gPrefManager) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  // save the new flag in prefs
  gPrefManager->SetBoolPref("plugins.enabled", (aFlag == 0) ? false : true);

  // go through all the windows and set the flag
  for (i = 0; i < gMaxWin; i++) 
    setPluginFlag(gBrowserArray[i], (aFlag == 0) ? false : true);
}

/*
static void requestCertificateRequest(int aID)
{
}
static void cleanupRequest()
{
}
static void memUsageRequest()
{
}
*/

// dialog callback functions

bool sslActivateCB(nsIDOMWindow *aWindow, bool aFlag)
{
  int tempID = getWindowID(aWindow);

  if (isIDValid(tempID))
    sslActiveSend(tempID, aFlag);
  else
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);

  return true;
}

static bool alertCB(nsIDOMWindow *aWindow, const char *aText)
{
  int tempID = getWindowID(aWindow);

  if (isIDValid(tempID))
    alertSend(tempID, aText);
  else
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);

  return true;
}

// utility functions

// sets the plugin state into the browser
static bool setPluginFlag(gtkEmbedBrowser *aBrowser, bool aFlag)
{
  nsCOMPtr<nsIWebBrowserSetup> browserSetup = NULL;

  if (!aBrowser || !aBrowser->webBrowser)
    return false;

  browserSetup = do_QueryInterface(aBrowser->webBrowser);

  if (!browserSetup)
    return false;

  browserSetup->SetProperty(nsIWebBrowserSetup::SETUP_ALLOW_PLUGINS, aFlag);

  return true;
}

// returns the plugin state from prefs (enabled/disabled)
static bool getPluginStateFromPrefs()
{
  if (!gPrefManager)
    return false;

  return gPrefManager->GetBoolPref("plugins.enabled");
}

// returns the id of the window, 0 if not found
static int getWindowID(nsIDOMWindow *aWindow)
{
  int i;
  nsIDOMWindow *tempWindow = NULL;

  if (!aWindow || !gBrowserArray)
    return 0;

  for (i = 0; i < gMaxWin; i++) {
    if (gBrowserArray[i] && gBrowserArray[i]->webBrowser)
      gBrowserArray[i]->webBrowser->GetContentDOMWindow(&tempWindow);

    if (tempWindow == aWindow)
      return i + 1;
  }

  return 0;
}

// returns the number of used window
static int getNumberOfUsedWindows()
{
  int i;
  int count = 0;

  if (!gBrowserArray)
    return 0;

  for (i = 0; i < gMaxWin; i++)
    if (gBrowserArray[i] && gBrowserArray[i]->usedFlag)
      count++;

  return count;
}


// returns the index of the next used window
static int getNextUsedWindow()
{
  int i;

  if (!gBrowserArray)
    return gCurrentIndex;

  // go through the windows after the current one
  for (i = gCurrentIndex + 1; i < gMaxWin; i++) {
    if (gBrowserArray && gBrowserArray[i]->usedFlag)
      return i;
  }

  // went past, now go through windows from 0 to the current one
  for (i = 0; i <= gCurrentIndex; i++) {
    if (gBrowserArray && gBrowserArray[i]->usedFlag)
      return i;
  }

  // none of the windows are used. shouldn't happen but
  // just in case we return the current one
  return gCurrentIndex;
}

// return true if the given ID is
// within range
static bool isIDValid(int aID)
{
  return (aID > 0 && aID <= gMaxWin);
}

static void destroyAll()
{
  int i;

  // free the browser array
  if (gBrowserArray) {
    for (i = 0; i < gMaxWin; i++) {
      if (gBrowserArray[i] && gBrowserArray[i]->topLevelWindow)
        g_free(gBrowserArray[i]);
    }
    free(gBrowserArray);
  }

  // destroy the global refs  
  if (gSocketListener)
    delete gSocketListener;

  if (gIDManager)
    delete gIDManager;

  if (gPrefManager)
    delete gPrefManager;

  if (gDialogManager)
    delete gDialogManager;
}

static gtkEmbedBrowser *createNewBrowser(guint32 aChromeMask, 
                                         guint32 aID, 
                                         bool aUsedFlag,
                                         bool aPluginState,
                                         gtkEmbedGeometry aGeometry)
{
  gtkEmbedBrowser *newBrowser               = NULL;
  nsCOMPtr<nsIWebBrowserSetup> browserSetup = NULL;
  char *tempClassString                     = NULL;

  newBrowser = g_new0(gtkEmbedBrowser, 1);

  newBrowser->browserID      = aID;
  newBrowser->usedFlag       = aUsedFlag;
  newBrowser->loadingFlag    = false;
  newBrowser->topLevelWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  newBrowser->topLevelVBox   = gtk_vbox_new(true, 0);

  gtk_container_add(GTK_CONTAINER(newBrowser->topLevelWindow),
                    newBrowser->topLevelVBox);

  newBrowser->mozEmbed = gtk_moz_embed_new();

  gtk_box_pack_start(GTK_BOX(newBrowser->topLevelVBox), 
                     newBrowser->mozEmbed,
                     true,
                     true,
                     0);

  // set the policy for the toplevel window so that it'll resize
  // with everything else
  gtk_window_set_policy(GTK_WINDOW(newBrowser->topLevelWindow),
                        true,
                        true,
                        true);

  // set wm class string
  tempClassString = g_strdup_printf("%s%d",
                                    BROWSER_WM_PREFIX,
                                    aID);
                                     
  gtk_window_set_wmclass(GTK_WINDOW(newBrowser->topLevelWindow),
                         BROWSER_WM_CLASSNAME,
                         tempClassString);

  g_free(tempClassString); 
  
  // set callback functions
  gtk_signal_connect(GTK_OBJECT(newBrowser->mozEmbed), "location",
                     GTK_SIGNAL_FUNC(changeLocationCB), newBrowser);
  gtk_signal_connect(GTK_OBJECT(newBrowser->mozEmbed), "title",
                     GTK_SIGNAL_FUNC(changeTitleCB), newBrowser);
  gtk_signal_connect(GTK_OBJECT(newBrowser->mozEmbed), "net_start",
                     GTK_SIGNAL_FUNC(loadStartCB), newBrowser);
  gtk_signal_connect(GTK_OBJECT(newBrowser->mozEmbed), "net_stop",
                     GTK_SIGNAL_FUNC(loadEndCB), newBrowser);
  gtk_signal_connect(GTK_OBJECT(newBrowser->mozEmbed), "net_state",
                     GTK_SIGNAL_FUNC(changeNetStateCB), newBrowser);
  gtk_signal_connect(GTK_OBJECT(newBrowser->mozEmbed), "net_state_all",
                     GTK_SIGNAL_FUNC(changeNetStateAllCB), newBrowser);
  gtk_signal_connect(GTK_OBJECT(newBrowser->mozEmbed), "progress",
                     GTK_SIGNAL_FUNC(changeProgressCB), newBrowser);
  gtk_signal_connect(GTK_OBJECT(newBrowser->mozEmbed), "progress_all",
                     GTK_SIGNAL_FUNC(changeProgressAllCB), newBrowser);
  gtk_signal_connect(GTK_OBJECT(newBrowser->mozEmbed), "link_message",
                     GTK_SIGNAL_FUNC(messageLinkCB), newBrowser);
  gtk_signal_connect(GTK_OBJECT(newBrowser->mozEmbed), "js_status",
                     GTK_SIGNAL_FUNC(changeStatusJSCB), newBrowser);
  gtk_signal_connect(GTK_OBJECT(newBrowser->mozEmbed), "new_window",
                     GTK_SIGNAL_FUNC(newWindowCB), newBrowser);
  gtk_signal_connect(GTK_OBJECT(newBrowser->mozEmbed), "visibility",
                     GTK_SIGNAL_FUNC(visibilityCB), newBrowser);
  gtk_signal_connect(GTK_OBJECT(newBrowser->mozEmbed), "destroy_browser",
                     GTK_SIGNAL_FUNC(destroyBrowserCB), newBrowser);
  gtk_signal_connect(GTK_OBJECT(newBrowser->mozEmbed), "open_uri",
                     GTK_SIGNAL_FUNC(openURICB), newBrowser);
  gtk_signal_connect(GTK_OBJECT(newBrowser->mozEmbed), "size_to",
                     GTK_SIGNAL_FUNC(changeSizeCB), newBrowser);
  gtk_signal_connect(GTK_OBJECT(newBrowser->mozEmbed), "destroy",
                     GTK_SIGNAL_FUNC(destroyCB), newBrowser);
  gtk_signal_connect(GTK_OBJECT(newBrowser->topLevelWindow), "delete_event",
                     GTK_SIGNAL_FUNC(deleteCB), newBrowser);

  if (aChromeMask == GTK_MOZ_EMBED_FLAG_DEFAULTCHROME)
    aChromeMask = GTK_MOZ_EMBED_FLAG_ALLCHROME;

  gtk_moz_embed_set_chrome_mask(GTK_MOZ_EMBED(newBrowser->mozEmbed),
                                aChromeMask);

  // show the widgets except the toplevel window
  gtk_widget_show(newBrowser->mozEmbed);
  gtk_widget_show(newBrowser->topLevelVBox);

  // realize everything to allow us to load
  // urls onto hidden windows
  gtk_widget_realize(newBrowser->mozEmbed);
  gtk_widget_realize(newBrowser->topLevelVBox);
  gtk_widget_realize(newBrowser->topLevelWindow);

  // set the webBrowser attribute
  gtk_moz_embed_get_nsIWebBrowser(GTK_MOZ_EMBED(newBrowser->mozEmbed),
                                  &(newBrowser->webBrowser));

  // set the plugin state
  setPluginFlag(newBrowser, aPluginState);

  // set size and position
  setGeometry(newBrowser, aGeometry);

  return newBrowser;  
}

bool getGeometryFromPrefs(gtkEmbedGeometry *aGeometry)
{
  if (!aGeometry)
    return false;

  // initialise with default values
  aGeometry->x      = BROWSER_WINDOW_X;
  aGeometry->y      = BROWSER_WINDOW_Y;
  aGeometry->width  = BROWSER_WINDOW_WIDTH;
  aGeometry->height = BROWSER_WINDOW_HEIGHT;

  if (!gPrefManager)
    return false;

  // retrieve values from prefs
  aGeometry->x      = gPrefManager->GetIntPref("browser.geometry.x");
  aGeometry->y      = gPrefManager->GetIntPref("browser.geometry.y");
  aGeometry->width  = gPrefManager->GetIntPref("browser.geometry.width");
  aGeometry->height = gPrefManager->GetIntPref("browser.geometry.height");
 
  // now check if the prefs have reasonable values
  if (aGeometry->x < 0)
    aGeometry->x = BROWSER_WINDOW_X;
  if (aGeometry->y < 0)
    aGeometry->y = BROWSER_WINDOW_Y;
  if (aGeometry->width <= 0)
    aGeometry->width = BROWSER_WINDOW_WIDTH;
  if (aGeometry->height <= 0)
    aGeometry->height = BROWSER_WINDOW_HEIGHT; 

  return true;
}

void initialiseBrowserArray(bool aRevertStateFlag)
{
  int i;
  gtkEmbedGeometry tempGeometry;

  // if the revert state flag is true, retrieve geometry
  // from prefs, otherwise just set to default
  if (aRevertStateFlag)
    getGeometryFromPrefs(&tempGeometry);
  else {
    tempGeometry.x      = BROWSER_WINDOW_X;
    tempGeometry.y      = BROWSER_WINDOW_Y;
    tempGeometry.width  = BROWSER_WINDOW_WIDTH;
    tempGeometry.height = BROWSER_WINDOW_HEIGHT;
  }

  // allocate memory to the list
  gBrowserArray = (gtkEmbedBrowser **) malloc(sizeof(gtkEmbedBrowser *) * gMaxWin);

  // create browser
  for (i = 0; i < gMaxWin; i++) {
    gBrowserArray[i] = createNewBrowser(GTK_MOZ_EMBED_FLAG_DEFAULTCHROME, 
                                        i + 1, 
                                        true,
                                        getPluginStateFromPrefs(),
                                        tempGeometry);

    // send a window opened message for every window
    windowOpenedSend(i + 1);
  }
}

void setGeometry(gtkEmbedBrowser *aBrowser, gtkEmbedGeometry aGeometry)
{
  if (!aBrowser || 
      aGeometry.x < 0 || 
      aGeometry.y < 0 || 
      aGeometry.width <= 0 ||
      aGeometry.height <= 0)
    return;

  // set size
  gtk_widget_set_usize(aBrowser->mozEmbed, aGeometry.width, aGeometry.height);
  gtk_widget_set_usize(aBrowser->topLevelVBox, aGeometry.width, aGeometry.height);
  gtk_widget_set_usize(aBrowser->topLevelWindow, aGeometry.width, aGeometry.height);
 
  // set position
  gtk_widget_set_uposition(aBrowser->topLevelWindow, aGeometry.x, aGeometry.y);
}

void setGeometry(gtkEmbedBrowser *aBrowser, int aX, int aY, int aWidth, int aHeight)
{
  gtkEmbedGeometry tempGeometry;

  tempGeometry.x      = aX;
  tempGeometry.y      = aY;
  tempGeometry.width  = aWidth;
  tempGeometry.height = aHeight;

  setGeometry(aBrowser, tempGeometry);
}

void setVisible(gtkEmbedBrowser *aBrowser, bool aVisible)
{
  if (!aBrowser)
    return;
  
  if (!aVisible)
    gtk_widget_hide(aBrowser->topLevelWindow);
  else
    gtk_widget_show(aBrowser->topLevelWindow);
}

static int parseMessage(char *aMessage,
                        char aArray[BROWSER_LISTENER_MAXPARAMS]
                                    [BROWSER_LISTENER_MAXLEN])
{
  char *tempMessage = aMessage;
  char *tempArrayRow;
  PRInt32 paramCount = 0;

  memset(aArray,
         0,
         BROWSER_LISTENER_MAXLEN * BROWSER_LISTENER_MAXPARAMS * sizeof(char));

  if (aMessage == nsnull || aMessage == NULL)
    return 0;

  while (*tempMessage != BROWSER_LISTENER_TERMINATOR) {
    tempArrayRow = aArray[paramCount++];

    while (*tempMessage != BROWSER_LISTENER_DELIMITER &&
           *tempMessage != BROWSER_LISTENER_TERMINATOR) {

      *tempArrayRow++ = *tempMessage++;
    }

    if (*tempMessage == BROWSER_LISTENER_DELIMITER)
       tempMessage++;

     *tempArrayRow = '\0';
  }
  return paramCount;
}

static void sendMessage(const char *aMessage)
{
  if (gOutgoingFD < 0 || aMessage == NULL)
    return;
  
  // if the terminator is \000, we have to manually
  // send it through the pipe
#if (BROWSER_LISTENER_TERMINATOR == '\000')
  write(gOutgoingFD, aMessage, strlen(aMessage) + 1);
#else
  write(gOutgoingFD, aMessage, strlen(aMessage));
#endif

  return;
}
