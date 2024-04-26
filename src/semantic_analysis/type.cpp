#include "semantic_analysis/type.h"
#include <map>
#include <fmt/core.h>
// symbol_table
std::vector<std::map<std::string, Type *>> variable_table;
std::vector<std::map<std::string, FunctionType *>> func_table;

Type *searchVariable(std::string name)
{
    for (int i = variable_table.size() - 1; i >= 0; i--)
    {
        auto iter = variable_table[i].find(name);
        if (iter != variable_table[i].end())
            if (iter->second->getType() == BASIC)
                return new BasicType(BASIC, reinterpret_cast<BasicType *>(iter->second)->getBasic());
            else
            {
                return new ArrayType(*(reinterpret_cast<ArrayType *>(iter->second)));
            }
    }
    // not found
    return nullptr;
}

FunctionType *searchFunction(std::string name)
{
    for (int i = func_table.size() - 1; i >= 0; i--)
    {
        auto iter = func_table[i].find(name);
        if (iter != func_table[i].end())
            return new FunctionType(*(reinterpret_cast<FunctionType *>(iter->second)));
        ;
    }
    // not found
    return nullptr;
}

bool isParamsTypeEqual(Type *type1, Type *type2)
{
    if (type1->getType() != type2->getType())
        return false;
    if (type1->getType() == BASIC && (reinterpret_cast<BasicType *>(type1)->getBasic() != reinterpret_cast<BasicType *>(type2)->getBasic()))
    {
        return false;
    }
    if (type1->getType() == ARRAY)
    {
        ArrayType *array_type1 = reinterpret_cast<ArrayType *>(type1);
        ArrayType *array_type2 = reinterpret_cast<ArrayType *>(type2);
        // dimention not match
        if (array_type1->getDimension() != array_type2->getDimension())
            return false;
        // 形参a[][5]   实参a[1][4] not match
        for (int i = 1; i < array_type1->getDimension(); i++)
        {
            if (array_type1->getSize(i) != array_type2->getSize(i))
                return false;
        }
    }
    return true;
}

void newScope()
{
    std::map<std::string, Type *> v_table;
    variable_table.push_back(v_table);
    std::map<std::string, FunctionType *> f_table;
    func_table.push_back(f_table);
}

void deleteScope()
{
    std::map<std::string, Type *> &v_table = variable_table.back();
    for (auto &pair : v_table)
    {
        delete pair.second;
    }
    v_table.clear();
    std::map<std::string, FunctionType *> &f_table = func_table.back();
    for (auto &pair : f_table)
    {
        delete pair.second;
    }
    f_table.clear();
    variable_table.pop_back();
    func_table.pop_back();
}

void semanticAnalysis(Node *root)
{
    newScope();
    // Input & output functions
    // int getint()
    func_table[func_table.size() - 1].insert(std::pair("getint", new FunctionType(1)));
    // int getch()
    func_table[func_table.size() - 1].insert(std::pair("getch", new FunctionType(1)));
    // int getarray(int a[])
    auto func_type = new FunctionType(1);
    auto array_type = new ArrayType(ARRAY, 1);
    array_type->addOneDimension(-1);
    func_type->addField(array_type);
    func_table[func_table.size() - 1].insert(std::pair("getarray", func_type));
    // void putint(int num)
    func_type = new FunctionType(0);
    func_type->addField(new BasicType(BASIC, 1));
    func_table[func_table.size() - 1].insert(std::pair("putint", func_type));
    // void putch(int ch)
    func_type = new FunctionType(0);
    func_type->addField(new BasicType(BASIC, 1));
    func_table[func_table.size() - 1].insert(std::pair("putch", func_type));
    // void putarray(int n, int a[])
    func_type = new FunctionType(0);
    func_type->addField(new BasicType(BASIC, 1));
    array_type = new ArrayType(ARRAY, 1);
    array_type->addOneDimension(-1);
    func_type->addField(array_type);
    func_table[func_table.size() - 1].insert(std::pair("putarray", func_type));
    // Timing functions
    // void starttime();
    func_table[func_table.size() - 1].insert(std::pair("starttime", new FunctionType(0)));
    // void stoptime();
    func_table[func_table.size() - 1].insert(std::pair("stoptime", new FunctionType(0)));
    analyzeCompUnit(reinterpret_cast<CompUnitNode *>(root));
    deleteScope();
}

