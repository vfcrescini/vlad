#include <stdio.h>

int main() {
  int temp;
  while ((temp = getc(stdin)) != EOF) {
    if (temp != 10)
      printf("%c", temp);
    else
      printf("%c%c", 13, 10);	
  }
  return 0;
}
