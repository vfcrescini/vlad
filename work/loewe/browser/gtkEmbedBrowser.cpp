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
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "gtkEmbedIDManager.h"
#include "gtkEmbedBrowser.h"
#include "gtkEmbedSocketListener.h"
#include "gtkEmbedPref.h"
#include "gtkEmbedDialogManager.h"

#define BROWSER_LISTENER_MAXPARAMS  16
#define BROWSER_LISTENER_MAXLEN     1024
#define BROWSER_LISTENER_DELIMITER  '\004'
#define BROWSER_LISTENER_TERMINATOR '\000'

#define BROWSER_WINDOW_MAX          64
#define BROWSER_WINDOW_DEFAULT      2
#define BROWSER_WINDOW_WIDTH        500
#define BROWSER_WINDOW_HEIGHT       500

#define BROWSER_WM_CLASSNAME        "L3WM_ID"
#define BROWSER_WM_PREFIX           "Browser-View"

#define BROWSER_FONT_DEFAULTSIZE    16
#define BROWSER_FONT_MINSIZE        8
#define BROWSER_FONT_MAXSIZE        72

#define BROWSER_FIND_CASESENSITIVE  1
#define BROWSER_FIND_WRAPAROUND     2
#define BROWSER_FIND_BACKWARDS      4
#define BROWSER_FIND_MAX            4

#define BROWSER_PROF_NAME           "gtkEmbedBrowser"

#define BROWSER_STARTUP_DEFAULTURL  "about:blank"

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

// shutdown flag
static bool gShutdown = false;

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

/*
static void enablePluginRequest(int);
static void printRequest(int, char *);
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

/*
static void showCertificateSend(char *);
static void memUsageReplySend(char *);
static void sslActiveSend(int, int);
static void yesNoDialogSend(int, int, int);
*/

// callback functions for dialogs
static bool alertCB(nsIDOMWindow *, const char *);
static bool promptCB(nsIDOMWindow *, const char *, const char *, bool *);
static bool confirmCB(nsIDOMWindow *, const char *, bool *);

// utility functions
static void readMessage();
static bool isIDValid(int);
static int getNextUsedWindow();
static void destroyBrowsers();
static void destroyAll();
static void sendMessage(const char *);
static gtkEmbedBrowser *createNewBrowser(guint32, guint32);
static void initialiseBrowserArray();
static void setVisible(gtkEmbedBrowser *, bool);
static void setSize(gtkEmbedBrowser *, int, int);
static void setPosition(gtkEmbedBrowser *, int, int);
static int parseMessage(char *,
                        char [BROWSER_LISTENER_MAXPARAMS][BROWSER_LISTENER_MAXLEN]);

