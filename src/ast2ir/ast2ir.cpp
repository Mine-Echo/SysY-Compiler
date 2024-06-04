#include "ast2ir/ast2ir.h"
#include <fmt/core.h>


void AST2IR::newScope(){
    std::map<std::string, std::pair<Value*,std::vector<std::optional<std::size_t>>>> v_table;
    variableTable.push_back(v_table);
}

void AST2IR::deleteScope(){
    variableTable.back().clear();
    variableTable.pop_back();
}

std::pair<Value*,std::vector<std::optional<std::size_t>>> AST2IR::lookUpVariTable(std::string name){
    for (int i = variableTable.size() - 1; i >= 0; i--){
        auto iter = variableTable[i].find(name);
        if (iter != variableTable[i].end()){
            return iter->second;
        }
    }
    //理论上一定能找到，否则语法分析会报错
    fmt::print("variable {} is not found\n",name);
}

void AST2IR::printVariTable(){
    for(int i=0;i<variableTable.size();i++){
        fmt::print("the {}th scope: {} variables\n",i,variableTable[i].size());
        for(auto j=variableTable[i].begin();j!=variableTable[i].end();j++){
            fmt::print("{}\n",j->first);
        }
    }
}

Function* AST2IR::lookUpFuncTable(std::string name){
    auto iter=functionTable.find(name);
    if(iter!=functionTable.end())
        return iter->second;
    fmt::print("function {} is not found\n",name);
}

void AST2IR::translate(Node* root,Module* module){
    fmt::print("translate\n");
    // std::unique_ptr<Module> module=std::make_unique<Module>();
    newScope();
    //添加运行时库函数
    Type* integerType=Type::getIntegerTy();
    Type* unitType=Type::getUnitTy();
    Type* intPtrType=PointerType::get(integerType);
    //int getint()
    functionTable.insert(std::pair("getint",Function::Create(FunctionType::get(integerType,{}),true,"getint",module)));
    //int getch()
    functionTable.insert(std::pair("getch",Function::Create(FunctionType::get(integerType,{}),true,"getch",module)));
    //int getarray(int[])
    functionTable.insert(std::pair("getarray",Function::Create(FunctionType::get(integerType,{intPtrType}),true,"getarray",module)));
    //void putint(int)
    functionTable.insert(std::pair("putint",Function::Create(FunctionType::get(unitType,{integerType}),true,"putint",module)));
    //void putch(int)
    functionTable.insert(std::pair("putch",Function::Create(FunctionType::get(unitType,{integerType}),true,"putch",module)));
    //void putarray(int, int[])
    functionTable.insert(std::pair("putarray",Function::Create(FunctionType::get(unitType,{integerType,intPtrType}),true,"putarray",module)));
    //void starttime()
    functionTable.insert(std::pair("starttime",Function::Create(FunctionType::get(unitType,{}),true,"starttime",module)));
    //void stoptime().
    functionTable.insert(std::pair("stoptime",Function::Create(FunctionType::get(unitType,{}),true,"stoptime",module)));
    //translate
    if(auto n=root->as<CompUnitNode*>()){
        translateCompUnit(n,module);
    }
    deleteScope();
    functionTable.clear();
    globalInit.clear();
    return ;
}

