#include <stdio.h>
#include "index.h"
#include "indexedlist.h"

int main()
{
  index_type index;
  indexedlist_type ilist;
  unsigned int x;
  char *data;
  int i;

  index_init(&index);
  index_add(&index, 1);
  index_add(&index, 2);
  index_add(&index, 3);
  index_get(&index, &x);
  printf("%lu\n", x);
  index_get(&index, &x);
  printf("%lu\n", x);
  index_get(&index, &x);
  printf("%lu\n", x);

  printf("XXXX\n");
  
  indexedlist_init(&ilist);
  indexedlist_add(&ilist, "one", &x);
  printf("%lu\n", x);
  indexedlist_add(&ilist, "two", &x);
  printf("%lu\n", x);
  indexedlist_add(&ilist, "three", &x);
  printf("%lu\n", x);
  indexedlist_add(&ilist, "four", &x);
  printf("%lu\n", x);

  printf("XXXX\n");
  
  for (i = 0; i < 4; i++) {
    indexedlist_get_data(ilist, &data, i);
    printf("%d %s\n", i, data);
  }

  printf("XXXX\n");

  indexedlist_get_index(ilist, "three", &x);
  printf("%lu\n", x);
  indexedlist_get_index(ilist, "one", &x);
  printf("%lu\n", x);
  indexedlist_get_index(ilist, "four", &x);
  printf("%lu\n", x);
  indexedlist_get_index(ilist, "two", &x);
  printf("%lu\n", x);

  printf("XXXX\n");

  indexedlist_del_index(&ilist, 2);
  indexedlist_del_index(&ilist, 1);
  
  printf("XXXX\n");
 
  for (i = 0; i < 4; i++) {
    indexedlist_get_data(ilist, &data, i);
    printf("%d %s\n", i, data);
    free(data);
    data = NULL;
  }

  printf("XXXX\n");

  indexedlist_add(&ilist, "dos", &x);
  printf("%lu\n", x);
  indexedlist_add(&ilist, "tres", &x);
  printf("%lu\n", x);

  printf("XXXX\n");
  
  for (i = 0; i < 4; i++) {
    indexedlist_get_data(ilist, &data, i);
    printf("%d %s\n", i, data);
    free(data);
    data = NULL;
  }

  printf("XXXX\n");

  indexedlist_del_data(&ilist, "four");
  indexedlist_del_data(&ilist, "tres");

  printf("XXXX\n");

  for (i = 0; i < 4; i++) {
    indexedlist_get_data(ilist, &data, i);
    printf("%d %s\n", i, data);
    free(data);
    data = NULL;
  }

  return 0;
}
