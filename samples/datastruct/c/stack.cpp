/*
 * stack.cpp
 * Vino Crescini
 * 8 December 2000
 */

#include <stdlib.h>
#include "stack.h"

stackType::stackType() 
{
  init();
}

stackType::~stackType()
{
  // cleanup
  int tempInt;
  while (pop(&tempInt)); 
}

void stackType::init()
{
  stack = NULL;
  size  = 0;
}

int stackType::length()
{
  return size;
}

bool stackType::push(int item)
{
  structure *tempNode = new structure();

  if (tempNode) {
    size++;
    tempNode->setStructure(item, stack, NULL);
    stack = tempNode;	

    return true; 
  }
	
  return false;
}

bool stackType::pop(int *item)
{
  structure *tempNode;

  if (!item || size <= 0) 
    return false;

  size--;
  tempNode = stack;
  *item    = stack->getItem();
  stack	   = stack->getNext();
	
  delete tempNode;

  return true;
}

bool stackType::top(int *item)
{
  if (!item || size <= 0)
    return false;

  *item = stack->getItem();

  return true;
}