void AST2IR::translateCompUnit(CompUnitNode* node, Module* module){
    fmt::print("translateCompUnit\n");
    for(int i=0;i<node->sons.size();i++){
        if(auto n=node->sons[i]->as<VarDeclNode*>()){
            for(int j=0;j<n->sons.size();j++){
                Type* type=Type::getIntegerTy();
                if(auto varDefNode=n->sons[j]->as<VarDefNode*>()){
                    //定义全局变量
                    //变量名
                    IdentNode* ident=reinterpret_cast<IdentNode*>(varDefNode->sons[0]);
                    int numElements=1;
                    if(varDefNode->sons.size()==3){
                        //IDENT ASSiGN EXP
                        auto globalVariable = GlobalVariable::Create(type, numElements, false, ident->value, module);
                        //符号表中插入数据
                        variableTable.back().insert(std::pair(ident->value,std::pair<Value*,std::vector<std::optional<std::size_t>>>(globalVariable,std::vector<std::optional<std::size_t>>{})));
                        //需要赋值
                        globalInit.insert(std::pair(ident->value,varDefNode->sons[2]));
                    }else if(varDefNode->sons.size()>3){
                        //IDENT [ INTCONST ]
                        std::vector<std::optional<std::size_t>> dimension{};
                        for(int k=0;k<varDefNode->sons.size();k++){
                            if(auto intConstNode=varDefNode->sons[k]->as<IntConstNode*>()){
                                numElements*=intConstNode->value;
                                dimension.push_back(intConstNode->value);
                            }
                        }
                        auto globalVariable = GlobalVariable::Create(type, numElements, false, ident->value, module);
                        variableTable.back().insert(std::pair(ident->value,std::pair<Value*,std::vector<std::optional<std::size_t>>>(globalVariable,dimension)));
                    }else{
                        auto globalVariable = GlobalVariable::Create(type, numElements, false, ident->value, module);
                        //符号表中插入数据
                        variableTable.back().insert(std::pair(ident->value,std::pair<Value*,std::vector<std::optional<std::size_t>>>(globalVariable,std::vector<std::optional<std::size_t>>{})));
                    }
                    
                }
            }
        }else if(auto n=node->sons[i]->as<FuncDefNode*>()){
            translateFuncDef(n,module);
        }else if(auto n=node->sons[i]->as<CompUnitNode*>()){
            translateCompUnit(n,module);
        }
    }
}

void AST2IR::translateFuncDef(FuncDefNode* node,Module* module){
    fmt::print("translateFuncDef\n");
    Type *integerType = Type::getIntegerTy();
    Type *unitType = Type::getUnitTy();
    //返回类型
    Type* returnType;
    if(node->sons[0]->node_type==NodeType::IntType)
        returnType=integerType;
    else
        returnType=unitType;
    //函数名
    IdentNode* ident=reinterpret_cast<IdentNode*>(node->sons[1]);
    //函数形参
    std::vector<Type *> params{};
    std::vector<std::string> names{};
    std::vector<std::vector<std::optional<std::size_t>>> dimensions{};
    for(int i=2;i<node->sons.size();i++){
        if(auto n=node->sons[i]->as<FuncFParamNode*>()){
            //参数名
            IdentNode* ident=reinterpret_cast<IdentNode*>(n->sons[1]);
            names.push_back(ident->value);
            if(n->sons.size()>2){
                //数组参数，返回指针类型
                params.push_back(PointerType::get(integerType));
                std::vector<std::optional<std::size_t>> dimension{std::nullopt};//函数形参第一维度为空
                for(int j=5;j<n->sons.size();j++){
                    if(auto intConstNode=n->sons[j]->as<IntConstNode*>())
                        dimension.push_back(intConstNode->value);
                }
                dimensions.push_back(dimension);
            }else{
                params.push_back(integerType);
                dimensions.push_back(std::vector<std::optional<std::size_t>>{});
            }
        }      
    }
    //定义函数类型和函数
    FunctionType* functionType=FunctionType::get(returnType,params);
    Function* function=Function::Create(functionType,false,ident->value,module);
    functionTable.insert(std::pair(ident->value,function));
    BasicBlock* basicBlock=BasicBlock::Create(function);
    //如果是main函数
    if(ident->value=="main"){
        //把全局变量的初始化放到main函数开始做
        List<GlobalVariable>& globalList=module->getGlobalList();
        for(auto iter=globalList.begin();iter!=globalList.end();iter++){
            //初始化赋值
            auto it=globalInit.find(std::string(iter->getName()));
            if(it!=globalInit.end()){
                Value* value=translateExpr(it->second,basicBlock);
                StoreInst::Create(value, &*iter, basicBlock);
            }
        }
    }
    //新作用域
    newScope();
    //函数返回值
    BasicBlock* returnBlock=BasicBlock::Create(function);
    if(functionType->getReturnType()==integerType){
        AllocaInst *retAddr = AllocaInst::Create(integerType, 1, basicBlock);
        variableTable.back().insert(std::pair("return",std::pair(retAddr,std::vector<std::optional<std::size_t>>{})));
        Value* retValue=LoadInst::Create(retAddr,returnBlock);
        RetInst::Create(retValue,returnBlock);
    }else{
        RetInst::Create(ConstantUnit::Create(),returnBlock);
    }
    //函数参数
    for(int i=0;i<params.size();i++){
        if(isa<PointerType>(params[i])){
            //参数是数组（指针）
            variableTable.back().insert(std::pair(names[i],std::pair(function->getArg(i),dimensions[i])));
        }else{
            AllocaInst* argAddr=AllocaInst::Create(integerType,1,basicBlock);
            StoreInst::Create(function->getArg(i),argAddr,basicBlock);
            variableTable.back().insert(std::pair(names[i],std::pair(argAddr,dimensions[i])));
        }
    }
    //Block
    BasicBlock* block=translateBlock(reinterpret_cast<BlockNode*>(node->sons[node->sons.size()-1]),basicBlock);
    //防止空块和没有返回的情况
    if(!block->size()||!block->back().isTerminator()){
        JumpInst::Create(returnBlock,block);
    }
    deleteScope();
}

