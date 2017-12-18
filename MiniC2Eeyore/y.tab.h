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
    TYPE_LONG = 263,
    IF = 264,
    ELSE = 265,
    WHILE = 266,
    RETURN = 267,
    DO = 268,
    LOGIC_AND = 269,
    LOGIC_OR = 270,
    LOGIC_NOT = 271,
    LT = 272,
    GT = 273,
    EQ = 274,
    NE = 275,
    ADD = 276,
    SUB = 277,
    MUL = 278,
    DIV = 279,
    MOD = 280,
    IFX = 281,
    ASSIGN = 282,
    MINUS = 283
  };
#endif
/* Tokens.  */
#define INTEGER 258
#define IDENTIFIER 259
#define SEMI 260
#define COMMA 261
#define TYPE_INT 262
#define TYPE_LONG 263
#define IF 264
#define ELSE 265
#define WHILE 266
#define RETURN 267
#define DO 268
#define LOGIC_AND 269
#define LOGIC_OR 270
#define LOGIC_NOT 271
#define LT 272
#define GT 273
#define EQ 274
#define NE 275
#define ADD 276
#define SUB 277
#define MUL 278
#define DIV 279
#define MOD 280
#define IFX 281
#define ASSIGN 282
#define MINUS 283

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 17 "parser.y" /* yacc.c:1909  */

	struct _var *var;
	char *container;
	long int IntVal;

#line 116 "y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
