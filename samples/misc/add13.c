#include <stdio.h>

int main() {
  int temp;
  while ((temp = getc(stdin)) != EOF) {
    if (temp != 10)
      printf("%c", temp);
    else
      printf("%c%c", 10, 13);	
  }
  return 0;
}
