class stack {
	private structure up;
	private int size;

	public stack() {
		init();
	}

	public void init() {
		up = null;
		size = 0;
	}

	public int size() {
		return size;
	}

	public boolean push(int item) {
		structure tempNode = new structure(item, null, null);

		if (tempNode == null)
			return false;

		size++;

		if (up == null)
			up = tempNode;
		else {
			up.setPrev(tempNode);
			tempNode.setNext(up);
			up = tempNode;
		}
			
		return true;
	}	
	
	public boolean pop() {
		size--;
		up = up.getNext();

		return true;
	}	

	public int top() {
		if (up != null)
			return up.getItem();
		else
			return 0;
	}

	public int bottom() {
		if (up != null)
			return up.getItem();
		else
			return 0;
	}
	
	public int length() {
		return size;
	}
}
