/*
 * client.c
 * Vino Crescini
 * 19 July 2001
 */

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define SOCKET_CLI_BACKLOG        5
#define SOCKET_CLI_MAXMESGLEN     1024

#define SOCKET_CLI_ERR_NOFILE     -1
#define SOCKET_CLI_ERR_NOSOCKET   -2
#define SOCKET_CLI_ERR_CANTWRITE  -3
#define SOCKET_CLI_ERR_CANTREAD   -4
#define SOCKET_CLI_ERR_BADFD      -5
#define SOCKET_CLI_ERR_NOCONNECT  -6

#define SOCKET_CLI_MAX(x,y) (((x) >= (y)) ? (x) : (y))

int createSocket(char *);
int checkSocket(int);
int copyBytes(int, int);
int transferData(int);

int main(int argc, char *argv[])
{
  int serverFD;

  if (argc != 2) {
    fprintf(stderr, "usage: %s <filename>\n", argv[0]);
    return SOCKET_CLI_ERR_NOFILE;
  }

  // set up the socket and make a connection
  if ((serverFD = createSocket(argv[1])) < 0)
    return SOCKET_CLI_ERR_NOSOCKET;

  return transferData(serverFD);
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
// copies SOCKET_CLI_MAXMESGLEN bytes
int copyBytes(int aSourceFD, int aTargetFD)
{
  int tempByteCount;
  char tempBytes[SOCKET_CLI_MAXMESGLEN];

  // check FD's 
  if (aSourceFD < 0 || aTargetFD < 0)
    return SOCKET_CLI_ERR_BADFD;
  
  if ((tempByteCount = read(aSourceFD, tempBytes, SOCKET_CLI_MAXMESGLEN)) <= 0)
    return SOCKET_CLI_ERR_CANTREAD;
  
  if (write(aTargetFD, tempBytes, tempByteCount) <= 0)
    return SOCKET_CLI_ERR_CANTWRITE;

  return 0;
}

// create socket and make a connection
// returns the server fd
int createSocket(char *aFilename)
{
  int socketFD;
  struct sockaddr_un socketAddress;
    
  if (aFilename == NULL)
    return SOCKET_CLI_ERR_NOFILE;

  // create socket
  socketFD = socket(PF_UNIX, SOCK_STREAM, 0);

  if (!checkSocket(socketFD))
    return SOCKET_CLI_ERR_NOSOCKET;

  // initialise address
  memset(&socketAddress, 0, sizeof(socketAddress));
  socketAddress.sun_family = AF_UNIX;
  strncpy(socketAddress.sun_path, aFilename, sizeof(socketAddress.sun_path));

  // make a connection

  if (connect(socketFD,
          (struct sockaddr *) &socketAddress,
          SUN_LEN(&socketAddress)) < 0)

    return SOCKET_CLI_ERR_NOCONNECT;

  return socketFD;
}

int transferData(int aServerFD)
{
  fd_set readFDSet;

  // check server
  if (!checkSocket(aServerFD))
    return SOCKET_CLI_ERR_NOSOCKET;
  
  while (1) {
    // initialise the fd set
    FD_ZERO(&readFDSet);
    FD_SET(aServerFD, &readFDSet);
    FD_SET(fileno(stdin), &readFDSet);

    // block until there's an input
    select(SOCKET_CLI_MAX(fileno(stdin), aServerFD) + 1,
           &readFDSet,
           NULL,
           NULL,
           NULL);

    if (FD_ISSET(aServerFD, &readFDSet)) {
      // incoming message
      if (copyBytes(aServerFD, fileno(stdout)) < 0)
        break;
    }
    if (FD_ISSET(fileno(stdin), &readFDSet)) {
      // outgoing message
      if (copyBytes(fileno(stdin), aServerFD) < 0)
        break;
    }
  }

  return 0;
}
