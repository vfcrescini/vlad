/* 
 * structure.java
 * Vino Crescini
 * 16 April 2001
 */
 
class structure
{
  private structure next;
  private structure prev;
  private int item;

  public structure() 
  {
    init();
  }
  public structure(int aitem, structure aprev, structure anext) 
  {
    init(aitem, aprev, anext);
  }
  public void init() 
  {
    prev = null;
    next = null;
    item = 0;
  }
  public void init(int aitem, structure aprev, structure anext) 
  {
    prev = aprev;
    next = anext;
    item = aitem;
  }
  public int getItem() 
  {
    return item;
  }
  public void setItem(int aitem) 
  {
    item = aitem;
  }
  public structure getNext() 
  {
    return next;
  }
  public void setNext(structure anext) 
  {
    next = anext;
  }
  public structure getPrev() 
  {
    return prev;
  }
  public void setPrev(structure aprev) 
  {
    prev = aprev;
  }
};
