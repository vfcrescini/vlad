/* A Bison parser, made from parse.yy
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

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

#line 21 "parse.yy"


#include "../config.h"
#include "parsetree.h"
#include "debug.h"
#include "term.h"
#include "symbol.h"
#include "global.h"
#ifndef ERROR_H
#include "error.h"
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef MS_WINDOWS
#include <malloc.h>
#define alloca _alloca
#endif

extern int yylex(void);
extern ParseNode *logic_program;

void read_option(char *);

ParseNode *tmp_node;


// Turn YYDEBUG on while debugging
#if DEBUG & DBG_PARSE
#define YYDEBUG 1
#else
#define YYDEBUG 0
#endif

//#define YYERROR_VERBOSE

long lineno = 1;
long lval1 = 0, lval2 = 0;
int return_symbol = 0;
int return_funcsymbol = 1;

extern char *yytext;

void yyerror(char *st)
{
  sys_data.num_errors++;
  fprintf(stderr, "%s: %s", error_file_and_line(lineno), st);
  if (strlen(yytext))
    fprintf(stderr, " before '%s'", yytext);
  fprintf(stderr, "\n");
}


#line 76 "parse.yy"
#ifndef YYSTYPE
typedef union {
   char *s;
   long l;
   ParseNode *pn;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		255
#define	YYFLAG		-32768
#define	YYNTBASE	52

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 287 ? yytranslate[x] : 84)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    33,     2,
      42,    43,    38,    36,    45,    37,    46,    39,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    44,    51,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    47,     2,    48,    35,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    49,    34,    50,    41,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    40
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     2,     6,    10,    14,    17,    18,    20,    22,
      24,    26,    29,    32,    35,    37,    42,    46,    51,    54,
      57,    60,    62,    66,    69,    71,    73,    74,    78,    80,
      82,    86,    88,    92,    94,   100,   106,   112,   118,   120,
     122,   126,   130,   132,   136,   138,   140,   143,   147,   152,
     154,   158,   162,   164,   166,   170,   172,   174,   179,   181,
     183,   184,   189,   195,   200,   206,   208,   212,   216,   218,
     220,   222,   224,   228,   230,   232,   236,   240,   244,   248,
     252,   256,   259,   263,   267,   271,   275,   279,   283,   287,
     291,   295,   299,   303,   306,   310,   314,   318,   322,   324,
     326,   328,   332,   336,   339,   343,   347,   351,   355,   359,
     363,   367,   371,   375,   379,   383,   386,   390,   394,   398,
     402,   404,   408,   410,   412,   413,   419,   424,   429,   434,
     439,   441
};
static const short yyrhs[] =
{
      53,     0,    53,    57,    46,     0,    53,    54,    46,     0,
      53,     1,    46,     0,    53,    55,     0,     0,    56,     0,
      81,     0,    82,     0,    19,     0,     9,    14,     0,     4,
      79,     0,     7,    79,     0,     7,     0,    16,    14,    28,
      77,     0,    16,    75,    77,     0,    16,    15,    28,    77,
       0,    17,    79,     0,    18,    79,     0,    20,    79,     0,
      58,     0,    58,    11,    59,     0,    11,    59,     0,    69,
       0,    62,     0,     0,    61,    45,    59,     0,    60,     0,
      61,     0,    60,    45,    66,     0,    66,     0,    60,    45,
      78,     0,    78,     0,    70,    47,    60,    48,    70,     0,
      70,    49,    60,    50,    70,     0,    70,    49,    79,    50,
      70,     0,    70,    47,    79,    48,    70,     0,    63,     0,
      64,     0,    69,    44,    64,     0,    63,    34,    69,     0,
      69,     0,    64,    14,    69,     0,    69,     0,    69,     0,
      12,    69,     0,    69,    44,    67,     0,    12,    69,    44,
      67,     0,    65,     0,    65,    28,    68,     0,    67,    44,
      69,     0,    69,     0,    78,     0,    67,    44,    78,     0,
      77,     0,    69,     0,    14,    42,    72,    43,     0,    14,
       0,    77,     0,     0,    10,    42,    73,    43,     0,    12,
      10,    42,    73,    43,     0,     4,    42,    74,    43,     0,
       4,    42,    12,    74,    43,     0,    73,     0,    72,    51,
      73,     0,    73,    45,    74,     0,    74,     0,    76,     0,
      77,     0,    69,     0,    69,    34,    77,     0,    14,     0,
       3,     0,    77,    25,    77,     0,    77,    30,    77,     0,
      42,    77,    43,     0,    34,    77,    34,     0,    31,    69,
      31,     0,    77,     8,    77,     0,    37,    77,     0,    77,
      36,    77,     0,    77,    37,    77,     0,    77,    38,    77,
       0,    77,    39,    77,     0,    77,    29,    77,     0,    77,
      23,    77,     0,    77,    24,    77,     0,    77,    26,    77,
       0,    77,    21,    77,     0,    77,    27,    77,     0,    77,
      22,    77,     0,    41,    77,     0,    77,    33,    77,     0,
      77,    34,    77,     0,    77,    35,    77,     0,    77,    28,
      77,     0,    71,     0,    75,     0,    15,     0,    42,    77,
      43,     0,    34,    77,    34,     0,    37,    77,     0,    77,
      36,    77,     0,    77,    37,    77,     0,    77,    38,    77,
       0,    77,    39,    77,     0,    77,    29,    77,     0,    77,
      23,    77,     0,    77,    24,    77,     0,    77,    26,    77,
       0,    77,    21,    77,     0,    77,    27,    77,     0,    77,
      22,    77,     0,    41,    77,     0,    77,    33,    77,     0,
      77,    34,    77,     0,    77,    35,    77,     0,    77,    28,
      77,     0,    71,     0,    79,    45,    66,     0,    66,     0,
      79,     0,     0,    13,    83,    49,    80,    50,     0,     6,
      49,    80,    50,     0,     6,    47,    80,    48,     0,     5,
      49,    80,    50,     0,     5,    47,    80,    48,     0,    77,
       0,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   111,   115,   117,   119,   121,   123,   127,   129,   131,
     135,   139,   141,   144,   146,   148,   196,   202,   214,   217,
     219,   224,   226,   228,   232,   234,   236,   240,   242,   244,
     248,   250,   252,   255,   260,   265,   272,   276,   280,   283,
     287,   295,   297,   302,   310,   316,   318,   320,   322,   327,
     329,   334,   336,   339,   342,   346,   348,   354,   356,   360,
     362,   367,   369,   372,   375,   381,   383,   387,   389,   393,
     395,   405,   417,   437,   446,   451,   455,   466,   468,   471,
     474,   476,   480,   482,   484,   486,   488,   490,   492,   494,
     496,   498,   500,   502,   507,   509,   511,   513,   516,   518,
     520,   524,   526,   529,   533,   535,   537,   539,   541,   543,
     545,   547,   549,   551,   553,   555,   560,   562,   564,   566,
     569,   574,   576,   580,   582,   587,   592,   595,   598,   601,
     606,   628
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "NUMBER", "WEIGHT", "MINIMIZE", "MAXIMIZE", 
  "HIDE", "EXP", "FUNCTION", "FUNCSYMBOL", "IF", "NOT", "COMPUTE", 
  "IDENTIFIER", "VARIABLE", "CONST", "EXTERNAL", "SHOW", "OPTION", 
  "VARIABLEDEF", "LE", "GE", "EQ", "NEQ", "DOTS", "LT", "GT", "ASSIGN", 
  "MOD", "COMMAS", "NORM", "METAPRIORITY", "'&'", "'|'", "'^'", "'+'", 
  "'-'", "'*'", "'/'", "UMINUS", "'~'", "'('", "')'", "':'", "','", "'.'", 
  "'['", "']'", "'{'", "'}'", "';'", "program", "rules", "statement", 
  "directive", "declaration", "rule", "head", "tail", "basic_tail", 
  "special_tail", "special_head", "headlist", "ordered_disjunction", 
  "basic_literal", "literal", "condition_list", "weight_def", "atom", 
  "bound", "function", "arguments", "arglist", "term", "constant", 
  "range", "expr", "nc_expr", "literal_list", "compute_list", 
  "compute_stmt", "optimize_stmt", "models", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    52,    53,    53,    53,    53,    53,    54,    54,    54,
      55,    56,    56,    56,    56,    56,    56,    56,    56,    56,
      56,    57,    57,    57,    58,    58,    58,    59,    59,    59,
      60,    60,    60,    60,    61,    61,    62,    62,    62,    62,
      62,    63,    63,    64,    64,    65,    65,    65,    65,    66,
      66,    67,    67,    67,    67,    68,    68,    69,    69,    70,
      70,    71,    71,    71,    71,    72,    72,    73,    73,    74,
      74,    74,    74,    75,    75,    76,    76,    77,    77,    77,
      77,    77,    77,    77,    77,    77,    77,    77,    77,    77,
      77,    77,    77,    77,    77,    77,    77,    77,    77,    77,
      77,    78,    78,    78,    78,    78,    78,    78,    78,    78,
      78,    78,    78,    78,    78,    78,    78,    78,    78,    78,
      78,    79,    79,    80,    80,    81,    82,    82,    82,    82,
      83,    83
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     1,     3,     3,     3,     2,     0,     1,     1,     1,
       1,     2,     2,     2,     1,     4,     3,     4,     2,     2,
       2,     1,     3,     2,     1,     1,     0,     3,     1,     1,
       3,     1,     3,     1,     5,     5,     5,     5,     1,     1,
       3,     3,     1,     3,     1,     1,     2,     3,     4,     1,
       3,     3,     1,     1,     3,     1,     1,     4,     1,     1,
       0,     4,     5,     4,     5,     1,     3,     3,     1,     1,
       1,     1,     3,     1,     1,     3,     3,     3,     3,     3,
       3,     2,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     3,     3,     3,     3,     1,     1,
       1,     3,     3,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     3,     3,     3,     3,
       1,     3,     1,     1,     0,     5,     4,     4,     4,     4,
       1,     0
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       6,     0,     0,    74,     0,     0,     0,    14,     0,     0,
      60,     0,   131,    73,   100,     0,     0,     0,    10,     0,
       0,     0,     0,     0,     0,     0,     5,     7,     0,    21,
      25,    38,    39,    24,     0,    98,    99,    59,     8,     9,
       4,     0,    58,     0,    49,   122,    45,    12,   124,   124,
     124,   124,    13,    11,     0,     0,     0,     0,     0,     0,
       0,    23,    28,    29,    31,     0,    98,    59,    33,     0,
      73,   130,     0,     0,    73,     0,     0,    18,    19,    20,
       0,     0,    81,    93,     0,     3,     2,    60,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    46,
       0,    71,     0,    69,    70,     0,     0,     0,   123,     0,
       0,     0,     0,     0,    68,     0,    81,    93,     0,     0,
      60,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   124,
       0,    65,     0,     0,    16,    79,    78,    77,    22,    41,
      43,    40,    44,     0,     0,    80,    90,    92,    87,    88,
      89,    91,    97,    86,    94,    95,    96,    82,    83,    84,
      85,     0,     0,     0,     0,    63,     0,     0,    50,    56,
      55,    47,    52,     0,    53,   121,   129,   128,   127,   126,
      61,     0,    78,    77,    30,    32,    27,     0,     0,    90,
      92,    87,    88,    89,    91,   119,    86,    94,    95,    96,
      82,    83,    84,    85,     0,     0,    57,     0,    15,    17,
      60,    60,    48,     0,    64,    72,    75,    76,     0,    67,
      60,    60,    62,   125,    66,    37,    36,     0,    51,    54,
      34,    35,    61,     0,     0,     0
};

static const short yydefgoto[] =
{
     253,     1,    25,    26,    27,    28,    29,    61,    62,    63,
      30,    31,    32,    44,    45,   191,   188,    46,    65,    35,
     150,   123,   124,    36,   113,   114,    68,   118,   119,    38,
      39,    72
};

static const short yypact[] =
{
  -32768,   264,   -11,-32768,    -4,    23,    40,    34,    22,    -2,
     591,    66,   626,   570,-32768,    36,    34,    34,-32768,    34,
      63,   626,   626,   626,   626,    38,-32768,-32768,    44,   138,
  -32768,    57,   136,    -7,    73,-32768,-32768,   826,-32768,-32768,
  -32768,    63,   119,   639,   125,-32768,   118,   120,    34,    34,
      34,    34,   120,-32768,   674,   122,    20,   626,   626,   626,
     626,-32768,   123,   127,-32768,    74,   207,   848,-32768,   131,
  -32768,   826,   126,   674,   139,   160,   626,   120,   120,   120,
     143,   870,   186,   186,   757,-32768,-32768,   591,    63,    63,
      63,    34,    34,   626,   626,   626,   626,   626,   626,   626,
     626,   626,   626,   626,   626,   626,   626,   626,   626,   151,
     687,   162,   154,-32768,   803,   674,   722,    34,   120,   155,
     149,   157,   156,    65,-32768,   892,    17,   176,   780,   591,
     591,   591,   591,   626,   626,   626,   626,   626,   626,   626,
     626,   626,   626,   626,   626,   626,   626,   626,   674,    34,
     -39,   165,   626,   626,   826,-32768,   735,-32768,-32768,-32768,
  -32768,   136,-32768,   -29,   -19,   186,    16,    16,    16,    16,
      16,    16,   914,   186,   934,   934,   934,    80,    80,   186,
     186,   722,   172,   173,   626,-32768,   626,   626,-32768,-32768,
     826,   174,-32768,   848,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,   674,   167,   600,-32768,-32768,-32768,    21,   -17,   320,
     406,   429,   447,   470,   488,   541,   199,   294,   354,   386,
      35,   502,   242,   511,   111,   175,-32768,   674,   826,   826,
     626,   626,   174,   674,-32768,   826,   826,   826,   722,-32768,
     626,   626,-32768,-32768,   165,-32768,-32768,   115,-32768,-32768,
  -32768,-32768,-32768,   223,   227,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,   -81,  -114,-32768,
  -32768,-32768,   148,-32768,    61,    47,-32768,   146,     1,    -5,
  -32768,   -70,   -42,   231,-32768,    -1,   -69,    25,   -36,-32768,
  -32768,-32768
};


#define	YYLAST		973


static const short yytable[] =
{
      37,   112,    34,   151,   226,    66,   158,   -44,    41,    67,
      42,    71,   227,   120,   121,   122,   117,   207,   208,   230,
      81,    82,    83,    84,    93,    93,   117,   -42,   129,    47,
      69,   231,    52,   241,    42,    40,    53,    90,    43,     3,
      54,    77,    78,    93,    79,   101,    41,   194,    42,   206,
      74,    75,   105,   106,   107,   108,   125,   126,   127,   128,
     205,  -103,  -103,  -103,   101,  -103,   129,  -103,   183,   240,
      48,    64,    49,   107,   108,   154,    69,    42,   224,  -104,
    -104,  -104,    66,  -104,    85,  -104,    67,    50,    93,    51,
      86,    88,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   200,   101,
     201,    66,   194,   225,   190,   193,   163,   164,   107,   108,
      91,   131,    92,   132,    66,    66,    66,    66,   193,    67,
     193,   193,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,    33,    64,    87,
      89,   228,   229,   115,   242,   175,   201,   244,   252,   239,
     201,    73,   116,   247,    43,   117,    80,   152,   129,   249,
       3,    55,   130,   148,   155,   149,    66,     9,   195,    11,
     193,    70,    14,   235,    93,   236,   237,   109,   153,   111,
     204,    64,    64,    64,    93,   181,   184,   185,    20,   197,
     111,   175,   109,   196,    22,   198,   199,    93,    23,    24,
     201,  -102,  -102,  -102,   233,  -102,   234,  -102,   238,   111,
    -115,  -115,  -115,   254,  -115,   243,  -115,   255,   232,    37,
      37,   245,   246,    66,   159,   160,   162,   193,   161,    37,
      37,   250,   251,  -108,  -108,  -108,    76,  -108,     0,  -108,
      93,  -120,  -120,  -120,     0,  -120,   111,  -120,     0,     0,
       0,   189,   192,     0,    -1,     2,     0,     3,     4,     5,
       6,     7,     0,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,     0,  -106,  -106,  -106,     0,
    -106,     0,  -106,     0,   111,    20,     0,     0,    21,     0,
       0,    22,    93,     0,     0,    23,    24,     0,     0,     0,
     -26,   -60,     0,   -60,     0,    94,    95,    96,    97,     0,
      98,    99,     0,   101,     0,     0,     0,   192,    93,     0,
     105,   106,   107,   108,     0,     0,     0,     0,  -116,  -116,
    -116,     0,  -116,     0,  -116,     0,     0,   111,     0,   101,
       0,     0,     0,     0,     0,     0,   105,   106,   107,   108,
       0,     0,    93,     0,  -112,  -112,  -112,     0,  -112,     0,
    -112,     0,     0,   111,     0,    94,    95,    96,    97,   111,
      98,    99,     0,   101,   248,     0,     0,     0,     0,     0,
     105,   106,   107,   108,    93,     0,     0,     0,  -117,  -117,
    -117,     0,  -117,     0,  -117,     0,     0,    94,    95,    96,
      97,     0,    98,    99,    93,   101,     0,     0,     0,     0,
       0,     0,   105,   106,   107,   108,     0,     0,     0,     0,
    -118,  -118,  -118,     0,  -118,   101,  -118,    93,     0,     0,
       0,     0,   105,   106,   107,   108,     0,     0,     0,     0,
    -114,  -114,  -114,     0,  -114,    93,  -114,     0,   101,     0,
       0,     0,     0,     0,     0,   105,   106,   107,   108,     0,
       0,     0,     0,  -109,  -109,  -109,   101,  -109,    93,  -109,
       0,     0,     0,   105,   106,   107,   108,     0,     0,     0,
       0,  -110,  -110,  -110,     0,  -110,    93,  -110,     0,   101,
       0,     0,     0,     0,     0,     0,   105,   106,   107,   108,
      93,     0,     0,     0,  -111,  -111,  -111,   101,  -111,    93,
    -111,     0,     0,     0,   105,   106,   107,   108,     0,     0,
       0,   101,  -113,  -113,  -113,     0,  -113,     0,  -113,     0,
     107,   108,     0,     0,     0,     0,  -105,  -105,  -105,    93,
    -105,     0,  -105,     0,     0,  -107,  -107,  -107,     0,  -107,
       0,  -107,    94,    95,    96,    97,     0,    98,    99,   -97,
     101,     0,     0,     0,   102,   103,   104,   105,   106,   107,
     108,   -58,     0,     0,   -58,     0,     0,     0,   -97,     0,
     -97,     0,     0,     0,     3,    55,     0,     0,   -58,     0,
       0,     9,     0,    56,   -58,    13,    14,     0,     0,     0,
       0,     0,    73,   -58,   -58,   -58,   -58,     0,   -58,     0,
     -58,   -58,    20,     0,     0,    57,     0,     0,    58,     3,
      55,     0,    59,    60,     0,     0,     9,     0,    11,     0,
      70,    14,     3,    55,  -101,  -101,  -101,     0,  -101,     9,
    -101,   110,     0,    13,    14,     0,     0,    20,     0,     0,
      21,     0,     0,    22,     0,     0,     0,    23,    24,     0,
      20,     0,     0,    21,     0,     0,    22,     3,    55,     0,
      23,    24,     0,     0,     9,     0,    11,     0,    13,    14,
       3,    55,     0,     0,     0,     0,     0,   182,     0,    11,
       0,    13,    14,     0,     0,    20,     0,     0,    21,     0,
       0,    22,     0,     0,     0,    23,    24,     0,    20,     0,
       0,    21,     0,     0,    22,     3,    55,     0,    23,    24,
       0,     0,     9,     0,    11,     0,    13,    14,     3,    55,
       0,     0,     0,     0,     0,     9,     0,    11,     0,    70,
      14,     0,     0,    20,     0,     0,    57,     0,     0,    58,
       0,     0,     0,    59,    60,    93,    20,     0,     0,     0,
       0,     0,    22,     0,     0,     0,    23,    24,    94,    95,
      96,    97,     0,    98,    99,   100,   101,     0,    93,     0,
     102,   103,   104,   105,   106,   107,   108,     0,     0,     0,
     157,    94,    95,    96,    97,     0,    98,    99,   100,   101,
       0,    93,     0,   102,   103,   104,   105,   106,   107,   108,
       0,     0,     0,   203,    94,    95,    96,    97,   186,    98,
      99,   100,   101,   187,    93,     0,   102,   103,   104,   105,
     106,   107,   108,     0,     0,     0,     0,    94,    95,    96,
      97,     0,    98,    99,   100,   101,    93,     0,     0,   102,
     103,   104,   105,   106,   107,   108,     0,     0,     0,   133,
     134,   135,   136,     0,   137,   138,   139,   140,    93,     0,
       0,   141,   142,   143,   144,   145,   146,   147,     0,     0,
       0,    94,    95,    96,    97,     0,    98,    99,   100,   101,
      93,     0,     0,   102,   156,   104,   105,   106,   107,   108,
       0,     0,     0,    94,    95,    96,    97,     0,    98,    99,
     100,   101,    93,     0,     0,   102,   202,   104,   105,   106,
     107,   108,     0,     0,     0,    94,    95,    96,    97,     0,
      98,    99,    93,   101,     0,     0,     0,   102,   103,   104,
     105,   106,   107,   108,     0,    94,    95,    96,    97,     0,
      98,    99,     0,   101,     0,     0,     0,     0,     0,     0,
     105,   106,   107,   108
};

static const short yycheck[] =
{
       1,    43,     1,    73,    43,    10,    87,    14,    12,    10,
      14,    12,    51,    49,    50,    51,    45,   131,   132,    48,
      21,    22,    23,    24,     8,     8,    45,    34,    45,     4,
      10,    50,     7,    50,    14,    46,    14,    44,    42,     3,
      42,    16,    17,     8,    19,    29,    12,   116,    14,   130,
      14,    15,    36,    37,    38,    39,    57,    58,    59,    60,
     129,    44,    45,    46,    29,    48,    45,    50,   110,    48,
      47,    10,    49,    38,    39,    76,    10,    14,   148,    44,
      45,    46,    87,    48,    46,    50,    87,    47,     8,    49,
      46,    34,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,    43,    29,
      45,   116,   181,   149,   115,   116,    91,    92,    38,    39,
      47,    47,    49,    49,   129,   130,   131,   132,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,     1,    87,    11,
      14,   152,   153,    28,    43,   156,    45,   227,    43,   201,
      45,    42,    44,   233,    42,    45,    20,    28,    45,   238,
       3,     4,    45,    42,    31,    49,   181,    10,   117,    12,
     181,    14,    15,   184,     8,   186,   187,    41,    28,    43,
     129,   130,   131,   132,     8,    44,    34,    43,    31,    50,
      54,   202,    56,    48,    37,    48,    50,     8,    41,    42,
      45,    44,    45,    46,    42,    48,    43,    50,    44,    73,
      44,    45,    46,     0,    48,    50,    50,     0,   181,   230,
     231,   230,   231,   238,    88,    89,    90,   238,    90,   240,
     241,   240,   241,    44,    45,    46,    15,    48,    -1,    50,
       8,    44,    45,    46,    -1,    48,   110,    50,    -1,    -1,
      -1,   115,   116,    -1,     0,     1,    -1,     3,     4,     5,
       6,     7,    -1,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    -1,    44,    45,    46,    -1,
      48,    -1,    50,    -1,   148,    31,    -1,    -1,    34,    -1,
      -1,    37,     8,    -1,    -1,    41,    42,    -1,    -1,    -1,
      46,    47,    -1,    49,    -1,    21,    22,    23,    24,    -1,
      26,    27,    -1,    29,    -1,    -1,    -1,   181,     8,    -1,
      36,    37,    38,    39,    -1,    -1,    -1,    -1,    44,    45,
      46,    -1,    48,    -1,    50,    -1,    -1,   201,    -1,    29,
      -1,    -1,    -1,    -1,    -1,    -1,    36,    37,    38,    39,
      -1,    -1,     8,    -1,    44,    45,    46,    -1,    48,    -1,
      50,    -1,    -1,   227,    -1,    21,    22,    23,    24,   233,
      26,    27,    -1,    29,   238,    -1,    -1,    -1,    -1,    -1,
      36,    37,    38,    39,     8,    -1,    -1,    -1,    44,    45,
      46,    -1,    48,    -1,    50,    -1,    -1,    21,    22,    23,
      24,    -1,    26,    27,     8,    29,    -1,    -1,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    -1,    -1,
      44,    45,    46,    -1,    48,    29,    50,     8,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    -1,    -1,
      44,    45,    46,    -1,    48,     8,    50,    -1,    29,    -1,
      -1,    -1,    -1,    -1,    -1,    36,    37,    38,    39,    -1,
      -1,    -1,    -1,    44,    45,    46,    29,    48,     8,    50,
      -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,    -1,
      -1,    44,    45,    46,    -1,    48,     8,    50,    -1,    29,
      -1,    -1,    -1,    -1,    -1,    -1,    36,    37,    38,    39,
       8,    -1,    -1,    -1,    44,    45,    46,    29,    48,     8,
      50,    -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,
      -1,    29,    44,    45,    46,    -1,    48,    -1,    50,    -1,
      38,    39,    -1,    -1,    -1,    -1,    44,    45,    46,     8,
      48,    -1,    50,    -1,    -1,    44,    45,    46,    -1,    48,
      -1,    50,    21,    22,    23,    24,    -1,    26,    27,    28,
      29,    -1,    -1,    -1,    33,    34,    35,    36,    37,    38,
      39,    11,    -1,    -1,    14,    -1,    -1,    -1,    47,    -1,
      49,    -1,    -1,    -1,     3,     4,    -1,    -1,    28,    -1,
      -1,    10,    -1,    12,    34,    14,    15,    -1,    -1,    -1,
      -1,    -1,    42,    43,    44,    45,    46,    -1,    48,    -1,
      50,    51,    31,    -1,    -1,    34,    -1,    -1,    37,     3,
       4,    -1,    41,    42,    -1,    -1,    10,    -1,    12,    -1,
      14,    15,     3,     4,    44,    45,    46,    -1,    48,    10,
      50,    12,    -1,    14,    15,    -1,    -1,    31,    -1,    -1,
      34,    -1,    -1,    37,    -1,    -1,    -1,    41,    42,    -1,
      31,    -1,    -1,    34,    -1,    -1,    37,     3,     4,    -1,
      41,    42,    -1,    -1,    10,    -1,    12,    -1,    14,    15,
       3,     4,    -1,    -1,    -1,    -1,    -1,    10,    -1,    12,
      -1,    14,    15,    -1,    -1,    31,    -1,    -1,    34,    -1,
      -1,    37,    -1,    -1,    -1,    41,    42,    -1,    31,    -1,
      -1,    34,    -1,    -1,    37,     3,     4,    -1,    41,    42,
      -1,    -1,    10,    -1,    12,    -1,    14,    15,     3,     4,
      -1,    -1,    -1,    -1,    -1,    10,    -1,    12,    -1,    14,
      15,    -1,    -1,    31,    -1,    -1,    34,    -1,    -1,    37,
      -1,    -1,    -1,    41,    42,     8,    31,    -1,    -1,    -1,
      -1,    -1,    37,    -1,    -1,    -1,    41,    42,    21,    22,
      23,    24,    -1,    26,    27,    28,    29,    -1,     8,    -1,
      33,    34,    35,    36,    37,    38,    39,    -1,    -1,    -1,
      43,    21,    22,    23,    24,    -1,    26,    27,    28,    29,
      -1,     8,    -1,    33,    34,    35,    36,    37,    38,    39,
      -1,    -1,    -1,    43,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,     8,    -1,    33,    34,    35,    36,
      37,    38,    39,    -1,    -1,    -1,    -1,    21,    22,    23,
      24,    -1,    26,    27,    28,    29,     8,    -1,    -1,    33,
      34,    35,    36,    37,    38,    39,    -1,    -1,    -1,    21,
      22,    23,    24,    -1,    26,    27,    28,    29,     8,    -1,
      -1,    33,    34,    35,    36,    37,    38,    39,    -1,    -1,
      -1,    21,    22,    23,    24,    -1,    26,    27,    28,    29,
       8,    -1,    -1,    33,    34,    35,    36,    37,    38,    39,
      -1,    -1,    -1,    21,    22,    23,    24,    -1,    26,    27,
      28,    29,     8,    -1,    -1,    33,    34,    35,    36,    37,
      38,    39,    -1,    -1,    -1,    21,    22,    23,    24,    -1,
      26,    27,     8,    29,    -1,    -1,    -1,    33,    34,    35,
      36,    37,    38,    39,    -1,    21,    22,    23,    24,    -1,
      26,    27,    -1,    29,    -1,    -1,    -1,    -1,    -1,    -1,
      36,    37,    38,    39
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
   Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif


#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

#ifdef YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif
#endif

#line 315 "/usr/share/bison/bison.simple"


/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 1:
#line 112 "parse.yy"
{ logic_program = yyvsp[0].pn; }
    break;
case 2:
#line 116 "parse.yy"
{ yyval.pn = new ParseNode(PT_RULES, NULL, yyvsp[-1].pn, yyvsp[-2].pn, lineno); }
    break;
case 3:
#line 118 "parse.yy"
{ yyval.pn = new ParseNode(PT_RULES, NULL, yyvsp[-1].pn, yyvsp[-2].pn, lineno); }
    break;
case 4:
#line 120 "parse.yy"
{ yyerrok; }
    break;
case 5:
#line 122 "parse.yy"
{ yyval.pn = new ParseNode(PT_RULES, NULL, yyvsp[0].pn, yyvsp[-1].pn, lineno); }
    break;
case 6:
#line 124 "parse.yy"
{ yyval.pn = NULL; }
    break;
case 7:
#line 128 "parse.yy"
{ yyval.pn = yyvsp[0].pn; }
    break;
case 8:
#line 130 "parse.yy"
{ yyval.pn = yyvsp[0].pn; }
    break;
case 9:
#line 132 "parse.yy"
{ yyval.pn = yyvsp[0].pn; }
    break;
case 10:
#line 136 "parse.yy"
{read_option(yyvsp[0].s); yyval.pn = NULL; }
    break;
case 11:
#line 140 "parse.yy"
{ function_table->Insert(yyvsp[0].s); yyval.pn = NULL; delete yyvsp[0].s;}
    break;
case 12:
#line 142 "parse.yy"
{ yyval.pn = new ParseNode(PT_DECLARATION, NULL, NULL, yyvsp[0].pn, 
	                         lineno); }
    break;
case 13:
#line 145 "parse.yy"
{ yyval.pn = new ParseNode(PT_HIDE, NULL, yyvsp[0].pn, NULL, lineno); }
    break;
case 14:
#line 147 "parse.yy"
{ yyval.pn = NULL; sys_data.hide_all = 1; }
    break;
case 15:
#line 149 "parse.yy"
{ 
              lval1 = yyvsp[0].pn->Eval();
              if (lval1 == ERROR_NUMBER) {
                 error(USR_ERR, "%s: Invalid constant declaration: ",
                       error_file_and_line(lineno));
                 fprintf(stderr, "\t\tconst %s = ", yyvsp[-2].s);
                 yyvsp[0].pn->PrintExpression();
                 fprintf(stderr, ".\n   The expression is not a"
    	                 " valid constant expression\n");
              }	 
              long index;
              int needed = 0;
              int exists = 0;

              if ((index = numeric_constant_table->LookIndex(yyvsp[-2].s)) >= 0) { 
	          exists = 1;
		  needed = 1;
                  if (lval1 != numeric_constant_table->Lookup(yyvsp[-2].s)) { 
                    long val;
                    if (command_line_constants) {
                      command_line_constants->ClearIterator();
                      while ((val = command_line_constants->Iterate())) {
		        --val;
                        if (val == index) {
                           needed = 0;
                           break;
                        }
                      }
                   }
                 }
              }
              if (!exists) {
                  numeric_constant_table->Insert(yyvsp[-2].s);
                  numeric_constant_table->SetValue(yyvsp[-2].s, lval1);
              }
              if (needed) {
	          warn(WARN_SIMILAR, lineno, "the numeric constant '%s'"
	               " has two values defined for it, '%ld' and"
		       " '%ld'", yyvsp[-2].s, numeric_constant_table->Lookup(yyvsp[-2].s), lval1);
              }
              if (enabled_warning(WARN_SIMILAR) && (constant_table->Lookup(yyvsp[-2].s) >= 0)) { 
                 warn(WARN_SIMILAR, lineno, "the constant '%s' is used as "
		     "both numeric and symbolic constant", yyvsp[-2].s);
              }
	      delete yyvsp[-2].s;
              delete yyvsp[0].pn;
              yyval.pn = NULL; }
    break;
case 16:
#line 197 "parse.yy"
{ yyerror("numeric constant declarations now need a '='");
	      yyval.pn = NULL;
	      delete yyvsp[-1].pn;
              delete yyvsp[0].pn;
	    }
    break;
case 17:
#line 203 "parse.yy"
{
              error(USR_ERR, "%s: Invalid constant declaration: ",
                    error_file_and_line(lineno));
              fprintf(stderr, "\t\tconst %s = ", yyvsp[-2].s);
              yyvsp[0].pn->PrintExpression();
              fprintf(stderr, ".\n   The symbol '%s' is a variable, not"
	              " a constant\n", yyvsp[-2].s);
              yyval.pn = NULL;
              delete yyvsp[-2].s;
              delete yyvsp[0].pn;
            }
    break;
case 18:
#line 215 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXTERNAL, NULL, yyvsp[0].pn, NULL, lineno);
	    }
    break;
case 19:
#line 218 "parse.yy"
{ yyval.pn = new ParseNode(PT_SHOW, NULL, yyvsp[0].pn, NULL, lineno); }
    break;
case 20:
#line 220 "parse.yy"
{ sys_data.has_implicit_domains = 1; yyval.pn = new
              ParseNode(PT_DIRECTIVE, NULL, NULL, yyvsp[0].pn, lineno) ; }
    break;
case 21:
#line 225 "parse.yy"
{ yyval.pn = new ParseNode(PT_RULE, NULL, yyvsp[0].pn, NULL, lineno); }
    break;
case 22:
#line 227 "parse.yy"
{ yyval.pn = new ParseNode(PT_RULE, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno); }
    break;
case 23:
#line 229 "parse.yy"
{ yyval.pn = new ParseNode(PT_RULE, NULL, NULL, yyvsp[0].pn, lineno); }
    break;
case 24:
#line 233 "parse.yy"
{ yyval.pn = yyvsp[0].pn; }
    break;
case 25:
#line 235 "parse.yy"
{ yyval.pn = yyvsp[0].pn; }
    break;
case 26:
#line 237 "parse.yy"
{ yyval.pn = NULL; }
    break;
case 27:
#line 241 "parse.yy"
{ yyval.pn = new ParseNode(PT_TAIL, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno); }
    break;
case 28:
#line 243 "parse.yy"
{ yyval.pn = new ParseNode(PT_TAIL, NULL, yyvsp[0].pn, NULL, lineno); }
    break;
case 29:
#line 245 "parse.yy"
{ yyval.pn = new ParseNode(PT_TAIL, NULL, yyvsp[0].pn, NULL, lineno); }
    break;
case 30:
#line 249 "parse.yy"
{ yyval.pn = new ParseNode(PT_BASIC_TAIL, NULL, yyvsp[0].pn, yyvsp[-2].pn, lineno);}
    break;
case 31:
#line 251 "parse.yy"
{ yyval.pn = new ParseNode(PT_BASIC_TAIL, NULL, yyvsp[0].pn, NULL, lineno);}
    break;
case 32:
#line 253 "parse.yy"
{ yyval.pn = new ParseNode(PT_BASIC_TAIL, NULL, yyvsp[0].pn, yyvsp[-2].pn, lineno);
             if (yyvsp[0].pn->type == PT_CONSTANT) yyvsp[0].pn->type = PT_ATOM; }
    break;
case 33:
#line 256 "parse.yy"
{ yyval.pn = new ParseNode(PT_BASIC_TAIL, NULL, yyvsp[0].pn, NULL,
                                lineno); }
    break;
case 34:
#line 261 "parse.yy"
{ tmp_node = new ParseNode(PT_BOUND, NULL, yyvsp[-4].pn, yyvsp[0].pn,
	                                lineno); 
               yyval.pn = new ParseNode(PT_SPECIAL_TAIL, NULL, tmp_node, yyvsp[-2].pn,
  	                          lineno, 0, 0, 1); }
    break;
case 35:
#line 266 "parse.yy"
{ tmp_node = new ParseNode(PT_BOUND, NULL, yyvsp[-4].pn, yyvsp[0].pn,
	                                lineno);
               yyval.pn = new ParseNode(PT_SPECIAL_TAIL, NULL, tmp_node, yyvsp[-2].pn,
  	                          lineno); }
    break;
case 36:
#line 273 "parse.yy"
{ tmp_node = new ParseNode(PT_BOUND, NULL, yyvsp[-4].pn, yyvsp[0].pn, lineno);
               yyval.pn = new ParseNode(PT_SPECIAL_HEAD, NULL, tmp_node, yyvsp[-2].pn,
  	                          lineno); }
    break;
case 37:
#line 277 "parse.yy"
{ tmp_node = new ParseNode(PT_BOUND, NULL, yyvsp[-4].pn, yyvsp[0].pn, lineno);
	       yyval.pn = new ParseNode(PT_SPECIAL_HEAD, NULL, tmp_node, yyvsp[-2].pn,
  	                          lineno, 0, 0,1); }
    break;
case 38:
#line 281 "parse.yy"
{ yyval.pn = new ParseNode(PT_SPECIAL_HEAD, NULL, NULL, yyvsp[0].pn, 
  	                          lineno);  }
    break;
case 39:
#line 284 "parse.yy"
{ yyval.pn = new ParseNode(PT_ORDERED_DISJUNCTION, NULL, NULL,
                         	  yyvsp[0].pn, lineno);
             }
    break;
case 40:
#line 288 "parse.yy"
{ yyval.pn = new ParseNode(PT_ORDERED_DISJUNCTION, NULL, yyvsp[-2].pn,
               yyvsp[0].pn, lineno);
             }
    break;
case 41:
#line 296 "parse.yy"
{ yyval.pn = new ParseNode(PT_LITERAL_LIST, NULL, yyvsp[0].pn, yyvsp[-2].pn, lineno); }
    break;
case 42:
#line 298 "parse.yy"
{ yyval.pn = new ParseNode(PT_LITERAL_LIST, NULL, yyvsp[0].pn, NULL,
	   lineno);  }
    break;
case 43:
#line 303 "parse.yy"
{ if (strcmp(yyvsp[-1].s, "x")) {
	     yyerror("parse error");
             yyval.pn = NULL;
           } else {
             yyval.pn = new ParseNode(PT_LITERAL_LIST, NULL, yyvsp[0].pn, yyvsp[-2].pn, lineno);
           }
         }
    break;
case 44:
#line 311 "parse.yy"
{ yyval.pn = new ParseNode(PT_LITERAL_LIST, NULL, yyvsp[0].pn, NULL,
	   lineno);  }
    break;
case 45:
#line 317 "parse.yy"
{ yyval.pn = yyvsp[0].pn; }
    break;
case 46:
#line 319 "parse.yy"
{ yyvsp[0].pn->l2.negative = 1; yyval.pn = yyvsp[0].pn; }
    break;
case 47:
#line 321 "parse.yy"
{ yyval.pn = new ParseNode(PT_CONDITION, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno); }
    break;
case 48:
#line 323 "parse.yy"
{ yyval.pn = new ParseNode(PT_CONDITION, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno);
	  yyval.pn->l2.negative = 1;}
    break;
case 49:
#line 328 "parse.yy"
{ yyval.pn = yyvsp[0].pn; }
    break;
case 50:
#line 330 "parse.yy"
{ yyval.pn = yyvsp[-2].pn; if (yyvsp[-2].pn->type != PT_CONDITION) yyval.pn->left = yyvsp[0].pn;
                   else yyval.pn->left->left = yyvsp[0].pn; }
    break;
case 51:
#line 335 "parse.yy"
{ yyval.pn = new ParseNode(PT_LITERAL_LIST, NULL, yyvsp[0].pn, yyvsp[-2].pn, lineno); }
    break;
case 52:
#line 337 "parse.yy"
{ yyval.pn = new ParseNode(PT_LITERAL_LIST, NULL, yyvsp[0].pn, NULL,
	   lineno);  }
    break;
case 53:
#line 340 "parse.yy"
{ yyval.pn = new ParseNode(PT_LITERAL_LIST, NULL, yyvsp[0].pn, NULL,
	   lineno);  }
    break;
case 54:
#line 343 "parse.yy"
{ yyval.pn = new ParseNode(PT_LITERAL_LIST, NULL, yyvsp[0].pn, yyvsp[-2].pn, lineno); }
    break;
case 55:
#line 347 "parse.yy"
{ yyval.pn =yyvsp[0].pn; }
    break;
case 56:
#line 349 "parse.yy"
{ yyval.pn = yyvsp[0].pn; }
    break;
case 57:
#line 355 "parse.yy"
{ yyval.pn = new ParseNode(PT_ATOM, yyvsp[-3].s, NULL, yyvsp[-1].pn, lineno); }
    break;
case 58:
#line 357 "parse.yy"
{ yyval.pn = new ParseNode(PT_ATOM, yyvsp[0].s, NULL, NULL, lineno); }
    break;
case 59:
#line 361 "parse.yy"
{ yyval.pn = yyvsp[0].pn; }
    break;
case 60:
#line 363 "parse.yy"
{ yyval.pn = NULL; }
    break;
case 61:
#line 368 "parse.yy"
{ yyval.pn = new ParseNode(PT_FUNCTION, yyvsp[-3].s, NULL, yyvsp[-1].pn, lineno);}
    break;
case 62:
#line 370 "parse.yy"
{ yyval.pn = new ParseNode(PT_FUNCTION, yyvsp[-3].s, NULL, yyvsp[-1].pn, lineno);
           yyval.pn->l2.negative = 1; }
    break;
case 63:
#line 373 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, NULL, yyvsp[-1].pn, lineno,
	                      FUN_WEIGHT); }
    break;
case 64:
#line 376 "parse.yy"
{ yyvsp[-1].pn->l2.negative = 1;
           yyval.pn = new ParseNode(PT_EXPR, NULL, NULL, yyvsp[-1].pn, lineno,
	                      FUN_WEIGHT); }
    break;
case 65:
#line 382 "parse.yy"
{ yyval.pn = new ParseNode(PT_ARGUMENTS, NULL, yyvsp[0].pn, NULL, lineno);}
    break;
case 66:
#line 384 "parse.yy"
{ yyval.pn = new ParseNode(PT_ARGUMENTS, NULL, yyvsp[0].pn, yyvsp[-2].pn, lineno); }
    break;
case 67:
#line 388 "parse.yy"
{ yyval.pn = new ParseNode(PT_ARGLIST, NULL, yyvsp[0].pn, yyvsp[-2].pn, lineno); }
    break;
case 68:
#line 390 "parse.yy"
{ yyval.pn = new ParseNode(PT_ARGLIST, NULL, yyvsp[0].pn, NULL , lineno); }
    break;
case 69:
#line 394 "parse.yy"
{ yyval.pn = yyvsp[0].pn; }
    break;
case 70:
#line 396 "parse.yy"
{ lval1 = yyvsp[0].pn->Eval();
       if (lval1 != ERROR_NUMBER) {
           yyval.pn = new ParseNode(PT_NUMBER, NULL, NULL, NULL,
                lineno,lval1);
           delete yyvsp[0].pn;
       } else {
         yyval.pn = yyvsp[0].pn;
       }
     }
    break;
case 71:
#line 406 "parse.yy"
{ yyval.pn = yyvsp[0].pn;
       if (yyval.pn->right == NULL) {
         lval1 = numeric_constant_table->Lookup(yyval.pn->sval);
         if (lval1 != ERROR_NUMBER) {
            yyval.pn->type = PT_NUMBER;
            yyval.pn->l1.val = lval1;
         } else {
          yyval.pn->type = PT_CONSTANT;
         }
       }
     }
    break;
case 72:
#line 418 "parse.yy"
{
      /* crude workaround for situations like a | 1, where 'a' is
	 actually a numeric constant */
       if ( yyvsp[-2].pn->right ) {
	error(USR_ERR, "%s: parse error before '|'\n",
              error_file_and_line(lineno));
       }
       yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_OR);
       lval1 = numeric_constant_table->Lookup(yyvsp[-2].pn->sval);
       if (lval1 != ERROR_NUMBER) {
         delete yyval.pn->left;
         yyval.pn->left = new ParseNode(PT_NUMBER, NULL, NULL, NULL, lineno,
                                  lval1);
       } else {
         yyvsp[-2].pn->type = PT_CONSTANT;
       }
     }
    break;
