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
#include <stdio.h>
#include <ctype.h>

#define SOCKET_SRV_BACKLOG             5
#define SOCKET_SRV_MAXMESGLEN          1024

#define SOCKET_SRV_ERR_NOFILE          -1
#define SOCKET_SRV_ERR_NOSOCKET        -2
#define SOCKET_SRV_ERR_CANTBIND        -3
#define SOCKET_SRV_ERR_CANTLISTEN      -4
#define SOCKET_SRV_ERR_CANTWRITE       -5
#define SOCKET_SRV_ERR_CANTREAD        -6
#define SOCKET_SRV_ERR_BADFD           -7

#define SOCKET_SRV_DELIMITER_UI        ' '
#define SOCKET_SRV_DELIMITER_UI_FIELD  '|'
#define SOCKET_SRV_DELIMITER_UI_TUPPLE '!'
#define SOCKET_SRV_DELIMITER           '\004'
#define SOCKET_SRV_DELIMITER_FIELD     '\002'
#define SOCKET_SRV_DELIMITER_TUPPLE    '\003'
#define SOCKET_SRV_TERMINATOR          '\000'

#define SOCKET_SRV_OUTPUTPREFIX        "[Received] "

#define SOCKET_SRV_MAX(x,y)            (((x) >= (y)) ? (x) : (y))

int checkSocket(int);
int acceptConnection(int);
int createSocket(char *);
int transferData(int);
void transformToUI(char *, char *);
void transformFromUI(char *, char *);

int main(int argc, char *argv[])
{
  int serverFD;
  int clientFD;

  if (argc != 2) {
    fprintf(stderr, "usage: %s <filename>\n", argv[0]);
    return SOCKET_SRV_ERR_NOFILE;
  }

  // set up the socket
  if ((serverFD = createSocket(argv[1])) < 0)
    return SOCKET_SRV_ERR_NOSOCKET;

  // wait for client connection
  while(1) {
    if ((clientFD = acceptConnection(serverFD)) < 0)
      continue;

    if (transferData(clientFD) < 0)
      continue;
  }
}

int checkSocket(int aInputFD)
{
  struct stat tempStat;

  if (aInputFD < 0)
    return 0;

  if (!fstat(aInputFD, &tempStat))
    return S_ISSOCK(tempStat.st_mode);

  return 0;
}

// assumes aTarget has enough mem allocation
// convert: DELIMITER        --> space
//          FIELD_DELIMITER  --> , 
//          TUPPLE_DELIMITER --> ;
void transformToUI(char *aSource, char *aTarget)
{
  char tempBuffer[SOCKET_SRV_MAXMESGLEN];
  char *tempBufferPtr = tempBuffer;
  char *tempSourcePtr = aSource;

  memset(tempBuffer, 0, SOCKET_SRV_MAXMESGLEN);
  memset(aTarget, 0, SOCKET_SRV_MAXMESGLEN);

  // copy and replace spaces
  while (*tempSourcePtr != '\0') {
    if (*tempSourcePtr == SOCKET_SRV_TERMINATOR)
      break;
    else if (*tempSourcePtr == SOCKET_SRV_DELIMITER) {
      *tempBufferPtr++ = SOCKET_SRV_DELIMITER_UI;
      tempSourcePtr++;
    }
    else if (*tempSourcePtr == SOCKET_SRV_DELIMITER_FIELD) {
      *tempBufferPtr++ = SOCKET_SRV_DELIMITER_UI_FIELD;
      tempSourcePtr++;
    }
    else if (*tempSourcePtr == SOCKET_SRV_DELIMITER_TUPPLE) {
      *tempBufferPtr++ = SOCKET_SRV_DELIMITER_UI_TUPPLE;
      tempSourcePtr++;
    }
    else
      *tempBufferPtr++ = *tempSourcePtr++;
  }
    
  // add null-char
  *tempBufferPtr = '\0';

  // add cosmetic stuff
  sprintf(aTarget, 
          "%s%s\n", 
          SOCKET_SRV_OUTPUTPREFIX,
          tempBuffer);
}

// assumes aTarget has enough mem allocation
// convert: space --> DELIMITER
//          ,     --> FIELD_DELIMITER
//          ;     --> TUPPLE_DELIMITER
void transformFromUI(char *aSource, char *aTarget)
{
  char *tempTargetPtr = aTarget;
  char *tempSourcePtr = aSource;
  
  memset(aTarget, 0, SOCKET_SRV_MAXMESGLEN);

  // convert space to DELIM
  while (*tempSourcePtr != '\0') { 
    if (*tempSourcePtr == '\n')
      break;
    else if (*tempSourcePtr == SOCKET_SRV_DELIMITER_UI) {
      *tempTargetPtr++ = SOCKET_SRV_DELIMITER;
      tempSourcePtr++;
    }
    else if (*tempSourcePtr == SOCKET_SRV_DELIMITER_UI_FIELD) {
      *tempTargetPtr++ = SOCKET_SRV_DELIMITER_FIELD;
      tempSourcePtr++;
    }
    else if (*tempSourcePtr == SOCKET_SRV_DELIMITER_UI_TUPPLE) {
      *tempTargetPtr++ = SOCKET_SRV_DELIMITER_TUPPLE;
      tempSourcePtr++;
    }
    else
      *tempTargetPtr++ = *tempSourcePtr++;
  }

  // add terminator
  *tempTargetPtr = SOCKET_SRV_TERMINATOR;
  // if the terminator is not \000, append it
#if (SOCKER_SRV_TERMINATOR != '\000')
  *(tempTargetPtr + 1) = '\000';
#endif
}

