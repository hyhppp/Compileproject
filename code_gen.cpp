#include "code_gen.h"

codeGen *generator;
stack<llvm::BasicBlock *> GlobalAfterBB;

llvm::Instruction::CastOps getCastInst(llvm::Type *src, llvm::Type *dst)
{
    if (src == llvm::Type::getFloatTy(context) && dst == llvm::Type::getInt32Ty(context))
    {
        return llvm::Instruction::FPToSI;
    }
    else if (src == llvm::Type::getInt32Ty(context) && dst == llvm::Type::getFloatTy(context))
    {
        return llvm::Instruction::SIToFP;
    }
    else if (src == llvm::Type::getInt8Ty(context) && dst == llvm::Type::getFloatTy(context))
    {
        return llvm::Instruction::UIToFP;
    }
    else if (src == llvm::Type::getInt8Ty(context) && dst == llvm::Type::getInt32Ty(context))
    {
        return llvm::Instruction::ZExt;
    }
    else if (src == llvm::Type::getInt32Ty(context) && dst == llvm::Type::getInt8Ty(context))
    {
        return llvm::Instruction::Trunc;
    }
    else
    {
        throw logic_error("[ERROR] Wrong typecast");
    }
}

llvm::Value *typeCast(llvm::Value *src, llvm::Type *dst)
{
    llvm::Instruction::CastOps op = getCastInst(src->getType(), dst);
    return builder.CreateCast(op, src, dst, "tmptypecast");
}

llvm::Value *IR(Node *node)
{
    if (node->nodeType->compare("Define") == 0)
    {
        if (node->childNode[1]->nodeType->compare("Defineblock") == 0)
        {
            return IRVar(node);
        }
        else
        {
            return IRFun(node);
        }
    }
    else if (node->nodeType->compare("Def") == 0)
    {
        return IRVar(node);
    }
    for (int i = 0; i < node->childNum; i++)
    {
        if (node->childNode[i] != nullptr)
            IR(node->childNode[i]);
    }
    return NULL;
}

