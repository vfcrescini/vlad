% ground program with more stuff.
a.
b.
c :- a, b.
d :- a, not b.
e :- c, not f.
f :- c, not e.
g :- d, not h.
h :- d, not g.
