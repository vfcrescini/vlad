/*
 * structure.h
 * Vino Crescini
 * 7 December 2000
 */

#include <stdlib.h>

#ifndef __STRUCTURE_H
#define __STRUCTURE_H

class structure 
{
  public :

    structure();
    ~structure();

    void setItem(int i);
    void setNext(structure *s);
    void setPrev(structure *s);

    int getItem();
    structure *getNext();
    structure *getPrev();

    void setStructure(int i, structure *n, structure *p);

  private :
  
    int item;
    structure *next;
    structure *prev;
};

#endif
