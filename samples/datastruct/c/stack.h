/*
 * stack.h
 * Vino Crescini
 * 8 December 2000
 *
 */

#include <stdlib.h>
#include "structure.h"

#ifndef __STACK_H
#define __STACK_H

class stackType {
	public :
		stackType();
		~stackType();

		int length();
		bool push(int item);
		bool pop(int *item);
		bool top(int *item);
		void init();
	private :
		int size;
		structure *stack;
};

#endif

