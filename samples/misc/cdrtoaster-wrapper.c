/*
 * cdrtoaster-wrapper.c
 * Vino Crescini
 * 4 Sept 2001
 */

#include <stdlib.h>

int main()
{
  return system("/usr/bin/setuid 0 cdrtoaster");
}
