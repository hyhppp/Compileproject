#include "type.h"
#include "code_gen.h"
#include "ast.h"
#include "grammar.hpp"
#include <fstream>

extern Node *ROOT;
extern codeGen *generator;
extern int yyparse();

int main() {
    yyparse();
    auto root = ROOT->jsonGen();

    string jsonFile = "./Ast_Tree_Visual/ast_tree.json";
    std::ofstream astJson(jsonFile);
    if(astJson.is_open()){
        astJson << root;
        astJson.close();
    }

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    generator = new codeGen();
    generator->generate(ROOT);

    return 0;
}
