#ifndef _CODE_GEN_H_
#define _CODE_GEN_H_

#include <iostream>
#include <stack>
#include <map>
#include <string>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/ValueSymbolTable.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/Interpreter.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Target/TargetMachine.h>
#include "ast.h"
#include "type.h"

using namespace std;

static llvm::LLVMContext context;
static llvm::IRBuilder<> builder(context);

llvm::Instruction::CastOps getCastInst(llvm::Type *src, llvm::Type *dst);
llvm::Value *typeCast(llvm::Value *src, llvm::Type *dst);

llvm::Value *irBuild(Node *node);
llvm::Value *irBuildExp(Node *node);
llvm::Value *irBuildFun(Node *node);
llvm::Value *irBuildVar(Node *node);
llvm::Value *irBuildDeclaration(Node *node);
llvm::Value *irBuildWhile(Node *node);
llvm::Value *irBuildIf(Node *node);
llvm::Value *irBuildReturn(Node *node);
llvm::Value *irBuildScope(Node *node);
llvm::Value *irBuildRELOP(Node *node);
llvm::Value *irBuildPrint(Node *node);
llvm::Value *irBuildPrintf(Node *node);
llvm::Value *irBuildScan(Node *node);
llvm::Value *irBuildAddr(Node *node);

llvm::Type *getLlvmType(int type, int arraySize);
vector<pair<string, int>> *getNameList(Node *node, int type);
vector<llvm::Value *> *getArgs(Node *cur);
vector<llvm::Value *> *getPrintArgs(Node *cur);
vector<llvm::Value *> *getArgsAddr(Node *cur);
vector<pair<string, llvm::Type *>> *getParam(Node *cur);

llvm::AllocaInst *CreateEntryBlockAlloca(llvm::Function *TheFunction, llvm::StringRef VarName, llvm::Type *type);

class codeGen
{
public:
    llvm::Module *module;
    llvm::Function *printf, *scanf;
    stack<llvm::Function *> funStack;

    llvm::Value *findValue(const std::string &name);
    llvm::Function *getCurFunction();
    llvm::Function *createPrintf();
    llvm::Function *createScanf();

    void pushFunction(llvm::Function *func);
    void popFunction();

    void generate(Node *root);

    codeGen();
    ~codeGen();
};

#endif