llvm::Value *IRExp(Node *node)
{
    if (node->childNode[0]->nodeType->compare("INT") == 0)
    {
        return builder.getInt32(stoi(*node->childNode[0]->nodeText));
    }
    else if (node->childNode[0]->nodeType->compare("FLOAT") == 0)
    {
        return llvm::ConstantFP::get(builder.getFloatTy(), llvm::APFloat(stof(*node->childNode[0]->nodeText)));
    }
    else if (node->childNode[0]->nodeType->compare("BOOL") == 0)
    {
        if (node->childNode[0]->nodeText->compare("true") == 0)
        {
            return builder.getInt1(true);
        }
        else
        {
            return builder.getInt1(false);
        }
    }
    else if (node->childNode[0]->nodeType->compare("CHAR") == 0)
    {
        // char --> '$ch'
        if (node->childNode[0]->nodeText->size() == 3)
            return builder.getInt8(node->childNode[0]->nodeText->at(1));
        else
        {
            if (node->childNode[0]->nodeText->compare("'\\n'") == 0)
            {
                return builder.getInt8('\n');
            }
            else if (node->childNode[0]->nodeText->compare("'\\\\'") == 0)
            {
                return builder.getInt8('\\');
            }
            else if (node->childNode[0]->nodeText->compare("'\\a'") == 0)
            {
                return builder.getInt8('\a');
            }
            else if (node->childNode[0]->nodeText->compare("'\\b'") == 0)
            {
                return builder.getInt8('\b');
            }
            else if (node->childNode[0]->nodeText->compare("'\\f'") == 0)
            {
                return builder.getInt8('\f');
            }
            else if (node->childNode[0]->nodeText->compare("'\\t'") == 0)
            {
                return builder.getInt8('\t');
            }
            else if (node->childNode[0]->nodeText->compare("'\\v'") == 0)
            {
                return builder.getInt8('\v');
            }
            else if (node->childNode[0]->nodeText->compare("'\\''") == 0)
            {
                return builder.getInt8('\'');
            }
            else if (node->childNode[0]->nodeText->compare("'\\\"'") == 0)
            {
                return builder.getInt8('\"');
            }
            else if (node->childNode[0]->nodeText->compare("'\\0'") == 0)
            {
                return builder.getInt8('\0');
            }
            else
            {
                throw logic_error("[ERROR] char not defined: " + *node->childNode[0]->nodeText);
            }
        }
    }
    else if (node->childNode[0]->nodeType->compare("STRING") == 0)
    {
        // string --> "$ch"
        string str = node->childNode[0]->nodeText->substr(1, node->childNode[0]->nodeText->length() - 2);
        llvm::Constant *strConst = llvm::ConstantDataArray::getString(context, str);
        llvm::Value *globalVar = new llvm::GlobalVariable(*generator->module, strConst->getType(), true, llvm::GlobalValue::PrivateLinkage, strConst, "_Const_String_");
        vector<llvm::Value *> indexList;
        indexList.push_back(builder.getInt32(0));
        indexList.push_back(builder.getInt32(0));
        // var value
        llvm::Value *varPtr = builder.CreateInBoundsGEP(globalVar, llvm::ArrayRef<llvm::Value *>(indexList), "tmpvar");
        return varPtr;
    }
    else if (node->childNode[0]->nodeType->compare("NAME") == 0)
    {
        if (node->childNum == 1)
        {
            // always return var value
            llvm::Value *varPtr = generator->findValue(*node->childNode[0]->nodeText);
            if (varPtr->getType()->isPointerTy() && !(varPtr->getType()->getPointerElementType()->isArrayTy()))
            {
                return builder.CreateLoad(varPtr->getType()->getPointerElementType(), varPtr, "tmpvar");
            }
            else
            {
                return varPtr;
            }
        }
        // NAME() function
        // NAME[] array or point
        else if (node->childNum == 3)
        {
            if (node->childNode[1]->nodeType->compare("LP") == 0)
            {
                llvm::Function *fun = generator->module->getFunction(*node->childNode[0]->nodeText);
                if (fun == nullptr)
                {
                    throw logic_error("[ERROR] Funtion not defined: " + *node->childNode[0]->nodeText);
                }
                return builder.CreateCall(fun, nullptr, "calltmp");
            }
            else
            {
                // var addr
                return generator->findValue(*node->childNode[0]->nodeText);
            }
        }
        else if (node->childNum == 4)
        {
            // NAME LP Args RP
            if (node->childNode[1]->nodeType->compare("LP") == 0)
            {
                if (node->childNode[0]->nodeText->compare("print") == 0)
                {
                    return IRPrint(node);
                }
                if (node->childNode[0]->nodeText->compare("printf") == 0)
                {
                    vector<llvm::Value *> *args = getPrintArgs(node->childNode[2]);
                    return builder.CreateCall(generator->printf, *args, "printf");
                }
                if (node->childNode[0]->nodeText->compare("scan") == 0)
                {
                    return IRScan(node);
                }
                if (node->childNode[0]->nodeText->compare("scanf") == 0)
                {
                    vector<llvm::Value *> *args = getPrintArgs(node->childNode[2]);
                    return builder.CreateCall(generator->scanf, *args, "scanf");
                    // throw logic_error("[ERROR] Funtion not defined: " + *node->childNode[0]->nodeText);
                }
                llvm::Function *fun = generator->module->getFunction(*node->childNode[0]->nodeText);
                if (fun == nullptr)
                {
                    throw logic_error("[ERROR] Funtion not defined: " + *node->childNode[0]->nodeText);
                }
                vector<llvm::Value *> *args = getArgs(node->childNode[2]);
                return builder.CreateCall(fun, *args, "calltmp");
            }
            else
            {
                llvm::Value *arrayValue = generator->findValue(*node->childNode[0]->nodeText);
                llvm::Value *indexValue = IRExp(node->childNode[2]);
                if (indexValue->getType() != llvm::Type::getInt32Ty(context))
                {
                    indexValue = typeCast(indexValue, llvm::Type::getInt32Ty(context));
                }
                vector<llvm::Value *> indexList;
                indexList.push_back(builder.getInt32(0));
                indexList.push_back(indexValue);
                // var value
                llvm::Value *varPtr = builder.CreateInBoundsGEP(arrayValue, llvm::ArrayRef<llvm::Value *>(indexList), "tmpvar");
                return builder.CreateLoad(varPtr->getType()->getPointerElementType(), varPtr, "tmpvar");
                // return builder.CreateLoad(builder.CreateInBoundsGEP(arrayValue, llvm::ArrayRef<llvm::Value*>(indexList)), "tmpvar");
            }
        }
    }
    else if (node->childNode[0]->nodeType->compare("LP") == 0)
    {
        return IRExp(node->childNode[1]);
    }
    else if (node->childNode[0]->nodeType->compare("MINUS") == 0)
    {
        return builder.CreateNeg(IRExp(node->childNode[1]), "tmpNeg");
    }
    else if (node->childNode[0]->nodeType->compare("NOT") == 0)
    {
        llvm::Value *tmp = IRExp(node->childNode[1]);
        if (tmp->getType() != llvm::Type::getInt1Ty(context))
        {
            throw logic_error("cannot use types other than bool in ! Exp");
        }
        return builder.CreateNot(tmp, "tmpNot");
    }
    // Exp op Exp LETH MOTH LETHE MOTHE ISE NOTE
    else
    {
        if (node->childNode[1]->nodeType->compare("ASSIGN") == 0)
        {
            llvm::Value *left = IRAddr(node->childNode[0]);
            llvm::Value *right = IRExp(node->childNode[2]);
            if (right->getType() != left->getType()->getPointerElementType())
            {
                right = typeCast(right, left->getType()->getPointerElementType());
            }
            return builder.CreateStore(right, left);
        }
        else if (node->childNode[1]->nodeType->compare("LETH") == 0 || node->childNode[1]->nodeType->compare("MOTH") == 0 || node->childNode[1]->nodeType->compare("LETHE") == 0 || node->childNode[1]->nodeType->compare("MOTHE") == 0 || node->childNode[1]->nodeType->compare("ISE") == 0 || node->childNode[1]->nodeType->compare("NOTE") == 0)
        {
            return IRCmp(node);
        }
        else
        {
            llvm::Value *left = IRExp(node->childNode[0]);
            llvm::Value *right = IRExp(node->childNode[2]);
            if (node->childNode[1]->nodeType->compare("AND") == 0)
            {
                if (left->getType() != llvm::Type::getInt1Ty(context) || right->getType() != llvm::Type::getInt1Ty(context))
                {
                    throw logic_error("cannot use types other than bool in and exp");
                }
                return builder.CreateAnd(left, right, "tmpAnd");
            }
            else if (node->childNode[1]->nodeType->compare("OR") == 0)
            {
                if (left->getType() != llvm::Type::getInt1Ty(context) || right->getType() != llvm::Type::getInt1Ty(context))
                {
                    throw logic_error("cannot use types other than bool in or exp");
                }
                return builder.CreateOr(left, right, "tmpOR");
            }
            else
            {
                if (left->getType() != right->getType())
                {
                    if (left->getType() == llvm::Type::getFloatTy(context))
                    {
                        right = typeCast(right, llvm::Type::getFloatTy(context));
                    }
                    else
                    {
                        if (right->getType() == llvm::Type::getFloatTy(context))
                        {
                            left = typeCast(left, llvm::Type::getFloatTy(context));
                        }
                        else
                        {
                            if (left->getType() == llvm::Type::getInt32Ty(context))
                            {
                                right = typeCast(right, llvm::Type::getInt32Ty(context));
                            }
                            else if (right->getType() == llvm::Type::getInt32Ty(context))
                            {
                                left = typeCast(left, llvm::Type::getInt32Ty(context));
                            }
                            else
                            {
                                throw logic_error("cann't use bool in +-*/");
                            }
                        }
                    }
                }
                if (node->childNode[1]->nodeType->compare("PLUS") == 0)
                {
                    return (left->getType() == llvm::Type::getFloatTy(context)) ? builder.CreateFAdd(left, right, "addtmpf") : builder.CreateAdd(left, right, "addtmpi");
                }
                else if (node->childNode[1]->nodeType->compare("MINUS") == 0)
                {
                    return (left->getType() == llvm::Type::getFloatTy(context)) ? builder.CreateFSub(left, right, "subtmpf") : builder.CreateSub(left, right, "subtmpi");
                }
                else if (node->childNode[1]->nodeType->compare("STAR") == 0)
                {
                    return (left->getType() == llvm::Type::getFloatTy(context)) ? builder.CreateFMul(left, right, "multmpf") : builder.CreateMul(left, right, "multmpi");
                }
                else if (node->childNode[1]->nodeType->compare("DIVISION") == 0)
                {
                    return (left->getType() == llvm::Type::getFloatTy(context)) ? builder.CreateFDiv(left, right, "divtmpf") : builder.CreateSDiv(left, right, "divtmpi");
                }
            }
        }
    }
    return NULL;
}

