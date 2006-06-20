/*
 * Test Run
 * Vino Fernando Crescini  <jcrescin@scm.uws.edu.au>
 */


ident sub alice;
ident sub-grp grp1, grp2, grp3;
ident acc read, write;
ident obj file;

ident obj objs01, objs02, objs03, objs04, objs05, objs06, objs07, objs08, objs09, objs10;
ident obj objs11, objs12, objs13, objs14, objs15, objs16, objs17, objs18, objs19, objs20;
ident obj-grp objg01, objg02, objg03, objg04, objg05, objg06, objg07, objg08, objg09, objg10;
ident obj-grp objg11, objg12, objg13, objg14, objg15, objg16, objg17, objg18, objg19, objg20;

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
