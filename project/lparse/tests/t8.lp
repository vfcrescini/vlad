% ground choice rules

a.
b.

{ d(a),
  e(a),
  f(a) } :- a, not e.

1 { d(b), e(b), f(b) } :- not e, a.

1 { d(c), e(c), f(c) } 1.

d(d) | e(d) | f(d).

2 { d(e), e(e), f(e) } 2.

c :- not d.
d :- not c.

{ d(f), e(f), f(f) } 1 :- c.

