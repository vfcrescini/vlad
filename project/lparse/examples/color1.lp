color(red). 
color(blue). 
color(yellow). 

col(X,red) :- node(X), not col(X, blue), not col(X,yellow). 
col(X,blue) :- node(X), not col(X, red), not col(X,yellow). 
col(X,yellow) :- node(X), not col(X, blue), not col(X,red). 
fail :- edge(X,Y), color(C), col(X,C), col(Y,C). 
node(a). node(b). 
node(c). node(d). 
edge(a,b). edge(b,c). 
edge(c,d). edge(d,a). 

compute 1 { not fail }.
