/*
 * portscan.c
 * 27 Feb 2002
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU CC; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 * 
 * Copyright (C) 2002 Vino Crescini
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

#define PS_DEFAULT_MIN 1
#define PS_DEFAULT_MAX 1024
#define PS_MIN_PORT    1
#define PS_MAX_PORT    65536

unsigned long int get_ip(char *);

int main(int argc, char *argv[])
{
  int socket_fd;
  int server_fd;
  struct sockaddr_in server_address;
  struct servent *port_service;
  unsigned long int server_ip;
  int option = 0;
  int verbose = 0;
  int max_port = PS_DEFAULT_MAX;
  int min_port = PS_DEFAULT_MIN;
  int i;

  // first, evaluate the options
  opterr = 0;

  while ((option = getopt(argc, argv, "vhs:e:")) != -1) {
    switch(option) {
      case 'v' :
        verbose = 1;
        break;
      case 'h' :
       fprintf(stderr, "usage: %s [-v] [-s start] [-e end] ip-addr\n", argv[0]);
       fprintf(stderr, "       %s -h\n", argv[0]);
       return 0;
      case 's' :
        min_port = (optarg) ? atoi(optarg) : 0;
        break;
      case 'e' :
        max_port = (optarg) ? atoi(optarg) : 0;
        break;
      default :
       fprintf(stderr, "usage: %s [-v] [-s start] [-e end] ip-addr\n", argv[0]);
       fprintf(stderr, "       %s -h\n", argv[0]);
       return -1;
    }
  }

  // check port range
  if (min_port < PS_MIN_PORT || max_port > PS_MAX_PORT || max_port < min_port) {
    fprintf(stderr, "%d <= minport <= maxport <= %d\n", PS_MIN_PORT, PS_MAX_PORT);
    return -1;
  }

  // if address is null, exit
  if (!argv[optind]) {
    fprintf(stderr, "usage: %s [-v] ip-addr\n", argv[0]);
    fprintf(stderr, "       %s -h\n", argv[0]);
    return -1;
  }

  // get address
  if ((server_ip = get_ip(argv[optind])) == INADDR_NONE) {
    fprintf(stderr, "invalid ip address: %s\n", argv[optind]);
    return -1;
  }

  for (i = min_port; i <= max_port; i++) {
    // create socket
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      fprintf(stderr, "socket creation error: %d\n", errno);
      return -1;
    }

    // compose address
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;;
    server_address.sin_port = htons(i);
    server_address.sin_addr.s_addr = server_ip;

    // try to connect
    server_fd = connect(socket_fd, 
                        (struct sockaddr *)&server_address, 
                        sizeof(server_address));

    if (server_fd < 0) {
      // unable to connect so port is closed
      close(socket_fd);
      continue; 
    }
    else {
      // connected

      // get servicename
      if (verbose) {
        port_service = getservbyport(htons(i), "tcp");
        printf("%d:%s\n", 
               i, 
               port_service ? port_service->s_name : "?");
      }
      else
        printf("%d\n", i);

      // cleanup
      close(server_fd);
      close(socket_fd);
    }
  }

  return 0; 
}

unsigned long int get_ip(char *address)
{
  struct hostent *ipaddress;

  if (!address)
    return INADDR_NONE;

  if (*address >= '0' && *address <= '9')
    return inet_addr(address);
  else {
    if (!(ipaddress = gethostbyname(address)))
      return INADDR_NONE;
    return ((struct in_addr *) ipaddress->h_addr_list[0])->s_addr;
  }
}
