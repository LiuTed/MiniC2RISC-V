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
    INTEGER = 258,
    IDENTIFIER = 259,
    SEMI = 260,
    COMMA = 261,
    TYPE_INT = 262,
    IF = 263,
    ELSE = 264,
    WHILE = 265,
    RETURN = 266,
    LOGIC_AND = 267,
    LOGIC_OR = 268,
    LOGIC_NOT = 269,
    LE = 270,
    LT = 271,
    GE = 272,
    GT = 273,
    EQ = 274,
    NE = 275,
    ASSIGN = 276,
    ADD = 277,
    SUB = 278,
    MUL = 279,
    DIV = 280,
    MOD = 281,
    IFX = 282,
    MINUS = 283
  };
#endif
/* Tokens.  */
#define INTEGER 258
#define IDENTIFIER 259
#define SEMI 260
#define COMMA 261
#define TYPE_INT 262
#define IF 263
#define ELSE 264
#define WHILE 265
#define RETURN 266
#define LOGIC_AND 267
#define LOGIC_OR 268
#define LOGIC_NOT 269
#define LE 270
#define LT 271
#define GE 272
#define GT 273
#define EQ 274
#define NE 275
#define ASSIGN 276
#define ADD 277
#define SUB 278
#define MUL 279
#define DIV 280
#define MOD 281
#define IFX 282
#define MINUS 283

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 14 "parser.y" /* yacc.c:1909  */

	struct node *NPtr;
	char *container;
	int IntVal;

#line 116 "y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
