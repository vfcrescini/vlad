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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <syslog.h>

#define RCMDD_PORT      514
#define RCMDD_MAXCLIENT 128
#define RCMDD_MAXSTRLEN 128
#define RCMDD_TIMEOUT   30
#define RCMDD_SYSLOGMSG "rcmdd"
#define RCMDD_CONFIG    "/etc/rcmdd.conf"

int init_daemon(int);
int accept_connection(int);
int make_connection(int);
void wait_for_child(int);
void drop_connection(int);
int read_from_fd(int, char [], int);
int check_command(char []);

int main(int aArgc, char *aArgv[])
{
  int tChildPID;
  int tServerFD;

  // fork the daemon
  tChildPID = fork();

  if (tChildPID == 0) {
    // child process

    // set up signal handler for exiting children
    signal(SIGCHLD, &wait_for_child);

    openlog(RCMDD_SYSLOGMSG, LOG_PID | LOG_ODELAY, LOG_DAEMON);

    syslog(LOG_INFO, "daemon started");

    // initialse the socket
    if ((tServerFD = init_daemon(RCMDD_PORT)) < 0)
      return -1;

    while(1)
      accept_connection(tServerFD);

    closelog();
    return 0;
  }
  else if (tChildPID == -1) {
    // fork failed
    fprintf(stderr, "falied to fork\n");
    return -1;
  }
  else {
    // fork successfull
    fprintf(stdout, "%d\n", tChildPID);
  }

  return 0;
}

