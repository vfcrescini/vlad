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

#include "gtkEmbedIDManager.h"

gtkEmbedIDManager::gtkEmbedIDManager(int aMaxID)
{
  gCurrentIndex = 0;
  gMaxID = 0;
  gIDCount = 0;
  int i;

  // set the new limit
  if (aMaxID > 0 && aMaxID < IDMANAGER_MAX_ID)
    gMaxID = aMaxID;  
  
  gIDCount = gMaxID;

  // allocate space for the array
  gIDArray = (int *) malloc(sizeof(int) * gMaxID);

  // initialise array to default 1 to gMaxID
  for (i = 0; i < gMaxID; i++)
    gIDArray[i] = i + 1;
}

gtkEmbedIDManager::~gtkEmbedIDManager()
{
  if (gIDArray != NULL)
    free(gIDArray);
}

int gtkEmbedIDManager::GetNextIndex()
{
  if (++gCurrentIndex >= gIDCount)
    gCurrentIndex = 0;

  return gIDArray[gCurrentIndex] - 1;
}

bool gtkEmbedIDManager::SetViews(char *aOrderList)
{
  char *tempList = aOrderList;
  char tempNum[IDMANAGER_MAX_DIGIT];
  char tempDigit;
  char *tempRef;
  int *tempArray;
  int i;
  
  if (!aOrderList) {
    return false;
  }

  // allocate space and initialise tempArray
  tempArray = (int *) malloc(sizeof(int) * gMaxID);
  memset(tempArray, 0, gMaxID);

  // parse the order list
  for (i = 0; i < gMaxID && *tempList != 0; i++) {
    tempRef = tempNum;
    memset(tempRef, 0, IDMANAGER_MAX_DIGIT);
    while ((tempDigit = *tempList) != 0 && tempDigit != IDMANAGER_DELIMITER)
      *tempRef++ = *tempList++;

    if (*tempList == IDMANAGER_DELIMITER)
      tempList++;

    *tempRef = 0;
    tempArray[i] = atoi(tempNum);
  }

  if (!IsArrayValid(tempArray, i)) {
    free(tempArray);
    return false;
  }
 
  // copy array
  gIDCount = i;
  for (i = 0; i < gIDCount; i++)
    gIDArray[i] = tempArray[i];

  free(tempArray);

  // set the current index to the last one
  gCurrentIndex = gIDCount;
  
  return true;
}

bool gtkEmbedIDManager::IsArrayValid(int aArray[], int aLength)
{
  int i;
  int j;

  if (aLength < 1 || aLength > gMaxID)
    return false;

  for (i = 0; i < aLength; i++) {
    // check if the numbers are within range
    if (aArray[i] > gMaxID || aArray[i] < 1)
      return false;
    // check if a number occurs more than once
    for (j = 0; j < i; j++)
      if (aArray[i] == aArray[j])
        return false;
  }
  return true;
}

// returns true if the given id is in the
// current id list
bool gtkEmbedIDManager::IsIDValid(int aID)
{
  int i;

  if (aID < 1 && aID > gMaxID)
    return false;

  for (i = 0; i < gIDCount; i++)
    if (aID == gIDArray[i])
      return true;

  return false;
}
