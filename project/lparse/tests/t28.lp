%% Test for symbolic functions.

foo(a(X)) :- b(X).

b(a(1) ; a(2)).
bb(1..2).

c(X) :- b(X), not d(X).
d(X) :- b(X), not c(X).

bar :- 1 { d(a(Y)) : b(a(Y)): bb(Y) }.
baz(a(X+5*X)) :- bb(X). 

bar(a(b(c(d(e(f(X))))))) :- bb(X).

flob(X) :- bar(a(b(c(d(e(X)))))), assign(X,f(Y)), bb(Y).

a(X) :- X=Y, Y=Z, Z=5.

maximize { b(a(X)) : bb(X) }.
minimize { not d(a(X)) : bb(X) }.
