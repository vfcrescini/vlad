% negative weights and empty heads

a :- not b.
b :- not a.
c(a) :- not d(a).
d(a) :- not c(a).

e :- 2 [ a = 2, c(a) = 2, d(a)=  -1, b = -1 ].
f :- 1 [ a = 3, not b = -1, not c(a) = -1, not d(a) = -1  ] 4.

 :- not e.
