#include <stdio.h>
#include "list.h"

int main()
{
  char xxx[] = "XXX";
  char yyy[] = "YYY";
  char zzz[] = "ZZZ";
  void *tmp = NULL;

  list_type list;
  int i;
  int in;

  list_init(&list);
  list_add(&list, (void *) xxx, &in);
  printf("%d\n", in);
  list_add(&list, (void *) yyy, &in);
  printf("%d\n", in);
  list_add(&list, (void *) zzz, &in);
  printf("%d\n", in);

  in = 69;

  for (i = 0; i < 5; i++) {
    list_get(list, i, &tmp);
    printf("%d=%s\n", i, (char *) tmp);
  }

  list_remove(&list, 0);
  printf("remove\n");

  for (i = 0; i < 5; i++) {
    list_get(list, i, &tmp);
    printf("%d=%s\n", i, (char *) tmp);
  }

  list_add(&list, xxx, &in);
  printf("add index=%d\n", in);

  for (i = 0; i < 5; i++) {
    list_get(list, i, &tmp);
    printf("%d=%s\n", i, (char *) tmp);
  }

  list_remove(&list, 2);
  printf("remove\n");

  for (i = 0; i < 5; i++) {
    list_get(list, i, &tmp);
    printf("%d=%s\n", i, (char *) tmp);
  }
  
  list_add(&list, zzz, &in);
  printf("add index=%d\n", in);

  for (i = 0; i < 5; i++) {
    list_get(list, i, &tmp);
    printf("%d=%s\n", i, (char *) tmp);
  }

  list_remove(&list, 1);
  printf("remove\n");

  for (i = 0; i < 5; i++) {
    list_get(list, i, &tmp);
    printf("%d=%s\n", i, (char *) tmp);
  }

  list_add(&list, yyy, &in);
  printf("add index=%d\n", in);

  for (i = 0; i < 5; i++) {
    list_get(list, i, &tmp);
    printf("%d=%s\n", i, (char *) tmp);
  }
  
  return 0;
}
