/*
 * hwaddr.c
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
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>

int main(int argc, char *argv[])
{
  int fd;
  u_int8_t hw_addr[6];
  struct ifreq if_data;

  if (argc != 2) {
    fprintf(stderr, "usage: %s device\n", argv[0]);
    return -1;
  } 

  memset(&if_data, 0, sizeof(if_data));
  memset(hw_addr, 0, sizeof(hw_addr));

  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    return -1;

  strcpy(if_data.ifr_name, argv[1]);

  if (ioctl(fd, SIOCGIFHWADDR, &if_data) < 0)
    return -1;

  memcpy(hw_addr, if_data.ifr_hwaddr.sa_data, 6); 

  printf("%02X:%02X:%02X:%02X:%02X:%02X\n",
         hw_addr[0],
         hw_addr[1],
         hw_addr[2],
         hw_addr[3],
         hw_addr[4],
         hw_addr[5]);

  close(fd);

  return 0;
}
