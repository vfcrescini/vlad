#domain a(X), b(X).

a(1..3).
b(2..4).

c(X).

d(X) :- X+2 <= 4.

e(foo(X)) :- not f(bar(X)).
f(bar(X)) :- not e(foo(X)).