// main
int main(int argc, char **argv)
{
  char *homePath    = NULL;
  char *profilePath = NULL;
  char *socketPath  = NULL;

  gtk_set_locale();
  gtk_init(&argc, &argv);

  // command line option stuff
  opterr = 0;
  
  if (getopt(argc, argv, "u:") == 'u' && optarg)
    socketPath = g_strdup(optarg);
  else {
    fprintf(stderr, 
            "usage %s -u <socket path> [-n <window num>] [url]\n", 
            argv[0]);
    return -1;
  }

  // check if n was given, if not just use the default
  if (getopt(argc, argv, "n:") == 'n' && optarg) {
    // check if the given value for n is within the valid range
    if (atoi(optarg) > 0 && atoi(optarg) < BROWSER_WINDOW_MAX)
      gMaxWin = atoi(optarg);
    else {
      fprintf(stderr, 
              "<window num> must be an integer between 0 and %d\n",
              BROWSER_WINDOW_MAX);
      return -1;
    }
  }
  else
    gMaxWin = BROWSER_WINDOW_DEFAULT;

  // profile stuff
  homePath = g_getenv("HOME");

  if (!homePath) {
    fprintf(stderr, "failed to get HOME\n");
    return -1;
  }

  profilePath = g_strdup_printf("%s/.%s", homePath, BROWSER_PROF_NAME);
  gtk_moz_embed_set_profile_path(profilePath, BROWSER_PROF_NAME);

  // id manager stuff
  gIDManager = new gtkEmbedIDManager(gMaxWin);

  if (!gIDManager)
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
  initialiseBrowserArray();

  // dialog stuff
  gDialogManager = new gtkEmbedDialogManager();

  if (!gDialogManager)
    return -1;

  gDialogManager->Init(&alertCB, &promptCB, &confirmCB);

  // open the default window
  gCurrentIndex = 0;
  setVisible(gBrowserArray[0], true);
  windowOpenedSend(1);

  // load startup url, if any
  if (argv[optind])
    openURLRequest(argv[optind], 1, NULL);  
  else
    openURLRequest(BROWSER_STARTUP_DEFAULTURL, 1, NULL);

  // pref stuff
  gPrefManager = new gtkEmbedPref();

  if (!gPrefManager || !gPrefManager->Init())
    return -1;

  gtk_main();

  destroyAll();

  // cleanup
  if (homePath)
    g_free(homePath);
  if (profilePath)
    g_free(profilePath);
  if (socketPath)
    g_free(socketPath);

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
    errorMessageSend(-1, BROWSER_ERROR_INVALID_PARAMS);
    return;
  }

  if (!strcmp(tempArray[0], "toggleView")) {
    if (paramCount == 2 && isIDValid(atoi(tempArray[1])))
      toggleViewRequest(atoi(tempArray[1]));
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
    if (paramCount == 4 && (isIDValid(atoi(tempArray[2])) ||
                            atoi(tempArray[2]) == 0))
      openURLRequest(tempArray[1], atoi(tempArray[2]), tempArray[3]);
    else if (paramCount == 3 && (isIDValid(atoi(tempArray[2])) ||
                                 atoi(tempArray[2]) == 0))
      openURLRequest(tempArray[1], atoi(tempArray[2]), "");
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
    else if (paramCount == 2 && isIDValid(atoi(tempArray[1])))
      getURLRequest(atoi(tempArray[1]));
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
    if (paramCount == 2 && (atoi(tempArray[1]) == -1 || atoi(tempArray[1]) == 0))
      enableJScriptRequest(atoi(tempArray[1]) == -1);
    else
      errorMessageSend(-1, BROWSER_ERROR_INVALID_PARAMS);
  }
  else if (!strcmp(tempArray[0], "enableJava")) {
    if (paramCount == 2 && (atoi(tempArray[1]) == -1 || atoi(tempArray[1]) == 0))
      enableJavaRequest(atoi(tempArray[1]) == -1);
    else
      errorMessageSend(-1, BROWSER_ERROR_INVALID_PARAMS);
  }
  else if (!strcmp(tempArray[0], "enableCookies")) {
    if (paramCount == 2 && (atoi(tempArray[1]) == -1 || atoi(tempArray[1]) == 0))
      enableCookiesRequest(atoi(tempArray[1]) == -1);
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
  else
    errorMessageSend(-1, BROWSER_ERROR_INVALID_MESSAGE);
}

// callback functions from the embedded widget
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
  char tempStatus[BROWSER_LISTENER_MAXLEN];

  if (!aEmbed || !aBrowser) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  if (aMax < 1) {
    tempPercentage = 0;
    sprintf(tempStatus, "%d bytes loaded", aCurr);
  }
  else {
    tempPercentage = (aCurr > aMax) ? 100 : (aCurr * 100) / aMax;
    sprintf(tempStatus, 
            "%d%% complete, %d bytes of %d loaded", 
            tempPercentage, aCurr, aMax);
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

  // if that particular window doesn't exist yet, 
  // send a window opened message
  if (!gBrowserArray[tempIndex]->usedFlag) {
    gBrowserArray[tempIndex]->usedFlag = true;
    windowOpenedSend(tempIndex + 1);
  }
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
  int tempIndex;

  if (!aWidget || !aBrowser) {
    errorMessageSend(-1, BROWSER_ERROR_GENERAL);
    return;
  }

  // check to see if shutdown is in progress to prevent
  // destroying stuff twice
  if (gShutdown)
    return;

  // if this was the only opened window, we can shutdown
  if (getNextUsedWindow() + 1 == aBrowser->browserID) {
    gShutdown = true;
    destroyBrowsers();
    gtk_main_quit();
    return;
  }

  // if it's not the only window, silently recreate it
  tempID    = aBrowser->browserID;
  tempIndex = tempID - 1;

  if (tempIndex < 0)
    return;

  // destroy the old one and create a new one
  gtk_widget_destroy(gBrowserArray[tempIndex]->topLevelWindow);
  g_free(gBrowserArray[tempIndex]);
  gBrowserArray[tempIndex] = createNewBrowser(GTK_MOZ_EMBED_FLAG_DEFAULTCHROME, 
                                              tempID);
}