BasicBlock* AST2IR::translateBlock(BlockNode* node,BasicBlock* currentBasicBlock){
    fmt::print("translateBlock\n");
    for(int i=0;i<node->sons.size();i++){
        if(auto n=node->sons[i]->as<VarDeclNode*>())
            translateVarDecl(n,currentBasicBlock);
        if(auto n=node->sons[i]->as<StmtNode*>())
            currentBasicBlock=translateStmt(n,currentBasicBlock);
    }
    return currentBasicBlock;
}

void AST2IR::translateVarDecl(VarDeclNode* node,BasicBlock* currentBasicBlock){
    fmt::print("translateVarDecl\n");
    Type* integerType=Type::getIntegerTy();
    for(int i=1;i<node->sons.size();i++){
        if(auto n=node->sons[i]->as<VarDefNode*>()){
            IdentNode* ident=reinterpret_cast<IdentNode*>(n->sons[0]);
            int numElements=1;
            if(n->sons.size()<=3){
                AllocaInst* addr=AllocaInst::Create(integerType,numElements,currentBasicBlock);
                variableTable.back().insert(std::pair(ident->value,std::pair(addr,std::vector<std::optional<std::size_t>>{})));
                if(n->sons.size()==3){
                    //IDENT ASSiGN EXP 需要初始化变量
                    Value* value=translateExpr(n->sons[2],currentBasicBlock);
                    StoreInst::Create(value,addr,currentBasicBlock);
                }
            }else{
                //IDENT [ INTCONST ]
                std::vector<std::optional<std::size_t>> dimension{};
                for(int j=0;j<n->sons.size();j++){
                    if(auto intConstNode=n->sons[j]->as<IntConstNode*>()){
                        numElements*=intConstNode->value;
                        dimension.push_back(intConstNode->value);
                    }
                }
                AllocaInst* addr=AllocaInst::Create(integerType,numElements,currentBasicBlock);
                variableTable.back().insert(std::pair(ident->value,std::pair(addr,dimension)));
            }
        }
    }
}

