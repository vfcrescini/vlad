#include <stdio.h>
#include "queue.h"
#include "stack.h"
#include "circular.h"

int main() {
	queueType q;
	stackType s;
	circularType c;
	int i;
	int tmp;

	printf("queue.............\n");

	printf("size : %d\n", q.length());

	for (i = 0; i < 10; i++)
		printf("enqueue  %d : %d\n", q.enqueue(i), i);

	printf("size : %d\n", q.length());

	for (i = q.length(); i > 0; i--) { 
		printf("dequeue %d : ", q.dequeue(&tmp));
		printf("%d\n", tmp);
	} 

	printf("size : %d\n", q.length());

	printf("stack..............\n");

        printf("size : %d\n", s.length());

        for (i = 0; i < 10; i++)
                printf("push  %d : %d\n", s.push(i), i);

        printf("size : %d\n", s.length());

        for (i = s.length(); i > 0; i--) { 
                printf("pop %d : ", s.pop(&tmp));
                printf("%d\n", tmp);
        } 

        printf("size : %d\n", s.length());


	printf("cicular buffer ..............\n");

        printf("size : %d\n", c.length());

        for (i = 0; i < 10; i++)
                printf("insert  %d : %d\n", c.insert(i), i);

        printf("size : %d\n", c.length());

        for (i = c.length(); i > 0; i--) { 
                printf("remove %d : ", c.remove(&tmp));
                printf("%d\n", tmp);
        } 

        printf("size : %d\n", c.length());


	return 0;
}
