#include "ast/ast.h"
#include "semant/type.h"
#include "ast2ir/ast2ir.h"
#include <fmt/core.h>
#include <fstream>
extern int yyparse();
extern int syntax_error;
extern FILE* yyin;

Node* root;
Node* tree;

int main(int argc, char **argv) {
    yyin = fopen(argv[1], "r");
    if(yyparse()==1)    // syntax error
        return 1;
    fmt::print("\n");

    if (!syntax_error){
        tree = construct_ast_tree(root, tree);
        print_tree(tree);
        //semantic analysis
        semant::semanticAnalysis(root);
        std::unique_ptr<Module> module=std::make_unique<Module>();
        //translate AST to IR
        AST2IR ast2ir;
        ast2ir.translate(tree,module.get());
        std::ofstream os(argv[2]);
        module->print(std::cout,true);
        module->print(os,false);
        // test();
        os.close();
    }

    return 0;
}