BasicBlock* AST2IR::translateStmt(StmtNode* node,BasicBlock* currentBasicBlock){
    fmt::print("translateStmt\n");
    BasicBlock* basicBlock=currentBasicBlock;
    switch(node->sons[0]->node_type){
        case NodeType::LVal:{
            Value* addr=translateLVal(reinterpret_cast<LValNode*>(node->sons[0]),basicBlock,false);
            if(node->sons.size()>1){
                Value* val=translateExpr(node->sons[2],basicBlock);
                StoreInst::Create(val,addr,basicBlock);
            }
            return basicBlock;
        }     
        case NodeType::Block:{
            newScope();
            basicBlock=translateBlock(reinterpret_cast<BlockNode*>(node->sons[0]),basicBlock);
            deleteScope();
            return basicBlock;
        }
        case NodeType::If:{
            if(node->sons.size()==3){
                //IF (Exp) Stmt
                BasicBlock* trueBlock=BasicBlock::Create(basicBlock->getParent(),&basicBlock->getParent()->back());
                BasicBlock* nextBlock=BasicBlock::Create(basicBlock->getParent(),&basicBlock->getParent()->back());
                Value* con=translateExpr(node->sons[1],basicBlock,trueBlock,nextBlock);
                if(con!=nullptr){
                    BranchInst::Create(trueBlock,nextBlock,con,basicBlock);
                }
                trueBlock=translateStmt(reinterpret_cast<StmtNode*>(node->sons[2]),trueBlock);
                //在trueblock的末尾Block加上jump to nextBlock
                if(!trueBlock->size()||!trueBlock->back().isTerminator())
                    JumpInst::Create(nextBlock,trueBlock);
                return nextBlock;
            }else{
                //IF (Exp) Stmt ELSE Stmt
                BasicBlock* trueBlock=BasicBlock::Create(basicBlock->getParent(),&basicBlock->getParent()->back());
                BasicBlock* falseBlock=BasicBlock::Create(basicBlock->getParent(),&basicBlock->getParent()->back());
                Value* con=translateExpr(node->sons[1],basicBlock,trueBlock,falseBlock);
                BasicBlock* nextBlock=BasicBlock::Create(basicBlock->getParent(),&basicBlock->getParent()->back());
                if(con!=nullptr){
                    BranchInst::Create(trueBlock,falseBlock,con,basicBlock);
                }
                trueBlock=translateStmt(reinterpret_cast<StmtNode*>(node->sons[2]),trueBlock);
                falseBlock=translateStmt(reinterpret_cast<StmtNode*>(node->sons[4]),falseBlock);
                //末尾加上jump to Block
                if(!trueBlock->size()||!trueBlock->back().isTerminator())
                    JumpInst::Create(nextBlock,trueBlock);
                if(!falseBlock->size()||!falseBlock->back().isTerminator())
                    JumpInst::Create(nextBlock,falseBlock);
                return nextBlock;
            }
        }
        case NodeType::While:{
            //while entry block: 负责判断循环的条件
            BasicBlock* entryBlock=BasicBlock::Create(basicBlock->getParent(),&basicBlock->getParent()->back());
            JumpInst::Create(entryBlock,basicBlock);
            BasicBlock* trueBlock=BasicBlock::Create(basicBlock->getParent(),&basicBlock->getParent()->back());
            BasicBlock* nextBlock=BasicBlock::Create(basicBlock->getParent(),&basicBlock->getParent()->back());
            Value* con=translateExpr(node->sons[1],entryBlock,trueBlock,nextBlock);
            if(con!=nullptr){
                BranchInst::Create(trueBlock,nextBlock,con,entryBlock);
            }
            //记录entryBlock和nextBlock
            whileInfo.push_back(std::pair(entryBlock,nextBlock));
            trueBlock=translateStmt(reinterpret_cast<StmtNode*>(node->sons[2]),trueBlock);
            //true末尾加上jump to entryBlock
            if(!trueBlock->size()||!trueBlock->back().isTerminator())
                JumpInst::Create(entryBlock,trueBlock);
            //删除entryBlock和nextBlock的记录
            whileInfo.pop_back();
            return nextBlock;
        }
        case NodeType::Break:{
            //需要直接找到外面第一层循环的nextBlock
            JumpInst::Create(whileInfo.back().second,basicBlock);
            return basicBlock;
        }
        case NodeType::Continue:{
            JumpInst::Create(whileInfo.back().first,basicBlock);
            return basicBlock;
        }
        case NodeType::Return:{
            if(node->sons.size()==1){
                JumpInst::Create(&currentBasicBlock->getParent()->back(),basicBlock);
                // RetInst::Create(ConstantUnit::Create(),basicBlock);
            }else{
                Value* retValue=translateExpr(node->sons[1],basicBlock);
                StoreInst::Create(retValue, lookUpVariTable("return").first, basicBlock);
                JumpInst::Create(&currentBasicBlock->getParent()->back(),basicBlock);
                // RetInst::Create(translateExpr(node->sons[1],basicBlock),basicBlock);
            }
            return basicBlock;
        }
        default:{
            translateExpr(node->sons[0],basicBlock);
            return basicBlock;
        }
    }
}

