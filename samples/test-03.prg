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
ident obj objs021, objs022, objs023, objs024, objs025, objs026, objs027, objs028, objs029, objs030;
ident obj objs031, objs032, objs033, objs034, objs035, objs036, objs037, objs038, objs039, objs040;
ident obj objs041, objs042, objs043, objs044, objs045, objs046, objs047, objs048, objs049, objs050;
ident obj objs051, objs052, objs053, objs054, objs055, objs056, objs057, objs058, objs059, objs060;
ident obj objs061, objs062, objs063, objs064, objs065, objs066, objs067, objs068, objs069, objs070;
ident obj objs071, objs072, objs073, objs074, objs075, objs076, objs077, objs078, objs079, objs080;
ident obj objs081, objs082, objs083, objs084, objs085, objs086, objs087, objs088, objs089, objs090;
ident obj objs091, objs092, objs093, objs094, objs095, objs096, objs097, objs098, objs099, objs100;

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
