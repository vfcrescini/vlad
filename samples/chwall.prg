/*
 * Chinese-Wall Policy Example
 * Vino Fernando Crescini  <jcrescin@scm.uws.edu.au>
 */

/* identifier declarations */
ident sub S;
ident acc Access, Accessible;
ident obj Object1, Object2;
ident obj-grp Company1, Company2;

/* initial facts */
initially
  holds(S, Accessible, Company1) &&
  holds(S, Accessible, Company2);

/* constraints */
always memb(Object1, Company1);
always memb(Object2, Company2);

/* transformations */
Request_Access_Company1(X, Y)
  causes
    holds(X, Access, Y) &&
    !holds(X, Accessible, Company2)
  if
    memb(Y, Company1) &&
    holds(X, Accessible, Company1);

Request_Access_Company2(X, Y)
  causes
    holds(X, Access, Y) &&
    !holds(X, Accessible, Company1)
  if
    memb(Y, Company2) &&
    holds(X, Accessible, Company2);
