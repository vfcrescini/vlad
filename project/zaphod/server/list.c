#include <stdlib.h>
#include <string.h>
#include "list.h"

typedef struct zaphod_list_node
{
  char *message;
  struct zaphod_list_node *next;
   
} zaphod_list_node;

typedef struct zaphod_list_node *zaphod_list_type;

static int zaphod_list_count = 0;
static zaphod_list_type zaphod_list = NULL;

int init()
{
  zaphod_list_count = 0;
  zaphod_list = NULL;

  return 0;
}

int insert(const char *message)
{
  zaphod_list_type new_node = NULL;

  /* check message */
  if (!message || strlen(message) <= 0)
    return -1;

  /* allocate space for the new node */
  if (!(new_node = (zaphod_list_type) malloc(sizeof(zaphod_list_node))))
    return -1;

  /* populate new node */
  new_node->next = zaphod_list;
  new_node->message = (char *) malloc(sizeof(char) * (strlen(message) + 1));

  if (!new_node->message) {
    free(new_node);
    return -1;
  }

  strcpy(new_node->message, message);

  /* make the new node the new head of list */
  zaphod_list = new_node;

  /* increment the counter */
  zaphod_list_count++;

  return 0;
}

int remove(int n)
{
  int i = 1;
  zaphod_list_type curr_list = zaphod_list;
  zaphod_list_type prev_list = NULL;

  /* check if we have enough messages */
  if (n <= 0 || n > zaphod_list_count)
    return -1;

  /* go through the list to find the one to delete */
  while(curr_list) {
    if (i == n) {
      /* found it */
      if (!prev_list)
        zaphod_list = zaphod_list->next;
      else
        prev_list->next = curr_list->next;

      /* decrement the counter and free allocations */
      zaphod_list_count--;
      free(curr_list->message);
      free(curr_list);

      return 0;

    }
    prev_list = curr_list;
    curr_list = curr_list->next;
    i++;
  }
  return -1;
}

int get(int n, char **message)
{
  int i = 1;
  zaphod_list_type curr_list = zaphod_list;

  if (n <= 0 || n > zaphod_list_count || !message)
    return -1;

  /* find the required message */
  while(curr_list) {
    if (i == n) {
      /* found it */
      *message = (char *) malloc(sizeof(char) * ((strlen(curr_list->message) + 1)));

      if (!*message)
        return -1;

      /* copy the message */
      strcpy(*message, curr_list->message);

      return 0;
    }
    curr_list = curr_list->next;
    i++;
  }

  return -1;
}

int count()
{
  return zaphod_list_count;
}
