#include "ast/ast.h"
#include <fmt/core.h>
#include <cassert>

void print_tree(Node* node, std::string prefix, std::string ident) {
    assert(node != nullptr);
    fmt::print(prefix);
    bool print_son=false;
    if(auto *n=node->as<CompUnitNode*>()){
        fmt::print("CompUnit\n");
        print_son=true;
    }
    if(auto *n=node->as<VarDeclNode*>()){
        fmt::print("VarDecl\n");
        print_son=true;
    }
    if(auto *n=node->as<VarDefListNode*>()){
        fmt::print("VarDefList\n");
        print_son=true;
    }
    if(auto *n=node->as<VarDefNode*>()){
        fmt::print("VarDef\n");
        print_son=true;
    }
    if(auto *n=node->as<ExpNode*>()){
        fmt::print("Exp\n");
        print_son=true;
    }
    if(auto *n=node->as<ArrayDefNode*>()){
        fmt::print("ArrayDef\n");
        print_son=true;
    }
    if(auto *n=node->as<FuncDefNode*>()){
        fmt::print("FuncDef\n");
        print_son=true;
    }
    if(auto *n=node->as<BlockNode*>()){
        fmt::print("Block\n");
        print_son=true;
    }
    if(auto *n=node->as<FuncFParamsNode*>()){
        fmt::print("FuncFParams\n");
        print_son=true;
    }
    if(auto *n=node->as<FuncFParamNode*>()){
        fmt::print("FuncFParam\n");
        print_son=true;
    }
    if(auto *n=node->as<BlockItemsNode*>()){
        fmt::print("BlockItems\n");
        print_son=true;
    }
    if(auto *n=node->as<BlockItemNode*>()){
        fmt::print("BlockItem\n");
        print_son=true;
    }
    if(auto *n=node->as<StmtNode*>()){
        fmt::print("Stmt\n");
        print_son=true;
    }
    if(auto *n=node->as<LValNode*>()){
        fmt::print("LVal\n");
        print_son=true;
    }  
    if(auto *n=node->as<IndexNode*>()){
        fmt::print("Index\n");
        print_son=true;
    }
    if(auto *n=node->as<PrimaryExpNode*>()){
        fmt::print("PrimaryExp\n");
        print_son=true;
    }
    if(auto *n=node->as<UnaryExpNode*>()){
        fmt::print("UnaryExp\n");
        print_son=true;
    }
    if(auto *n=node->as<FuncRParamsNode*>()){
        fmt::print("FuncRParams\n");
        print_son=true;
    }
    if(auto *n=node->as<UnaryOpNode*>()){
        fmt::print("UnaryOp\n");
        print_son=true;
    }
    if(auto *n=node->as<MulExpNode*>()){
        fmt::print("MulExp\n");
        print_son=true;
    }
    if(auto *n=node->as<AddExpNode*>()){
        fmt::print("AddExp\n");
        print_son=true;
    }
    if(auto *n=node->as<RelExpNode*>()){
        fmt::print("RelExp\n");
        print_son=true;
    }
    if(auto *n=node->as<EqExpNode*>()){
        fmt::print("EqExp\n");
        print_son=true;
    }
    if(auto *n=node->as<LAndExpNode*>()){
        fmt::print("LAndExp\n");
        print_son=true;
    }
    if(auto *n=node->as<IntTypeNode*>()){
        fmt::print("int\n");
    } 
    // if(auto *n=node->as<SemicolonNode*>()){
    //     fmt::print(";\n");
    // } 
    if(auto *n=node->as<CommaNode*>()){
        fmt::print(",\n");
    } 
    if(auto *n=node->as<IdentNode*>()){
        fmt::print("Ident {}\n", n->value);
    } 
    if(auto *n=node->as<AssignNode*>()){
        fmt::print("=\n");
    } 
    if(auto *n=node->as<LeftBracketNode*>()){
        fmt::print("[\n");
    } 
    if(auto *n=node->as<RightBracketNode*>()){
        fmt::print("]\n");
    } 
    if(auto *n=node->as<IntConstNode*>()){
        fmt::print("IntConst {}\n",n->value);
    } 
    // if(auto *n=node->as<LeftParenNode*>()){
    //     fmt::print("(\n");
    // } 
    // if(auto *n=node->as<RightParenNode*>()){
    //     fmt::print(")\n");
    // } 
    if(auto *n=node->as<VoidTypeNode*>()){
        fmt::print("void\n");
    } 
    // if(auto *n=node->as<LeftCurlyBracketNode*>()){
    //     fmt::print("{{\n");
    // } 
    // if(auto *n=node->as<RightCurlyBracketNode*>()){
    //     fmt::print("}}\n");
    // } 
    if(auto *n=node->as<IfNode*>()){
        fmt::print("if\n");
    } 
    if(auto *n=node->as<ElseNode*>()){
        fmt::print("else\n");
    } 
    if(auto *n=node->as<WhileNode*>()){
        fmt::print("while\n");
    } 
    if(auto *n=node->as<BreakNode*>()){
        fmt::print("break\n");
    } 
    if(auto *n=node->as<ContinueNode*>()){
        fmt::print("continue\n");
    } 
    if(auto *n=node->as<ReturnNode*>()){
        fmt::print("return\n");
    } 
    if(auto *n=node->as<AddNode*>()){
        fmt::print("+\n");
    } 
    if(auto *n=node->as<SubNode*>()){
        fmt::print("-\n");
    } 
    if(auto *n=node->as<NotNode*>()){
        fmt::print("!\n");
    } 
    if(auto *n=node->as<MulNode*>()){
        fmt::print("*\n");
    } 
    if(auto *n=node->as<DivNode*>()){
        fmt::print("/\n");
    } 
    if(auto *n=node->as<ModNode*>()){
        fmt::print("%\n");
    } 
    if(auto *n=node->as<GNode*>()){
        fmt::print(">\n");
    } 
    if(auto *n=node->as<LNode*>()){
        fmt::print("<\n");
    } 
    if(auto *n=node->as<GeNode*>()){
        fmt::print(">=\n");
    }
    if(auto *n=node->as<LeNode*>()){
        fmt::print("<=\n");
    }
    if(auto *n=node->as<EqNode*>()){
        fmt::print("==\n");
    }
    if(auto *n=node->as<NeNode*>()){
        fmt::print("!=\n");
    }
    if(auto *n=node->as<AndNode*>()){
        fmt::print("&&\n");
    }
    if(auto *n=node->as<OrNode*>()){
        fmt::print("||\n");
    }
    if(print_son){
        NonTerminalNode* n=static_cast<NonTerminalNode*>(node);
        for(int i=0;i<n->sons.size();i++){
            if(i!=n->sons.size()-1)
                print_tree(n->sons[i], ident + "├─ ", ident + "│  ");
            else{
                print_tree(n->sons[i], ident + "└─ ", ident + "   ");
            }
        }
    }
}


