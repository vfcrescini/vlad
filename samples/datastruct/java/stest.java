import java.io.*;
import java.lang.*;

class stest {
	public static void main(String arg[]) {
		int i;
		queue q = new queue();

		System.out.println("size" + q.length());
		
		for (i = 0; i < 10; i++)
			q.enqueue(i);

		System.out.println("size" + q.length());

		for (i = 0; i < 10; i++) {
			System.out.println(q.front());
			q.dequeue();
		}
		
		System.out.println("size" + q.length());

		stack s = new stack();

		System.out.println("size" + s.length());
		
		for (i = 0; i < 10; i++)
			s.push(i);

		System.out.println("size" + s.length());

		for (i = 0; i < 10; i++) {
			System.out.println(s.top());
			s.pop();
		}
		
		System.out.println("size" + s.length());

	}
}