case 73:
#line 438 "parse.yy"
{ lval1 = numeric_constant_table->Lookup(yyvsp[0].s);
	  if (lval1 == ERROR_NUMBER)  
		yyval.pn = new ParseNode(PT_CONSTANT, yyvsp[0].s, NULL, NULL,
                                   lineno);
	  else {
	     yyval.pn = new ParseNode(PT_NUMBER, NULL, NULL, NULL, lineno,lval1);
             delete yyvsp[0].s; }
	}
    break;
case 74:
#line 447 "parse.yy"
{ yyval.pn = new ParseNode(PT_NUMBER, NULL, NULL, NULL, lineno,
	 yyvsp[0].l);}
    break;
case 75:
#line 452 "parse.yy"
{ 
        yyval.pn = new ParseNode(PT_RANGE, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno);
      }
    break;
case 76:
#line 456 "parse.yy"
{
        yyval.pn = NULL;
	yyerror("conjunctive ranges are no longer supported, "
	        "please use conditions instead");
        /* lval1 = $1->Eval();
        lval2 = $3->Eval();
  	$$ = new ParseNode(PT_RANGE, NULL, NULL, NULL, lineno, lval1,
                           lval2, 1); */}
    break;
case 77:
#line 467 "parse.yy"
{ yyval.pn = yyvsp[-1].pn; yyval.pn->i1.paren =1; }
    break;