void analyzeCompUnit(CompUnitNode *node)
{
    fmt::print("analyzeCompUnit\n");
    if (auto n = node->sons[0]->as<CompUnitNode *>())
        analyzeCompUnit(n);
    if (auto n = node->sons[node->sons.size() - 1]->as<VarDeclNode *>())
        analyzeVarDecl(n);
    else if (auto n = node->sons[node->sons.size() - 1]->as<FuncDefNode *>())
        analyzeFuncDef(reinterpret_cast<FuncDefNode *>(n));
}

void analyzeVarDecl(VarDeclNode *node)
{
    fmt::print("analyzeVarDecl\n");
    analyzeVarDefList(reinterpret_cast<VarDefListNode *>(node->sons[1]));
}

void analyzeVarDefList(VarDefListNode *node)
{
    fmt::print("VarDefListNode\n");
    if (auto n = node->sons[0]->as<VarDefNode *>()) // VarDef
        analyzeVarDef(n);
    else
    {
        // VarDefList , VarDef
        analyzeVarDefList(reinterpret_cast<VarDefListNode *>(node->sons[0]));
        analyzeVarDef(reinterpret_cast<VarDefNode *>(node->sons[2]));
    }
}

void analyzeVarDef(VarDefNode *node)
{
    fmt::print("analyzeVarDef\n");
    IdentNode *id = reinterpret_cast<IdentNode *>(node->sons[0]);
    int num = node->sons.size();
    // check redefine
    if (variable_table[variable_table.size() - 1].find(id->value) != variable_table[variable_table.size() - 1].end())
    {
        fmt::print("error:redefine variable {}\n", id->value);
        exit(1);
    }
    if (num == 1)
    {
        // IDENT
        variable_table[variable_table.size() - 1].insert(std::pair(id->value, static_cast<Type *>(new BasicType(BASIC, 1))));
    }
    else if (num == 2)
    {
        // IDENT ArrayDef
        ArrayType *type = new ArrayType(ARRAY, 1);
        analyzeArrayDef(reinterpret_cast<ArrayDefNode *>(node->sons[1]), type);
        variable_table[variable_table.size() - 1].insert(std::pair(id->value, static_cast<Type *>(type)));
    }
    else
    {
        // IDENT ASSIGN Exp
        // 左右表达式类型必须相同，左边只能int
        Type *right_type = analyzeExp(reinterpret_cast<ExpNode *>(node->sons[2]));
        if (right_type->getType() != BASIC || reinterpret_cast<BasicType *>(right_type)->getBasic() != 1)
        {
            fmt::print("error: left and right expression types do not match\n");
            exit(1);
        }
        delete right_type;
        variable_table[variable_table.size() - 1].insert(std::pair(id->value, static_cast<Type *>(new BasicType(BASIC, 1))));
    }
}

void analyzeArrayDef(ArrayDefNode *node, ArrayType *type)
{
    fmt::print("analyzeArrayDef\n");
    if (auto n = node->sons[0]->as<ArrayDefNode *>())
    {
        //ArrayDef [ INT_CONST ]
        analyzeArrayDef(n, type);
        type->addOneDimension(reinterpret_cast<IntConstNode *>(node->sons[2])->value);
    }
    else
    {
        //[ INT_CONST ]
        type->addOneDimension(reinterpret_cast<IntConstNode *>(node->sons[1])->value);
    }
}

