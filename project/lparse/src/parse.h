#ifndef BISON_Y_TAB_H
# define BISON_Y_TAB_H

#ifndef YYSTYPE
typedef union {
   char *s;
   long l;
   ParseNode *pn;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	NUMBER	257
# define	WEIGHT	258
# define	MINIMIZE	259
# define	MAXIMIZE	260
# define	HIDE	261
# define	EXP	262
# define	FUNCTION	263
# define	FUNCSYMBOL	264
# define	IF	265
# define	NOT	266
# define	COMPUTE	267
# define	IDENTIFIER	268
# define	VARIABLE	269
# define	CONST	270
# define	EXTERNAL	271
# define	SHOW	272
# define	OPTION	273
# define	VARIABLEDEF	274
# define	LE	275
# define	GE	276
# define	EQ	277
# define	NEQ	278
# define	DOTS	279
# define	LT	280
# define	GT	281
# define	ASSIGN	282
# define	MOD	283
# define	COMMAS	284
# define	NORM	285
# define	METAPRIORITY	286
# define	UMINUS	287


extern YYSTYPE yylval;

#endif /* not BISON_Y_TAB_H */
