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
/* CompUnit : Exp {root = $1; }
Exp : Term { $$ = $1;}
    | SUB Exp %prec UMINUS { $$ = new TreeUnaryExpr(OP_Neg, $2); }
    | ADD Exp %prec UPLUS  { $$ = $2; }
    | Exp ADD Exp { $$ = new TreeBinaryExpr(OpType::OP_Add, $1, $3); }
    | Exp SUB Exp { $$ = new TreeBinaryExpr(OpType::OP_Sub, $1, $3); }
    | Exp MUL Exp { $$ = new TreeBinaryExpr(OpType::OP_Mul, $1, $3); }
    | Exp DIV Exp { $$ = new TreeBinaryExpr(OpType::OP_Div, $1, $3); }
    ;

Term : INT { $$ = new TreeIntegerLiteral($1); }
     ; */

CompUnit : Decl
         | FuncDef
         | CompUnit Decl
         | CompUnit FuncDef
         ;

/* BType : INT_TYPE
      ; */
Decl : VarDecl
     ;
//修改
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
//修改
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
    extern int yylineno;
    printf("error: %s\n%d\n", s,yylineno);
    syntax_error = 1;
}
