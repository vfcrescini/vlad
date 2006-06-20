/*
 * Test Run
 * Vino Fernando Crescini  <jcrescin@scm.uws.edu.au>
 */


ident sub alice;
ident sub-grp grp1, grp2, grp3;
ident acc read, write;
ident obj file;

ident obj-grp objg001, objg002, objg003, objg004, objg005, objg006, objg007, objg008, objg009, objg010;
ident obj-grp objg011, objg012, objg013, objg014, objg015, objg016, objg017, objg018, objg019, objg020;
ident obj-grp objg021, objg022, objg023, objg024, objg025, objg026, objg027, objg028, objg029, objg030;
ident obj-grp objg031, objg032, objg033, objg034, objg035, objg036, objg037, objg038, objg039, objg040;
ident obj-grp objg041, objg042, objg043, objg044, objg045, objg046, objg047, objg048, objg049, objg050;
ident obj-grp objg051, objg052, objg053, objg054, objg055, objg056, objg057, objg058, objg059, objg060;
ident obj-grp objg061, objg062, objg063, objg064, objg065, objg066, objg067, objg068, objg069, objg070;
ident obj-grp objg071, objg072, objg073, objg074, objg075, objg076, objg077, objg078, objg079, objg080;
ident obj-grp objg081, objg082, objg083, objg084, objg085, objg086, objg087, objg088, objg089, objg090;
ident obj-grp objg091, objg092, objg093, objg094, objg095, objg096, objg097, objg098, objg099, objg100;

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
