% basic variable use 

a(a ; b ; c).

b(X) :- a(X), not c(X).
c(X) :- a(X), not b(X).
