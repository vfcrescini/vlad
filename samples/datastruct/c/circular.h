/*
 * circular.h
 * Vino Crescini
 * 8 December 2000
 */

#include <stdlib.h>
#include "structure.h"

#ifndef __CIRCULAR_H
#define __CIRCULAR_H

class circularType
{
  public :
	  
    circularType();
    ~circularType();

    int length();
    bool insert(int item);
    bool remove(int *item);
    bool next();
    bool prev();
    bool peek(int *item);	
    void init();
    
  private :
    int size;
    structure *current;
};

#endif
