/*
 * google17.c - find n where f(n) == n, n > 1, f(n) returns the number of ones
 *              '1' required when writing out all numbers between 0 and n
 *              (google labs aptitude test)
 *
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 * 20 Sept 2004
 *
 */

#include <stdio.h>
#include <string.h>
#include <limits.h>

/* returns the number of 1's in n */
unsigned int digits(unsigned int n)
{
  char str[16];
  int i;
  int count = 0;

  sprintf(str, "%d", n);

  for (i = 0; i < strlen(str); i++)
    if (str[i] == '1')
      count++;

  return count;
}

int main()
{
  unsigned int i;
  unsigned long long int tot = 0;

  for (i = 2; i < INT_MAX; i++) {
    tot += digits(i);
    if (tot == i) {
      printf("f(%lu) = %lu\n", i, tot);
#ifndef SHOW_ALL
      break;
#endif
    }
  }
  return 0;
}
