#include "ast/ast.h"
#include "semant/type.h"
#include <fmt/core.h>

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
        semanticAnalysis(root);
    }

    return 0;
}