// outgoing messages

static void errorMessageSend(int aID, int aErrorCode)
{
  char tempString[BROWSER_LISTENER_MAXLEN];
  
  sprintf(tempString, 
          "error%c%d%c%d%c",
          BROWSER_LISTENER_DELIMITER,
          aID,
          BROWSER_LISTENER_DELIMITER,
          aErrorCode,
          BROWSER_LISTENER_TERMINATOR);

  sendMessage(tempString); 
}

static void windowOpenedSend(int aID)
{
  char tempString[BROWSER_LISTENER_MAXLEN];

  sprintf(tempString,
          "windowOpened%c%d%c",
          BROWSER_LISTENER_DELIMITER,
          aID,
          BROWSER_LISTENER_TERMINATOR);

  sendMessage(tempString);
}

static void enableForwardSend(int aID, bool aFlag)
{
  char tempString[BROWSER_LISTENER_MAXLEN];

  sprintf(tempString, 
          "enableForward%c%d%c%d%c",
          BROWSER_LISTENER_DELIMITER,
          aID,
          BROWSER_LISTENER_DELIMITER,
          (aFlag) ? 1 : 2,
          BROWSER_LISTENER_TERMINATOR);

  sendMessage(tempString);
}

static void enableBackSend(int aID, bool aFlag)
{
  char tempString[BROWSER_LISTENER_MAXLEN];

  sprintf(tempString, 
          "enableBack%c%d%c%d%c",
          BROWSER_LISTENER_DELIMITER,
          aID,
          BROWSER_LISTENER_DELIMITER,
          (aFlag) ? 1 : 2,
          BROWSER_LISTENER_TERMINATOR);

  sendMessage(tempString);
}

static void enableReloadSend(int aID, bool aFlag)
{
  char tempString[BROWSER_LISTENER_MAXLEN];

  sprintf(tempString, 
          "enableReload%c%d%c%d%c",
          BROWSER_LISTENER_DELIMITER,
          aID,
          BROWSER_LISTENER_DELIMITER,
          (aFlag) ? 1 : 2,
          BROWSER_LISTENER_TERMINATOR);

  sendMessage(tempString);
}

static void enableStopSend(int aID, bool aFlag)
{
  char tempString[BROWSER_LISTENER_MAXLEN];

  sprintf(tempString, 
          "enableStop%c%d%c%d%c",
          BROWSER_LISTENER_DELIMITER,
          aID,
          BROWSER_LISTENER_DELIMITER,
          (aFlag) ? 1 : 2,
          BROWSER_LISTENER_TERMINATOR);

  sendMessage(tempString);
}

static void setLocationBarURLSend(int aID, const char *aURL)
{
  char tempString[BROWSER_LISTENER_MAXLEN];

  sprintf(tempString,
          "setLocationBarURL%c%d%c%s%c",
          BROWSER_LISTENER_DELIMITER,
          aID,
          BROWSER_LISTENER_DELIMITER,
          aURL,
          BROWSER_LISTENER_TERMINATOR);

  sendMessage(tempString);
}