Value* AST2IR::translateExpr(Node* node, BasicBlock* currentBasicBlock,BasicBlock* trueBlock,BasicBlock* falseBlock){
    fmt::print("translateExpr\n");
    switch(node->node_type){
        case NodeType::Exp:
            return translateExp(reinterpret_cast<ExpNode*>(node),currentBasicBlock,trueBlock,falseBlock);
        case NodeType::LAndExp:
            return translateLAndExp(reinterpret_cast<LAndExpNode*>(node),currentBasicBlock,trueBlock,falseBlock);
        case NodeType::EqExp:
            return translateEqExp(reinterpret_cast<EqExpNode*>(node),currentBasicBlock);
        case NodeType::RelExp:
            return translateRelExp(reinterpret_cast<RelExpNode*>(node),currentBasicBlock);
        case NodeType::AddExp:
            return translateAddExp(reinterpret_cast<AddExpNode*>(node),currentBasicBlock);
        case NodeType::MulExp:
            return translateMulExp(reinterpret_cast<MulExpNode*>(node),currentBasicBlock);
        case NodeType::UnaryExp:
            return translateUnaryExp(reinterpret_cast<UnaryExpNode*>(node),currentBasicBlock);
        case NodeType::LVal:
            return translateLVal(reinterpret_cast<LValNode*>(node),currentBasicBlock,true);
        case NodeType::IntConst:
            return translateIntConst(reinterpret_cast<IntConstNode*>(node),currentBasicBlock);  
    }
}

Value* AST2IR::translateExp(ExpNode* node,BasicBlock* currentBasicBlock,BasicBlock* trueBlock,BasicBlock* falseBlock){
    fmt::print("translateExp\n");
    if(trueBlock&&falseBlock){//如果是if或while语句的短路表达式
        BasicBlock* rightBlock=BasicBlock::Create(currentBasicBlock->getParent(),&currentBasicBlock->getParent()->back());
        Value* left=translateExpr(node->sons[0],currentBasicBlock,trueBlock,rightBlock);
        if(left!=nullptr){
            BinaryInst* con1=BinaryInst::CreateEq(ConstantInt::Create(1),left,Type::getIntegerTy(),currentBasicBlock);
            BranchInst::Create(trueBlock,rightBlock,con1,currentBasicBlock);
        }
        Value* right=translateExpr(node->sons[2],rightBlock,trueBlock,falseBlock);
        if(right!=nullptr){
            BinaryInst* con2=BinaryInst::CreateEq(ConstantInt::Create(1),right,Type::getIntegerTy(),rightBlock);
            BranchInst::Create(trueBlock,falseBlock,con2,rightBlock);
        }
        return nullptr;
    }else{  
        Value* left=translateExpr(node->sons[0],currentBasicBlock);
        Value* right=translateExpr(node->sons[2],currentBasicBlock);
        BinaryInst* res=BinaryInst::CreateOr(left,right,Type::getIntegerTy(),currentBasicBlock);
        return res;
    }    
}

