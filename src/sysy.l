%option noinput
%option nounput
%option noyywrap
%option yylineno

%{
#include <ast/ast.h>
#include "sysy.tab.hh"
#include <stdio.h>
#include <stdlib.h>
extern int syntax_error;
%}

dec_digit [0-9]
oct_digit [0-7]
hex_digit [0-9a-fA-F]
blank [ \t\n]
letter_ [a-zA-Z_]

%%

"//"([^\n])*                    {}
"/*"([^*]|(\*+[^*/]))*"*/"      {}

"int"                           {return INT_TYPE;}
"void"                          {return VOID_TYPE;}
"if"                            {return IF;}
"else"                          {return ELSE;}
"while"                         {return WHILE;}
"break"                         {return BREAK;}
"continue"                      {return CONTINUE;}
"return"                        {return RETURN;}

";"                             {return SEMICOLON;}
","                             {return COMMA;}
"("                             {return LEFT_PAREN;}
")"                             {return RIGHT_PAREN;}
"["                             {return LEFT_BRACKET;}
"]"                             {return RIGHT_BRACKET;}
"{"                             {return LEFT_CURLY_BRACKET;}
"}"                             {return RIGHT_CURLY_BRACKET;}

">="                            {return GE;}
"<="                            {return LE;}
"=="                            {return EQ;}
"!="                            {return NE;}
"+"                             {return ADD;}
"&&"                            {return AND;}
"||"                            {return OR;}
"-"                             {return SUB;}
"!"                             {return NOT;}
"*"                             {return MUL;}
"/"                             {return DIV;}
"%"                             {return MOD;}
">"                             {return G;}
"<"                             {return L;}
"="                             {return ASSIGN;}

[1-9]{dec_digit}*                   {yylval.ival = atoi(yytext); return INT_CONST;}
"0"{oct_digit}*                     {yylval.ival = (int)strtol(yytext, NULL, 8); return INT_CONST;}
("0x"|"0X"){hex_digit}+             {yylval.ival = (int)strtol(yytext, NULL, 16); return INT_CONST;}
{letter_}({letter_}|{dec_digit})*   {yylval.str = strdup(yytext);return IDENT;}

{blank}                         {}

"0"{dec_digit}+                 {syntax_error=1; printf("[SysY] lexical error at line %d: Illegal octal number '%s'.\n", yylineno, yytext); yylval.ival = 0; return INT_CONST;}
("0x"|"0X")[0-9a-zA-Z]+         {syntax_error=1; printf("[SysY] lexical error at line %d: Illegal hexadecimal number '%s'.\n", yylineno, yytext); yylval.ival = 0; return INT_CONST;}
.                               {syntax_error=1; printf("[SysY] lexical error at Line %d: Unexpected characters '%s'.\n", yylineno, yytext);}
{dec_digit}+{letter_}+          {syntax_error=1; printf("[SysY] lexical error at Line %d: Illegal identifier '%s'.\n", yylineno, yytext);}

%%
