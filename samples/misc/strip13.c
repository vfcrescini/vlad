#include <stdio.h>

int main() {
  int temp;
  while ((temp = getc(stdin)) != EOF) {
    if (temp != 13)
      printf("%c", temp);
  }
  return 0;
}