Value* AST2IR::translateLAndExp(LAndExpNode* node, BasicBlock* currentBasicBlock,BasicBlock* trueBlock,BasicBlock* falseBlock){
    fmt::print("translateLAndExp\n");
    if(trueBlock&&falseBlock){//如果是if或while语句的短路表达式
        BasicBlock* rightBlock=BasicBlock::Create(currentBasicBlock->getParent(),&currentBasicBlock->getParent()->back());
        Value* left=translateExpr(node->sons[0],currentBasicBlock,rightBlock,falseBlock);
        if(left!=nullptr){
            BinaryInst* con1=BinaryInst::CreateEq(ConstantInt::Create(1),left,Type::getIntegerTy(),currentBasicBlock);
            BranchInst::Create(rightBlock,falseBlock,con1,currentBasicBlock);
        }
        Value* right=translateExpr(node->sons[2],rightBlock,trueBlock,falseBlock);
        if(right!=nullptr){
            BinaryInst* con2=BinaryInst::CreateEq(ConstantInt::Create(1),right,Type::getIntegerTy(),rightBlock);
            BranchInst::Create(trueBlock,falseBlock,con2,rightBlock);
        }
        return nullptr;
    }else{
        Value* left=translateExpr(node->sons[0],currentBasicBlock);
        Value* right=translateExpr(node->sons[2],currentBasicBlock);
        BinaryInst* res=BinaryInst::CreateAnd(left,right,Type::getIntegerTy(),currentBasicBlock);
        return res;
    }
}

Value* AST2IR::translateEqExp(EqExpNode* node, BasicBlock* currentBasicBlock){
    fmt::print("translateEqExp\n");
    Value* left=translateExpr(node->sons[0],currentBasicBlock);
    Value* right=translateExpr(node->sons[2],currentBasicBlock);
    BinaryInst* res=nullptr;
    if(node->sons[1]->node_type==NodeType::Eq)
        res=BinaryInst::CreateEq(left,right,Type::getIntegerTy(),currentBasicBlock);
    else
        res=BinaryInst::CreateNe(left,right,Type::getIntegerTy(),currentBasicBlock);
    return res;
}

Value* AST2IR::translateRelExp(RelExpNode* node, BasicBlock* currentBasicBlock){
    fmt::print("translateRelExp\n");
    Value* left=translateExpr(node->sons[0],currentBasicBlock);
    Value* right=translateExpr(node->sons[2],currentBasicBlock);
    BinaryInst* res=nullptr;
    if(node->sons[1]->node_type==NodeType::Greater)
        res=BinaryInst::CreateGt(left,right,Type::getIntegerTy(),currentBasicBlock);
    else if(node->sons[1]->node_type==NodeType::Less)
        res=BinaryInst::CreateLt(left,right,Type::getIntegerTy(),currentBasicBlock);
    else if(node->sons[1]->node_type==NodeType::Ge)
        res=BinaryInst::CreateGe(left,right,Type::getIntegerTy(),currentBasicBlock);
    else
        res=BinaryInst::CreateLe(left,right,Type::getIntegerTy(),currentBasicBlock);
    return res;
}

Value* AST2IR::translateAddExp(AddExpNode* node, BasicBlock* currentBasicBlock){
    fmt::print("translateAddExp\n");
    Value* left=translateExpr(node->sons[0],currentBasicBlock);
    Value* right=translateExpr(node->sons[2],currentBasicBlock);
    BinaryInst* res=nullptr;
    if(node->sons[1]->node_type==NodeType::Add)
        res=BinaryInst::CreateAdd(left,right,Type::getIntegerTy(),currentBasicBlock);
    else
        res=BinaryInst::CreateSub(left,right,Type::getIntegerTy(),currentBasicBlock);
    return res;
}

