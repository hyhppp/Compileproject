#include "ast.h"

Json::Value Node::jsonGen()
{
    Json::Value root;
    string padding = "";
    if (this->nodeType->compare("Specifier") == 0 || this->nodeType->compare("Exp") == 0)
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