// Datatype FunDec Scope
llvm::Value *IRFun(Node *node)
{
    vector<pair<string, llvm::Type *>> *params = nullptr;
    vector<llvm::Type *> argTypes;
    // FunDec --> NAME LP Variableline RP
    if (node->childNode[1]->childNum == 4)
    {
        params = getParam(node->childNode[1]->childNode[2]);
        for (auto it : *params)
        {
            argTypes.push_back(it.second);
        }
    }

    llvm::FunctionType *funcType = llvm::FunctionType::get(getLlvmType(node->childNode[0]->getValueType(), 0), argTypes, false);
    llvm::Function *function = llvm::Function::Create(funcType, llvm::GlobalValue::ExternalLinkage, *node->childNode[1]->childNode[0]->nodeText, generator->module);
    generator->pushFunction(function);

    // Block
    llvm::BasicBlock *newBlock = llvm::BasicBlock::Create(context, "entrypoint", function);
    builder.SetInsertPoint(newBlock);

    // Parameters
    if (params != nullptr)
    {
        int index = 0;
        for (auto &Arg : function->args())
        {
            Arg.setName(params->at(index++).first);
        }
    }

    // Sub routine
    IRScope(node->childNode[2]);

    // Pop back
    generator->popFunction();
    return function;
}

// Define --> Datatype Defineblock SEMICOLON
// Def --> Datatype Variablelist SEMICOLON
llvm::Value *IRVar(Node *node)
{
    int type = node->childNode[0]->getValueType();
    vector<pair<string, int>> *nameList = getNameList(node->childNode[1], type);
    llvm::Type *llvmType;
    for (auto it : *nameList)
    {
        if (it.second == VAR)
        {
            llvmType = getLlvmType(type, 0);
        }
        else
        {
            llvmType = getLlvmType(type + ARRAY, it.second - ARRAY);
        }
        if (generator->funStack.empty())
        {
            llvm::Value *tmp = generator->module->getGlobalVariable(it.first, true);
            if (tmp != nullptr)
            {
                throw logic_error("Redefined Variable: " + it.first);
            }
            llvm::GlobalVariable *globalVar = new llvm::GlobalVariable(*generator->module, llvmType, false, llvm::GlobalValue::PrivateLinkage, 0, it.first);
            if (llvmType->isArrayTy())
            {
                std::vector<llvm::Constant *> constArrayElem;
                llvm::Constant *constElem = llvm::ConstantInt::get(llvmType->getArrayElementType(), 0);
                for (int i = 0; i < llvmType->getArrayNumElements(); i++)
                {
                    constArrayElem.push_back(constElem);
                }
                llvm::Constant *constArray = llvm::ConstantArray::get(llvm::ArrayType::get(llvmType->getArrayElementType(), llvmType->getArrayNumElements()), constArrayElem);
                globalVar->setInitializer(constArray);
            }
            else
            {
                globalVar->setInitializer(llvm::ConstantInt::get(llvmType, 0));
            }
        }
        else
        {
            llvm::Value *tmp = generator->funStack.top()->getValueSymbolTable()->lookup(it.first);
            if (tmp != nullptr)
            {
                throw logic_error("Redefined Variable: " + it.first);
            }
            llvm::Function *TheFunction = generator->getCurFunction();
            llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
            llvm::Value *alloc = TmpB.CreateAlloca(llvmType, nullptr, it.first);
        }
    }
    return NULL;
}