void analyzeFuncDef(FuncDefNode *node)
{
    fmt::print("analyzeFuncDef\n");
    int num = node->sons.size();
    IdentNode *id = reinterpret_cast<IdentNode *>(node->sons[1]);
    // check redefine
    if (func_table[func_table.size() - 1].find(id->value) != func_table[func_table.size() - 1].end())
    {
        fmt::print("error:redefine function {}\n", id->value);
        exit(1);
    }
    if (num == 5)
    {
        if (node->sons[0]->node_type == IntType)
        {
            //INT_TYPE IDENT [ ] Block 
            func_table[func_table.size() - 1].insert(std::pair(id->value, new FunctionType(1)));
        }
        else
        {
            //VOID_TYPE IDENT [ ] Block 
            func_table[func_table.size() - 1].insert(std::pair(id->value, new FunctionType(0)));
        }
        newScope();
        analyzeBlock(reinterpret_cast<BlockNode *>(node->sons[4]), func_table[func_table.size() - 2].at(id->value));
    }
    else
    {
        if (node->sons[0]->node_type == IntType)
        {
            //INT_TYPE IDENT [ FuncFParams ] Block
            FunctionType *func_type = new FunctionType(1);
            func_table[func_table.size() - 1].insert(std::pair(id->value, func_type));
        }
        else
        {
            //VOID_TYPE IDENT [ FuncFParams ] Block
            FunctionType *func_type = new FunctionType(0);
            func_table[func_table.size() - 1].insert(std::pair(id->value, func_type));
        }
        newScope();
        analyzeFuncFParams(reinterpret_cast<FuncFParamsNode *>(node->sons[3]), func_table[func_table.size() - 2].at(id->value));
        analyzeBlock(reinterpret_cast<BlockNode *>(node->sons[5]), func_table[func_table.size() - 2].at(id->value));
    }
}

void analyzeFuncFParams(FuncFParamsNode *node, FunctionType *func_type)
{
    fmt::print("analyzeFuncFParams\n");
    if (auto n = node->sons[0]->as<FuncFParamNode *>())
    {
        //FuncFParam
        analyzeFuncFParam(n, func_type);
    }
    else
    {
        //FuncFParams , FuncFParam
        analyzeFuncFParams(reinterpret_cast<FuncFParamsNode *>(node->sons[0]), func_type);
        analyzeFuncFParam(reinterpret_cast<FuncFParamNode *>(node->sons[2]), func_type);
    }
}

void analyzeFuncFParam(FuncFParamNode *node, FunctionType *func_type)
{
    fmt::print("analyzeFuncFParam\n");
    int num = node->sons.size();
    IdentNode *id = reinterpret_cast<IdentNode *>(node->sons[1]);
    // check redefine
    if (variable_table[variable_table.size() - 1].find(id->value) != variable_table[variable_table.size() - 1].end())
    {
        fmt::print("error:redefine variable {}\n", id->value);
        exit(1);
    }
    if (num == 2)
    {
        //INT_TYPE IDENT
        func_type->addField(new BasicType(BASIC, 1));
        variable_table[variable_table.size() - 1].insert(std::pair(id->value, static_cast<Type *>(new BasicType(BASIC, 1))));
    }
    else
    {
        ArrayType *array_type = new ArrayType(ARRAY, 1);
        array_type->addOneDimension(-1); //-1 means int function(int a[],...);
        if (num == 5) //NT_TYPE IDENT [ ] ArrayDef
            analyzeArrayDef(reinterpret_cast<ArrayDefNode *>(node->sons[4]), array_type);
        func_type->addField(array_type);
        variable_table[variable_table.size() - 1].insert(std::pair(id->value, static_cast<Type *>(new ArrayType(*array_type))));
    }
}

void analyzeBlock(BlockNode *node, FunctionType *func_type)
{
    fmt::print("analyzeBlock\n");
    if (node->sons.size() == 3)
    {
        //{ BlockItems }
        analyzeBlockItems(reinterpret_cast<BlockItemsNode *>(node->sons[1]), func_type);
    }
    deleteScope();
}

