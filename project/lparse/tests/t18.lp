% negative weights

a :- not b.
b :- not a.

c  :- 2 [ b = 4, a = -3 ] 8.
d  :- 2 [ not b = 4, not a = -3] 8.

