% mixing variables and ground rules

a(1..3).
b.

c(X,X) :- a(X), not b(X,X), b, not c.
b(X,X) :- a(X), not c(X,X), b, not c.

cc(X) :- a(X), not b.
dd(X) :- a(X), b, c.

