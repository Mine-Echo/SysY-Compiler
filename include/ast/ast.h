#pragma once
#include <cstdint>
#include <type_traits>
#include <string>
#include <vector>
#include <iostream>

enum NodeType{
    //nonterminal
    CompUnit,
    VarDecl,
    VarDefList,
    VarDef,
    Exp,
    ArrayDef,
    FuncDef,
    Block,
    FuncFParams,
    FuncFParam,
    BlockItems,
    BlockItem,
    Stmt,
    LVal,
    Index,
    PrimaryExp,
    UnaryExp,
    FuncRParams,
    UnaryOp,
    MulExp,
    AddExp,
    RelExp,
    EqExp,
    LAndExp,
    
    //terminal
    IntType,
    Semicolon,
    Comma,
    Ident,
    Assign,
    LeftBracket,
    RightBracket,
    IntConst,
    LeftParen,
    RightParen,
    VoidType,
    LeftCurlyBracket,
    RightCurlyBracket,
    If,
    Else,
    While,
    Break,
    Continue,
    Return,
    Add,
    Sub,
    Not,
    Mul,
    Div,
    Mod,
    Greater,
    Less,
    Ge,
    Le,
    Eq,
    Ne,
    And,
    Or
};

struct Node {
    NodeType node_type;
    Node(NodeType type) : node_type(type) {}
    template <typename T> bool is() {
        return node_type == std::remove_pointer_t<T>::this_type;
    }
    template <typename T> T as() {
        if (is<T>())
            return static_cast<T>(this);
        return nullptr;
    }
    template <typename T> T as_unchecked() { return static_cast<T>(this); }
};

struct NonTerminalNode: public Node{
    std::vector<Node*> sons;
    //constructor
    NonTerminalNode(NodeType type):Node(type){}
    //add son
    void addSon(Node* node){
        this->sons.push_back(node);
    }
};

struct CompUnitNode: public NonTerminalNode{
    //type
    constexpr static NodeType this_type = CompUnit;
    //constructor
    CompUnitNode():NonTerminalNode(this_type){}
};

struct VarDeclNode: public NonTerminalNode{
    //type
    constexpr static NodeType this_type = VarDecl;
    //constructor
    VarDeclNode():NonTerminalNode(this_type){}
};

struct VarDefListNode: public NonTerminalNode{
    //type
    constexpr static NodeType this_type = VarDefList;
    //constructor
    VarDefListNode():NonTerminalNode(this_type){}
};

struct VarDefNode: public NonTerminalNode{
    //type
    constexpr static NodeType this_type = VarDef;
    //constructor
    VarDefNode():NonTerminalNode(this_type){}
};

struct ExpNode: public NonTerminalNode{
    //type
    constexpr static NodeType this_type = Exp;
    //constructor
    ExpNode():NonTerminalNode(this_type){}
};

struct ArrayDefNode: public NonTerminalNode{
    //type
    constexpr static NodeType this_type = ArrayDef;
    //constructor
    ArrayDefNode():NonTerminalNode(this_type){}
};

struct FuncDefNode: public NonTerminalNode{
    //type
    constexpr static NodeType this_type = FuncDef;
    //constructor
    FuncDefNode():NonTerminalNode(this_type){}
};

struct BlockNode: public NonTerminalNode{
    //type
    constexpr static NodeType this_type = Block;
    //constructor
    BlockNode():NonTerminalNode(this_type){}
};

struct FuncFParamsNode: public NonTerminalNode{
    //type
    constexpr static NodeType this_type = FuncFParams;
    //constructor
    FuncFParamsNode():NonTerminalNode(this_type){}
};

struct FuncFParamNode: public NonTerminalNode{
    //type
    constexpr static NodeType this_type = FuncFParam;
    //constructor
    FuncFParamNode():NonTerminalNode(this_type){}
};

struct BlockItemsNode: public NonTerminalNode{
    //type
    constexpr static NodeType this_type = BlockItems;
    //constructor
    BlockItemsNode():NonTerminalNode(this_type){}
};

