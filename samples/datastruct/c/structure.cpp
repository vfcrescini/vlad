/*
 * structure.cpp
 * Vino Crescini
 * 7 December 2000
 */

#include "structure.h"

structure::structure() 
{
  item = 0;
  next = NULL;
  prev = NULL;
}

structure::~structure()
{
}

void structure::setItem(int i) 
{
  item = i;
}

void structure::setNext(structure *s) 
{
  next = s;
}

void structure::setPrev(structure *s) 
{
  prev = s;
}

void structure::setStructure(int i, structure *n, structure *p) 
{
  setItem(i);
  setNext(n);
  setPrev(p);
}

int structure::getItem() 
{
  return item;
}
structure *structure::getNext() 
{
  return next;
}
structure *structure::getPrev() 
{
  return prev;
}

