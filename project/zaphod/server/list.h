#ifndef ZAPHOD_LIST
#define ZAPHOD_LIST

int init();
int insert(const char *message);
int remove(int n);
int get(int n, char **message);
int count();

#endif