case 78:
#line 469 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, NULL, yyvsp[-1].pn, lineno,
	                  FUN_ABS); yyval.pn->i1.paren = 1;}
    break;
case 79:
#line 472 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, NULL, yyvsp[-1].pn, lineno,
                          FUN_NORM); }
    break;
case 80:
#line 475 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_EXP); }
    break;
case 81:
#line 477 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, NULL, yyvsp[0].pn , lineno,
                          FUN_MINUS);
       yyval.pn->left = new ParseNode(PT_NUMBER, NULL, NULL, NULL, lineno, 0);}
    break;
case 82:
#line 481 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_PLUS); }
    break;
case 83:
#line 483 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_MINUS); }
    break;
case 84:
#line 485 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_TIMES); }
    break;
case 85:
#line 487 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_DIV); }
    break;
case 86:
#line 489 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_MOD); }
    break;
case 87:
#line 491 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_EQ); }
    break;
case 88:
#line 493 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_NEQ); }
    break;
case 89:
#line 495 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_LT); }
    break;
case 90:
#line 497 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_LE); }
    break;
case 91:
#line 499 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_GT); }
    break;
case 92:
#line 501 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_GE); }
    break;
case 93:
#line 503 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[0].pn ,NULL , lineno,
                          FUN_NOT);
       yyval.pn->right = new ParseNode(PT_NUMBER, NULL, NULL, NULL, lineno,
                                 0);}
    break;
