#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "handler.h"

int accept_command(char *input, char **output)
{
  if (!output)
    return -1;

  *output = (char *) malloc(sizeof(char) * (strlen(input) + 1));
  strcpy(*output, input);

  printf("%s\n", input);

  return 0;
}
