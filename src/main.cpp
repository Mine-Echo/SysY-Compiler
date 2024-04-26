#include "ast/ast.h"
#include "semantic_analysis/type.h"
#include <fmt/core.h>

extern int yyparse();
// int syntax_error=0;
extern FILE* yyin;

Node* root;

int main(int argc, char **argv) {
    yyin = fopen(argv[1], "r");
    if(yyparse()==1)
        return 1;
    fmt::print("\n");
    print_tree(root);
    //semanticAnalysis
    semanticAnalysis(root);
    return 0;
}
