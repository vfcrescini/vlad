/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is Goanna.
 *
 * The Initial Developer of the Original Code is Tuxia
 * Labs PTY LTD.  Portions created by Tuxia Labs 
 * PTY LTD are Copyright (C) 2001 Tuxia Labs PTY LTD. All
 * Rights Reserved.
 *
 * Contributor(s):
 *   Vino Crescini <vino.crescini@tuxia.com>
 */

#include <stdlib.h>
#include <string.h>
#include "gtkEmbedParamList.h"

#ifdef DEBUG
#include <stdio.h>
#endif

gtkEmbedParamListNode::gtkEmbedParamListNode()
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedParamListNode::gtkEmbedParamListNode()\n");
#endif

  gField = NULL;
  gValue = NULL;
  gNext  = NULL;
}

gtkEmbedParamListNode::~gtkEmbedParamListNode()
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedParamListNode::~gtkEmbedParamListNode()\n");
#endif

  if (gField)
    free(gField);
  if (gValue)
    free(gValue);
}

bool gtkEmbedParamListNode::Replace(const char *aValue)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedParamListNode::Replace()\n");
#endif

  if (gValue)
    free(gValue);

  if ((gValue = (char *) malloc(sizeof(char) * (strlen(aValue) + 1))))
    strcpy(gValue, aValue);
  else
    return false;

  return true;
}

bool gtkEmbedParamListNode::Set(const char *aField, const char *aValue)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedParamListNode::Set()\n");
#endif

  if ((gField = (char *) malloc(sizeof(char) * (strlen(aField) + 1))))
    strcpy(gField, aField);
  else
    return false;

  if ((gValue = (char *) malloc(sizeof(char) * (strlen(aValue) + 1))))
    strcpy(gValue, aValue);
  else
    return false;

  return true;
}

bool gtkEmbedParamListNode::Get(char **aField, char **aValue)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedParamListNode::Get()\n");
#endif

  if (aField) {
    if ((*aField = (char *) malloc(sizeof(char) * (strlen(gField) + 1))))
      strcpy(*aField, gField);
    else
      return false;
  }

  if (aValue) {
    if ((*aValue = (char *) malloc(sizeof(char) * (strlen(gValue) + 1))))
      strcpy(*aValue, gValue);
    else
      return false;
  }

  return true;
}

gtkEmbedParamList::gtkEmbedParamList()
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedParamList::gtkEmbedParamList()\n");
#endif

  gHead = NULL;
  gTail = NULL;
  gSize = 0;
}

gtkEmbedParamList::~gtkEmbedParamList()
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedParamList::~gtkEmbedParamList()\n");
#endif

  while (Pop(NULL, NULL));
}

void gtkEmbedParamList::Init()
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedParamList::Size()\n");
#endif

  while(Pop(NULL, NULL));
}

int gtkEmbedParamList::Size()
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedParamList::Size()\n");
#endif

  return gSize;
}

bool gtkEmbedParamList::Push(const char *aField, const char *aValue)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedParamList::Push()\n");
#endif

  gtkEmbedParamListNode *newNode = new gtkEmbedParamListNode();

  if (!newNode || !newNode->Set(aField, aValue))
    return false;

  newNode->gNext = NULL;

  if (gTail) {
    gTail->gNext = newNode;
    gTail = newNode;
  }
  else {
    gHead = newNode;
    gTail = newNode;
  }

  gSize++;

  return true;
}

bool gtkEmbedParamList::Pop(char **aField, char **aValue)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedParamList::Pop()\n");
#endif

  gtkEmbedParamListNode *tempNode;

  if (!gHead)
    return false;

  tempNode = gHead;
  gHead    = gHead->gNext;

  if (!tempNode->Get(aField, aValue))
    return false;

  gSize--;
  delete tempNode;

  return true;
}

bool gtkEmbedParamList::Get(int aIndex, char **aField, char **aValue)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedParamList::Get()\n");
#endif

  int i;

  // check the if the index if valid
  if (aIndex < 0 || aIndex > gSize)
    return false;

  gtkEmbedParamListNode *tempNode = gHead;

  // move the pointer to the index
  for (i = 0; i < aIndex && tempNode; i++)
    tempNode = tempNode->gNext;

  if (!tempNode)
    return false;

  // return values of that node
  return tempNode->Get(aField, aValue);
}

bool gtkEmbedParamList::Get(const char *aField, char **aValue)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedParamList::Get()\n");
#endif

  char *tempField = NULL;
  char *tempValue = NULL;
  gtkEmbedParamListNode *tempNode = gHead;

  while (tempNode) {
    tempNode->Get(&tempField, &tempValue);

    if (!strcmp(tempField, aField)) {
      *aValue = tempValue;
      if (tempField)
        free(tempField);
      return true;
    }

    tempNode = tempNode->gNext;

    if (tempField)
      free(tempField);
    if (tempValue)
      free(tempValue);
  }

  return false;
}

bool gtkEmbedParamList::Replace(const char *aField, const char *aValue)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedParamList::Replace()\n");
#endif

  char *tempField = NULL;
  char *tempValue = NULL;
  gtkEmbedParamListNode *tempNode = gHead;

  while (tempNode) {
    tempNode->Get(&tempField, &tempValue);

    if (!strcmp(tempField, aField)) {
      if (tempField)
        free(tempField);
      if (tempValue)
        free(tempValue);
      return tempNode->Replace(aValue);
    }

    tempNode = tempNode->gNext;

    if (tempField)
      free(tempField);
    if (tempValue)
      free(tempValue);
  }

  return false;
}

int gtkEmbedParamList::Find(const char *aField)
{
#ifdef DEBUG
  fprintf(stderr, "gtkEmbedParamList::Find()\n");
#endif

  char *tempField = NULL;
  gtkEmbedParamListNode *tempNode = gHead;
  int i;

  for (i = 0; i < gSize; i++) {
    if (tempNode->Get(&tempField, NULL)) {
      if (!strcmp(tempField, aField)) {
        free(tempField);
        return i;
      }
      tempNode = tempNode->gNext;
    }
  }
  return -1;
}
