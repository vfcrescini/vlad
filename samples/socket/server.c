/*
 * server.c
 * Vino Crescini
 * 19 July 2001
 */

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define SOCKET_SRV_BACKLOG        5
#define SOCKET_SRV_MAXMESGLEN     1024

#define SOCKET_SRV_ERR_NOFILE     -1
#define SOCKET_SRV_ERR_NOSOCKET   -2
#define SOCKET_SRV_ERR_CANTBIND   -3
#define SOCKET_SRV_ERR_CANTLISTEN -4
#define SOCKET_SRV_ERR_CANTWRITE  -5
#define SOCKET_SRV_ERR_CANTREAD   -6
#define SOCKET_SRV_ERR_BADFD      -7

#define SOCKET_SRV_MAX(x,y) (((x) >= (y)) ? (x) : (y))

int copyBytes(int, int);
int checkSocket(int);
int acceptConnection(int);
int createSocket(char *);
int transferData(int);

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
  if ((clientFD = acceptConnection(serverFD)) < 0)
    return SOCKET_SRV_ERR_NOSOCKET;

  return transferData(clientFD);
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

// copy bytes from source to target
// copies SOCKET_SRV_MAXMESGLEN bytes 
int copyBytes(int aSourceFD, int aTargetFD)
{
  int tempByteCount;
  char tempBytes[SOCKET_SRV_MAXMESGLEN];

  // check FD's
  if (aSourceFD < 0 || aTargetFD < 0)
    return SOCKET_SRV_ERR_BADFD;

  if ((tempByteCount = read(aSourceFD, tempBytes, SOCKET_SRV_MAXMESGLEN)) <= 0)
    return SOCKET_SRV_ERR_CANTREAD; 
  
  if (write(aTargetFD, tempBytes, tempByteCount) <= 0)
    return SOCKET_SRV_ERR_CANTWRITE;

  return 0;
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

  // check client
  if (!checkSocket(aClientFD))
    return SOCKET_SRV_ERR_NOSOCKET;

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
      if (copyBytes(aClientFD, fileno(stdout)) < 0)
        break;
    }
    if (FD_ISSET(fileno(stdin), &readFDSet)) {
      // outgoing message
      if (copyBytes(fileno(stdin), aClientFD) < 0)
        break;
    }
  }

  return 0;
}