// Declaration
llvm::Value *IRDeclaration(Node *node)
{
    if (node->childNode[0]->nodeType->compare("Op") == 0)
    {
        return IRExp(node->childNode[0]);
    }
    else if (node->childNode[0]->nodeType->compare("IF") == 0)
    {
        return IRIf(node);
    }
    else if (node->childNode[0]->nodeType->compare("WHILE") == 0)
    {
        return IRWhile(node);
    }
    else if (node->childNode[0]->nodeType->compare("RETURN") == 0)
    {
        return IRReturn(node);
    }
    else if (node->childNode[0]->nodeType->compare("BREAK") == 0)
    {
        return builder.CreateBr(GlobalAfterBB.top());
    }
    else if (node->childNode[0]->nodeType->compare("Scope") == 0)
    {
        return IRScope(node->childNode[0]);
    }
    return NULL;
}

// WHILE LP Exp RP Declaration
llvm::Value *IRWhile(Node *node)
{
    llvm::Function *TheFunction = generator->getCurFunction();
    llvm::BasicBlock *condBB = llvm::BasicBlock::Create(context, "cond", TheFunction);
    llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(context, "loop", TheFunction);
    llvm::BasicBlock *afterBB = llvm::BasicBlock::Create(context, "afterLoop", TheFunction);

    GlobalAfterBB.push(afterBB);

    // Cond
    builder.CreateBr(condBB);
    builder.SetInsertPoint(condBB);
    // WHILE LP Exp RP Declaration
    llvm::Value *condValue = IRExp(node->childNode[2]);
    condValue = builder.CreateICmpNE(condValue, llvm::ConstantInt::get(llvm::Type::getInt1Ty(context), 0, true), "whileCond");
    auto branch = builder.CreateCondBr(condValue, loopBB, afterBB);
    condBB = builder.GetInsertBlock();

    // Loop
    builder.SetInsertPoint(loopBB);
    IRDeclaration(node->childNode[4]);
    builder.CreateBr(condBB);

    // After
    builder.SetInsertPoint(afterBB);
    GlobalAfterBB.pop();
    return branch;
}