// create, bind and listen to a socket for aPort
int init_daemon(int aPort)
{
  struct sockaddr_in tServerAddr;
  int tServerFD;

  // make socket
  if ((tServerFD = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    syslog(LOG_ERR, "failed to create a socket: %d", errno);
    return -1;
  }

  // init address
  memset(&tServerAddr, 0, sizeof(tServerAddr));
  tServerAddr.sin_family = AF_INET;
  tServerAddr.sin_port = htons(aPort);
  tServerAddr.sin_addr.s_addr = INADDR_ANY;
  memset(&(tServerAddr.sin_zero), 0, 8);

  // now bind the socket
  if (bind(tServerFD, (struct sockaddr *) &tServerAddr, sizeof(struct sockaddr)) < 0) {
    syslog(LOG_ERR, "failed to bind socket %d: %d", tServerFD, errno);
    return -1;
  }

  // listen for connections
  if (listen(tServerFD, RCMDD_MAXCLIENT) < 0) {
    syslog(LOG_ERR, "failed to listen to socket %d: %d", tServerFD, errno);
    return -1;
  }
  
  return tServerFD;
}

// wait for one connection
int accept_connection(int aServerFD)
{
  int tChildPID;
  int tClientFD;
  struct sockaddr_in tClientAddr;
  socklen_t tAddrLen;

  // check server fd
  if (aServerFD < 0) {
    syslog(LOG_ERR, "connection failed");
    return -1;
  }

  // init parameters
  tAddrLen = sizeof(tClientAddr);
  memset(&tClientAddr, 0, tAddrLen);

  // accept one connection
  if ((tClientFD = accept(aServerFD, &tClientAddr, &tAddrLen)) < 0) {
    syslog(LOG_ERR, "failed to accept connection: %d", errno);
    return -1;
  }

  // ok we got one connection, so f**k off (fork off)
  tChildPID = fork();

  if (tChildPID == 0) {
    // child process

    // register signal handler
    signal(SIGALRM, &drop_connection);

    make_connection(tClientFD);

    exit(0);
  }
  else if (tChildPID == -1) {
    // fork failure
    syslog(LOG_ERR, "failed to spawn a connection: %d", errno);
    return -1;
  }
  else { 
    // fork successful
    close(tClientFD);
  }
  return 0;
}

// connection is made so go through the rsh protocol
int make_connection(int aClientFD)
{
  char tString[RCMDD_MAXSTRLEN];
  int tStderrPort = 0;
  int tStderrFD = 0;
  int tLPort;
  struct sockaddr_in tClientStderrAddr;
  size_t tClientAddrLen;

  // check clientFD
  if (aClientFD < 0) {
    syslog(LOG_ERR, "connection failed");
    return -1;
  }

  // begin protocol timer
  alarm(RCMDD_TIMEOUT);

  // read stderr port
  memset(tString, 0, RCMDD_MAXSTRLEN);
  read_from_fd(aClientFD, tString, RCMDD_MAXSTRLEN);

  if ((tStderrPort = atoi(tString)) < 0 || tStderrPort > IPPORT_RESERVED) {
    syslog(LOG_ERR, "invalid stderr port");
    return -1;
  }
  else if (tStderrPort != 0) {
    // nonzero so we have to make a connection
    tLPort = IPPORT_RESERVED - 1;
    if ((tStderrFD = rresvport(&tLPort)) < 0) {
      syslog(LOG_ERR, "failed to create socket: %d", errno);
      return -1;
    }

    // get our address
    tClientAddrLen = sizeof(tClientStderrAddr);
    memset(&tClientStderrAddr, 0, tClientAddrLen);

    if (getpeername(aClientFD, (struct sockaddr *)&tClientStderrAddr, &tClientAddrLen) < 0) {
      syslog(LOG_ERR, "failed to retrieve address: %d", errno);
      return -1;
    }

    // set stderr port
    tClientStderrAddr.sin_port = htons(tStderrPort);

    // now make the connection
    if (connect(tStderrFD, (struct sockaddr_in *) &tClientStderrAddr, sizeof(tClientStderrAddr)) < 0) {
      syslog(LOG_ERR, "failed to connect to %d: %d", tStderrPort, errno);
      return -1;
    }
  }

  // read remote username
  memset(tString, 0, RCMDD_MAXSTRLEN);
  read_from_fd(aClientFD, tString, RCMDD_MAXSTRLEN);

  // read local username
  memset(tString, 0, RCMDD_MAXSTRLEN);
  read_from_fd(aClientFD, tString, RCMDD_MAXSTRLEN);

  // read command
  memset(tString, 0, RCMDD_MAXSTRLEN);
  read_from_fd(aClientFD, tString, RCMDD_MAXSTRLEN);

  // now send in a zero to complete the protocol
  write(aClientFD, "\0", 1);
  if (tStderrPort > 0)
    write(tStderrFD, "\0", 1);

  // clear timeout
  alarm(0);

  // close all the streams
  close(fileno(stdin));
  close(fileno(stdout));
  close(fileno(stderr));
  
  // redirect the streams
  dup2(aClientFD, fileno(stdin));
  dup2(aClientFD, fileno(stdout));

  if (tStderrFD >= 0)
    dup2(tStderrFD, fileno(stderr));
  else
    dup2(aClientFD, fileno(stderr));

  // check if the given command is valid
  if (!check_command(tString)) {
    fprintf(stderr, "permission denied: %s\n", tString);
    return -1;
  }

  // finally, exec the command
  execlp(tString, "");

  // if we're still here, there is an error
  fprintf(stderr, "exec error: %s\n", tString);

  return -1;
}

void wait_for_child(int aSignal)
{
  // wait for any child but don't block
  waitpid(-1, NULL, WNOHANG);
}

void drop_connection(int aSignal)
{
  switch(aSignal) {
    case SIGALRM :
      syslog(LOG_WARNING, "connection timed out");
      exit(-1);
    default :
  }
}

// read from fd until we find a '\0' character, until read fails or 
// until we fill up the space. return number of chars read
int read_from_fd(int aFD, char aString[], int aLength)
{
  int i = 0;

  if (aFD < 0)
    return 0;

  while(read(aFD, &(aString[i]), 1) == 1 && i < aLength) {
    if (aString[i] == 0)
      return i;
    i++;
  }

  return i;
}

// return 1 if command is valid, as specified in the config file
// strict config file format:
//   one command per line
//   no arguments/parameters
//   lines must be no more than RCMDD_MAXSTRLEN
int check_command(char aCommand[])
{
  FILE *tConfigFile;
  char tString[RCMDD_MAXSTRLEN];
  char *tStringPtr = tString;
  int tRetVal = 0;
  int i;

  if (!aCommand)
    return 0;

  // open the config file
  if ((tConfigFile = fopen(RCMDD_CONFIG, "r")) == NULL)
    return 0;

  // go through the file a line at a time
  while (fgets(tStringPtr, RCMDD_MAXSTRLEN, tConfigFile)) {
    // clear out the whitespace before the string
    while (*tStringPtr && isspace(*tStringPtr))
      tStringPtr++;

    // check if the string is still not empty
    if (strlen(tStringPtr) <= 0)
      continue;

    // clear out the comment (if any)
    for (i = 0; i < strlen(tStringPtr); i++) {
      if (tStringPtr[i] == '#')
        tStringPtr[i] = 0;
    }

    // check if the string is still not empty
    if (strlen(tStringPtr) <= 0)
      continue;

    // clear out the whitespace after the string
    for (i = strlen(tStringPtr) - 1; i >= 0; i--) {
      if (isspace(tStringPtr[i]))
        tStringPtr[i] = 0;
      else
        break;
    }

    // now compare it with the command
    if ((tRetVal = !strcmp(tStringPtr, aCommand)))
      break;
  }

  fclose(tConfigFile);

  return tRetVal;
}
