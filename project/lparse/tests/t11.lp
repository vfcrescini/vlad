% variables with special rules

a(1..2).
b(2..3).
c(1..3).

e(X) :- 2 { a(X), not d(X) }, c(X).
ee(X) :- 2 { not a(X), b(X) }, c(X).

f(X) :- 1 { a(Y), b(Y) }, c(X).
ff(X) :- 2 { a(X), b(X) }, c(X).

{ g(X), h(X) } :- a(X), b(X).

i(X) :- 1 { a(Y), b(X) }, 1 { not a(Y), not b(X) }, c(X).

j(X) :- 2 [ a(Y) = 2, b(X) = 2], 2 [not a(Y) = 2, not b(X) =2], c(X).