// IF LP Exp RP Declaration %prec LOWER_THAN_ELSE
// IF LP Exp RP Declaration ELSE Declaration
llvm::Value *IRIf(Node *node)
{
    llvm::Value *condValue = IRExp(node->childNode[2]), *thenValue = nullptr, *elseValue = nullptr;
    condValue = builder.CreateICmpNE(condValue, llvm::ConstantInt::get(llvm::Type::getInt1Ty(context), 0, true), "ifCond");

    llvm::Function *TheFunction = generator->getCurFunction();
    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(context, "then", TheFunction);
    llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(context, "else", TheFunction);
    llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(context, "merge", TheFunction);

    // Then
    auto branch = builder.CreateCondBr(condValue, thenBB, elseBB);
    builder.SetInsertPoint(thenBB);
    thenValue = IRDeclaration(node->childNode[4]);
    builder.CreateBr(mergeBB);
    thenBB = builder.GetInsertBlock();

    // else
    builder.SetInsertPoint(elseBB);
    if (node->childNum == 7)
    {
        elseValue = IRDeclaration(node->childNode[6]);
    }
    builder.CreateBr(mergeBB);
    elseBB = builder.GetInsertBlock();

    builder.SetInsertPoint(mergeBB);
    return branch;
}

// RETURN Exp SEMICOLON
// RETURN SEMICOLON
llvm::Value *IRReturn(Node *node)
{
    if (node->childNum == 3)
    {
        auto returnInst = IRExp(node->childNode[1]);
        return builder.CreateRet(returnInst);
    }
    return builder.CreateRetVoid();
}

// Scope --> LC Defineformallist Declarationline RC
// Defineformallist --> Def Defineformallist
// Def --> Datatype Variablelist SEMICOLON
// Declarationline --> Declaration Declarationline
llvm::Value *IRScope(Node *node)
{
    Node *defNodes = node->childNode[1];
    Node *DeclarationNodes = node->childNode[2];
    while (true)
    {
        if (defNodes != nullptr && defNodes->childNum == 2)
        {
            IRVar(defNodes->childNode[0]);
            defNodes = defNodes->childNode[1];
        }
        else
        {
            break;
        }
    }
    while (true)
    {
        if (DeclarationNodes != nullptr && DeclarationNodes->childNum == 2)
        {
            IRDeclaration(DeclarationNodes->childNode[0]);
            DeclarationNodes = DeclarationNodes->childNode[1];
        }
        else
        {
            break;
        }
    }
    return NULL;
}

// Exp RELOP Exp
llvm::Value *IRCmp(Node *node)
{
    llvm::Value *left = IRExp(node->childNode[0]);
    llvm::Value *right = IRExp(node->childNode[2]);
    if (left->getType() != right->getType())
    {
        if (left->getType() == llvm::Type::getFloatTy(context))
        {
            right = typeCast(right, llvm::Type::getFloatTy(context));
        }
        else
        {
            if (right->getType() == llvm::Type::getFloatTy(context))
            {
                left = typeCast(left, llvm::Type::getFloatTy(context));
            }
            else
            {
                if (left->getType() == llvm::Type::getInt32Ty(context))
                {
                    right = typeCast(right, llvm::Type::getInt32Ty(context));
                }
                else if (right->getType() == llvm::Type::getInt32Ty(context))
                {
                    left = typeCast(left, llvm::Type::getInt32Ty(context));
                }
                else
                {
                    throw logic_error("cann't use bool in == != >= <= < >");
                }
            }
        }
    }
    if (node->childNode[1]->nodeText->compare("==") == 0)
    {
        return (left->getType() == llvm::Type::getFloatTy(context)) ? builder.CreateFCmpOEQ(left, right, "fcmptmp") : builder.CreateICmpEQ(left, right, "icmptmp");
    }
    else if (node->childNode[1]->nodeText->compare(">=") == 0)
    {
        return (left->getType() == llvm::Type::getFloatTy(context)) ? builder.CreateFCmpOGE(left, right, "fcmptmp") : builder.CreateICmpSGE(left, right, "icmptmp");
    }
    else if (node->childNode[1]->nodeText->compare("<=") == 0)
    {
        return (left->getType() == llvm::Type::getFloatTy(context)) ? builder.CreateFCmpOLE(left, right, "fcmptmp") : builder.CreateICmpSLE(left, right, "icmptmp");
    }
    else if (node->childNode[1]->nodeText->compare(">") == 0)
    {
        return (left->getType() == llvm::Type::getFloatTy(context)) ? builder.CreateFCmpOGT(left, right, "fcmptmp") : builder.CreateICmpSGT(left, right, "icmptmp");
    }
    else if (node->childNode[1]->nodeText->compare("<") == 0)
    {
        return (left->getType() == llvm::Type::getFloatTy(context)) ? builder.CreateFCmpOLT(left, right, "fcmptmp") : builder.CreateICmpSLT(left, right, "icmptmp");
    }
    else if (node->childNode[1]->nodeText->compare("!=") == 0)
    {
        return (left->getType() == llvm::Type::getFloatTy(context)) ? builder.CreateFCmpONE(left, right, "fcmptmp") : builder.CreateICmpNE(left, right, "icmptmp");
    }
    return NULL;
}