static void getURLSend(int aID, const char *aURL)
{
  char tempString[BROWSER_LISTENER_MAXLEN];

  sprintf(tempString,
          "getURLReply%c%d%c%s%c",
          BROWSER_LISTENER_DELIMITER,
          aID,
          BROWSER_LISTENER_DELIMITER,
          aURL,
          BROWSER_LISTENER_TERMINATOR);

  sendMessage(tempString);
}

static void toggleViewSend(int aID)
{
  char tempString[BROWSER_LISTENER_MAXLEN];

  sprintf(tempString,
          "toggleViewReply%c%d%c", 
          BROWSER_LISTENER_DELIMITER,
          aID,
          BROWSER_LISTENER_TERMINATOR);
          sendMessage(tempString);
}

static void pongSend()
{
  char tempString[BROWSER_LISTENER_MAXLEN];

  sprintf(tempString, "pong%c", BROWSER_LISTENER_TERMINATOR);

  sendMessage(tempString);
}

static void loadStartSend(int aWindow, const char *aURL)
{
  char tempString[BROWSER_LISTENER_MAXLEN];

  sprintf(tempString, 
          "loadingStarted%c%d%c%s%c",
          BROWSER_LISTENER_DELIMITER,
          aWindow,
          BROWSER_LISTENER_DELIMITER,
          aURL,
          BROWSER_LISTENER_TERMINATOR);
  
  sendMessage(tempString);
}

static void loadEndSend(int aID, const char *aURL)
{
  char tempString[BROWSER_LISTENER_MAXLEN];

  sprintf(tempString, 
          "loadingFinished%c%d%c%s%c",
          BROWSER_LISTENER_DELIMITER,
          aID,
          BROWSER_LISTENER_DELIMITER,
          aURL,
          BROWSER_LISTENER_TERMINATOR);

  sendMessage(tempString);
}

static void loadProgressSend(int aID, int aPercent)
{
  char tempString[BROWSER_LISTENER_MAXLEN];
  
  sprintf(tempString,
          "loadingProgress%c%d%c%d%c",
          BROWSER_LISTENER_DELIMITER,
          aID,
          BROWSER_LISTENER_DELIMITER,
          aPercent,
          BROWSER_LISTENER_TERMINATOR);
  sendMessage(tempString);
}

static void statusSend(int aID, int aMessageNum, const char *aStatus)
{
  char tempString[BROWSER_LISTENER_MAXLEN];

  if (aStatus == NULL)
    return;

  sprintf(tempString, 
          "statusMessage%c%d%c%d%c%s%c", 
          BROWSER_LISTENER_DELIMITER,
          aID,
          BROWSER_LISTENER_DELIMITER,
          aMessageNum,
          BROWSER_LISTENER_DELIMITER,
          aStatus,
          BROWSER_LISTENER_TERMINATOR);

  sendMessage(tempString);
}

static void setTitleSend(int aID, const char *aTitle)
{
  char tempString[BROWSER_LISTENER_MAXLEN];

  sprintf(tempString,
          "setTitle%c%d%c%s%c",
          BROWSER_LISTENER_DELIMITER,
          aID,
          BROWSER_LISTENER_DELIMITER,
          aTitle,
          BROWSER_LISTENER_TERMINATOR);
  sendMessage(tempString);
}

static void findReplySend(const char *aString, bool aFound)
{
  char tempString[BROWSER_LISTENER_MAXLEN];

  if (!aString)
    return;
  
  sprintf(tempString,
          "findReply%c%s%c%d%c",
          BROWSER_LISTENER_DELIMITER,
          aString,
          BROWSER_LISTENER_DELIMITER,
          (aFound ? 1 : 0),
          BROWSER_LISTENER_TERMINATOR);

  sendMessage(tempString);
}