void analyzeBlockItems(BlockItemsNode *node, FunctionType *func_type)
{
    fmt::print("analyzeBlockItems\n");
    if (auto n = node->sons[0]->as<BlockItemNode *>())//BlockItem
        analyzeBlockItem(n, func_type);
    else
    {
        //BlockItems BlockItem
        analyzeBlockItems(reinterpret_cast<BlockItemsNode *>(node->sons[0]), func_type);
        analyzeBlockItem(reinterpret_cast<BlockItemNode *>(node->sons[1]), func_type);
    }
}

void analyzeBlockItem(BlockItemNode *node, FunctionType *func_type)
{
    fmt::print("analyzeBlockItem\n");
    if (auto n = node->sons[0]->as<VarDeclNode *>())//VarDecl
        analyzeVarDecl(n);
    else //Stmt
        analyzeStmt(reinterpret_cast<StmtNode *>(node->sons[0]), func_type);
}

void analyzeStmt(StmtNode *node, FunctionType *func_type)
{
    fmt::print("analyzeStmt\n");
    int num = node->sons.size();
    if (num == 1)
    {
        // Block
        newScope();
        analyzeBlock(reinterpret_cast<BlockNode *>(node->sons[0]), func_type);
    }
    else if (node->sons[0]->node_type == Return)
    {
        // Return [Exp] ;
        if (num == 2 && func_type->getReturnType() != 0)
        {
            fmt::print("error: return type error\n");
            exit(1);
        }
        if (num == 3)
        {
            Type *type = analyzeExp(reinterpret_cast<ExpNode *>(node->sons[1]));
            if (type->getType() != BASIC || reinterpret_cast<BasicType *>(type)->getBasic() != func_type->getReturnType())
            {
                fmt::print("error: return type error\n");
                exit(1);
            }
        }
    }
    else if (auto n = node->sons[0]->as<ExpNode *>())
    {
        // Exp ;
        analyzeExp(n);
    }
    else if (node->sons[0]->node_type == If || node->sons[0]->node_type == While)
    {
        if (num == 5)
        {
            // IF ( Exp ) Stmt
            // WHILE ( Exp ) Stmt
            analyzeExp(reinterpret_cast<ExpNode *>(node->sons[2]));
            analyzeStmt(reinterpret_cast<StmtNode *>(node->sons[4]), func_type);
        }
        else
        {
            // IF ( Exp ) Stmt ELSE Stmt
            analyzeExp(reinterpret_cast<ExpNode *>(node->sons[2]));
            analyzeStmt(reinterpret_cast<StmtNode *>(node->sons[4]), func_type);
            analyzeStmt(reinterpret_cast<StmtNode *>(node->sons[6]), func_type);
        }
    }
    else if (node->sons[0]->node_type == LVal)
    {
        // LVal = Exp ;
        Type *left_type = analyzeLVal(reinterpret_cast<LValNode *>(node->sons[0]));
        Type *right_type = analyzeExp(reinterpret_cast<ExpNode *>(node->sons[2]));
        if (left_type->getType() == ARRAY)
        {
            fmt::print("error: array type is not assignable\n");
            exit(1);
        }
        else if (reinterpret_cast<BasicType *>(left_type)->getBasic() != reinterpret_cast<BasicType *>(right_type)->getBasic())
        {
            fmt::print("error: error: left and right expression types do not match\n");
            exit(1);
        }
        delete left_type;
    }
}

