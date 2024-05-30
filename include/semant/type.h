#pragma once
#include <string>
#include <vector>
#include "ast/ast.h"
class Type;

enum TypeKind
{
    BASIC,
    ARRAY
};

class Type
{
protected:
    TypeKind kind;

public:
    Type(TypeKind kind) : kind(kind) {}
    TypeKind getType()
    {
        return this->kind;
    }
    virtual ~Type(){};
};

class BasicType : public Type
{
private:
    int basic; // 0 for void 1 for int
public:
    BasicType(TypeKind kind, int basic) : Type(kind), basic(basic) {}
    int getBasic() { return basic; }
    ~BasicType() {}
};

class ArrayType : public Type
{
private:
    int basic; // 0 for void 1 for int
    int dimension;
    std::vector<int> size; // the size of each dimension
public:
    ArrayType(TypeKind kind, int basic) : Type(kind), basic(basic), dimension(0) {}
    ArrayType(const ArrayType &other)
        : Type(other.kind), basic(other.basic), dimension(other.dimension), size(other.size) {}
    void addOneDimension(int n)
    {
        size.push_back(n);
        this->dimension++;
    }
    bool subOneDimension()
    {
        if (this->dimension == 0)
            return false;
        size.erase(size.begin());
        this->dimension--;
        return true;
    }
    bool isBasic()
    {
        return this->dimension == 0;
    }
    int getBasic()
    {
        return this->basic;
    }
    int getDimension()
    {
        return this->dimension;
    }
    int getSize(int i)
    {
        return this->size[i];
    }
    ~ArrayType() {}
};

class FunctionType
{
private:
    int return_type; // 0 for void 1 for int
    std::vector<Type *> fields;

public:
    FunctionType(int return_type) : return_type(return_type) {}
    FunctionType(const FunctionType &other) : return_type(other.return_type)
    {
        for (int i = 0; i < other.fields.size(); i++)
        {
            if (other.fields[i]->getType() == BASIC)
            {
                this->addField(new BasicType(BASIC, reinterpret_cast<BasicType *>(other.fields[i])->getBasic()));
            }
            else
            {
                ArrayType *array_type = new ArrayType(*reinterpret_cast<ArrayType *>(other.fields[i]));
                this->addField(array_type);
            }
        }
    }
    int getReturnType()
    {
        return this->return_type;
    }
    int getFieldSize()
    {
        return this->fields.size();
    }
    void addField(Type *field)
    {
        this->fields.push_back(field);
    }
    Type *getField(int i)
    {
        return this->fields[i];
    }
    void deleteField(int i)
    {
        delete this->fields[i];
        this->fields.erase(this->fields.begin());
    }
    ~FunctionType()
    {
        for (int i = 0; i < this->fields.size(); i++)
        {
            delete fields[i];
        }
    }
};

// search variable in variable_table
Type *searchVariable(std::string name);
// search function in func_table
FunctionType *searchFunction(std::string name);
// if parameter形参 and argument实参 type are equal
bool isParamsTypeEqual(Type *type1, Type *type2);
// create a new spoce and symbol_table
void newScope();
// delete a scope and symbol_table
void deleteScope();
// semanticAnalysis
void semanticAnalysis(Node *root);

void analyzeCompUnit(CompUnitNode *node);

void analyzeVarDecl(VarDeclNode *node);

void analyzeVarDefList(VarDefListNode *node);

void analyzeVarDef(VarDefNode *node);

void analyzeArrayDef(ArrayDefNode *node, ArrayType *type);

void analyzeFuncDef(FuncDefNode *node);

void analyzeFuncFParams(FuncFParamsNode *node, FunctionType *func_type);

void analyzeFuncFParam(FuncFParamNode *node, FunctionType *func_type);

void analyzeBlock(BlockNode *node, FunctionType *func_type);

void analyzeBlockItems(BlockItemsNode *node, FunctionType *func_type);

void analyzeBlockItem(BlockItemNode *node, FunctionType *func_type);

void analyzeStmt(StmtNode *node, FunctionType *func_type);

Type *analyzeLVal(LValNode *node);

void analyzeIndex(IndexNode *node, ArrayType *array_type);

Type *analyzeExp(ExpNode *node);

Type *analyzeLAndExp(LAndExpNode *node);

Type *analyzeEqExp(EqExpNode *node);

Type *analyzeRelExp(RelExpNode *node);

Type *analyzeAddExp(AddExpNode *node);

Type *analyzeMulExp(MulExpNode *node);

Type *analyzeUnaryExp(UnaryExpNode *node);

Type *analyzePrimaryExp(PrimaryExpNode *node);

void analyzeFuncRParams(FuncRParamsNode *node, FunctionType *func_type);