%{
#include <stdio.h>
#include <ast/ast.h>
void yyerror(const char *s);
extern int yylex(void);
extern int syntax_error;
extern NodePtr root;
%}


%union {
    int ival;
    char* str;
}

%token INT_TYPE VOID_TYPE IF ELSE WHILE BREAK CONTINUE RETURN
%token COMMA SEMICOLON LEFT_BRACKET RIGHT_BRACKET LEFT_PAREN RIGHT_PAREN LEFT_CURLY_BRACKET RIGHT_CURLY_BRACKET
%token ASSIGN OR AND EQ NE GE LE G L ADD SUB MUL DIV MOD NOT
%token <ival>INT_CONST
%token <str>IDENT

/* %right ASSIGN
%left OR
%left AND
%left EQ NE
%left GE LE G L
%left ADD SUB
%left MUL DIV MOD
%right NOT
%left LEFT_BRACKET RIGHT_BRACKET LEFT_PAREN RIGHT_PAREN */

%start CompUnit



%%
CompUnit : Decl
         | FuncDef
         | CompUnit Decl
         | CompUnit FuncDef
         ;

/* BType : INT_TYPE
      ; */
Decl : VarDecl
     ;
//replace BType with INT_TYPE
VarDecl : INT_TYPE VarDefList SEMICOLON
        ;
VarDefList : VarDef
           | VarDefList COMMA VarDef
           ;
VarDef : IDENT ASSIGN InitVal
       | IDENT ArrayDef
       ;
ArrayDef : /*empty*/
         | ArrayDef LEFT_BRACKET INT_CONST RIGHT_BRACKET
         ;
InitVal : Exp
        ;
//replace FuncType with INT_TYPE|VOID_TYP
FuncDef : INT_TYPE IDENT LEFT_PAREN RIGHT_PAREN Block
        | VOID_TYPE IDENT LEFT_PAREN RIGHT_PAREN Block
        | INT_TYPE IDENT LEFT_PAREN FuncFParams RIGHT_PAREN Block
        | VOID_TYPE IDENT LEFT_PAREN FuncFParams RIGHT_PAREN Block
        ;
/* FuncType : INT_TYPE
         | VOID_TYPE
         ; */
FuncFParams : FuncFParam
            | FuncFParams COMMA FuncFParam
            ;
//replace BType with INT_TYPE
FuncFParam : INT_TYPE IDENT 
           | INT_TYPE IDENT LEFT_BRACKET RIGHT_BRACKET ArrayDef
           ;

Block : LEFT_CURLY_BRACKET BlockItems RIGHT_CURLY_BRACKET
      ;
BlockItems : /*empty*/
           | BlockItems BlockItem
           ;
BlockItem : Decl 
          | Stmt
          ;
Stmt : LVal ASSIGN Exp SEMICOLON
     | Exp SEMICOLON
     | Block
     | IF LEFT_PAREN Exp RIGHT_PAREN Stmt
     | IF LEFT_PAREN Exp RIGHT_PAREN Stmt ELSE Stmt
     | WHILE LEFT_PAREN Exp RIGHT_PAREN Stmt
     | BREAK SEMICOLON
     | CONTINUE SEMICOLON
     | RETURN SEMICOLON
     | RETURN Exp SEMICOLON
     ;

Exp : LOrExp
    ;
LVal : IDENT Index
     ;
Index : /*empty*/
      | Index LEFT_BRACKET Exp RIGHT_BRACKET
      ;
PrimaryExp : LEFT_PAREN Exp RIGHT_PAREN
           | LVal
           | Number
           ;
Number : INT_CONST
       ;
UnaryExp : PrimaryExp
         | IDENT LEFT_PAREN FuncRParams RIGHT_PAREN
         | IDENT LEFT_PAREN RIGHT_PAREN
         | UnaryOp UnaryExp
         ;
UnaryOp : ADD
        | SUB
        | NOT
        ;
FuncRParams : Exp
            | FuncRParams COMMA Exp
            ;

MulExp : UnaryExp
       | MulExp MUL UnaryExp
       | MulExp DIV UnaryExp
       | MulExp MOD UnaryExp
       ;
AddExp : MulExp
       | AddExp ADD MulExp
       | AddExp SUB MulExp
       ;

RelExp : AddExp
       | RelExp G AddExp
       | RelExp L AddExp
       | RelExp GE AddExp
       | RelExp LE AddExp
       ;
EqExp : RelExp
      | EqExp EQ RelExp
      | EqExp NE RelExp
      ;
LAndExp : EqExp
        | LAndExp AND EqExp
        ;
LOrExp : LAndExp
       | LOrExp OR LAndExp
       ;
%%

void yyerror(const char *s) {
    printf("error: %s\n", s);
    syntax_error = 1;
}
