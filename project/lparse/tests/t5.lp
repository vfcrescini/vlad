% ground constraint rules

a.
b.
c.
d(a).
e(a).
f(a).

% satisfied rule heads
g :- 1 { a, b, not c }.
h :- { not d, not e, not a } 3.
i :- 1 { a, b, c, not d(a), not e(a) } 4.
j(a) :- 1 { d(a) }.
k(a) :- { e(a), f(a) } 3.
l(a) :- 1 { f(a), a, not d, not b, not f(a)} 4.
m :- 1 { a, not d  } 3, a, not d.
n(a) :- 1 { d(a), a  } 3, d(a), not a(a).
o :- 3 { not d, not e, b, f(a)},
	{ a, d, f } 2,
	2 { not a, not b, c, d(a), e(a), not f(a)} 4, a, not d.
o(a) :- 3 { a, b, c}, { not d(a), not e(a)} 2,
	2 { d, e, e(a), not f} 4, d(a), not a(a).


% unsatisfied rule heads.

gg :- 1 { not a, not b, d}.
hh :- { a, b, c, not d, not e} 3.
ii :- 1 { not a } 4.
ii(a) :- 1 { a, not d, c, d(a), e(a) } 4.
jj(a) :- 1 { a(a), b(a), not d(a)}.
kk(a) :- { e(a), f(a), d(a), not c(a)} 3.
ll(a) :- 1 { d(a), e(a), not a(a), not b(a), not c(a) } 4.
ll(b) :- 1 { not d(a), not e(a) } 4.
mm :- 1 { not a, not b, not c } 3, a, not d.
mm :- 1 { a, b, c, d(a) } 3, a, not d.
nn(a) :- 1 { not a, a(a) } 3, d(a), not a.
nn(b) :- 1 { a, d(a), not a(a), not b(a) } 3, d(a), not a.
oo :- 1 { not a, d }, { a, b } 2, 2 { d(a), e(a), f(a) } 4, a, not d. 

foo :- g, not bar.
bar :- g, not foo.

baz :- gg, not blez.
blez :- gg, not baz.