// Exp --> NAME LP Args RP
llvm::Value *IRPrint(Node *node)
{
    string formatStr = "";
    vector<llvm::Value *> *args = getPrintArgs(node->childNode[2]);
    for (auto &arg : *args)
    {
        if (arg->getType() == builder.getInt32Ty())
        {
            formatStr += "%d";
        }
        else if (arg->getType() == builder.getInt8Ty())
        {
            formatStr += "%c";
        }
        else if (arg->getType() == builder.getInt1Ty())
        {
            formatStr += "%d";
        }
        else if (arg->getType() == builder.getDoubleTy())
        {
            formatStr += "%lf";
        }
        else if (arg->getType() == builder.getInt8PtrTy())
        {
            formatStr += "%s";
        }
        else if (arg->getType()->getPointerElementType()->isArrayTy() && arg->getType()->getPointerElementType()->getArrayElementType() == builder.getInt8Ty())
        {
            formatStr += "%s";
            vector<llvm::Value *> indexList;
            indexList.push_back(builder.getInt32(0));
            indexList.push_back(builder.getInt32(0));
            arg = builder.CreateGEP(arg, indexList);
        }
        else
        {
            throw logic_error("[ERROR]Invalid type to write.");
        }
    }
    formatStr += "\n";
    auto formatConst = llvm::ConstantDataArray::getString(context, formatStr.c_str());
    auto formatStrVar = new llvm::GlobalVariable(*(generator->module), llvm::ArrayType::get(builder.getInt8Ty(), formatStr.size() + 1), true, llvm::GlobalValue::ExternalLinkage, formatConst, ".str");
    auto zero = llvm::Constant::getNullValue(builder.getInt32Ty());
    llvm::Constant *indices[] = {zero, zero};
    auto varRef = llvm::ConstantExpr::getGetElementPtr(formatStrVar->getType()->getElementType(), formatStrVar, indices);
    args->insert(args->begin(), varRef);
    return builder.CreateCall(generator->printf, *args, "printf");
}

// Args --> Exp COMMA Args
// Args --> Exp
llvm::Value *IRScan(Node *node)
{
    string formatStr = "";
    vector<llvm::Value *> *args = getArgsAddr(node->childNode[2]);
    // Just common variable
    for (auto arg : *args)
    {
        if (arg->getType()->getPointerElementType() == builder.getInt32Ty())
        {
            formatStr += "%d";
        }
        else if (arg->getType()->getPointerElementType() == builder.getInt8Ty())
        {
            formatStr += "%c";
        }
        else if (arg->getType()->getPointerElementType() == builder.getInt1Ty())
        {
            formatStr += "%d";
        }
        else if (arg->getType()->getPointerElementType() == builder.getFloatTy())
        {
            formatStr += "%f";
        }
        else if (arg->getType()->getPointerElementType()->isArrayTy() && arg->getType()->getPointerElementType()->getArrayElementType() == builder.getInt8Ty())
        {
            formatStr += "%s";
        }
        else
        {
            throw logic_error("[ERROR]Invalid type to read.");
        }
    }
    args->insert(args->begin(), builder.CreateGlobalStringPtr(formatStr));
    return builder.CreateCall(generator->scanf, *args, "scanf");
}

