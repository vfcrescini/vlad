% optimize statements

a :- not b.
b :- not a.

c(X) :- d(X), not e(X).
e(X) :- d(X), not c(X).

d(1..2).


maximize [ c(X) : d(X) = -X ].
minimize [ a=1 ].

