% Node coloring problem by Tommi Syrj"anen.
% Given a graph given as a set of nodes and edges
% find a way to color the nodes with 'n' colors such that
% two adjacent nodes are not colored with the same color.

color(1..n).
1 { node_color(N, C) : color(C) } 1 :- node(N).
 :- node_color(X, C), node_color(Y,C), edge(X,Y), color(C).

% Typical command line
% lparse -d none -c n=3 color2.lp graph | smodels