struct BlockItemNode: public NonTerminalNode{
    //type
    constexpr static NodeType this_type = BlockItem;
    //constructor
    BlockItemNode():NonTerminalNode(this_type){}
};

struct StmtNode: public NonTerminalNode{
    //type
    constexpr static NodeType this_type = Stmt;
    //constructor
    StmtNode():NonTerminalNode(this_type){}
};

struct LValNode: public NonTerminalNode{
    //type
    constexpr static NodeType this_type = LVal;
    //constructor
    LValNode():NonTerminalNode(this_type){}
};

struct IndexNode: public NonTerminalNode{
    //type
    constexpr static NodeType this_type = Index;
    //constructor
    IndexNode():NonTerminalNode(this_type){}
};

struct PrimaryExpNode: public NonTerminalNode{
    //type
    constexpr static NodeType this_type = PrimaryExp;
    //constructor
    PrimaryExpNode():NonTerminalNode(this_type){}
};

struct UnaryExpNode: public NonTerminalNode{
    //type
    constexpr static NodeType this_type = UnaryExp;
    //constructor
    UnaryExpNode():NonTerminalNode(this_type){}
};

struct FuncRParamsNode: public NonTerminalNode{
    //type
    constexpr static NodeType this_type = FuncRParams;
    //constructor
    FuncRParamsNode():NonTerminalNode(this_type){}
};

struct UnaryOpNode: public NonTerminalNode{
    //type
    constexpr static NodeType this_type = UnaryOp;
    //constructor
    UnaryOpNode():NonTerminalNode(this_type){}
};

struct MulExpNode: public NonTerminalNode{
    //type
    constexpr static NodeType this_type = MulExp;
    //constructor
    MulExpNode():NonTerminalNode(this_type){}
};

struct AddExpNode: public NonTerminalNode{
    //type
    constexpr static NodeType this_type = AddExp;
    //constructor
    AddExpNode():NonTerminalNode(this_type){}
};

struct RelExpNode: public NonTerminalNode{
    //type
    constexpr static NodeType this_type = RelExp;
    //constructor
    RelExpNode():NonTerminalNode(this_type){}
};

struct EqExpNode: public NonTerminalNode{
    //type
    constexpr static NodeType this_type = EqExp;
    //constructor
    EqExpNode():NonTerminalNode(this_type){}
};

struct LAndExpNode: public NonTerminalNode{
    //type
    constexpr static NodeType this_type = LAndExp;
    //constructor
    LAndExpNode():NonTerminalNode(this_type){}
};




struct IntTypeNode: public Node{
    //type
    constexpr static NodeType this_type = IntType;
    //constructor
    IntTypeNode():Node(this_type){}
};

struct SemicolonNode: public Node{
    //type
    constexpr static NodeType this_type = Semicolon;
    //constructor
    SemicolonNode():Node(this_type){}
};

struct CommaNode: public Node{
    //type
    constexpr static NodeType this_type = Comma;
    //constructor
    CommaNode():Node(this_type){}
};

struct IdentNode: public Node{
    //type
    constexpr static NodeType this_type = Ident;
    //
    std::string value;
    //constructor
    IdentNode(char* str):Node(this_type),value(str){}
};

struct AssignNode: public Node{
    //type
    constexpr static NodeType this_type = Assign;
    //constructor
    AssignNode():Node(this_type){}
};

struct LeftBracketNode: public Node{
    //type
    constexpr static NodeType this_type = LeftBracket;
    //constructor
    LeftBracketNode():Node(this_type){}
};

struct RightBracketNode: public Node{
    //type
    constexpr static NodeType this_type = RightBracket;
    //constructor
    RightBracketNode():Node(this_type){}
};

struct IntConstNode: public Node{
    //type
    constexpr static NodeType this_type = IntConst;
    //
    int value;
    //constructor
    IntConstNode(int num):Node(this_type),value(num){}
};

struct LeftParenNode: public Node{
    //type
    constexpr static NodeType this_type = LeftParen;
    //constructor
    LeftParenNode():Node(this_type){}
};

