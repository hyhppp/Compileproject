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
#include <json/json.h>

#include "type.h"

using namespace std;

static llvm::LLVMContext context;
static llvm::IRBuilder<> builder(context);

llvm::AllocaInst *CreateEntryBlockAlloca(llvm::Function *TheFunction, llvm::StringRef VarName, llvm::Type *type);

class Node
{
public:
    // Value or name of node, if type of node is int, the value of nodeName is the value of the integer, float, bool, char are similar
    // if type is var, the value is the name of this variable
    string *nodeName;
    // The type of the node
    string *nodeType;
    // The type of exp, var or const
    int valueType;
    // The number of child of the node
    int childNum;
    // Child nodes of this node
    Node **childNode;
    // The number of rows of the node in the file
    int lineNo;

    llvm::Value *irBuild();
    llvm::Value *irBuildExp();
    llvm::Value *irBuildFun();
    llvm::Value *irBuildVar();
    llvm::Value *irBuildStmt();
    llvm::Value *irBuildWhile();
    llvm::Value *irBuildIf();
    llvm::Value *irBuildReturn();
    llvm::Value *irBuildCompSt();
    llvm::Value *irBuildRELOP();
    llvm::Value *irBuildPrint();
    llvm::Value *irBuildPrintf();
    llvm::Value *irBuildScan();
    llvm::Value *irBuildAddr();
    llvm::Instruction::CastOps getCastInst(llvm::Type *src, llvm::Type *dst);
    llvm::Value *typeCast(llvm::Value *src, llvm::Type *dst);
    int getValueType();
    int getValueType(Node *node);
    void setValueType(int type);
    llvm::Type *getLlvmType(int type, int arraySize);
    vector<pair<string, int>> *getNameList(int type);
    vector<llvm::Value *> *getArgs();
    vector<llvm::Value *> *getPrintArgs();
    vector<llvm::Value *> *getArgsAddr();
    vector<pair<string, llvm::Type *>> *getParam();

    Node(char *nodeName, string nodeType, int lineNo);
    Node(string nodeName, string nodeType, int childNum, ...);
    ~Node();

    Json::Value jsonGen();
};

class codeGen
{
public:
    llvm::Module *module;
    llvm::Function *printf, *scanf;
    stack<llvm::Function *> funStack;

    llvm::Function *getCurFunction();
    void pushFunction(llvm::Function *func);
    void popFunction();
    llvm::Value *findValue(const std::string &name);
    llvm::Function *createPrintf();
    llvm::Function *createScanf();
    void generate(Node *root);
    codeGen(/* args */);
    ~codeGen();
};

#endif
