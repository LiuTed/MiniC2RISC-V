/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ADD = 258,
    SUB = 259,
    MUL = 260,
    DIV = 261,
    MOD = 262,
    AND = 263,
    OR = 264,
    NOT = 265,
    LT = 266,
    GT = 267,
    EQ = 268,
    NE = 269,
    VARIABLE = 270,
    PARAMETER = 271,
    LABEL = 272,
    FUNC = 273,
    INTEGER = 274,
    VAR = 275,
    CALL = 276,
    END = 277,
    GOTO = 278,
    PARAM = 279,
    RETURN = 280,
    IF = 281,
    NL = 282,
    AEV = 283,
    VEA = 284,
    VEI = 285,
    VEV = 286,
    INFUNC = 287,
    OUTFUNC = 288,
    DYOP = 289,
    UNOP = 290,
    DYOPI = 291
  };
#endif
/* Tokens.  */
#define ADD 258
#define SUB 259
#define MUL 260
#define DIV 261
#define MOD 262
#define AND 263
#define OR 264
#define NOT 265
#define LT 266
#define GT 267
#define EQ 268
#define NE 269
#define VARIABLE 270
#define PARAMETER 271
#define LABEL 272
#define FUNC 273
#define INTEGER 274
#define VAR 275
#define CALL 276
#define END 277
#define GOTO 278
#define PARAM 279
#define RETURN 280
#define IF 281
#define NL 282
#define AEV 283
#define VEA 284
#define VEI 285
#define VEV 286
#define INFUNC 287
#define OUTFUNC 288
#define DYOP 289
#define UNOP 290
#define DYOPI 291

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 17 "parser.y" /* yacc.c:1909  */

	const char *name;
	int op;
	struct node *NPtr;

#line 132 "y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