struct RightParenNode: public Node{
    //type
    constexpr static NodeType this_type = RightParen;
    //constructor
    RightParenNode():Node(this_type){}
};

struct VoidTypeNode: public Node{
    //type
    constexpr static NodeType this_type = VoidType;
    //constructor
    VoidTypeNode():Node(this_type){}
};

struct LeftCurlyBracketNode: public Node{
    //type
    constexpr static NodeType this_type = LeftCurlyBracket;
    //constructor
    LeftCurlyBracketNode():Node(this_type){}
};

struct RightCurlyBracketNode: public Node{
    //type
    constexpr static NodeType this_type = RightCurlyBracket;
    //constructor
    RightCurlyBracketNode():Node(this_type){}
};

struct IfNode: public Node{
    //type
    constexpr static NodeType this_type = If;
    //constructor
    IfNode():Node(this_type){}
};

struct ElseNode: public Node{
    //type
    constexpr static NodeType this_type = Else;
    //constructor
    ElseNode():Node(this_type){}
};

struct WhileNode: public Node{
    //type
    constexpr static NodeType this_type = While;
    //constructor
    WhileNode():Node(this_type){}
};

struct BreakNode: public Node{
    //type
    constexpr static NodeType this_type = Break;
    //constructor
    BreakNode():Node(this_type){}
};

struct ContinueNode: public Node{
    //type
    constexpr static NodeType this_type = Continue;
    //constructor
    ContinueNode():Node(this_type){}
};

struct ReturnNode: public Node{
    //type
    constexpr static NodeType this_type = Return;
    //constructor
    ReturnNode():Node(this_type){}
};

struct AddNode: public Node{
    //type
    constexpr static NodeType this_type = Add;
    //constructor
    AddNode():Node(this_type){}
};

struct SubNode: public Node{
    //type
    constexpr static NodeType this_type = Sub;
    //constructor
    SubNode():Node(this_type){}
};

struct NotNode: public Node{
    //type
    constexpr static NodeType this_type = Not;
    //constructor
    NotNode():Node(this_type){}
};

struct MulNode: public Node{
    //type
    constexpr static NodeType this_type = Mul;
    //constructor
    MulNode():Node(this_type){}
};

struct DivNode: public Node{
    //type
    constexpr static NodeType this_type = Div;
    //constructor
    DivNode():Node(this_type){}
};

struct ModNode: public Node{
    //type
    constexpr static NodeType this_type = Mod;
    //constructor
    ModNode():Node(this_type){}
};

struct GNode: public Node{
    //type
    constexpr static NodeType this_type = Greater;
    //constructor
    GNode():Node(this_type){}
};

struct LNode: public Node{
    //type
    constexpr static NodeType this_type = Less;
    //constructor
    LNode():Node(this_type){}
};

struct GeNode: public Node{
    //type
    constexpr static NodeType this_type = Ge;
    //constructor
    GeNode():Node(this_type){}
};

struct LeNode: public Node{
    //type
    constexpr static NodeType this_type = Le;
    //constructor
    LeNode():Node(this_type){}
};

struct EqNode: public Node{
    //type
    constexpr static NodeType this_type = Eq;
    //constructor
    EqNode():Node(this_type){}
};

struct NeNode: public Node{
    //type
    constexpr static NodeType this_type = Ne;
    //constructor
    NeNode():Node(this_type){}
};

struct AndNode: public Node{
    //type
    constexpr static NodeType this_type = And;
    //constructor
    AndNode():Node(this_type){}
};

struct OrNode: public Node{
    //type
    constexpr static NodeType this_type = Or;
    //constructor
    OrNode():Node(this_type){}
};

//print the tree
void print_tree(Node* node, std::string prefix = "", std::string ident = "");
// void print_tree(Node* node, std::string prefix = "", std::string ident = "", bool print_bar = true, bool flag = false);
NonTerminalNode* construct_ast_tree(Node* node, Node* tree);

// bool need_print(Node* node);