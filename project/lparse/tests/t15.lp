% conditions

a(1..2).
b(1..2, 1..2).
baz(X) :- a(X), not bar(X).
bar(X) :- a(X), not baz(X).


foo(X) :- bar(Y):a(Y), a(X).

bar :- not foo, b(X,Y) :a(X) : a(Y) : X > Y.
foo :- not bar.

1 { b(X) : a(X) } 1.



