/*
 * circular.cpp
 * Vino Crescini
 * 8 December 2000
 *
 */

#include <stdlib.h>
#include "circular.h"

circularType::circularType() {
	init();
}

circularType::~circularType() {
	// cleanup
	int tempInt;
	while (remove(&tempInt)); 
}

void circularType::init() {
	current	= NULL;
	size	= 0;
}

int circularType::length() {
	return size;
}

bool circularType::insert(int item) {
	structure *tempNode = new structure();

	if (tempNode) {
		size++;
		tempNode->setStructure(item, tempNode, tempNode);

		if (current) {
			tempNode->setNext(current->getNext());
			tempNode->setPrev(current);
			current->getPrev()->setNext(tempNode);
			current->getNext()->setPrev(tempNode);
		}

		current = tempNode;

		return true; 
	}
	
	return false;
}

bool circularType::remove(int *item) {
	structure *tempNode;

	if (!item || size <= 0) 
		return false;

	size--;
	tempNode	= current;
	*item 		= current->getItem();

	if (size > 0) {
		current->getPrev()->setNext(current->getNext());
		current->getNext()->setPrev(current->getPrev());
		current = current->getNext();
	}
	else
		current = NULL;
	
	delete tempNode;

	return true;
}

bool circularType::next() {
	if (size <= 0)
		return false;
	
	current = current->getNext();
	return true;
}

bool circularType::prev() {
	if (size <= 0)
		return false;
	
	current = current->getPrev();
	return true;
}

bool circularType::peek(int *item) {
	if (!item || size <= 0)
		return false;	

	*item = current->getItem();
	return true;
}
