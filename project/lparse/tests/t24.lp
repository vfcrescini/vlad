%% conditions with constraint rules

% first the base

dom1(1).
dom2(1..2).
dom3(1..2, 1..2).

a(X) :- dom2(X), not nota(X).
nota(X) :- dom2(X), not a(X).

a(X,Y) :- dom3(X,Y), not nota(X,Y).
nota(X,Y) :- dom3(X,Y), not a(X,Y).


% ground condition, true right side, true rule
d1.
d2.

a :- 1 { d1 : d2 } 2.

% ground condition, true right side, false rule

aa :- 2 { d1 : d2 } 3.

% ground condition, false right side

b :- 1 { d1 : d3 } 2.

% condition with a single variable, true rule

c :- 1 { dom2(X) : dom1(X) } 1.

% condition with a single variable, false rule

dd :- 2 { dom2(X) : dom1(X) } 2.

% condition with two variables, true rule

e :- 4 { dom3(X,Y) : dom2(X) : dom2(Y) } 4.

% condition with two variables, false rule

ee :- 3 { dom3(X,Y) : dom2(X) : dom2(Y) } 3 .

% constraint rule with two conditions, true rule

f :- 2 { dom2(X) : dom1(X), not d1(Y) : dom1(Y) } 2.

% constraint rule with two conditions, false rule

ff :- 3 { dom2(X) : dom1(X), not d1(Y) : dom1(Y) } 3.

% a condition with a global variable, true rule

g(Y) :- 2 { dom3(X,Y) : dom2(X) } 2, dom2(Y).

% a condition with a global variable, false rule

gg(Y) :- 3 { dom3(X,Y) : dom2(X) } 3, dom2(Y).

% two conditions sharing a global variable

h :- 2 { a(X,Y) : dom3(X,Y), not a(Z,Y) : dom3(Z,Y) }, dom2(Y).

% two non-domain conditions

i :- 2 { a(X) : dom2(X) } 2.