// create, bind and set up socket for listening
// returns socket FD
int createSocket(char *aFilename)
{
  int socketFD;
  struct sockaddr_un serverAddress;

  if (aFilename == NULL)
    return SOCKET_SRV_ERR_NOFILE;

  // delete file if it exists
  unlink(aFilename);

  // create socket
  socketFD = socket(PF_UNIX, SOCK_STREAM, 0);

  if (!checkSocket(socketFD))
    return SOCKET_SRV_ERR_NOSOCKET;

  // initialise address
  memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sun_family = AF_UNIX;
  strncpy(serverAddress.sun_path, aFilename, sizeof(serverAddress.sun_path));

  // bind the socket
  if ((bind(socketFD, (struct sockaddr *) &serverAddress, 
            SUN_LEN(&serverAddress))) < 0) 
    return SOCKET_SRV_ERR_CANTBIND;

  // listen to the socket
  if ((listen(socketFD, SOCKET_SRV_BACKLOG)) < 0 ) 
    return SOCKET_SRV_ERR_CANTLISTEN;

  return socketFD;
}

// wait for a client to connect
// returns client FD
int acceptConnection(int aServerFD)
{
  int clientFD;
  struct sockaddr_un serverAddress;
  socklen_t socketLength;

  // check server socket
  if (!checkSocket(aServerFD))
    return SOCKET_SRV_ERR_NOSOCKET;

  // accept one connection
  clientFD = accept(aServerFD, 
                    (struct sockaddr *) &serverAddress, 
                    &socketLength);

  return clientFD;
}

int transferData(int aClientFD)
{
  fd_set readFDSet;
  char tempIncomingBuf[SOCKET_SRV_MAXMESGLEN];
  char *tempIncomingPtr;
  char tempOutgoingBuf[SOCKET_SRV_MAXMESGLEN];
  char *tempOutgoingPtr;
  char tempString[SOCKET_SRV_MAXMESGLEN];
  char tempChar;

  // check client
  if (!checkSocket(aClientFD))
    return SOCKET_SRV_ERR_NOSOCKET;

  // init buffers
  memset(tempIncomingBuf, 0, SOCKET_SRV_MAXMESGLEN);
  memset(tempOutgoingBuf, 0, SOCKET_SRV_MAXMESGLEN);
  tempIncomingPtr = tempIncomingBuf;
  tempOutgoingPtr = tempOutgoingBuf;

  while (1) {
    // initialise the fd set
    FD_ZERO(&readFDSet);
    FD_SET(aClientFD, &readFDSet);
    FD_SET(fileno(stdin), &readFDSet);

    // block until there's an input
    select(SOCKET_SRV_MAX(fileno(stdin), aClientFD) + 1, 
           &readFDSet, 
           NULL, 
           NULL, 
           NULL);

    if (FD_ISSET(aClientFD, &readFDSet)) {
      // incoming message
      if (read(aClientFD, &tempChar, 1) <= 0)
        break;
      if (strlen(tempIncomingBuf) >= SOCKET_SRV_MAXMESGLEN)
        break;
      if ((*tempIncomingPtr++ = tempChar) == SOCKET_SRV_TERMINATOR ||
          tempChar == '\0') {
        // message complete
        memset(tempString, 0, SOCKET_SRV_MAXMESGLEN);
        transformToUI(tempIncomingBuf, tempString);
        write(fileno(stdout), tempString, strlen(tempString));
        // flush buffer
        tempIncomingPtr = tempIncomingBuf;
        memset(tempIncomingBuf, 0, SOCKET_SRV_MAXMESGLEN);
      }
    }
    if (FD_ISSET(fileno(stdin), &readFDSet)) {
      // outgoing message
      if (read(fileno(stdin), &tempChar, 1) <= 0)
        break;
      if (strlen(tempOutgoingBuf) >= SOCKET_SRV_MAXMESGLEN)
        break;
      if ((*tempOutgoingPtr++ = tempChar) == '\n' ||
          tempChar == '\0') {
        // message complete
        memset(tempString, 0, SOCKET_SRV_MAXMESGLEN);
        transformFromUI(tempOutgoingBuf, tempString);
#if (SOCKET_SRV_TERMINATOR == '\000')
        write(aClientFD, tempString, strlen(tempString) + 1);
#else
        write(aClientFD, tempString, strlen(tempString));
#endif
        // flush buffer
        tempOutgoingPtr = tempOutgoingBuf;
        memset(tempOutgoingBuf, 0, SOCKET_SRV_MAXMESGLEN);
      }
    }
  }
  return 0;
}