Type *analyzeLVal(LValNode *node)
{
    fmt::print("analyzeLVal\n");
    IdentNode *id = reinterpret_cast<IdentNode *>(node->sons[0]);
    // check scope
    Type *type = searchVariable(id->value);
    if (type == nullptr)
    {
        fmt::print("error: variable {} is not defined\n", id->value);
        exit(1);
    }
    if (node->sons.size() == 1)
    {
        //IDENT
        return type;
    }
    else
    {
        //IDENT Index
        if (type->getType() == BASIC)
        {
            fmt::print("error: variable {} is not an array\n", id->value);
            exit(1);
        }
        analyzeIndex(reinterpret_cast<IndexNode *>(node->sons[1]), reinterpret_cast<ArrayType *>(type));
        if (reinterpret_cast<ArrayType *>(type)->isBasic())
        {
            Type *basic = new BasicType(BASIC, reinterpret_cast<ArrayType *>(type)->getBasic());
            delete type;
            return basic;
        }
        else
            return type;
    }
}

void analyzeIndex(IndexNode *node, ArrayType *array_type)
{
    fmt::print("analyzeIndex\n");
    int num = node->sons.size();
    if (num == 3)
    {
        //  [Exp]
        Type *index_type = analyzeExp(reinterpret_cast<ExpNode *>(node->sons[1]));
        if (index_type->getType() != BASIC || reinterpret_cast<BasicType *>(index_type)->getBasic() != 1)
        {
            fmt::print("array subscript is not an integer\n");
            exit(1);
        }
        bool success = array_type->subOneDimension();
        if (success == false)
        {
            fmt::print("error: array's dimension overflow\n");
            exit(1);
        }
    }
    else
    {
        //Index [ Exp ]
        analyzeIndex(reinterpret_cast<IndexNode *>(node->sons[0]), array_type);
        Type *index_type = analyzeExp(reinterpret_cast<ExpNode *>(node->sons[2]));
        if (index_type->getType() != BASIC || reinterpret_cast<BasicType *>(index_type)->getBasic() != 1)
        {
            fmt::print("array subscript is not an integer\n");
            exit(1);
        }
        bool success = array_type->subOneDimension();
        if (success == false)
        {
            fmt::print("error: array's dimension overflow\n");
            exit(1);
        }
    }
}

Type *analyzeExp(ExpNode *node)
{
    fmt::print("analyzeExp\n");
    int num = node->sons.size();
    if (num == 1)
    {
        //LAndExp
        return analyzeLAndExp(reinterpret_cast<LAndExpNode *>(node->sons[0]));
    }
    else
    {
        //Exp OR LAndExp
        Type *type1 = analyzeExp(reinterpret_cast<ExpNode *>(node->sons[0]));
        Type *type2 = analyzeLAndExp(reinterpret_cast<LAndExpNode *>(node->sons[2]));
        // 假定||可以对两个int或两个array使用
        if (type1->getType() != type2->getType())
        {
            fmt::print("error: different expression types for ||\n");
            exit(1);
        }
        if (type1->getType() == BASIC && (reinterpret_cast<BasicType *>(type1)->getBasic() == 0 || reinterpret_cast<BasicType *>(type2)->getBasic() == 0))
        {
            fmt::print("error: || is invalid operator to void type\n");
            exit(1);
        }
        // 假定||返回的类型为int
        delete type1;
        delete type2;
        return new BasicType(BASIC, 1);
    }
}

Type *analyzeLAndExp(LAndExpNode *node)
{
    fmt::print("analyzeLAndExp\n");
    int num = node->sons.size();
    if (num == 1)
    {
        //EqExp
        return analyzeEqExp(reinterpret_cast<EqExpNode *>(node->sons[0]));
    }
    else
    {
        //LAndExp AND EqExp
        Type *type1 = analyzeLAndExp(reinterpret_cast<LAndExpNode *>(node->sons[0]));
        Type *type2 = analyzeEqExp(reinterpret_cast<EqExpNode *>(node->sons[2]));
        // 假定&&可以对两个int或两个array使用
        if (type1->getType() != type2->getType())
        {
            fmt::print("error: different expression types for &&\n");
            exit(1);
        }
        if (type1->getType() == BASIC && (reinterpret_cast<BasicType *>(type1)->getBasic() == 0 || reinterpret_cast<BasicType *>(type2)->getBasic() == 0))
        {
            fmt::print("error: && is invalid operator to void type\n");
            exit(1);
        }
        // 假定&&返回的类型为int
        delete type1;
        delete type2;
        return new BasicType(BASIC, 1);
    }
}