// Exp --> NAME
// Exp --> NAME[Exp]
// Exp --> NAME[]
llvm::Value *IRAddr(Node *node)
{
    if (node->childNum == 1)
    {
        return generator->findValue(*node->childNode[0]->nodeText);
    }
    else if (node->childNum == 4)
    {
        llvm::Value *arrayValue = generator->findValue(*node->childNode[0]->nodeText);
        llvm::Value *indexValue = IRExp(node->childNode[2]);
        vector<llvm::Value *> indexList;
        indexList.push_back(builder.getInt32(0));
        indexList.push_back(indexValue);
        // var value
        return builder.CreateInBoundsGEP(arrayValue, llvm::ArrayRef<llvm::Value *>(indexList), "tmpvar");
    }
    else if (node->childNum == 3)
    {
        return generator->findValue(*node->childNode[0]->nodeText);
    }
    else
    {
        // ERROR
    }
    return NULL;
}

llvm::Type *getLlvmType(int type, int arraySize)
{
    switch (type)
    {
    case TYPE_INT:
        return llvm::Type::getInt32Ty(context);
        break;
    case TYPE_INT_ARRAY:
        if (arraySize > 0)
        {
            return llvm::ArrayType::get(llvm::Type::getInt32Ty(context), arraySize);
        }
        else
        {
            return llvm::Type::getInt32PtrTy(context);
        }
        break;
    case TYPE_FLOAT:
        return llvm::Type::getFloatTy(context);
        break;
    case TYPE_FLOAT_ARRAY:
        if (arraySize > 0)
        {
            return llvm::ArrayType::get(llvm::Type::getFloatTy(context), arraySize);
        }
        else
        {
            return llvm::Type::getFloatPtrTy(context);
        }
        break;
    case TYPE_BOOL:
        return llvm::Type::getInt1Ty(context);
        break;
    case TYPE_BOOL_ARRAY:
        if (arraySize > 0)
        {
            return llvm::ArrayType::get(llvm::Type::getInt1Ty(context), arraySize);
        }
        else
        {
            return llvm::Type::getInt1PtrTy(context);
        }
        break;
    case TYPE_CHAR:
        return llvm::Type::getInt8Ty(context);
        break;
    case TYPE_CHAR_ARRAY:
        if (arraySize > 0)
        {
            return llvm::ArrayType::get(llvm::Type::getInt8Ty(context), arraySize);
        }
        else
        {
            return llvm::Type::getInt8PtrTy(context);
        }
        break;
    default:
        break;
    }
    return llvm::Type::getVoidTy(context);
}

// Defineblock --> Variable
// Defineblock --> Variable COMMA Defineblock
// Variablelist --> Variable
// Variablelist --> Variable COMMA Variablelist
vector<pair<string, int>> *getNameList(Node *node, int type)
{
    if (node->nodeType->compare("Defineblock") != 0 && node->nodeType->compare("Variablelist") != 0)
    {
        throw logic_error("[ERROR]Wrong function call : getNameList.");
    }
    Node *temp = node;
    vector<pair<string, int>> *nameList = new vector<pair<string, int>>;
    while (true)
    {
        // Variable --> NAME[INT]
        if (temp->childNode[0]->childNum == 4)
        {
            int arraySize = stoi(*temp->childNode[0]->childNode[2]->nodeText);
            nameList->push_back(make_pair(*temp->childNode[0]->childNode[0]->nodeText, ARRAY + arraySize));
            temp->childNode[0]->childNode[0]->setValueType(type + ARRAY);
        }
        // Variable --> NAME
        else if (temp->childNode[0]->childNum == 1)
        {
            nameList->push_back(make_pair(*temp->childNode[0]->childNode[0]->nodeText, VAR));
            temp->childNode[0]->childNode[0]->setValueType(type);
        }
        else if (temp->childNode[0]->childNum == 3)
        {
            nameList->push_back(make_pair(*temp->childNode[0]->childNode[0]->nodeText, ARRAY));
            temp->childNode[0]->childNode[0]->setValueType(type + ARRAY);
        }
        else
        {
            throw logic_error("[ERROR]Wrong var def");
            return nameList;
        }
        // Defineblock --> Variable COMMA Defineblock
        // Variablelist --> Variable COMMA Variablelist
        if (temp->childNum == 3)
            temp = temp->childNode[2];
        // Defineblock --> Variable
        // Variablelist --> Variable
        else
            break;
    }
    return nameList;
}

// Args --> Exp COMMA Args
// Args --> Exp
vector<llvm::Value *> *getArgs(Node *cur)
{
    vector<llvm::Value *> *args = new vector<llvm::Value *>;
    Node *node = cur;
    while (true)
    {
        llvm::Value *tmp = IRExp(node->childNode[0]);
        if (node->childNum == 1)
        {
            args->push_back(tmp);
            break;
        }
        else
        {
            args->push_back(tmp);
            node = node->childNode[2];
        }
    }
    return args;
}