case 94:
#line 508 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_AND); }
    break;
case 95:
#line 510 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_OR); }
    break;
case 96:
#line 512 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_XOR); }
    break;
case 97:
#line 514 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno,
                         FUN_ASSIGN); }
    break;
case 98:
#line 517 "parse.yy"
{ yyval.pn = yyvsp[0].pn; }
    break;
case 99:
#line 519 "parse.yy"
{ yyval.pn = yyvsp[0].pn; }
    break;
case 100:
#line 521 "parse.yy"
{ yyval.pn = new ParseNode(PT_VARIABLE, yyvsp[0].s, NULL, NULL, lineno); }
    break;
case 101:
#line 525 "parse.yy"
{ yyval.pn = yyvsp[-1].pn; }
    break;
case 102:
#line 527 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, NULL, yyvsp[-1].pn, lineno,
	                  FUN_ABS);}
    break;
case 103:
#line 530 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, NULL, yyvsp[0].pn , lineno,
                          FUN_MINUS);
       yyval.pn->left = new ParseNode(PT_NUMBER, NULL, NULL, NULL, lineno, 0);}
    break;
case 104:
#line 534 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_PLUS); }
    break;
case 105:
#line 536 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_MINUS); }
    break;
case 106:
#line 538 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_TIMES); }
    break;
