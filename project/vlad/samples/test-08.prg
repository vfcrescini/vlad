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

u001() causes memb(objs011, objg011);
u002() causes memb(objs011, objg012);
u003() causes memb(objs011, objg013);
u004() causes memb(objs011, objg014);
u005() causes memb(objs011, objg015);
u006() causes memb(objs011, objg016);
u007() causes memb(objs011, objg017);
u008() causes memb(objs011, objg018);
u009() causes memb(objs011, objg019);
u010() causes memb(objs011, objg020);
u011() causes memb(objs012, objg011);
u012() causes memb(objs012, objg012);
u013() causes memb(objs012, objg013);
u014() causes memb(objs012, objg014);
u015() causes memb(objs012, objg015);
u016() causes memb(objs012, objg016);
u017() causes memb(objs012, objg017);
u018() causes memb(objs012, objg018);
u019() causes memb(objs012, objg019);
u020() causes memb(objs012, objg020);
u021() causes memb(objs013, objg011);
u022() causes memb(objs013, objg012);
u023() causes memb(objs013, objg013);
u024() causes memb(objs013, objg014);
u025() causes memb(objs013, objg015);
u026() causes memb(objs013, objg016);
u027() causes memb(objs013, objg017);
u028() causes memb(objs013, objg018);
u029() causes memb(objs013, objg019);
u030() causes memb(objs013, objg020);
u031() causes memb(objs014, objg011);
u032() causes memb(objs014, objg012);
u033() causes memb(objs014, objg013);
u034() causes memb(objs014, objg014);
u035() causes memb(objs014, objg015);
u036() causes memb(objs014, objg016);
u037() causes memb(objs014, objg017);
u038() causes memb(objs014, objg018);
u039() causes memb(objs014, objg019);
u040() causes memb(objs014, objg020);
u041() causes memb(objs015, objg011);
u042() causes memb(objs015, objg012);
u043() causes memb(objs015, objg013);
u044() causes memb(objs015, objg014);
u045() causes memb(objs015, objg015);
u046() causes memb(objs015, objg016);
u047() causes memb(objs015, objg017);
u048() causes memb(objs015, objg018);
u049() causes memb(objs015, objg019);
u050() causes memb(objs015, objg020);
u051() causes memb(objs016, objg011);
u052() causes memb(objs016, objg012);
u053() causes memb(objs016, objg013);
u054() causes memb(objs016, objg014);
u055() causes memb(objs016, objg015);
u056() causes memb(objs016, objg016);
u057() causes memb(objs016, objg017);
u058() causes memb(objs016, objg018);
u059() causes memb(objs016, objg019);
u060() causes memb(objs016, objg020);
u061() causes memb(objs017, objg011);
u062() causes memb(objs017, objg012);
u063() causes memb(objs017, objg013);
u064() causes memb(objs017, objg014);
u065() causes memb(objs017, objg015);
u066() causes memb(objs017, objg016);
u067() causes memb(objs017, objg017);
u068() causes memb(objs017, objg018);
u069() causes memb(objs017, objg019);
u070() causes memb(objs017, objg020);
u071() causes memb(objs018, objg011);
u072() causes memb(objs018, objg012);
u073() causes memb(objs018, objg013);
u074() causes memb(objs018, objg014);
u075() causes memb(objs018, objg015);
u076() causes memb(objs018, objg016);
u077() causes memb(objs018, objg017);
u078() causes memb(objs018, objg018);
u079() causes memb(objs018, objg019);
u080() causes memb(objs018, objg020);
u081() causes memb(objs019, objg011);
u082() causes memb(objs019, objg012);
u083() causes memb(objs019, objg013);
u084() causes memb(objs019, objg014);
u085() causes memb(objs019, objg015);
u086() causes memb(objs019, objg016);
u087() causes memb(objs019, objg017);
u088() causes memb(objs019, objg018);
u089() causes memb(objs019, objg019);
u090() causes memb(objs019, objg020);
u091() causes memb(objs020, objg011);
u092() causes memb(objs020, objg012);
u093() causes memb(objs020, objg013);
u094() causes memb(objs020, objg014);
u095() causes memb(objs020, objg015);
u096() causes memb(objs020, objg016);
u097() causes memb(objs020, objg017);
u098() causes memb(objs020, objg018);
u099() causes memb(objs020, objg019);
u100() causes memb(objs020, objg020);
