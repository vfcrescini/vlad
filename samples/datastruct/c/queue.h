/*
 * queue.h
 * Vino Crescini
 * 7 December 2000
 *
 */

#include <stdlib.h>
#include "structure.h"

#ifndef __QUEUE_H
#define __QUEUE_H

class queueType {
	public :
		queueType();
		~queueType();

		int length();
		bool enqueue(int item);
		bool dequeue(int *item);
		bool front(int *item);
		bool back(int *item);
		void init();
	private :
		int size;
		structure *head;
		structure *tail;
};

#endif