case 107:
#line 540 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_DIV); }
    break;
case 108:
#line 542 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_MOD); }
    break;
case 109:
#line 544 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_EQ); }
    break;
case 110:
#line 546 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_NEQ); }
    break;
case 111:
#line 548 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_LT); }
    break;
case 112:
#line 550 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_LE); }
    break;
case 113:
#line 552 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_GT); }
    break;
case 114:
#line 554 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_GE); }
    break;
case 115:
#line 556 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[0].pn ,NULL , lineno,
                          FUN_NOT);
       yyval.pn->right = new ParseNode(PT_NUMBER, NULL, NULL, NULL, lineno,
                                 0);}
    break;
case 116:
#line 561 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_AND); }
    break;
case 117:
#line 563 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_OR); }
    break;
case 118:
#line 565 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL, yyvsp[-2].pn, yyvsp[0].pn, lineno, FUN_XOR); }
    break;
case 119:
#line 567 "parse.yy"
{ yyval.pn = new ParseNode(PT_EXPR, NULL,  yyvsp[-2].pn, yyvsp[0].pn, lineno,
                          FUN_ASSIGN);}
    break;
case 120:
#line 570 "parse.yy"
{ yyval.pn = yyvsp[0].pn; }
    break;
case 121:
#line 575 "parse.yy"
{ yyval.pn = new ParseNode(PT_LITERAL_LIST, NULL, yyvsp[0].pn, yyvsp[-2].pn, lineno);}
    break;
