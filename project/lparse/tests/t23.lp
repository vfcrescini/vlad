%% Tests conditions for basic rules
%% basic rules

% ground condition that is true
a1.

b :- d1 : a1.

d1 :- not notd1.
notd1 :- not d1.


% ground condition that is false

bb :- d1 : aa1.

% condition with a variable (domain 1)
dom1(1).

c :- a(X) : dom1(X).

a(X) :- dom2(X), not nota(X).
nota(X) :- dom2(X), not a(X).

% condition with a variable (domain 2)

dom2(1..2).

d :- a(X) : dom2(X).

% condition where the domain of left part is bigger than that of right part

dom3(1..3).

e :- dom3(X) : dom2(X).

% condition with two right parts
dom4(1,1 ; 1,2 ; 1,3).

a(X,Y) :- dom2(X), dom2(Y), not nota(X,Y).
nota(X,Y) :- dom2(X), dom2(Y), not a(X,Y).

f :- a(X,Y) : dom4(X,Y) : dom2(Y).

% condition that is a function

g :- a(X) : X = 5.

% condition that has two parts with a function

domn(1..2).
h :- a(X+1) : domn(X).

% two conditions in a rule body, one positive and one negative

ii :- a(X) : domn(X), not a(Y) : domn(Y).

%a global variable with conditions
j(X) :- a(X,Y) : domn(Y) : Y > X , domn(X).




