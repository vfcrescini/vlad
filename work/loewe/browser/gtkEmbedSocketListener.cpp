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

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include "nscore.h"
#include "prthread.h"
#include "gtkEmbedSocketListener.h"

#define SOCK_LISTENER_MAX(x,y) ((x)>(y)?(x):(y))
#define SOCK_LISTENER_MAXLEN   1024

static PRThread *gLoopThread;
static int gServerFD;
static int gPipeOutgoing[2];
static int gPipeIncoming[2];
static bool CheckSocket(int aInputFD);
static void Listen(void *);

gtkEmbedSocketListener::gtkEmbedSocketListener()
{
  mFilename = NULL;
  gServerFD = nsnull;

  memset(gPipeIncoming, 0, sizeof(int) * 2);
  memset(gPipeOutgoing, 0, sizeof(int) * 2);

  gLoopThread = nsnull;
}

gtkEmbedSocketListener::~gtkEmbedSocketListener()
{
  // dealocate mFilename
  if (mFilename != NULL)
    free(mFilename);

  // close fd's
  
  if (gPipeIncoming[0] >= 0)
    close(gPipeIncoming[0]);
  if (gPipeIncoming[1] >= 0)
    close(gPipeIncoming[1]);
  if (gPipeOutgoing[0] >= 0)
    close(gPipeOutgoing[0]);
  if (gPipeOutgoing[1] >= 0)
    close(gPipeOutgoing[1]);
}

bool gtkEmbedSocketListener::Init(const char *aFilename)
{
  if (aFilename == NULL)
    return false;

  mFilename = (char *) malloc(sizeof(char) * (strlen(aFilename) + 1));
  strcpy(mFilename, aFilename);

  // create an outgoing pipe to send 
  //  messages to the server
  if (pipe(gPipeOutgoing) < 0)
    return false;

  // create an incoming pipe to receive 
  // messages from the server
  if (pipe(gPipeIncoming) < 0)
    return false;

  if (!InitSocket())
    return false;

  return true;
}

int gtkEmbedSocketListener::GetIncomingFD()
{
  if (gPipeIncoming[0] < 0)
    return -1;

  return gPipeIncoming[0];
}

int gtkEmbedSocketListener::GetOutgoingFD()
{
  if (gPipeOutgoing[1] < 0)
    return -1;

  return gPipeOutgoing[1];
}

void gtkEmbedSocketListener::Start()
{
  if (gLoopThread == nsnull) {
    gLoopThread = PR_CreateThread(PR_USER_THREAD,
                                  &Listen,
                                  nsnull,
                                  PR_PRIORITY_NORMAL,
                                  PR_GLOBAL_THREAD,
                                  PR_JOINABLE_THREAD,
                                  0);
  }
}

void gtkEmbedSocketListener::Stop()
{
}

bool gtkEmbedSocketListener::InitSocket()
{
  struct sockaddr_un socketAddress;

  if (mFilename == NULL)
    return false;

  // create the socket
  gServerFD = socket(PF_UNIX, SOCK_STREAM, 0);

  if (!CheckSocket(gServerFD))
    return false;

  // initialise address
  memset(&socketAddress, 0, sizeof(socketAddress));
  socketAddress.sun_family = AF_UNIX;
  strcpy(socketAddress.sun_path, mFilename);

  // make a connection
  if (connect(gServerFD,
              (struct sockaddr *) &socketAddress,
              SUN_LEN(&socketAddress)) < 0)

    return false;

  return true;
}

static bool CheckSocket(int aInputFD)
{
  struct stat tempStat;

  if (aInputFD < 0)
    return false;

  // make sure it's a socket
  if (!fstat(aInputFD, &tempStat))
    return S_ISSOCK(tempStat.st_mode);

  return false;
}

static void PR_CALLBACK Listen(void *aData)
{
  fd_set readFDSet;
  char tempBuf[SOCK_LISTENER_MAXLEN];
  int tempByteCount;

  // check server
  if (!CheckSocket(gServerFD))
    return ;

  // check pipes
  if (gPipeOutgoing[0] < 0 || gPipeIncoming[1] < 0)
    return ;

  // loop indefinitely
  while (1) {
    // initialise the fd set
    FD_ZERO(&readFDSet);
    FD_SET(gServerFD, &readFDSet);
    FD_SET(gPipeOutgoing[0], &readFDSet);

    // block until there's an input
    select(SOCK_LISTENER_MAX(gServerFD, gPipeOutgoing[0]) + 1,
           &readFDSet,
           NULL,
           NULL,
           NULL);

    // init buffer
    memset(tempBuf, 0, SOCK_LISTENER_MAXLEN);

    if (FD_ISSET(gServerFD, &readFDSet)) {
      // incoming message
      // copy raw bytes from the socket to the incoming pipe
      tempByteCount = read(gServerFD, tempBuf, SOCK_LISTENER_MAXLEN);
      if (tempByteCount > 0)
        write(gPipeIncoming[1], tempBuf, tempByteCount);
    }

    // init buffer
    memset(tempBuf, 0, SOCK_LISTENER_MAXLEN);

    if (FD_ISSET(gPipeOutgoing[0], &readFDSet)) {
     // outgoing message
     // copy raw bytes from the outgoing pipe to the socket
      tempByteCount = read(gPipeOutgoing[0], tempBuf, SOCK_LISTENER_MAXLEN);
      if (tempByteCount > 0)
        write(gServerFD, tempBuf, tempByteCount);
    }
  }
  return;
}
