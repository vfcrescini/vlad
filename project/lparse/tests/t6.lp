% basic tests for weight rules
a.
b.
c(a).
d(a).

%satisfied rule heads

e :- 2 [ a=1, not c=1, not c(a)=1 ], a, not a(a).
f :- [ a=1, not a(a)=1, not a=1, not d(a)=1 ] 3, not c, d(a).
g :- 1 [ b=1, not a(a) = 2  ] 3, b, a, d(a), c(a).

%unsatisfied rule heads

ee :- 2 [ a = 1 ], a, not a(a).
ff :- 2 [ a = 2 ], a(a).
gg :- 2 [ a = 2 ], not c(a).
hh :- [ a=1, c(a) = 1, not a(a) = 1 ] 2, a.
ii :- 2 [ a=1 ] 3.
jj :- 2 [ a=1, not b=2, c(a)=3 ] 3.

