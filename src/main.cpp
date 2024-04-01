#include "ast/ast.h"

#include <fmt/core.h>

extern int yyparse();
int syntax_error=0;
extern FILE* yyin;

NodePtr root;

int main(int argc, char **argv) {
    yyin = fopen(argv[1], "r");
    yyparse();
    if(syntax_error==1)
        return 1;
    // print_expr(static_cast<ExprPtr>(root));
    // fmt::print("Hello, World!\n");
    return 0;
}