case 122:
#line 577 "parse.yy"
{ yyval.pn = new ParseNode(PT_LITERAL_LIST, NULL, yyvsp[0].pn, NULL, lineno);}
    break;
case 123:
#line 581 "parse.yy"
{ yyval.pn = yyvsp[0].pn; }
    break;
case 124:
#line 583 "parse.yy"
{ yyval.pn = 0; }
    break;
case 125:
#line 588 "parse.yy"
{ yyval.pn = new ParseNode(PT_COMPUTE, NULL, NULL, yyvsp[-1].pn,
	                          lineno, yyvsp[-3].l);}
    break;
case 126:
#line 593 "parse.yy"
{ yyval.pn = new ParseNode(PT_MAXIMIZE, NULL, NULL, yyvsp[-1].pn,
	                          lineno); }
    break;
case 127:
#line 596 "parse.yy"
{ yyval.pn = new ParseNode(PT_MAXIMIZE, NULL, NULL, yyvsp[-1].pn,
	                          lineno, 0, 0, 1); }
    break;
case 128:
#line 599 "parse.yy"
{ yyval.pn = new ParseNode(PT_MINIMIZE, NULL, NULL, yyvsp[-1].pn,
	                          lineno); }
    break;
case 129:
#line 602 "parse.yy"
{ yyval.pn = new ParseNode(PT_MINIMIZE, NULL, NULL, yyvsp[-1].pn,
	                          lineno,0,0,1); }
    break;
case 130:
#line 607 "parse.yy"
{
         if (yyvsp[0].pn->type == PT_CONSTANT) {
           if (!strcmp(yyvsp[0].pn->sval, "all")) {
             yyval.l = 0;
           } else {
             error(USR_ERR, "%s: invalid number of models. Use a "
                   "positive integer, a constant valued expression, "
                   "or \"all\"", 
                   error_file_and_line(lineno));
                   yyval.l = 1;
           }
         } else {
           yyval.l = yyvsp[0].pn->Eval();
           if (yyval.l == ERROR_NUMBER || yyval.l < 0) {
             error(USR_ERR, "%s: invalid number of models. Use a "
                  "positive integer, a constant valued expression, or "
	          "\"all\"", error_file_and_line(lineno));
           }
         }
         delete yyvsp[0].pn;
       }
    break;
case 131:
#line 629 "parse.yy"
{ yyval.l = 1; }
    break;
}

#line 705 "/usr/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "Error: state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

/*--------------.
| yyerrhandle.  |
`--------------*/
yyerrhandle:
  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}
#line 632 "parse.yy"
