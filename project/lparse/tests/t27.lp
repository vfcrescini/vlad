% a few more tests for symbolic functions

a(foo(bar(X+(Y*5)))) :- d(X), d(Y).

d(1..2).

e(a ; "b-13:").
f(floop(X)) :- e(X).

g(X,floop(Y)) :- f(X), e(Y), Y < X.

{ h(X) : f(X) }.
