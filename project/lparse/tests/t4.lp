% ground, arity > 0

a(a).
b(b,b).
c(c,c,c).

d(a, b, c, d) :- a(a), b(b,b), not e(a, b, c, d).
e(a, b, c, d) :- a(a), b(b,b), not d(a, b, c, d).

f(a,
  b,
  c) :-
	a(a).

f(c,b,a) :- a(a).

g(a) :- not a(a), not b.
