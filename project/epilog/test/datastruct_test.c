/*
 * datastruct_test.c
 * Vino Crescini  <jcrescin@cit.uws.edu.au>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simplelist.h"

int cmp(void *p1, void *p2);
int fr(void *p1);
char *str_dup(const char *);

int main()
{
  simplelist_type slist;
  char *data;
  unsigned int i;
  unsigned int len;

  simplelist_init(&slist);

  simplelist_length(slist, &len);
  printf("length is %d\n",len);

  simplelist_add(&slist, (void *) str_dup("one"));
  simplelist_add(&slist, (void *) str_dup("two"));
  simplelist_add(&slist, (void *) str_dup("three"));

  simplelist_length(slist, &len);
  printf("length is %d\n", len);

  for (i = 0; i < len; i++) {
    simplelist_get_index(slist, i, (void **) &data);
    printf("i: %d, d: %s\n", i, (char *) data);
  }

  simplelist_get_data(slist, "two", (void **) &data, cmp);
  printf("two = %s\n", data);

  simplelist_del_data(&slist, "two", cmp, fr);
 
  simplelist_length(slist, &len);
  printf("length is %d\n", len);

  for (i = 0; i < len; i++) {
    simplelist_get_index(slist, i, (void **) &data);
    printf("i: %d, d: %s\n", i, (char *) data);
  }

  simplelist_del_index(&slist, 1, fr);

  simplelist_length(slist, &len);
  printf("length is %d\n", len);

  for (i = 0; i < len; i++) {
    simplelist_get_index(slist, i, (void **) &data);
    printf("i: %d, d: %s\n", i, (char *) data);
  }

  printf("find one: %d\n", simplelist_find_data(slist, (void *) "one", cmp));
  printf("find two: %d\n", simplelist_find_data(slist, (void *) "two", cmp));
  printf("find three: %d\n", simplelist_find_data(slist, (void *) "three", cmp));

  simplelist_del_index(&slist, 0, fr);

  simplelist_length(slist, &len);
  printf("length is %d\n", len);

  for (i = 0; i < len; i++) {
    simplelist_get_index(slist, i, (void **) &data);
    printf("i: %d, d: %s\n", i, (char *) data);
  }
  
  return 0;
}

int cmp(void *p1, void *p2)
{
  return strcmp((char *) p1, (char *) p2);
}

int fr(void *p1)
{
  free(p1);
  return 0;
}

char *str_dup(const char *str)
{
  char *tmp;
  
  if (str == NULL)
    return NULL;
  if ((tmp = (char *) malloc(sizeof(char) * strlen(str))) == NULL)
    return NULL;
  strcpy(tmp, str);
  return tmp;
}