// print need cast float to double
vector<llvm::Value *> *getPrintArgs(Node *cur)
{
    vector<llvm::Value *> *args = new vector<llvm::Value *>;
    Node *node = cur;
    while (true)
    {
        llvm::Value *tmp = IRExp(node->childNode[0]);
        if (tmp->getType() == llvm::Type::getFloatTy(context))
            tmp = builder.CreateFPExt(tmp, llvm::Type::getDoubleTy(context), "tmpdouble");
        if (node->childNum == 1)
        {
            args->push_back(tmp);
            break;
        }
        else
        {
            args->push_back(tmp);
            node = node->childNode[2];
        }
    }
    return args;
}

vector<llvm::Value *> *getArgsAddr(Node *cur)
{
    vector<llvm::Value *> *args = new vector<llvm::Value *>;
    Node *node = cur;
    while (true)
    {
        if (node->childNum == 1)
        {
            args->push_back(IRAddr(node->childNode[0]));
            break;
        }
        else
        {
            args->push_back(IRAddr(node->childNode[0]));
            node = node->childNode[2];
        }
    }
    return args;
}

// Variableline --> Singledef COMMA Variableline
// Variableline --> Singledef
// Singledef --> Datatype Variable
vector<pair<string, llvm::Type *>> *getParam(Node *cur)
{
    if (cur->nodeType->compare("Variableline") != 0)
    {
        throw logic_error("[ERROR]Wrong function call : getParam.");
        return NULL;
    }
    Node *temp0 = cur;
    // Singledef
    Node *temp1 = cur->childNode[0];
    vector<pair<string, llvm::Type *>> *paramList = new vector<pair<string, llvm::Type *>>;
    while (true)
    {
        temp1 = temp0->childNode[0];
        // Singledef --> Datatype Variable
        // Variable --> NAME[]
        if (temp1->childNode[1]->childNum == 3)
        {
            paramList->push_back(make_pair(*temp1->childNode[1]->childNode[0]->nodeText, getLlvmType(ARRAY + temp1->childNode[0]->getValueType(), 0)));
        }
        // Variable --> NAME
        else if (temp1->childNode[1]->childNum == 1)
        {
            paramList->push_back(make_pair(*temp1->childNode[1]->childNode[0]->nodeText, getLlvmType(VAR + temp1->childNode[0]->getValueType(), 0)));
        }
        else
        {
            throw logic_error("[ERROR]Wrong var def.");
            return paramList;
        }
        // Variableline --> Singledef COMMA Variableline
        if (temp0->childNum == 3)
            temp0 = temp0->childNode[2];
        // Variableline --> Singledef
        else
            break;
    }
    return paramList;
}

llvm::Function *codeGen::getCurFunction()
{
    return funStack.top();
}

void codeGen::pushFunction(llvm::Function *func)
{
    funStack.push(func);
}

void codeGen::popFunction()
{
    funStack.pop();
}

llvm::Value *codeGen::findValue(const std::string &name)
{
    llvm::Value *result = funStack.top()->getValueSymbolTable()->lookup(name);
    if (result != nullptr)
    {
        return result;
    }
    result = module->getGlobalVariable(name, true);
    if (result == nullptr)
    {
        cout << "[ERROR]Undeclared variable: " << name << endl;
    }
    return result;
}

llvm::Function *codeGen::createPrintf()
{
    std::vector<llvm::Type *> arg_types;
    arg_types.push_back(builder.getInt8PtrTy());
    auto printf_type = llvm::FunctionType::get(builder.getInt32Ty(), llvm::makeArrayRef(arg_types), true);
    auto func = llvm::Function::Create(printf_type, llvm::Function::ExternalLinkage, llvm::Twine("printf"), this->module);
    func->setCallingConv(llvm::CallingConv::C);
    return func;
}

llvm::Function *codeGen::createScanf()
{
    auto scanf_type = llvm::FunctionType::get(builder.getInt32Ty(), true);
    auto func = llvm::Function::Create(scanf_type, llvm::Function::ExternalLinkage, llvm::Twine("scanf"), this->module);
    func->setCallingConv(llvm::CallingConv::C);
    return func;
}

void codeGen::generate(Node *root)
{
    IR(root);
    // this->module->print(llvm::outs(), nullptr);
    std::error_code EC;
    llvm::raw_fd_ostream Out("test.ll", EC);
    this->module->print(Out, nullptr);
}

codeGen::codeGen()
{
    this->module = new llvm::Module("main", context);
    this->printf = this->createPrintf();
    this->scanf = this->createScanf();
}

codeGen::~codeGen()
{
}