/*
static void showCertificateSend(char *aFilename)
{
}
static void memUsageReplySend(char *aUsage)
{
}
static void sslActiveSend(int aID, int aFlag)
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

  setPosition(gBrowserArray[gCurrentIndex], aX, aY);
  setSize(gBrowserArray[gCurrentIndex], aWidth, aHeight);
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
  // set the shutdown flag
  gShutdown = true;

  destroyBrowsers();
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
   findInstance->SetSearchString(tempString.GetUnicode());

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

/*
static void enablePluginRequest(int aFlag)
{
}
static void printRequest(int aID, char *aFlag)
{
}
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

// callback functions from the dialogs
bool alertCB(nsIDOMWindow *aParent, const char *aMessage)
{
  printf("alert from app: %s\n", aMessage);
  return true;
}

bool promptCB(nsIDOMWindow *aParent, const char *aMessage, const char *aValue, bool *aOK)
{
  printf("prompt from app\n");
  *aOK = true;

  return true;
}

bool confirmCB(nsIDOMWindow *aParent, const char *aMessage, bool *aOK)
{
  printf("confirm from app\n");
  *aOK = true;

  return true;
}


// utility functions

// returns the index of the next used window
static int getNextUsedWindow()
{
  int i;

  // go through the windows after the current one
  for (i = gCurrentIndex + 1; i < gMaxWin; i++) {
    if (gBrowserArray[i]->usedFlag)
      return i;
  }

  // went past, now go through windows from 0 to the current one
  for (i = 0; i <= gCurrentIndex; i++) {
    if (gBrowserArray[i]->usedFlag)
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

static void destroyBrowsers()
{
  int i;

  // destroy all our windows
  if (gBrowserArray) {
    for (i = 0; i < gMaxWin; i++) {
      if (gBrowserArray[i] && gBrowserArray[i]->topLevelWindow)
        gtk_widget_destroy(gBrowserArray[i]->topLevelWindow);

      if (gBrowserArray[i])
        g_free(gBrowserArray[i]);
    }

    free(gBrowserArray);
    gBrowserArray = NULL;
  }
}

static void destroyAll()
{
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

static gtkEmbedBrowser *createNewBrowser(guint32 aChromeMask, guint32 aID)
{
  gtkEmbedBrowser *newBrowser      = NULL;
  char            *tempClassString = NULL;

  newBrowser = g_new0(gtkEmbedBrowser, 1);

  newBrowser->browserID      = aID;
  newBrowser->usedFlag       = true;
  newBrowser->loadingFlag    = false;
  newBrowser->blockedFlag    = false;
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

  if (aChromeMask == GTK_MOZ_EMBED_FLAG_DEFAULTCHROME)
    aChromeMask = GTK_MOZ_EMBED_FLAG_ALLCHROME;

  gtk_moz_embed_set_chrome_mask(GTK_MOZ_EMBED(newBrowser->mozEmbed),
                                aChromeMask);

  setSize(newBrowser, 
          BROWSER_WINDOW_WIDTH, 
          BROWSER_WINDOW_HEIGHT);

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

  return newBrowser;  
}

void initialiseBrowserArray()
{
  int i;

  // allocate memory
  gBrowserArray = (gtkEmbedBrowser **) malloc(sizeof(gtkEmbedBrowser *) * gMaxWin);
  // create browser
  for (i = 0; i < gMaxWin; i++)
    gBrowserArray[i] = createNewBrowser(GTK_MOZ_EMBED_FLAG_DEFAULTCHROME, i + 1);
}

void setSize(gtkEmbedBrowser *aBrowser, int aWidth, int aHeight)
{
  if (!aBrowser || aWidth <= 0 || aHeight <= 0)
    return;

  // have to set all of our widgets
  gtk_widget_set_usize(aBrowser->mozEmbed, aWidth, aHeight);
  gtk_widget_set_usize(aBrowser->topLevelVBox, aWidth, aHeight);
  gtk_widget_set_usize(aBrowser->topLevelWindow, aWidth, aHeight);
}

void setPosition(gtkEmbedBrowser *aBrowser, int aX, int aY)
{
  if (!aBrowser || aX < 0 || aY < 0)
    return;

  gtk_widget_set_uposition(aBrowser->topLevelWindow, aX, aY);
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
