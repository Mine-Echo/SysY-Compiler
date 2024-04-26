%{
#include <stdio.h>
#include <ast/ast.h>
void yyerror(const char *s);
extern int yylex(void);
extern Node* root;
%}


%union {
    int ival;
    char* str;
    NonTerminalNode* ntnode; 
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

%type <ntnode> CompUnit VarDecl VarDefList VarDef Exp ArrayDef FuncDef Block FuncFParams FuncFParam BlockItems BlockItem Stmt LVal Index PrimaryExp UnaryExp FuncRParams UnaryOp MulExp AddExp RelExp EqExp LAndExp


%%
CompUnit : VarDecl          {$$=new CompUnitNode();$$->addSon($1);root=$$;}
         | FuncDef          {$$=new CompUnitNode();$$->addSon($1);root=$$;}
         | CompUnit VarDecl {$$=new CompUnitNode();$$->addSon($1);$$->addSon($2);root=$$;}
         | CompUnit FuncDef {$$=new CompUnitNode();$$->addSon($1);$$->addSon($2);root=$$;}
         ;

/* BType : INT_TYPE
      ; */
/* Decl : VarDecl       
     ;*/
//replace BType with INT_TYPE
VarDecl : INT_TYPE VarDefList SEMICOLON {$$=new VarDeclNode();$$->addSon(new IntTypeNode());$$->addSon($2);$$->addSon(new SemicolonNode());}
        ;
VarDefList : VarDef                  {$$=new VarDefListNode();$$->addSon($1);}
           | VarDefList COMMA VarDef {$$=new VarDefListNode();$$->addSon($1);$$->addSon(new CommaNode());$$->addSon($3);}
           ;
VarDef : IDENT ASSIGN Exp   {$$=new VarDefNode();$$->addSon(new IdentNode($1));$$->addSon(new AssignNode());$$->addSon($3);}
       | IDENT              {$$=new VarDefNode();$$->addSon(new IdentNode($1));}
       | IDENT ArrayDef     {$$=new VarDefNode();$$->addSon(new IdentNode($1));$$->addSon($2);}
       ;
ArrayDef : ArrayDef LEFT_BRACKET INT_CONST RIGHT_BRACKET       {$$=new ArrayDefNode();$$->addSon($1);$$->addSon(new LeftBracketNode());$$->addSon(new IntConstNode($3));$$->addSon(new RightBracketNode());}
         | LEFT_BRACKET INT_CONST RIGHT_BRACKET                {$$=new ArrayDefNode();$$->addSon(new LeftBracketNode());$$->addSon(new IntConstNode($2));$$->addSon(new RightBracketNode());}
         ;
/* InitVal : Exp
        ; */
//replace FuncType with INT_TYPE|VOID_TYP
FuncDef : INT_TYPE IDENT LEFT_PAREN RIGHT_PAREN Block                 {$$=new FuncDefNode();$$->addSon(new IntTypeNode());$$->addSon(new IdentNode($2));$$->addSon(new LeftParenNode());$$->addSon(new RightParenNode());$$->addSon($5);}
        | VOID_TYPE IDENT LEFT_PAREN RIGHT_PAREN Block                {$$=new FuncDefNode();$$->addSon(new VoidTypeNode());$$->addSon(new IdentNode($2));$$->addSon(new LeftParenNode());$$->addSon(new RightParenNode());$$->addSon($5);}
        | INT_TYPE IDENT LEFT_PAREN FuncFParams RIGHT_PAREN Block     {$$=new FuncDefNode();$$->addSon(new IntTypeNode());$$->addSon(new IdentNode($2));$$->addSon(new LeftParenNode());$$->addSon($4);$$->addSon(new RightParenNode());$$->addSon($6);}
        | VOID_TYPE IDENT LEFT_PAREN FuncFParams RIGHT_PAREN Block    {$$=new FuncDefNode();$$->addSon(new VoidTypeNode());$$->addSon(new IdentNode($2));$$->addSon(new LeftParenNode());$$->addSon($4);$$->addSon(new RightParenNode());$$->addSon($6);}
        ;
/* FuncType : INT_TYPE
         | VOID_TYPE
         ; */
FuncFParams : FuncFParam                         {$$=new FuncFParamsNode();$$->addSon($1);}
            | FuncFParams COMMA FuncFParam       {$$=new FuncFParamsNode();$$->addSon($1);$$->addSon(new CommaNode());$$->addSon($3);}
            ;
//replace BType with INT_TYPE
FuncFParam : INT_TYPE IDENT                                           {$$=new FuncFParamNode();$$->addSon(new IntTypeNode());$$->addSon(new IdentNode($2));}
           | INT_TYPE IDENT LEFT_BRACKET RIGHT_BRACKET                {$$=new FuncFParamNode();$$->addSon(new IntTypeNode());$$->addSon(new IdentNode($2));$$->addSon(new LeftBracketNode());$$->addSon(new RightBracketNode());}
           | INT_TYPE IDENT LEFT_BRACKET RIGHT_BRACKET ArrayDef       {$$=new FuncFParamNode();$$->addSon(new IntTypeNode());$$->addSon(new IdentNode($2));$$->addSon(new LeftBracketNode());$$->addSon(new RightBracketNode());$$->addSon($5);}
           ;

Block : LEFT_CURLY_BRACKET BlockItems RIGHT_CURLY_BRACKET      {$$=new BlockNode();$$->addSon(new LeftCurlyBracketNode());$$->addSon($2);$$->addSon(new RightCurlyBracketNode());}
      | LEFT_CURLY_BRACKET RIGHT_CURLY_BRACKET                 {$$=new BlockNode();$$->addSon(new LeftCurlyBracketNode());$$->addSon(new RightCurlyBracketNode());}
      ;
BlockItems : BlockItem             {$$=new BlockItemsNode();$$->addSon($1);}
           | BlockItems BlockItem  {$$=new BlockItemsNode();$$->addSon($1);$$->addSon($2);}
           ;
BlockItem : VarDecl  {$$=new BlockItemNode();$$->addSon($1);}
          | Stmt     {$$=new BlockItemNode();$$->addSon($1);}
          ;
Stmt : LVal ASSIGN Exp SEMICOLON                        {$$=new StmtNode();$$->addSon($1);$$->addSon(new AssignNode());$$->addSon($3);$$->addSon(new SemicolonNode());}
     | Exp SEMICOLON                                    {$$=new StmtNode();$$->addSon($1);$$->addSon(new SemicolonNode());}
     | Block                                            {$$=new StmtNode();$$->addSon($1);}
     | IF LEFT_PAREN Exp RIGHT_PAREN Stmt               {$$=new StmtNode();$$->addSon(new IfNode());$$->addSon(new LeftParenNode());$$->addSon($3);$$->addSon(new RightParenNode());$$->addSon($5);}
     | IF LEFT_PAREN Exp RIGHT_PAREN Stmt ELSE Stmt     {$$=new StmtNode();$$->addSon(new IfNode());$$->addSon(new LeftParenNode());$$->addSon($3);$$->addSon(new RightParenNode());$$->addSon($5);$$->addSon(new ElseNode());$$->addSon($7);}
     | WHILE LEFT_PAREN Exp RIGHT_PAREN Stmt            {$$=new StmtNode();$$->addSon(new WhileNode());$$->addSon(new LeftParenNode());$$->addSon($3);$$->addSon(new RightParenNode());$$->addSon($5);}
     | BREAK SEMICOLON                                  {$$=new StmtNode();$$->addSon(new BreakNode());$$->addSon(new SemicolonNode());}
     | CONTINUE SEMICOLON                               {$$=new StmtNode();$$->addSon(new ContinueNode());$$->addSon(new SemicolonNode());}
     | RETURN SEMICOLON                                 {$$=new StmtNode();$$->addSon(new ReturnNode());$$->addSon(new SemicolonNode());}
     | RETURN Exp SEMICOLON                             {$$=new StmtNode();$$->addSon(new ReturnNode());$$->addSon($2);$$->addSon(new SemicolonNode());}
     ;

/* Exp : LOrExp
    ; */
LVal : IDENT         {$$=new LValNode();$$->addSon(new IdentNode($1));}
     | IDENT Index   {$$=new LValNode();$$->addSon(new IdentNode($1));$$->addSon($2);}
     ;
Index : LEFT_BRACKET Exp RIGHT_BRACKET           {$$=new IndexNode();$$->addSon(new LeftBracketNode());$$->addSon($2);$$->addSon(new RightBracketNode());}
      | Index LEFT_BRACKET Exp RIGHT_BRACKET     {$$=new IndexNode();$$->addSon($1);$$->addSon(new LeftBracketNode());$$->addSon($3);$$->addSon(new RightBracketNode());}
      ;
PrimaryExp : LEFT_PAREN Exp RIGHT_PAREN   {$$=new PrimaryExpNode();$$->addSon(new LeftParenNode());$$->addSon($2);$$->addSon(new RightParenNode());}
           | LVal                         {$$=new PrimaryExpNode();$$->addSon($1);}
           | INT_CONST                    {$$=new PrimaryExpNode();$$->addSon(new IntConstNode($1));}
           ;
/* Number : INT_CONST
       ; */
UnaryExp : PrimaryExp                                   {$$=new UnaryExpNode();$$->addSon($1);}
         | IDENT LEFT_PAREN FuncRParams RIGHT_PAREN     {$$=new UnaryExpNode();$$->addSon(new IdentNode($1));$$->addSon(new LeftParenNode());$$->addSon($3);$$->addSon(new RightParenNode());}
         | IDENT LEFT_PAREN RIGHT_PAREN                 {$$=new UnaryExpNode();$$->addSon(new IdentNode($1));$$->addSon(new LeftParenNode());$$->addSon(new RightParenNode());}
         | UnaryOp UnaryExp                             {$$=new UnaryExpNode();$$->addSon($1);$$->addSon($2);}
         ;
UnaryOp : ADD {$$=new UnaryOpNode();$$->addSon(new AddNode());}
        | SUB {$$=new UnaryOpNode();$$->addSon(new SubNode());}
        | NOT {$$=new UnaryOpNode();$$->addSon(new NotNode());}
        ;
FuncRParams : Exp                         {$$=new FuncRParamsNode();$$->addSon($1);}
            | FuncRParams COMMA Exp       {$$=new FuncRParamsNode();$$->addSon($1);$$->addSon(new CommaNode());$$->addSon($3);}
            ;

MulExp : UnaryExp                  {$$=new MulExpNode();$$->addSon($1);}
       | MulExp MUL UnaryExp       {$$=new MulExpNode();$$->addSon($1);$$->addSon(new MulNode());$$->addSon($3);}
       | MulExp DIV UnaryExp       {$$=new MulExpNode();$$->addSon($1);$$->addSon(new DivNode());$$->addSon($3);}
       | MulExp MOD UnaryExp       {$$=new MulExpNode();$$->addSon($1);$$->addSon(new ModNode());$$->addSon($3);}
       ;
AddExp : MulExp             {$$=new AddExpNode();$$->addSon($1);}
       | AddExp ADD MulExp  {$$=new AddExpNode();$$->addSon($1);$$->addSon(new AddNode());$$->addSon($3);}
       | AddExp SUB MulExp  {$$=new AddExpNode();$$->addSon($1);$$->addSon(new SubNode());$$->addSon($3);}
       ;

RelExp : AddExp             {$$=new RelExpNode();$$->addSon($1);}
       | RelExp G AddExp    {$$=new RelExpNode();$$->addSon($1);$$->addSon(new GNode());$$->addSon($3);}
       | RelExp L AddExp    {$$=new RelExpNode();$$->addSon($1);$$->addSon(new LNode());$$->addSon($3);}
       | RelExp GE AddExp   {$$=new RelExpNode();$$->addSon($1);$$->addSon(new GeNode());$$->addSon($3);}
       | RelExp LE AddExp   {$$=new RelExpNode();$$->addSon($1);$$->addSon(new LeNode());$$->addSon($3);}
       ;
EqExp : RelExp              {$$=new EqExpNode();$$->addSon($1);}
      | EqExp EQ RelExp     {$$=new EqExpNode();$$->addSon($1);$$->addSon(new EqNode());$$->addSon($3);}
      | EqExp NE RelExp     {$$=new EqExpNode();$$->addSon($1);$$->addSon(new NeNode());$$->addSon($3);}
      ;
LAndExp : EqExp             {$$=new LAndExpNode();$$->addSon($1);}
        | LAndExp AND EqExp {$$=new LAndExpNode();$$->addSon($1);$$->addSon(new AndNode());$$->addSon($3);}
        ;
Exp : LAndExp        {$$=new ExpNode();$$->addSon($1);}
    | Exp OR LAndExp {$$=new ExpNode();$$->addSon($1);$$->addSon(new OrNode());$$->addSon($3);}
       ;
%%

void yyerror(const char *s) {
    printf("error: %s\n", s);
}
