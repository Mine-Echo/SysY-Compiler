%option noinput
%option nounput
%option noyywrap

%{
#include <ast/ast.h>
#include "sysy.tab.hh"
%}

digit [0-9]
blank [ \t\n]
letter_ [a-zA-Z_]

%%

"//"(.)*(\n)?                   {printf("%s",yytext);}
"/*"([^*]|(\*+[^*/]))*"*/"      {printf("%s",yytext);}

"int"                           {printf("%s",yytext);return INT_TYPE;}
"void"                          {printf("%s",yytext);return VOID_TYPE;}
"if"                            {printf("%s",yytext);return IF;}
"else"                          {printf("%s",yytext);return ELSE;}
"while"                         {printf("%s",yytext);return WHILE;}
"break"                         {printf("%s",yytext);return BREAK;}
"continue"                      {printf("%s",yytext);return CONTINUE;}
"return"                        {printf("%s",yytext);return RETURN;}

";"                             {printf("%s",yytext);return SEMICOLON;}
","                             {printf("%s",yytext);return COMMA;}
"("                             {printf("%s",yytext);return LEFT_PAREN;}
")"                             {printf("%s",yytext);return RIGHT_PAREN;}
"["                             {printf("%s",yytext);return LEFT_BRACKET;}
"]"                             {printf("%s",yytext);return RIGHT_BRACKET;}
"{"                             {printf("%s",yytext);return LEFT_CURLY_BRACKET;}
"}"                             {printf("%s",yytext);return RIGHT_CURLY_BRACKET;}

">="                            {printf("%s",yytext);return GE;}
"<="                            {printf("%s",yytext);return LE;}
"=="                            {printf("%s",yytext);return EQ;}
"!="                            {printf("%s",yytext);return NE;}
"+"                             {printf("%s",yytext);return ADD;}
"&&"                            {printf("%s",yytext);return AND;}
"||"                            {printf("%s",yytext);return OR;}
"-"                             {printf("%s",yytext);return SUB;}
"!"                             {printf("%s",yytext);return NOT;}
"*"                             {printf("%s",yytext);return MUL;}
"/"                             {printf("%s",yytext);return DIV;}
"%"                             {printf("%s",yytext);return MOD;}
">"                             {printf("%s",yytext);return G;}
"<"                             {printf("%s",yytext);return L;}
"="                             {printf("%s",yytext);return ASSIGN;}

{digit}+                        {printf("%s",yytext);yylval.ival = atoi(yytext);return INT_CONST;}
{letter_}({letter_}|{digit})*   {printf("%s",yytext);yylval.str = strdup(yytext);return IDENT;}

{blank}                         {printf("%s",yytext);}
.                               {printf("ERROR(%s)\n", yytext); }

%%
