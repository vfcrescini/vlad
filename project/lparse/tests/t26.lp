% test quoted strings and relational string operators

foo( a1 ;"a2" ; "-12.,:,.-" ).

more(X,Y) :- foo(X), foo(Y), X > Y.
less(X,Y) :- foo(X), foo(Y), X < Y.
more_equal(X,Y) :- foo(X), foo(Y), X >= Y.
less_equal(X,Y) :- foo(X), foo(Y), X <= Y.