// 抽象这颗语法树，忽略不必要的中间节点
NonTerminalNode* construct_ast_tree(Node* node, Node* tree){
    NonTerminalNode* n=static_cast<NonTerminalNode*>(node);
    NonTerminalNode* t=static_cast<NonTerminalNode*>(tree);
    if (t == nullptr){
        t = new NonTerminalNode(n->node_type);
    }
    // std::cout << "node_type: " << t->node_type << std::endl;
    for(int i=0;i<n->sons.size();i++){
        if(n->sons[i]->node_type <= 23){
            bool flag = 0;
            if (n->sons[i]->node_type == NodeType::CompUnit){
                flag = 1;
            }
            if (n->sons[i]->node_type == NodeType::VarDecl){
                flag = 1;
            }
            // 删除此节点
            // if (n->sons[i]->node_type == NodeType::VarDefList){
            //     flag = 1;
            // }
            if (n->sons[i]->node_type == NodeType::VarDef){
                flag = 1;
            }
            // 删除此节点
            // if (n->sons[i]->node_type == NodeType::ArrayDef){
            //     flag = 1;
            // }
            if (n->sons[i]->node_type == NodeType::FuncDef){
                flag = 1;
            }
            // 删除此节点
            // if (n->sons[i]->node_type == NodeType::FuncFParams){
            //     flag = 1;
            // }
            if (n->sons[i]->node_type == NodeType::FuncFParam){
                flag = 1;
            }
            if (n->sons[i]->node_type == NodeType::Block){
                flag = 1;
            }
            // if (n->sons[i]->node_type == NodeType::BlockItems){
            //     flag = 1;
            // }
            // if (n->sons[i]->node_type == NodeType::BlockItem){
            //     flag = 1;
            // }
            if (n->sons[i]->node_type == NodeType::Stmt){
                flag = 1;
            }
            if (n->sons[i]->node_type == NodeType::LVal){
                flag = 1;
            }
            // if (n->sons[i]->node_type == NodeType::Index){
            //     flag = 1;
            // }
            // if (n->sons[i]->node_type == NodeType::PrimaryExp){
            //     flag = 1;
            // }
            // 不必要时删除
            if (n->sons[i]->node_type == NodeType::UnaryExp){
                if(static_cast<NonTerminalNode*>(n->sons[i])->sons.size()>1){
                    flag = 1;  
                }
            }
            // if (n->sons[i]->node_type == NodeType::UnaryOp){
            //     flag = 1;
            // }
            // if (n->sons[i]->node_type == NodeType::FuncRParams){
            //     flag = 1;
            // }
            if (n->sons[i]->node_type == NodeType::MulExp){
                if(static_cast<NonTerminalNode*>(n->sons[i])->sons.size()>1){
                    flag = 1;  
                }
            }
            if (n->sons[i]->node_type == NodeType::AddExp){
                if(static_cast<NonTerminalNode*>(n->sons[i])->sons.size()>1){
                    flag = 1;  
                }
            }
            if (n->sons[i]->node_type == NodeType::RelExp){
                if(static_cast<NonTerminalNode*>(n->sons[i])->sons.size()>1){
                    flag = 1;  
                }
            }
            if (n->sons[i]->node_type == NodeType::EqExp){
                if(static_cast<NonTerminalNode*>(n->sons[i])->sons.size()>1){
                    flag = 1;  
                }
            }
            if (n->sons[i]->node_type == NodeType::LAndExp){
                if(static_cast<NonTerminalNode*>(n->sons[i])->sons.size()>1){
                    flag = 1;  
                }
            }
            if (n->sons[i]->node_type == NodeType::Exp){
                if(static_cast<NonTerminalNode*>(n->sons[i])->sons.size()>1){
                    flag = 1;  
                }
            }
            if (flag){
                NonTerminalNode* son = new NonTerminalNode(n->sons[i]->node_type);
                // son->sons = static_cast<NonTerminalNode*>(n->sons[i])->sons;
                t->sons.push_back(son);
                construct_ast_tree(n->sons[i], son);
            }else{
                construct_ast_tree(n->sons[i], t);
            }
        }else{
            int flag = 1;
            if (n->sons[i]->node_type == NodeType::Semicolon){
                flag = 0;
            }
            if (n->sons[i]->node_type == NodeType::LeftParen){
                flag = 0;
            }
            if (n->sons[i]->node_type == NodeType::RightParen){
                flag = 0;
            }
            if (n->sons[i]->node_type == NodeType::LeftCurlyBracket){
                flag = 0;
            }
            if (n->sons[i]->node_type == NodeType::RightCurlyBracket){
                flag = 0;
            }
            if (flag) {
                t->sons.push_back(n->sons[i]);
            }
        }
    }
    return t;
}