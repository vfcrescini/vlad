% built in functions with variables

a(1..2).
b(1..2).

c(2 * (X + 5)) :- a(X).
d(10 mod X) :- b(X).
e(40 / (5 - X)) :- a(X).
f(plus(1,2,3,4,5)).

g(X,Y) :- X == Y,a(X), b(Y).
h(X,Y) :- X != Y,a(X), b(Y).
i(X,Y) :- X < Y, a(X), b(Y).
j(X,Y) :- X <= Y, a(X), b(Y).
k(X,Y) :- X > Y, a(X), b(Y).
l(X,Y) :- X >= Y, a(X), b(Y).
