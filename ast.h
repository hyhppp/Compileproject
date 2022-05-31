#ifndef _AST_H_
#define _AST_H_

#include <llvm/IR/Value.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/GlobalVariable.h>
#include <json/json.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <ostream>

#include "type.h"

using namespace std;

class Node
{
public:
    // Value or name of node, if type of node is int, the value of nodeText is the value of the integer, float, bool, char are similar
    // if type is var, the value is the name of this variable
    string *nodeText;
    // The type of the node
    string *nodeType;
    // The type of exp, var or const
    int valueType;
    // The number of child of the node
    int childNum;
    // Child nodes of this node
    Node **childNode;

    int getValueType();
    int getValueType(Node *node);
    void setValueType(int type);

    Json::Value jsonGen();

    Node(char *nodeText, string nodeType);
    Node(string nodeText, string nodeType, int childNum, ...);

    ~Node();
};

#endif