#include "ast.h"
#include "code_gen.h"
#include <cstdarg>

Node *ROOT;
extern stack<llvm::BasicBlock *> GlobalAfterBB;
extern codeGen *generator;

Node::Node(char *nodeText, string nodeType)
{
    this->nodeText = new string(nodeText);
    this->nodeType = new string(nodeType);
    this->childNum = 0;
}

Node::Node(string nodeText, string nodeType, int childNum, ...)
{
    this->nodeText = new string(nodeText);
    this->nodeType = new string(nodeType);

    this->childNum = childNum;
    this->childNode = new Node *[childNum];

    va_list l;
    va_start(l, childNum);
    for (int i = 0; i < childNum; i++)
    {
        Node *node;
        node = va_arg(l, Node *);
        this->childNode[i] = node;
    }
    va_end(l);
}

void Node::setValueType(int type)
{
    this->valueType = type;
}

int Node::getValueType()
{
    return getValueType(this);
}

int Node::getValueType(Node *node)
{
    if (node->nodeType->compare("Datatype") == 0)
    {
        if (node->childNode[0]->nodeText->compare("int") == 0)
        {
            return TYPE_INT;
        }
        else if (node->childNode[0]->nodeText->compare("float") == 0)
        {
            return TYPE_FLOAT;
        }
        else if (node->childNode[0]->nodeText->compare("char") == 0)
        {
            return TYPE_CHAR;
        }
        else if (node->childNode[0]->nodeText->compare("boolean") == 0)
        {
            return TYPE_BOOL;
        }
    }
    else if (node->nodeType->compare("Op") == 0)
    {
        return node->valueType;
    }
    // Error
    return -1;
}

Json::Value Node::jsonGen()
{
    Json::Value root;
    string padding = "";
    if (this->nodeType->compare("Datatype") == 0 || this->nodeType->compare("Op") == 0)
    {
        int valueType = this->getValueType();
        switch (valueType)
        {
        case VOID:
            padding = "void";
            break;
        case VAR:
            padding = "var";
            break;
        case ARRAY:
            padding = "array";
            break;
        case FUN:
            padding = "func";
            break;
        case TYPE_INT:
            padding = "int";
            break;
        case TYPE_INT_ARRAY:
            padding = "int array";
            break;
        case TYPE_FLOAT:
            padding = "float";
            break;
        case TYPE_FLOAT_ARRAY:
            padding = "float array";
            break;
        case TYPE_CHAR:
            padding = "char";
            break;
        case TYPE_CHAR_ARRAY:
            padding = "char array";
            break;
        case TYPE_BOOL:
            padding = "bool";
            break;
        case TYPE_BOOL_ARRAY:
            padding = "bool array";
            break;
        default:
            break;
        }
    }

    root["name"] = *this->nodeType + (padding == "" ? "" : ": " + padding);

    for (int i = 0; i < this->childNum; i++)
    {
        if (this->childNode[i])
        {
            root["children"].append(this->childNode[i]->jsonGen());
        }
    }
    return root;
}