/*
 * version.c
 * Vino Crescini
 * 31 December 2001
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// VERSION_BASE must be a power of 2
// VERSION_BASE_LEN is the number of bits required to represent VERSION_BASE

#define VERSION_BASE     1024
#define VERSION_BASE_LEN 10
#define VERSION_BASE_1   (VERSION_BASE - 1)
#define VERSION_BASE_2   (VERSION_BASE_1 << VERSION_BASE_LEN)
#define VERSION_BASE_3   (VERSION_BASE_2 << VERSION_BASE_LEN)

int indecrange(int);
int innumrange(unsigned long int);
int isnum(char *);
int extract(char *, int *, int *, int *);
void help(char *);

int main(int aCount, char *aArgs[])
{
  unsigned long int number = 0;
  int base1 = 0;
  int base2 = 0;
  int base3 = 0;

  if (aCount == 2 && !strcmp(aArgs[1], "-h"))
    help(aArgs[0]);
  else if (aCount == 3 && !strcmp(aArgs[1], "-n")) {
    // check if its valid
    if (!isnum(aArgs[2]))
      return -1;

    // convert string -> unsigned lomg
    number = strtoul(aArgs[2], NULL, 10);

    // check if within range
    if (!innumrange(number))
      return -1;

    // mask 'n shift
    printf("%lu.%lu.%lu\n",
           (number & VERSION_BASE_3) >> (VERSION_BASE_LEN << 1),
           (number & VERSION_BASE_2) >> VERSION_BASE_LEN,
           number & VERSION_BASE_1);
  } 
  else if (aCount == 3 && !strcmp(aArgs[1], "-d")) {
    // extract numbers
    if (!extract(aArgs[2], &base3, &base2, &base1))
      return -1;

    // check if valid
    if (!indecrange(base3) || !indecrange(base2) || !indecrange(base1))
      return -1;
 
    // compose number
    printf("%lu\n",
           (unsigned long)
           (base3 << (VERSION_BASE_LEN << 1)) +
           (base2 << VERSION_BASE_LEN) +
           base1); 
  } 
  else {
    help(aArgs[0]);
    return -2;
  }

  return 0;
}

int isnum(char *aString)
{
   char *tempString = aString;

   while (*tempString) {
     if (*tempString < '0' || *tempString > '9')
       return 0;
     tempString += sizeof(char);
   }

   return 1;
}

int indecrange(int aInt)
{
  return (aInt >= 0 && aInt < VERSION_BASE);
}

int innumrange(unsigned long int aInt)
{
  return (aInt & ~((VERSION_BASE << (VERSION_BASE_LEN << 1)) - 1)) ? 0 : 1;
}

int extract(char *aString, int *aBase3, int *aBase2, int *aBase1)
{
  return aBase3 &&
         aBase2 &&
         aBase1 &&
         (3 == sscanf(aString, "%d.%d.%d", aBase3, aBase2, aBase1));
}

void help(char *aName)
{
  printf("Usage:\n");
  printf("  %s -h\n", aName);
  printf("  %s -n number\n", aName);
  printf("  %s -d decimal\n", aName);
}