#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <netdb.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define ZAPHODD_TCPPORT    4321
#define ZAPHODD_MAXCLIENTS 32
#define ZAPHODD_TIMEOUT    5

int init_socket();
int wait_connect(int server_fd);
int serve_connect(int client_fd);

void drop_connection(int sig);
void wait_child(int sig);

int main()
{
  int pid;

  /* fork off */
  if ((pid = fork()) < 0) {
    /* error */
    fprintf(stderr, "-1");
    return -1;
  }
  else if (pid > 0) {
    /* parent */
    fprintf(stderr, "%d\n", pid);
    return 0;
  }
  else {
    /* child */
    int server_fd;

    setsid();
    chdir("/");
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
 
    if ((server_fd = init_socket()) < 0)
      return -1;

    while(1)
      wait_connect(server_fd);
  }

  return 0;
}

/* connection timed out*/
void drop_connection(int sig)
{
  /* exit */
  exit(0);
}

/* wait for child */
void wait_child(int sig)
{
  /* wait but do not block */
  waitpid(-1, NULL, WNOHANG);
}

/* create and bind a socket, returns server FD */
int init_socket()
{
  struct sockaddr_in server_addr;
  int server_fd;

  /* create tcp socket */
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    return -1;

  /* init address */
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(ZAPHODD_TCPPORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  memset(&(server_addr.sin_zero), 0, 8);

  /* bind socket */
  if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr)) < 0)
    return -1;

  /* listen for connections */
  if (listen(server_fd, ZAPHODD_MAXCLIENTS) < 0)
    return -1;

  return server_fd; 
}

/* wait for one connection */
int wait_connect(int server_fd)
{
  int pid;
  int client_fd;
  struct sockaddr client_addr;
  socklen_t addr_len;

  /* check server fd */
  if (server_fd < 0)
    return -1;

  /* init params */
  addr_len = sizeof(client_addr);
  memset(&client_addr, 0, addr_len);

  /* register the sigchld handler */
  signal(SIGCHLD, &wait_child);

  /* wait until we get one connection */
  if ((client_fd = accept(server_fd, &client_addr, &addr_len)) < 0)
    return -1;

  /* connection accepted, so fork off into a new process */
  if ((pid = fork()) < 0) {
    /* error */
    return -1;
  }
  else if (pid > 0) {
    /* parent */
    close(client_fd);
    return 0;
  }
  else {
    /* child */

    /* register timeout handler */
    signal(SIGALRM, &drop_connection);

    /* now serve the connection */
    serve_connect(client_fd);

    exit(0);
  }
  
  return 0;
}

int serve_connect(int client_fd)
{
  /* check client fd */
  if (client_fd < 0)
    return -1;

  /* begin timer */
  alarm(ZAPHODD_TIMEOUT);

  /* send a string */
  write(client_fd, (void *) "test successful\n", 17);

  /* clear alarm */
  alarm(0);

  /* close socket */
  close(client_fd);

  return 0;
}