Type *analyzeEqExp(EqExpNode *node)
{
    fmt::print("analyzeEqExp\n");
    int num = node->sons.size();
    if (num == 1)
    {
        //RelExp
        return analyzeRelExp(reinterpret_cast<RelExpNode *>(node->sons[0]));
    }
    else
    {
        //EqExp EQ RelExp 
        //EqExp NE RelExp 
        Type *type1 = analyzeEqExp(reinterpret_cast<EqExpNode *>(node->sons[0]));
        Type *type2 = analyzeRelExp(reinterpret_cast<RelExpNode *>(node->sons[2]));
        // 假定==和!=可以对两个int或两个array使用
        if (type1->getType() != type2->getType())
        {
            fmt::print("error: different expression types for == or !=\n");
            exit(1);
        }
        if (type1->getType() == BASIC && (reinterpret_cast<BasicType *>(type1)->getBasic() == 0 || reinterpret_cast<BasicType *>(type2)->getBasic() == 0))
        {
            fmt::print("error: == or != is invalid operators to void type\n");
            exit(1);
        }
        // 假定==和!=返回的类型为int
        delete type1;
        delete type2;
        return new BasicType(BASIC, 1);
    }
}

Type *analyzeRelExp(RelExpNode *node)
{
    fmt::print("analyzeRelExp\n");
    if (node->sons.size() == 1)
    {
        //AddExp
        return analyzeAddExp(reinterpret_cast<AddExpNode *>(node->sons[0]));
    }
    else
    {
        //RelExp G AddExp
        //RelExp L AddExp
        //RelExp GE AddExp
        //RelExp LE AddExp
        Type *type1 = analyzeRelExp(reinterpret_cast<RelExpNode *>(node->sons[0]));
        Type *type2 = analyzeAddExp(reinterpret_cast<AddExpNode *>(node->sons[2]));
        // 假定RelExp可以对两个int或两个array使用
        if (type1->getType() != type2->getType())
        {
            fmt::print("error: different expression types for RelExp\n");
            exit(1);
        }
        if (type1->getType() == BASIC && (reinterpret_cast<BasicType *>(type1)->getBasic() == 0 || reinterpret_cast<BasicType *>(type2)->getBasic() == 0))
        {
            fmt::print("error: RelExp is invalid operators to void type\n");
            exit(1);
        }
        // 假定==和!=返回的类型为int
        delete type1;
        delete type2;
        return new BasicType(BASIC, 1);
    }
}

Type *analyzeAddExp(AddExpNode *node)
{
    fmt::print("analyzeAddExp\n");
    if (node->sons.size() == 1)
    {
        //MulExp
        return analyzeMulExp(reinterpret_cast<MulExpNode *>(node->sons[0]));
    }
    else
    {
        //AddExp ADD MulExp
        //AddExp SUB MulExp
        Type *type1 = analyzeAddExp(reinterpret_cast<AddExpNode *>(node->sons[0]));
        Type *type2 = analyzeMulExp(reinterpret_cast<MulExpNode *>(node->sons[2]));
        // 假定只能对两个int使用
        if (type1->getType() == BASIC && type2->getType() == BASIC && reinterpret_cast<BasicType *>(type1)->getBasic() == 1 && reinterpret_cast<BasicType *>(type2)->getBasic() == 1)
        {
            delete type1;
            return type2;
        }
        else
        {
            fmt::print("error: + or - is only valid for int\n");
            exit(1);
        }
    }
}