Value* AST2IR::translateMulExp(MulExpNode* node, BasicBlock* currentBasicBlock){
    fmt::print("translateMulExp\n");
    Value* left=translateExpr(node->sons[0],currentBasicBlock);
    Value* right=translateExpr(node->sons[2],currentBasicBlock);
    BinaryInst* res=nullptr;
    if(node->sons[1]->node_type==NodeType::Mul)
        res=BinaryInst::CreateMul(left,right,Type::getIntegerTy(),currentBasicBlock);
    else if(node->sons[1]->node_type==NodeType::Div)
        res=BinaryInst::CreateDiv(left,right,Type::getIntegerTy(),currentBasicBlock);
    else
        res=BinaryInst::CreateMod(left,right,Type::getIntegerTy(),currentBasicBlock);
    return res;
}

Value* AST2IR::translateUnaryExp(UnaryExpNode* node, BasicBlock* currentBasicBlock){
    fmt::print("translateUnaryExp\n");
    if(node->sons.size()==1){
        //IDENT() 无参函数
        IdentNode* ident=reinterpret_cast<IdentNode*>(node->sons[0]);
        return CallInst::Create(functionTable[ident->value], {}, currentBasicBlock);
    }else if(node->sons[0]->node_type==NodeType::Add){
        return translateExpr(node->sons[1],currentBasicBlock);
    }else if(node->sons[0]->node_type==NodeType::Sub){
        Value* right=translateExpr(node->sons[1],currentBasicBlock);
        Value* left=ConstantInt::Create(0);
        return BinaryInst::CreateSub(left,right,Type::getIntegerTy(),currentBasicBlock);
    }else if(node->sons[0]->node_type==NodeType::Not){
        Value* right=translateExpr(node->sons[1],currentBasicBlock);
        Value* left=ConstantInt::Create(0);
        return BinaryInst::CreateEq(left,right,Type::getIntegerTy(),currentBasicBlock);
    }else{
        //IDENT(Exp,...) 有参函数
        IdentNode* ident=reinterpret_cast<IdentNode*>(node->sons[0]);
        std::vector<Value *> args{};
        for(int i=1;i<node->sons.size();i+=2){
            args.push_back(translateExpr(node->sons[i],currentBasicBlock));
        }
        return CallInst::Create(functionTable[ident->value], args, currentBasicBlock);
    }
}

Value* AST2IR::translateLVal(LValNode* node, BasicBlock* currentBasicBlock, bool isExpr){
    fmt::print("translateLVal\n");
    IdentNode* ident=reinterpret_cast<IdentNode*>(node->sons[0]);
    auto vari=lookUpVariTable(ident->value);
    //根据维度判断是数组还是整型
    if(vari.second.size()==0){
        //整型变量
        if(isExpr)//如果是表达式，求值
            return LoadInst::Create(vari.first,currentBasicBlock);
        else//如果是左值（需要被赋值），返回地址
            return vari.first;
    }else{
        //数组变量
        std::vector<Value*> indices{};
        std::vector<std::optional<std::size_t>> bounds=vari.second;
        for(int i=2;i<node->sons.size();i+=3){
            indices.push_back(translateExpr(node->sons[i],currentBasicBlock));
        }
        if(indices.size()==bounds.size()&&isExpr){//如果是表达式，并且是整型，需要load值
            OffsetInst* addr=OffsetInst::Create(Type::getIntegerTy(), vari.first, indices, bounds, currentBasicBlock);
            return LoadInst::Create(addr,currentBasicBlock);
        }
        //如果还是个数组，需要在多余的维度上补0
        while(indices.size()<bounds.size()){
            indices.push_back(ConstantInt::Create(0));
        }
        return OffsetInst::Create(Type::getIntegerTy(), vari.first, indices, bounds, currentBasicBlock);
    }
}

Value* AST2IR::translateIntConst(IntConstNode* node, BasicBlock* currentBasicBlock){
    fmt::print("translateIntConst\n");
    return ConstantInt::Create(node->value);
}