class queue {
	private structure head;
	private structure tail;
	private int size;

	public queue() {
		init();
	}

	public void init() {
		head = null;
		tail = null;
		size = 0;
	}

	public int size() {
		return size;
	}

	public boolean enqueue(int item) {
		structure tempNode = new structure(item, null, null);

		if (tempNode == null)
			return false;

		size++;

		if (head == null && tail == null) {
			head = tempNode;
			tail = tempNode;
		}
		else {
			tail.setNext(tempNode);
			tempNode.setPrev(tail);
			tail = tempNode;
		}
			
		return true;
	}	
	
	public boolean dequeue() {
		size--;
		head = head.getNext();

		return true;
	}	

	public int front() {
		if (head != null)
			return head.getItem();
		else
			return 0;
	}

	public int back() {
		if (tail != null)
			return tail.getItem();
		else
			return 0;
	}
	
	public int length() {
		return size;
	}

}
