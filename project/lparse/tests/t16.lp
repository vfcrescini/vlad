% weight definitions

p(1).
q(1..2).

weight p(X) = X.

a1 :- 1 [ p(X) ] 1. % weight should be 1

weight p(X) = X+2.

a2 :- 3 [ p(X) ] 3. % should be: 3

weight q(X) = 1.
weight q(2) = 10.

d(a;b).

b :- 11 [ q(Y) : q(Y) ] 11. % should be 11

r(a,a ; a,b ; b,a ; b,b).

weight r(X,X) = 1.
weight r(a,X) = 2.
weight r(a,b) = 4.

c :- 8 [ r(X,Y) : d(X) : d(Y) ] 8.

weight r(b,a) = 8.

d :- 15 [ r(X,Y) : d(X) : d(Y) ] 15.
