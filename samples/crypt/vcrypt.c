/*
 * vcrypt.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <crypt.h>

char *generate_salt();

int main(int argc, char *argv[])
{
  char *salt;
  char *string;

  if (argc <= 1) {
    fprintf(stderr, "usage: %s pass1 [pass2 [...]]\n", argv[0]);
    return -1;
  }

  srand((unsigned int) time(NULL));

  while(argc-- > 1) { 
    salt = generate_salt();
    string = crypt(argv[argc], salt);
    fprintf(stderr, "%s\n", string);
  }

  return 0;
}

char *generate_salt()
{
  int i;
  char *map = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ./";
  static char key[12];

  sprintf(key, "$1$");

  for(i = 0; i < 8; i++)
    key[i + 3] = map[rand() % strlen(map)];

  return key;
}
