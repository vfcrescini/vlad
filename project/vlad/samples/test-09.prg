/*
 * Test Run
 * Vino Fernando Crescini  <jcrescin@scm.uws.edu.au>
 */


ident sub alice;
ident sub-grp grp1, grp2, grp3;
ident acc read, write;
ident obj file;

ident obj objs001, objs002, objs003, objs004, objs005, objs006, objs007, objs008, objs009, objs010;
ident obj objs011, objs012, objs013, objs014, objs015, objs016, objs017, objs018, objs019, objs020;
ident obj-grp objg001, objg002, objg003, objg004, objg005, objg006, objg007, objg008, objg009, objg010;
ident obj-grp objg011, objg012, objg013, objg014, objg015, objg016, objg017, objg018, objg019, objg020;

initially
  memb(alice, grp2) &&
  holds(grp1, read, file) &&
  subst(grp2, grp1);

always holds(grp1, write, file)
  implied by
    holds(grp1, read, file)
  with absence
    !holds(grp3, write, file);

delete_read(SG0, OS0)
  causes !holds(SG0, read, OS0);
