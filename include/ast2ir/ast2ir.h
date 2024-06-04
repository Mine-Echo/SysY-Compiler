#pragma once
#include "ir/ir.h"
#include <map>
#include "ast/ast.h"


class AST2IR{
public:
    //变量符号表
    std::vector<std::map<std::string,std::pair<Value*,std::vector<std::optional<std::size_t>>>>> variableTable;
    //函数符号表
    std::map<std::string,Function*> functionTable;
    //记录需要初始化的全局变量，在main函数开头初始化
    std::map<std::string,Node*> globalInit;
    //记录whlie循环的entryBlock和nextBlock，用于break和continue的翻译
    std::vector<std::pair<BasicBlock*,BasicBlock*>> whileInfo;
    //translate Ast to IR
    void translate(Node* root,Module* module);
private:
    //enter a new scope, create a new variable table
    void newScope();
    //exit a scope, delete a variable table
    void deleteScope();
    //print variable table
    void printVariTable();
    //look up variable table
    std::pair<Value*,std::vector<std::optional<std::size_t>>> lookUpVariTable(std::string name);
    //look up function table
    Function* lookUpFuncTable(std::string name);

    void translateCompUnit(CompUnitNode* node, Module* module);

    void translateFuncDef(FuncDefNode* node, Module* module);

    std::pair<std::string,Type*> translateFuncFParam(FuncFParamNode* node);

    BasicBlock* translateBlock(BlockNode* node, BasicBlock* currentBasicBlock);

    void translateVarDecl(VarDeclNode* node,BasicBlock* currentBasicBlock);

    BasicBlock* translateStmt(StmtNode* node,BasicBlock* currentBasicBlock);

    Value* translateExpr(Node* node, BasicBlock* currentBasicBlock,BasicBlock* trueBlock=nullptr,BasicBlock* falseBlock=nullptr);

    Value* translateExp(ExpNode* node, BasicBlock* currentBasicBlock,BasicBlock* trueBlock=nullptr,BasicBlock* falseBlock=nullptr);

    Value* translateLAndExp(LAndExpNode* node, BasicBlock* currentBasicBlock,BasicBlock* trueBlock=nullptr,BasicBlock* falseBlock=nullptr);

    Value* translateEqExp(EqExpNode* node, BasicBlock* currentBasicBlock);

    Value* translateRelExp(RelExpNode* node, BasicBlock* currentBasicBlock);

    Value* translateAddExp(AddExpNode* node, BasicBlock* currentBasicBlock);

    Value* translateMulExp(MulExpNode* node, BasicBlock* currentBasicBlock);

    Value* translateUnaryExp(UnaryExpNode* node, BasicBlock* currentBasicBlock);

    Value* translateLVal(LValNode* node, BasicBlock* currentBasicBlock, bool isExpr);

    Value* translateIntConst(IntConstNode* node, BasicBlock* currentBasicBlock);
};