Type *analyzeMulExp(MulExpNode *node)
{
    fmt::print("analyzeMulExp\n");
    if (node->sons.size() == 1)
    {
        //UnaryExp
        return analyzeUnaryExp(reinterpret_cast<UnaryExpNode *>(node->sons[0]));
    }
    else
    {
        //MulExp MUL UnaryExp
        //MulExp DIV UnaryExp
        //MulExp MOD UnaryExp
        Type *type1 = analyzeMulExp(reinterpret_cast<MulExpNode *>(node->sons[0]));
        Type *type2 = analyzeUnaryExp(reinterpret_cast<UnaryExpNode *>(node->sons[2]));
        // 假定只能对两个int使用
        if (type1->getType() == BASIC && type2->getType() == BASIC && reinterpret_cast<BasicType *>(type1)->getBasic() == 1 && reinterpret_cast<BasicType *>(type2)->getBasic() == 1)
        {
            delete type1;
            return type2;
        }
        else
        {
            fmt::print("error: MulExp is only valid for int\n");
            exit(1);
        }
    }
}

Type *analyzeUnaryExp(UnaryExpNode *node)
{
    fmt::print("analyzeUnaryExp\n");
    int num = node->sons.size();
    if (num == 1)
    {
        //PrimaryExp 
        return analyzePrimaryExp(reinterpret_cast<PrimaryExpNode *>(node->sons[0]));
    }
    else if (num == 2)
    {
        // UnaryOp UnaryExp
        Type *type = analyzeUnaryExp(reinterpret_cast<UnaryExpNode *>(node->sons[1]));
        if (type->getType() != BASIC || reinterpret_cast<BasicType *>(type)->getBasic() != 1)
        {
            fmt::print("error: UnaryOp is only valid for int\n");
            exit(1);
        }
        return type;
    }
    else
    {
        // IDENT ( [FuncRParams] )
        // check scope
        IdentNode *id = reinterpret_cast<IdentNode *>(node->sons[0]);
        FunctionType *func_type = searchFunction(id->value);
        if (func_type == nullptr)
        {
            fmt::print("error: function {} is not defined\n", id->value);
            exit(1);
        }
        if (num == 3)
        {
            if (func_type->getFieldSize() != 0)
            {
                fmt::print("error: function {}'s arguments not match\n", id->value);
                exit(1);
            }
        }
        else
        {
            analyzeFuncRParams(reinterpret_cast<FuncRParamsNode *>(node->sons[2]), func_type);
        }
        int return_type = func_type->getReturnType();
        delete func_type;
        return new BasicType(BASIC, return_type);
    }
}

Type *analyzePrimaryExp(PrimaryExpNode *node)
{
    fmt::print("analyzePrimaryExp\n");
    if (node->sons[0]->node_type == IntConst)
    {
        //INT_CONST
        return new BasicType(BASIC, 1);
    }
    else if (node->sons[0]->node_type == LVal)
    {
        //LVal
        return analyzeLVal(reinterpret_cast<LValNode *>(node->sons[0]));
    }
    else
    {
        //( Exp )
        return analyzeExp(reinterpret_cast<ExpNode *>(node->sons[1]));
    }
}

void analyzeFuncRParams(FuncRParamsNode *node, FunctionType *func_type)
{
    fmt::print("analyzeFuncRParams\n");
    if (node->sons.size() == 1)
    {
        //Exp
        Type *type1 = analyzeExp(reinterpret_cast<ExpNode *>(node->sons[0]));
        Type *type2 = func_type->getField(0);
        if (!isParamsTypeEqual(type1, type2))
        {
            fmt::print("error: function's arguments not match\n");
            exit(1);
        }
        func_type->deleteField(0);
        delete type1;
    }
    else
    {
        //FuncRParams , Exp
        analyzeFuncRParams(reinterpret_cast<FuncRParamsNode *>(node->sons[0]), func_type);
        Type *type1 = analyzeExp(reinterpret_cast<ExpNode *>(node->sons[2]));
        Type *type2 = func_type->getField(0);
        if (!isParamsTypeEqual(type1, type2))
        {
            fmt::print("error: function's arguments not match\n");
            exit(1);
        }
        func_type->deleteField(0);
        delete type1;
    }
}