/*
 * Document Release Example
 * Vino Fernando Crescini  <jcrescin@scm.uws.edu.au>
 */

ident sub Sci, Pat_Off;
ident acc Own, Read, Write, Review, Pat_Ok, Pat_Reject, Release;
ident obj Doc;

/* initial facts */

initially holds(Sci, Own, Doc);
initially holds(Sci, Read, Doc);
initially holds(Sci, Write, Doc);

/* transformations */

Request(X, Y, Z)
  causes
    holds(Z, Review, Y) &&
    !holds(X, Write, Y)
  if
    holds(X, Own, Y) &&
    holds(X, Write, Y);

Get_Approval(X, Y, Z)
  causes
    holds(X, Pat_Ok, Y) &&
    !holds(Z, Review, Y)
  if
    holds(Z, Review, Y) &&
    holds(X, Own, Y);

Get_Rejection(X, Y, Z)
  causes
    holds(X, Pat_Reject, Y) &&
    !holds(Z, Review, Y)
  if
    holds(Z, Review, Y) &&
    holds(X, Own, Y);

Release_Doc(X, Y)
  causes
    holds(X, Release, Y) &&
    !holds(X, Pat_Ok, Y)
  if
    holds(X, Pat_Ok, Y);

Revise_Doc(X, Y)
  causes
    holds(X, Write, Y)
  if
    holds(X, Pat_Reject, Y);
