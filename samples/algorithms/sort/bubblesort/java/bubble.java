
/*
 * bubble.java
 * Vino Crescini
 * 22 May 2001
 */

public class bubble
{
  public void dumpArray(int[] inputArray)
  {
    int length = java.lang.reflect.Array.getLength(inputArray);
    int i;

    if (length > 0) {
      for (i = 0; i < length; i++)
        System.out.println("array[" + i + "]=" + inputArray[i]);
    }
  }
  public int[] sort(int[] inputArray)
  {
    int length = java.lang.reflect.Array.getLength(inputArray);
    int i;
    int j;
    int temp;
    
    if (length > 0) {
      for ( i = length - 1; i > 0; i--) {
        for (j = 0; j < i; j++) {
	  if (inputArray[j] > inputArray[j + 1]) {
	    temp = inputArray[j];
	    inputArray[j] = inputArray[j + 1];
	    inputArray[j + 1] = temp;
	  }
        }
      }
    }
    return inputArray;
  }

  public static void main(String[] arg)
  {
    int[] tempArray = {2, 4, 6, 8, 0, 9, 7, 5, 3, 1};
    bubble bubbleInstance = new bubble();

    bubbleInstance.dumpArray(bubbleInstance.sort(tempArray));
  }
}
