/*
 * queue.cpp
 * Vino Crescini
 * 7 December 2000
 *
 */

#include <stdlib.h>
#include "queue.h"

queueType::queueType() {
	init();
}

queueType::~queueType() {
	// cleanup
	int tempInt;
	while (dequeue(&tempInt)); 
}

void queueType::init() {
	head	= NULL;
	tail	= NULL;
	size	= 0;
}

int queueType::length() {
	return size;
}

bool queueType::enqueue(int item) {
	structure *tempNode = new structure();

	if (tempNode) {
		size++;
		tempNode->setStructure(item, NULL, NULL);

		// attach it to the queue 
		if (head == NULL && tail == NULL) {
			// empty
			head = tempNode;
			tail = tempNode;
		}
		else {
			// not empty, append
			tempNode->setPrev(tail);
			tail->setNext(tempNode);
			tail = tempNode;
		}

		return true; 
	}
	
	return false;
}

bool queueType::dequeue(int *item) {
	structure *tempNode;

	if (!item || size <= 0) 
		return false;

	size--;
	tempNode	= head;
	*item 		= head->getItem();
	head		= head->getNext();
	
	delete tempNode;

	return true;
}

bool queueType::front(int *item) {
	if (!item || size <= 0)
		return false;
	
	*item = head->getItem();

	return true;
}

bool queueType::back(int *item) {
	if (!item || size <= 0)
		return false;
	
	*item = tail->getItem();

	return true;
}

