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

always memb(objs001, objg001);
always memb(objs001, objg002);
always memb(objs001, objg003);
always memb(objs001, objg004);
always memb(objs001, objg005);
always memb(objs001, objg006);
always memb(objs001, objg007);
always memb(objs001, objg008);
always memb(objs001, objg009);
always memb(objs001, objg010);
always memb(objs002, objg001);
always memb(objs002, objg002);
always memb(objs002, objg003);
always memb(objs002, objg004);
always memb(objs002, objg005);
always memb(objs002, objg006);
always memb(objs002, objg007);
always memb(objs002, objg008);
always memb(objs002, objg009);
always memb(objs002, objg010);
always memb(objs003, objg001);
always memb(objs003, objg002);
always memb(objs003, objg003);
always memb(objs003, objg004);
always memb(objs003, objg005);
always memb(objs003, objg006);
always memb(objs003, objg007);
always memb(objs003, objg008);
always memb(objs003, objg009);
always memb(objs003, objg010);
always memb(objs004, objg001);
always memb(objs004, objg002);
always memb(objs004, objg003);
always memb(objs004, objg004);
always memb(objs004, objg005);
always memb(objs004, objg006);
always memb(objs004, objg007);
always memb(objs004, objg008);
always memb(objs004, objg009);
always memb(objs004, objg010);
always memb(objs005, objg001);
always memb(objs005, objg002);
always memb(objs005, objg003);
always memb(objs005, objg004);
always memb(objs005, objg005);
always memb(objs005, objg006);
always memb(objs005, objg007);
always memb(objs005, objg008);
always memb(objs005, objg009);
always memb(objs005, objg010);
always memb(objs006, objg001);
always memb(objs006, objg002);
always memb(objs006, objg003);
always memb(objs006, objg004);
always memb(objs006, objg005);
always memb(objs006, objg006);
always memb(objs006, objg007);
always memb(objs006, objg008);
always memb(objs006, objg009);
always memb(objs006, objg010);
always memb(objs007, objg001);
always memb(objs007, objg002);
always memb(objs007, objg003);
always memb(objs007, objg004);
always memb(objs007, objg005);
always memb(objs007, objg006);
always memb(objs007, objg007);
always memb(objs007, objg008);
always memb(objs007, objg009);
always memb(objs007, objg010);
always memb(objs008, objg001);
always memb(objs008, objg002);
always memb(objs008, objg003);
always memb(objs008, objg004);
always memb(objs008, objg005);
always memb(objs008, objg006);
always memb(objs008, objg007);
always memb(objs008, objg008);
always memb(objs008, objg009);
always memb(objs008, objg010);
always memb(objs009, objg001);
always memb(objs009, objg002);
always memb(objs009, objg003);
always memb(objs009, objg004);
always memb(objs009, objg005);
always memb(objs009, objg006);
always memb(objs009, objg007);
always memb(objs009, objg008);
always memb(objs009, objg009);
always memb(objs009, objg010);
always memb(objs010, objg001);
always memb(objs010, objg002);
always memb(objs010, objg003);
always memb(objs010, objg004);
always memb(objs010, objg005);
always memb(objs010, objg006);
always memb(objs010, objg007);
always memb(objs010, objg008);
always memb(objs010, objg009);
always memb(objs010, objg010);

always holds(grp1, write, file)
  implied by
    holds(grp1, read, file)
  with absence
    !holds(grp3, write, file);

delete_read(SG0, OS0)
  causes !holds(SG0, read, OS0);
