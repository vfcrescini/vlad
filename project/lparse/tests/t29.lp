
a(1..3).
b(1..3).

1 { chosen(X, Y) : a(X) } 1 :-
    b(Y).

1 { chosen(X, Y) : b(Y) } 1 :-
    a(X).
